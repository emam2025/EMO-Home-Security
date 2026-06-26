#include "credential_manager.h"
#include "../nvs/nvs_manager.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_random.h>
#include <mbedtls/gcm.h>
#include <mbedtls/hkdf.h>
#include <cstring>

// Pairing secret baked at compile time (min 16 bytes).
// In production this is provisioned per-device during manufacturing.
static const char* PAIRING_SECRET = "EMO_H0ME_SECUR1TY";

CredentialManager::CredentialManager(NvsManager& nvs)
    : nvs_(nvs)
{
    memset(aesKey_, 0, KEY_LENGTH);
}

bool CredentialManager::deriveKey() {
    uint8_t mac[MAC_LENGTH];
    WiFi.macAddress(mac);

    // Salt: first 16 bytes of SHA-256(device MAC) to avoid pre-computation
    uint8_t salt[SALT_LENGTH];
    mbedtls_md_context_t mdCtx;
    mbedtls_md_init(&mdCtx);
    mbedtls_md_setup(&mdCtx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
    mbedtls_md_starts(&mdCtx);
    mbedtls_md_update(&mdCtx, mac, MAC_LENGTH);
    mbedtls_md_finish(&mdCtx, salt);
    mbedtls_md_free(&mdCtx);
    salt[SALT_LENGTH - 1] = 0;

    // HKDF-SHA256: IKM = MAC || pairing_secret
    uint8_t ikm[MAC_LENGTH + 32];
    memcpy(ikm, mac, MAC_LENGTH);
    size_t secretLen = strlen(PAIRING_SECRET);
    memcpy(ikm + MAC_LENGTH, PAIRING_SECRET, secretLen);
    size_t ikmLen = MAC_LENGTH + secretLen;

    int ret = mbedtls_hkdf(
        mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
        salt, SALT_LENGTH,
        ikm, ikmLen,
        nullptr, 0,
        aesKey_, KEY_LENGTH
    );

    memset(ikm, 0, sizeof(ikm));
    memset(salt, 0, sizeof(salt));

    return ret == 0;
}

std::vector<uint8_t> CredentialManager::encrypt(const uint8_t* plaintext, size_t length) {
    if (!deriveKey()) return {};

    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);
    mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, aesKey_, KEY_LENGTH * 8);

    uint8_t iv[IV_LENGTH];
    esp_fill_random(iv, IV_LENGTH);

    // Format: ciphertext || IV (12B) || tag (16B) per document spec
    std::vector<uint8_t> result(length + IV_LENGTH + TAG_LENGTH);
    uint8_t* ciphertext = result.data();
    uint8_t* ivOut = ciphertext + length;
    uint8_t* tagOut = ivOut + IV_LENGTH;

    int ret = mbedtls_gcm_crypt_and_tag(
        &gcm, MBEDTLS_GCM_ENCRYPT,
        length, iv, IV_LENGTH,
        nullptr, 0,
        plaintext, ciphertext,
        TAG_LENGTH, tagOut
    );

    memcpy(ivOut, iv, IV_LENGTH);

    mbedtls_gcm_free(&gcm);
    memset(aesKey_, 0, KEY_LENGTH);

    if (ret != 0) return {};
    return result;
}

std::vector<uint8_t> CredentialManager::decrypt(const uint8_t* data, size_t length) {
    if (length < IV_LENGTH + TAG_LENGTH + 1) return {};
    if (!deriveKey()) return {};

    // Format: ciphertext || IV (12B) || tag (16B)
    size_t ciphertextLen = length - IV_LENGTH - TAG_LENGTH;
    const uint8_t* ciphertext = data;
    const uint8_t* iv = data + ciphertextLen;
    const uint8_t* tag = data + ciphertextLen + IV_LENGTH;

    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);
    mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, aesKey_, KEY_LENGTH * 8);

    std::vector<uint8_t> plaintext(ciphertextLen);

    int ret = mbedtls_gcm_auth_decrypt(
        &gcm,
        ciphertextLen, iv, IV_LENGTH,
        nullptr, 0,
        tag, TAG_LENGTH,
        ciphertext, plaintext.data()
    );

    mbedtls_gcm_free(&gcm);
    memset(aesKey_, 0, KEY_LENGTH);

    if (ret != 0) {
        // SECURITY: Tamper detected — auth tag mismatch
        plaintext.clear();
        return plaintext;
    }

    return plaintext;
}

bool CredentialManager::load() {
    routerIp_ = nvs_.loadString(NVS_KEY_IP);
    routerUsername_ = nvs_.loadString(NVS_KEY_USER);

    std::string encryptedPass = nvs_.loadString(NVS_KEY_PASS);
    if (!encryptedPass.empty()) {
        std::vector<uint8_t> raw(
            reinterpret_cast<const uint8_t*>(encryptedPass.data()),
            reinterpret_cast<const uint8_t*>(encryptedPass.data()) + encryptedPass.size()
        );
        std::vector<uint8_t> decrypted = decrypt(raw.data(), raw.size());
        if (decrypted.empty()) {
            // SECURITY: Decryption failed — possible tamper
            nvs_.remove(NVS_KEY_IP);
            nvs_.remove(NVS_KEY_USER);
            nvs_.remove(NVS_KEY_PASS);
            nvs_.commit();
            routerIp_.clear();
            routerUsername_.clear();
            routerPassword_.clear();
            return false;
        }
        routerPassword_.assign(reinterpret_cast<const char*>(decrypted.data()), decrypted.size());
    }

    return hasStoredCredentials();
}

bool CredentialManager::save() {
    if (routerPassword_.empty()) return false;

    std::vector<uint8_t> encrypted = encrypt(
        reinterpret_cast<const uint8_t*>(routerPassword_.data()),
        routerPassword_.size()
    );

    if (encrypted.empty()) return false;

    std::string encryptedStr(reinterpret_cast<const char*>(encrypted.data()), encrypted.size());

    bool ok = true;
    ok &= nvs_.saveString(NVS_KEY_IP, routerIp_);
    ok &= nvs_.saveString(NVS_KEY_USER, routerUsername_);
    ok &= nvs_.saveString(NVS_KEY_PASS, encryptedStr);
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
