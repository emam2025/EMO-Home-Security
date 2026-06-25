#pragma once

#include <cstdint>
#include <vector>
#include <string>

struct RouterDevice {
    std::string mac;
    std::string ip;
    std::string hostname;
    std::string vendor;
    bool isOnline;
};

struct RouterStatistics {
    uint32_t totalDevices;
    uint32_t onlineDevices;
    uint32_t uptime;
};

class IRouterDriver {
public:
    virtual ~IRouterDriver() = default;

    virtual bool login() = 0;
    virtual bool logout() = 0;

    virtual std::vector<RouterDevice> getDevices() = 0;
    virtual bool blockDevice(const std::string& mac) = 0;
    virtual bool unblockDevice(const std::string& mac) = 0;

    virtual bool setDNS(const std::string& primary, const std::string& secondary) = 0;
    virtual bool enableWhitelist() = 0;
    virtual bool disableWhitelist() = 0;

    virtual RouterStatistics getStatistics() = 0;
    virtual bool reboot() = 0;
};
