#include "device_registry.h"
#include "config.h"
#include "nvs/nvs_manager.h"
#include <ArduinoJson.h>
#include <esp_mac.h>
#include <esp_system.h>

DeviceRegistry::DeviceRegistry() {}

bool DeviceRegistry::isKnownDevice(const std::string& mac) const {
    return devices_.find(mac) != devices_.end();
}

void DeviceRegistry::addOrUpdateDevice(const DeviceEntry& entry) {
    devices_[entry.mac] = entry;
}

DeviceEntry DeviceRegistry::getDevice(const std::string& mac) const {
    auto it = devices_.find(mac);
    if (it != devices_.end()) {
        return it->second;
    }
    DeviceEntry empty;
    empty.mac = mac;
    empty.isApproved = false;
    empty.firstSeen = 0;
    empty.lastSeen = 0;
    return empty;
}

std::vector<DeviceEntry> DeviceRegistry::getAllDevices() const {
    std::vector<DeviceEntry> result;
    result.reserve(devices_.size());
    for (const auto& pair : devices_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<DeviceEntry> DeviceRegistry::getUnregisteredDevices() const {
    std::vector<DeviceEntry> result;
    for (const auto& pair : devices_) {
        if (pair.second.profileId.empty()) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<DeviceEntry> DeviceRegistry::getDevicesByProfile(const std::string& profileId) const {
    std::vector<DeviceEntry> result;
    for (const auto& pair : devices_) {
        if (pair.second.profileId == profileId) {
            result.push_back(pair.second);
        }
    }
    return result;
}

void DeviceRegistry::removeDevice(const std::string& mac) {
    devices_.erase(mac);
}

bool DeviceRegistry::saveToNVS() {
    NvsManager nvs;

    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (const auto& pair : devices_) {
        JsonObject obj = arr.add<JsonObject>();
        obj["mac"] = pair.second.mac;
        obj["ip"] = pair.second.ip;
        obj["hostname"] = pair.second.hostname;
        obj["profile_id"] = pair.second.profileId;
        obj["is_approved"] = pair.second.isApproved;
        obj["first_seen"] = pair.second.firstSeen;
        obj["last_seen"] = pair.second.lastSeen;
    }

    std::string json;
    serializeJson(doc, json);

    return nvs.saveString(NVS_KEY, json);
}

bool DeviceRegistry::loadFromNVS() {
    NvsManager nvs;

    std::string json = nvs.loadString(NVS_KEY);
    if (json.empty()) return false;

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) return false;

    JsonArray arr = doc.as<JsonArray>();
    for (JsonObject obj : arr) {
        DeviceEntry entry;
        entry.mac = obj["mac"].as<std::string>();
        entry.ip = obj["ip"].as<std::string>();
        entry.hostname = obj["hostname"].as<std::string>();
        entry.profileId = obj["profile_id"].as<std::string>();
        entry.isApproved = obj["is_approved"].as<bool>();
        entry.firstSeen = obj["first_seen"].as<uint32_t>();
        entry.lastSeen = obj["last_seen"].as<uint32_t>();
        devices_[entry.mac] = entry;
    }

    return true;
}

void DeviceRegistry::clear() {
    devices_.clear();
}

bool DeviceRegistry::isRegistered() {
    NvsManager nvs;
    return nvs.loadBool(config::NVS_KEY_REGISTERED, false);
}

std::string DeviceRegistry::generateDeviceId() {
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);

    char macStr[13];
    snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X%02X%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    std::string macFull(macStr);
    std::string macSuffix = macFull.substr(macFull.length() - 6);

    uint64_t chipId = ESP.getEfuseMac();
    uint32_t hash = static_cast<uint32_t>(chipId ^ (chipId >> 32));
    char chipHash[9];
    snprintf(chipHash, sizeof(chipHash), "%08X", hash);

    return "EMO-" + macSuffix + "-" + std::string(chipHash);
}

std::string DeviceRegistry::generatePairingCode() {
    const char charset[] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    char code[7];
    for (int i = 0; i < 6; i++) {
        code[i] = charset[esp_random() % (sizeof(charset) - 1)];
    }
    code[6] = '\0';
    return std::string(code);
}

void DeviceRegistry::markRegistered() {
    NvsManager nvs;
    nvs.saveBool(config::NVS_KEY_REGISTERED, true);
}

std::string DeviceRegistry::getDeviceId() {
    NvsManager nvs;
    std::string id = nvs.loadString(config::NVS_KEY_DEVICE_ID);
    if (id.empty()) {
        id = generateDeviceId();
        nvs.saveString(config::NVS_KEY_DEVICE_ID, id);
    }
    return id;
}

std::string DeviceRegistry::getPairingCode() {
    NvsManager nvs;
    std::string code = nvs.loadString(config::NVS_KEY_PAIRING_CODE);
    if (code.empty()) {
        code = generatePairingCode();
        nvs.saveString(config::NVS_KEY_PAIRING_CODE, code);
    }
    return code;
}
