#include "credential_manager.h"
#include "nvs_manager.h"
#include <Arduino.h>

// SECURITY: Device-specific XOR key derived from MAC address
// This is NOT military-grade encryption but prevents plaintext NVS dumps
// from immediately exposing router credentials.
static uint8_t xorKey[16] = {0};

static void deriveXorKey() {
    // Derive key from ESP32's MAC address (unique per device)
    uint8_t mac[6];
    WiFi.macAddress(mac);
    for (int i = 0; i < 16; i++) {
        xorKey[i] = mac[i % 6] ^ (i * 0x37 + 0xAB);
    }
}

static void xorEncrypt(uint8_t* data, size_t len) {
    if (xorKey[0] == 0 && xorKey[1] == 0) deriveXorKey();
    for (size_t i = 0; i < len; i++) {
        data[i] ^= xorKey[i % 16];
    }
}

CredentialManager::CredentialManager(NvsManager& nvs)
    : nvs_(nvs)
{}

bool CredentialManager::load() {
    routerIp_ = nvs_.loadString(NVS_KEY_IP);
    routerUsername_ = nvs_.loadString(NVS_KEY_USER);
    routerPassword_ = nvs_.loadString(NVS_KEY_PASS);

    // SECURITY: Decrypt loaded credentials using XOR obfuscation
    if (!routerPassword_.empty()) {
        xorEncrypt(reinterpret_cast<uint8_t*>(routerPassword_.data()), routerPassword_.size());
    }

    return hasStoredCredentials();
}

bool CredentialManager::save() {
    // SECURITY: Create encrypted copy before storing
    std::string encryptedPass = routerPassword_;
    if (!encryptedPass.empty()) {
        xorEncrypt(reinterpret_cast<uint8_t*>(encryptedPass.data()), encryptedPass.size());
    }

    bool ok = true;
    ok &= nvs_.saveString(NVS_KEY_IP, routerIp_);
    ok &= nvs_.saveString(NVS_KEY_USER, routerUsername_);
    ok &= nvs_.saveString(NVS_KEY_PASS, encryptedPass);
    return ok && nvs_.commit();
}

std::string CredentialManager::getRouterIp() const {
    return routerIp_;
}

std::string CredentialManager::getRouterUsername() const {
    return routerUsername_;
}

std::string CredentialManager::getRouterPassword() const {
    return routerPassword_;
}

void CredentialManager::setRouterCredentials(const std::string& ip, const std::string& username, const std::string& password) {
    routerIp_ = ip;
    routerUsername_ = username;
    routerPassword_ = password;
    save();
}

bool CredentialManager::hasStoredCredentials() const {
    return !routerIp_.empty() && !routerUsername_.empty() && !routerPassword_.empty();
}
