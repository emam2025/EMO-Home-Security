#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.h"
#include "config_defaults.h"
#include "IRouterDriver.h"
#include "device_registry.h"
#include "drivers/huawei_hg8145v5_driver.h"
#include "mqtt/mqtt_client.h"
#include "nvs/nvs_manager.h"
#include "policy/policy_engine.h"
#include "network/network_manager.h"
#include "security/tamper_detector.h"
#include "security/credential_manager.h"
#include "ota/ota_updater.h"

static NetworkManager networkManager;
static HuaweiHG8145V5Driver routerDriver(
    config::ROUTER_DEFAULT_IP,
    config::ROUTER_DEFAULT_USERNAME,
    config::ROUTER_DEFAULT_PASSWORD
);
static MqttClient mqttClient;
static DeviceRegistry deviceRegistry;
static PolicyEngine policyEngine(&routerDriver);
static NvsManager nvs;
static CredentialManager credentialManager(nvs);
static TamperDetector tamperDetector;
static OtaUpdater otaUpdater;

static uint32_t lastDevicePollMs = 0;
static uint32_t lastPolicyCheckMs = 0;
static uint32_t lastUsageReportMs = 0;
static bool lastMqttConnected = false;
static bool registered = false;
static std::string deviceId;
static std::string pairingCode;

static void setupMqttCallbacks() {
    mqttClient.onCommand([](const std::string& topic, const std::string& payload) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
            mqttClient.publishAlert(std::string("{\"error\":\"invalid_json\"}"));
            return;
        }

        const char* cmd = doc["command"];
        if (!cmd) return;

        std::string commandStr(cmd);

        if (commandStr == "block_device") {
            std::string mac = doc["mac"].as<std::string>();
            if (!mac.empty()) {
                bool ok = routerDriver.blockDevice(mac);
                mqttClient.publishEvent(std::string("{\"event\":\"block_device\",\"mac\":\"") + mac + "\",\"success\":" + (ok ? "true" : "false") + "}");
            }
        } else if (commandStr == "unblock_device") {
            std::string mac = doc["mac"].as<std::string>();
            if (!mac.empty()) {
                bool ok = routerDriver.unblockDevice(mac);
                mqttClient.publishEvent(std::string("{\"event\":\"unblock_device\",\"mac\":\"") + mac + "\",\"success\":" + (ok ? "true" : "false") + "}");
            }
        } else if (commandStr == "set_dns") {
            std::string primary = doc["primary_dns"].as<std::string>();
            std::string secondary = doc["secondary_dns"].as<std::string>();
            bool ok = routerDriver.setDNS(primary, secondary);
            mqttClient.publishEvent(std::string("{\"event\":\"set_dns\",\"success\":") + (ok ? "true" : "false") + "}");
        } else if (commandStr == "reboot") {
            bool ok = routerDriver.reboot();
            mqttClient.publishEvent(std::string("{\"event\":\"reboot\",\"success\":") + (ok ? "true" : "false") + "}");
        } else if (commandStr == "pause_internet") {
            bool ok = routerDriver.enableWhitelist();
            mqttClient.publishEvent(std::string("{\"event\":\"pause_internet\",\"success\":") + (ok ? "true" : "false") + "}");
        } else if (commandStr == "resume_internet") {
            bool ok = routerDriver.disableWhitelist();
            mqttClient.publishEvent(std::string("{\"event\":\"resume_internet\",\"success\":") + (ok ? "true" : "false") + "}");
        } else if (commandStr == "sync_policies") {
            std::string policiesJson;
            serializeJson(doc["policies"], policiesJson);
            policyEngine.updatePolicies(policiesJson);
            mqttClient.publishEvent(std::string("{\"event\":\"policies_synced\"}"));
        } else if (commandStr == "update_credentials") {
            std::string ip = doc["ip"].as<std::string>();
            std::string user = doc["username"].as<std::string>();
            std::string pass = doc["password"].as<std::string>();
            credentialManager.setRouterCredentials(ip, user, pass);
            routerDriver.setCredentials(ip, user, pass);
            bool ok = routerDriver.login();
            tamperDetector.reportRouterLoginResult(ok);
            mqttClient.publishEvent(std::string("{\"event\":\"credentials_updated\",\"success\":") + (ok ? "true" : "false") + "}");
        } else if (commandStr == "recover_router") {
            credentialManager.load();
            routerDriver.setCredentials(
                credentialManager.getRouterIp(),
                credentialManager.getRouterUsername(),
                credentialManager.getRouterPassword()
            );
            bool ok = routerDriver.login();
            tamperDetector.reportRouterLoginResult(ok);
            mqttClient.publishEvent(std::string("{\"event\":\"recover_router\",\"success\":") + (ok ? "true" : "false") + "}");
        } else if (commandStr == "ota_update") {
            std::string url = doc["url"].as<std::string>();
            std::string version = doc["version"].as<std::string>();
            otaUpdater.startUpdate(url, version);
        }
    });
}

