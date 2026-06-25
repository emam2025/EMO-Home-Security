#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>

struct DeviceEntry {
    std::string mac;
    std::string ip;
    std::string hostname;
    bool isApproved;
    std::string profileId;
    uint32_t firstSeen;
    uint32_t lastSeen;
};

class DeviceRegistry {
public:
    DeviceRegistry();

    bool isKnownDevice(const std::string& mac) const;
    void addOrUpdateDevice(const DeviceEntry& entry);
    DeviceEntry getDevice(const std::string& mac) const;
    std::vector<DeviceEntry> getAllDevices() const;
    std::vector<DeviceEntry> getUnregisteredDevices() const;
    std::vector<DeviceEntry> getDevicesByProfile(const std::string& profileId) const;
    void removeDevice(const std::string& mac);

    bool loadFromNVS();
    bool saveToNVS();
    void clear();
    size_t size() const { return devices_.size(); }

    bool isRegistered();
    std::string generateDeviceId();
    std::string generatePairingCode();
    void markRegistered();
    std::string getDeviceId();
    std::string getPairingCode();

private:
    std::map<std::string, DeviceEntry> devices_;
    static constexpr const char* NVS_KEY = "device_registry";
};
