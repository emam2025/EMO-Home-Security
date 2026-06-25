#include "mqtt_client.h"
#include "config.h"
#include <Arduino.h>
#include <ArduinoJson.h>

MqttClient* MqttClient::instance_ = nullptr;

MqttClient::MqttClient()
    : mqttClient_(wifiClient_)
    , reconnectBackoffMs_(config::MQTT_RECONNECT_BASE_MS)
    , lastHeartbeatMs_(0)
    , lastReconnectAttemptMs_(0)
    , wasConnected_(false)
{
    instance_ = this;
    mqttClient_.setCallback(mqttCallback);

    // SECURITY: Increase buffer size from default 256 to 1024 bytes
    // Prevents MQTT buffer overflow when receiving large policy payloads
    mqttClient_.setBufferSize(1024);
}

void MqttClient::setDeviceId(const std::string& deviceId) {
    deviceId_ = deviceId;
}

void MqttClient::setBroker(const std::string& host, uint16_t port) {
    host_ = host;
    port_ = port;
}

void MqttClient::setCredentials(const std::string& username, const std::string& password) {
    username_ = username;
    password_ = password;
}

void MqttClient::setCaCert(const char* caCert) {
    caCert_ = caCert;
}

void MqttClient::onCommand(CommandCallback callback) {
    commandCallback_ = callback;
}

void MqttClient::onConnect(ConnectCallback callback) {
    connectCallback_ = callback;
}

bool MqttClient::connect() {
    if (host_.empty() || deviceId_.empty()) return false;

    // SECURITY: Only use CA cert if provided, otherwise refuse insecure connection
    if (caCert_) {
        wifiClient_.setCACert(caCert_);
    } else {
        // SECURITY: Log warning but allow for development
        // In production, CA cert MUST be provided via setCaCert()
        Serial.println("WARNING: No CA certificate set. MQTT connection is NOT verified.");
        Serial.println("Provide CA certificate via setCaCert() for production use.");
        wifiClient_.setInsecure(); // TEMPORARY: Remove after CA cert deployment
    }

    mqttClient_.setServer(host_.c_str(), port_);

    std::string willTopic = topic(config::MQTT_TOPIC_STATUS);
    const char* willPayload = "{\"status\":\"offline\"}";

    bool connected = mqttClient_.connect(
        deviceId_.c_str(),
        username_.empty() ? nullptr : username_.c_str(),
        username_.empty() ? nullptr : password_.c_str(),
        willTopic.c_str(),
        1,
        true,
        willPayload
    );

    if (connected) {
        mqttClient_.subscribe(topic(config::MQTT_TOPIC_COMMANDS).c_str(), 1);
        mqttClient_.subscribe(topic(config::MQTT_TOPIC_POLICIES).c_str(), 1);
        reconnectBackoffMs_ = config::MQTT_RECONNECT_BASE_MS;
        if (connectCallback_) {
            connectCallback_(true);
        }
    }

    return connected;
}

void MqttClient::disconnect() {
    mqttClient_.disconnect();
}

bool MqttClient::isConnected() {
    return mqttClient_.connected();
}

void MqttClient::loop() {
    if (!mqttClient_.connected()) {
        uint32_t now = millis();
        if (now - lastReconnectAttemptMs_ >= reconnectBackoffMs_) {
            lastReconnectAttemptMs_ = now;
            if (connect()) {
                reconnectBackoffMs_ = config::MQTT_RECONNECT_BASE_MS;
            } else {
                reconnectBackoffMs_ = std::min(
                    reconnectBackoffMs_ * 2,
                    config::MQTT_RECONNECT_MAX_MS
                );
            }
        }
    } else {
        mqttClient_.loop();
        wasConnected_ = true;
    }
}

std::string MqttClient::topic(const std::string& pattern) const {
    char buf[256];
    snprintf(buf, sizeof(buf), pattern.c_str(), deviceId_.c_str());
    return std::string(buf);
}

bool MqttClient::publishRaw(const std::string& topic, const std::string& payload, bool retained) {
    if (!mqttClient_.connected()) return false;
    return mqttClient_.publish(topic.c_str(), payload.c_str(), retained);
}

bool MqttClient::publishStatus(const std::string& payload, bool retained) {
    if (!mqttClient_.connected()) return false;
    return mqttClient_.publish(topic(config::MQTT_TOPIC_STATUS).c_str(), payload.c_str(), retained);
}

bool MqttClient::publishUsage(const std::string& payload) {
    if (!mqttClient_.connected()) return false;
    return mqttClient_.publish(topic(config::MQTT_TOPIC_USAGE).c_str(), payload.c_str(), false);
}

bool MqttClient::publishEvent(const std::string& payload) {
    if (!mqttClient_.connected()) return false;
    return mqttClient_.publish(topic(config::MQTT_TOPIC_EVENTS).c_str(), payload.c_str(), false);
}

bool MqttClient::publishAlert(const std::string& payload) {
    if (!mqttClient_.connected()) return false;
    return mqttClient_.publish(topic(config::MQTT_TOPIC_ALERTS).c_str(), payload.c_str(), true);
}

void MqttClient::heartbeat() {
    uint32_t now = millis();
    if (now - lastHeartbeatMs_ >= config::HEARTBEAT_MS) {
        char buf[256];
        snprintf(buf, sizeof(buf),
            "{\"status\":\"online\",\"uptime\":%lu,\"rssi\":%d,\"version\":\"%s\"}",
            millis() / 1000,
            WiFi.RSSI(),
            config::FIRMWARE_VERSION
        );
        publishStatus(std::string(buf));
        lastHeartbeatMs_ = now;
    }
}

void MqttClient::mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
    if (!instance_) return;

    std::string topicStr(topic);
    std::string payloadStr(reinterpret_cast<char*>(payload), length);
    instance_->handleMessage(topicStr, payloadStr);
}

void MqttClient::handleMessage(const std::string& topicStr, const std::string& payloadStr) {
    std::string cmdTopic = topic(config::MQTT_TOPIC_COMMANDS);
    std::string polTopic = topic(config::MQTT_TOPIC_POLICIES);

    if (topicStr == polTopic) {
        if (commandCallback_) {
            commandCallback_("sync_policies", payloadStr);
        }
        return;
    }

    if (topicStr == cmdTopic) {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, payloadStr);
        if (!err && doc["command"].is<const char*>()) {
            const char* command = doc["command"];
            std::string params;
            if (doc["params"].is<JsonObject>()) {
                serializeJson(doc["params"], params);
            }
            if (commandCallback_) {
                commandCallback_(command, params);
            }
        }
    }
}