static void handleDevicePoll() {
    uint32_t now = millis();
    if (now - lastDevicePollMs < config::DEVICE_POLL_MS) return;
    lastDevicePollMs = now;

    auto devices = routerDriver.getDevices();
    uint32_t newCount = 0;

    // SECURITY: Limit device list to prevent JSON buffer overflow
    // ESP32 has limited heap (8KB stack). Max 50 devices per poll cycle.
    const size_t MAX_DEVICES_PER_POLL = 50;
    size_t deviceCount = 0;

    for (const auto& dev : devices) {
        if (deviceCount >= MAX_DEVICES_PER_POLL) {
            Serial.printf("WARNING: Device list truncated at %u devices (heap limit)\n", MAX_DEVICES_PER_POLL);
            break;
        }

        if (!deviceRegistry.isKnownDevice(dev.mac)) {
            DeviceEntry entry;
            entry.mac = dev.mac;
            entry.ip = dev.ip;
            entry.hostname = dev.hostname;
            entry.isApproved = false;
            entry.firstSeen = now / 1000;
            entry.lastSeen = now / 1000;
            deviceRegistry.addOrUpdateDevice(entry);
            newCount++;

            mqttClient.publishEvent(
                std::string("{\"event\":\"new_device\",\"mac\":\"") + dev.mac + "\",\"hostname\":\"" + dev.hostname + "\"}"
            );
        } else {
            auto existing = deviceRegistry.getDevice(dev.mac);
            existing.ip = dev.ip;
            existing.lastSeen = now / 1000;
            existing.hostname = dev.hostname;
            deviceRegistry.addOrUpdateDevice(existing);
        }
        deviceCount++;
    }

    if (newCount > 0) {
        deviceRegistry.saveToNVS();

        auto unregistered = deviceRegistry.getUnregisteredDevices();
        JsonDocument alertDoc;
        JsonArray arr = alertDoc.to<JsonArray>();
        for (const auto& dev : unregistered) {
            JsonObject obj = arr.add<JsonObject>();
            obj["mac"] = dev.mac;
            obj["hostname"] = dev.hostname;
        }
        std::string alertJson;
        serializeJson(arr, alertJson);
        mqttClient.publishAlert(
            std::string("{\"alert\":\"unregistered_devices\",\"count\":") + std::to_string(unregistered.size()) + ",\"devices\":" + alertJson + "}"
        );
    }
}

static void handlePolicyCheck() {
    uint32_t now = millis();
    if (now - lastPolicyCheckMs < config::POLICY_CHECK_MS) return;
    lastPolicyCheckMs = now;

    auto actions = policyEngine.evaluate();
    if (actions.empty()) return;

    bool success = policyEngine.applyActions(actions);

    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();
    for (const auto& action : actions) {
        JsonObject obj = arr.add<JsonObject>();
        obj["device_mac"] = action.deviceMac;
        obj["profile_id"] = action.profileId;
        obj["action"] = (action.type == PolicyAction::BLOCK) ? "block" : "unblock";
        obj["reason"] = action.reason;
    }
    std::string eventJson;
    serializeJson(arr, eventJson);
    mqttClient.publishEvent(
        std::string("{\"event\":\"policy_applied\",\"success\":") + (success ? "true" : "false") + ",\"actions\":" + eventJson + "}"
    );
}

static void handleUsageReport() {
    uint32_t now = millis();
    if (now - lastUsageReportMs < config::USAGE_REPORT_MS) return;
    lastUsageReportMs = now;

    auto devices = routerDriver.getDevices();
    auto stats = routerDriver.getStatistics();

    // SECURITY: Limit device list in usage report to prevent buffer overflow
    const size_t MAX_USAGE_DEVICES = 30;
    size_t usageCount = 0;

    JsonDocument doc;
    doc["timestamp"] = now / 1000;
    doc["uptime"] = stats.uptime;
    doc["total_devices"] = stats.totalDevices;
    doc["online_devices"] = stats.onlineDevices;

    JsonArray arr = doc["devices"].to<JsonArray>();
    for (const auto& dev : devices) {
        if (usageCount >= MAX_USAGE_DEVICES) break;
        JsonObject obj = arr.add<JsonObject>();
        obj["mac"] = dev.mac;
        obj["ip"] = dev.ip;
        obj["hostname"] = dev.hostname;
        obj["online"] = dev.isOnline;
        usageCount++;
    }

    std::string payload;
    serializeJson(doc, payload);
    mqttClient.publishUsage(payload);
}

