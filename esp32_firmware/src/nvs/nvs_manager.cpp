#include "nvs_manager.h"

NvsManager::NvsManager(const std::string& namespaceName)
    : namespaceName_(namespaceName), initialized_(false) {}

NvsManager::~NvsManager() {
    if (initialized_) {
        prefs_.end();
    }
}

bool NvsManager::begin(bool readOnly) {
    if (initialized_) return true;

    bool ok = prefs_.begin(namespaceName_.c_str(), readOnly);
    initialized_ = ok;
    return ok;
}

bool NvsManager::saveString(const std::string& key, const std::string& value) {
    if (!initialized_ && !begin()) return false;
    return prefs_.putString(key.c_str(), value.c_str());
}

std::string NvsManager::loadString(const std::string& key, const std::string& defaultValue) {
    if (!initialized_ && !begin()) return defaultValue;
    return prefs_.getString(key.c_str(), defaultValue.c_str()).c_str();
}

bool NvsManager::saveBytes(const std::string& key, const uint8_t* data, size_t length) {
    if (!initialized_ && !begin()) return false;
    return prefs_.putBytes(key.c_str(), data, length);
}

std::vector<uint8_t> NvsManager::loadBytes(const std::string& key) {
    std::vector<uint8_t> result;
    if (!initialized_ && !begin()) return result;

    size_t length = prefs_.getBytesLength(key.c_str());
    if (length == 0) return result;

    result.resize(length);
    prefs_.getBytes(key.c_str(), result.data(), length);
    return result;
}

bool NvsManager::saveUint32(const std::string& key, uint32_t value) {
    if (!initialized_ && !begin()) return false;
    return prefs_.putUInt(key.c_str(), value);
}

uint32_t NvsManager::loadUint32(const std::string& key, uint32_t defaultValue) {
    if (!initialized_ && !begin()) return defaultValue;
    return prefs_.getUInt(key.c_str(), defaultValue);
}

bool NvsManager::saveBool(const std::string& key, bool value) {
    if (!initialized_ && !begin()) return false;
    return prefs_.putBool(key.c_str(), value);
}

bool NvsManager::loadBool(const std::string& key, bool defaultValue) {
    if (!initialized_ && !begin()) return defaultValue;
    return prefs_.getBool(key.c_str(), defaultValue);
}

bool NvsManager::remove(const std::string& key) {
    if (!initialized_ && !begin()) return false;
    return prefs_.remove(key.c_str());
}

void NvsManager::clear() {
    if (!initialized_ && !begin()) return;
    prefs_.clear();
}

bool NvsManager::commit() {
    if (!initialized_) return false;
    prefs_.end();
    return true;
}
