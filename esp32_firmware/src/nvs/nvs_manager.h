#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <Preferences.h>

class NvsManager {
public:
    explicit NvsManager(const std::string& namespaceName = "emo-storage");
    ~NvsManager();

    bool begin(bool readOnly = false);

    bool saveString(const std::string& key, const std::string& value);
    std::string loadString(const std::string& key, const std::string& defaultValue = "");

    bool saveBytes(const std::string& key, const uint8_t* data, size_t length);
    std::vector<uint8_t> loadBytes(const std::string& key);

    bool saveUint32(const std::string& key, uint32_t value);
    uint32_t loadUint32(const std::string& key, uint32_t defaultValue = 0);

    bool saveBool(const std::string& key, bool value);
    bool loadBool(const std::string& key, bool defaultValue = false);

    bool remove(const std::string& key);
    void clear();

    bool commit();

private:
    std::string namespaceName_;
    Preferences prefs_;
    bool initialized_;
};