void setup() {
    Serial.begin(config::MONITOR_SPEED);
    delay(1000);
    Serial.println("\nEMO Family Internet Manager v" + String(config::FIRMWARE_VERSION));

    if (!nvs.begin()) {
        Serial.println("FATAL: NVS init failed");
    } else {
        Serial.println("NVS initialized");
    }

    if (credentialManager.load()) {
        Serial.println("Stored router credentials loaded");
        routerDriver.setCredentials(
            credentialManager.getRouterIp(),
            credentialManager.getRouterUsername(),
            credentialManager.getRouterPassword()
        );
    }

    tamperDetector.setCallback([](TamperEvent event, const std::string& detail) {
        const char* eventName = "";
        switch (event) {
            case TamperEvent::ROUTER_RESTARTED: eventName = "router_restarted"; break;
            case TamperEvent::ROUTER_CREDENTIALS_FAILED: eventName = "router_credentials_failed"; break;
            case TamperEvent::ROUTER_UNREACHABLE: eventName = "router_unreachable"; break;
            case TamperEvent::MQTT_CONNECTION_LOST: eventName = "mqtt_connection_lost"; break;
            case TamperEvent::EMO_OFFLINE: eventName = "emo_offline"; break;
        }
        mqttClient.publishAlert(
            std::string("{\"alert\":\"") + eventName + "\",\"detail\":\"" + detail + "\"}"
        );
    });

    otaUpdater.setCallbacks(
        [](int progress, int total) {
            int pct = (total > 0) ? (progress * 100 / total) : 0;
            mqttClient.publishEvent(
                std::string("{\"event\":\"ota_progress\",\"progress\":") + std::to_string(pct) + "}"
            );
        },
        [](bool success, const std::string& message) {
            mqttClient.publishEvent(
                std::string("{\"event\":\"ota_result\",\"success\":") + (success ? "true" : "false") + ",\"message\":\"" + message + "\"}"
            );
        }
    );

    deviceId = deviceRegistry.getDeviceId();
    pairingCode = deviceRegistry.getPairingCode();
    registered = deviceRegistry.isRegistered();

    Serial.printf("Device ID: %s\n", deviceId.c_str());
    Serial.printf("Pairing Code: %s\n", pairingCode.c_str());
    Serial.printf("Registered: %s\n", registered ? "yes" : "no");

    networkManager.begin();

    mqttClient.setDeviceId(deviceId);
    mqttClient.setBroker(std::string(config::MQTT_BROKER_HOST), config::MQTT_BROKER_PORT);
    mqttClient.setCredentials(std::string(config::MQTT_USERNAME), std::string(config::MQTT_PASSWORD));

    setupMqttCallbacks();

    if (registered) {
        if (networkManager.isConnected()) {
            mqttClient.connect();
        }
        bool loginOk = routerDriver.login();
        tamperDetector.reportRouterLoginResult(loginOk);
        if (loginOk) {
            Serial.println("Router login successful");
        } else {
            Serial.println("Router login failed, will retry");
        }
    } else {
        Serial.println("Device not registered. Waiting for network to send registration...");
    }

    deviceRegistry.loadFromNVS();
    Serial.printf("Device registry loaded: %u devices\n", deviceRegistry.size());

    lastDevicePollMs = millis();
    lastPolicyCheckMs = millis();
    lastUsageReportMs = millis();
    lastMqttConnected = mqttClient.isConnected();
}

void loop() {
    networkManager.loop();
    mqttClient.loop();
    mqttClient.heartbeat();

    bool mqttConnected = mqttClient.isConnected();
    tamperDetector.reportMqttConnected(mqttConnected);

    if (mqttConnected && !lastMqttConnected) {
        if (!registered) {
            JsonDocument regDoc;
            regDoc["device_id"] = deviceId;
            regDoc["mac"] = networkManager.getMacAddress();
            regDoc["pairing_code"] = pairingCode;
            regDoc["firmware_version"] = config::FIRMWARE_VERSION;
            std::string regJson;
            serializeJson(regDoc, regJson);
            mqttClient.publishRaw(config::MQTT_TOPIC_REGISTER, regJson);
            Serial.println("Registration request sent to " + String(config::MQTT_TOPIC_REGISTER));
        }

        std::string statusPayload = "{\"status\":\"online\",\"device_id\":\"" + deviceId +
            "\",\"version\":\"" + config::FIRMWARE_VERSION + "\"}";
        mqttClient.publishStatus(statusPayload);
    }
    lastMqttConnected = mqttConnected;

    if (mqttConnected && routerDriver.isAuthenticated()) {
        auto stats = routerDriver.getStatistics();
        tamperDetector.reportRouterUptime(stats.uptime);
    }

    handleDevicePoll();
    handlePolicyCheck();
    handleUsageReport();
    tamperDetector.loop();
    otaUpdater.loop();
}
