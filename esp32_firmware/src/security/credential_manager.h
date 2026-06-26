#pragma once

#include <string>
#include <cstdint>
#include <vector>

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

    static constexpr size_t KEY_LENGTH = 32;
    static constexpr size_t IV_LENGTH = 12;
    static constexpr size_t TAG_LENGTH = 16;
    static constexpr size_t SALT_LENGTH = 16;
    static constexpr size_t MAC_LENGTH = 6;

    uint8_t aesKey_[KEY_LENGTH];

    bool deriveKey();
    std::vector<uint8_t> encrypt(const uint8_t* plaintext, size_t length);
    std::vector<uint8_t> decrypt(const uint8_t* ciphertext, size_t length);
};
