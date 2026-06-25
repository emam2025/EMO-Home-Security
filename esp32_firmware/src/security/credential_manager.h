#pragma once

#include <string>
#include <cstdint>

class NvsManager;

class CredentialManager {
public:
    explicit CredentialManager(NvsManager& nvs);

    bool load();
    bool save();

    std::string getRouterIp() const;
    std::string getRouterUsername() const;
    std::string getRouterPassword() const;

    void setRouterCredentials(const std::string& ip, const std::string& username, const std::string& password);

    bool hasStoredCredentials() const;

private:
    NvsManager& nvs_;

    std::string routerIp_;
    std::string routerUsername_;
    std::string routerPassword_;

    static constexpr const char* NVS_KEY_IP = "router_ip";
    static constexpr const char* NVS_KEY_USER = "router_user";
    static constexpr const char* NVS_KEY_PASS = "router_pass";
};
