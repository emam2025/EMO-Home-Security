#include <Arduino.h>
#include <WiFi.h>
#include <ETH.h>

#include "config.h"
#include "network_manager.h"

NetworkManager::NetworkManager()
    : activeType_(NetworkType::NONE)
    , preferredType_(NetworkType::NONE)
    , connectCallback_(nullptr)
    , lastReconnectAttemptMs_(0)
    , lastConnectivityCheckMs_(0)
    , initialized_(false)
    , ethernetAvailable_(false)
{}

bool NetworkManager::begin() {
    if (config::ETHERNET_ENABLED) {
        ETH.begin(config::ETH_CS_PIN, config::ETH_RST_PIN, config::ETH_IRQ_PIN);
        preferredType_ = NetworkType::ETHERNET;
        Serial.printf("[Network] Ethernet init (CS:%d, RST:%d, IRQ:%d)\n",
            config::ETH_CS_PIN, config::ETH_RST_PIN, config::ETH_IRQ_PIN);
    } else {
        connectWiFi();
    }

    lastConnectivityCheckMs_ = millis();
    lastReconnectAttemptMs_ = millis();
    initialized_ = true;
    return true;
}

void NetworkManager::loop() {
    if (!initialized_) return;

    uint32_t now = millis();

    if (now - lastConnectivityCheckMs_ >= 5000) {
        lastConnectivityCheckMs_ = now;
        checkConnectivity();
    }

    if (activeType_ != NetworkType::NONE) return;

    if (preferredType_ == NetworkType::ETHERNET && ETH.linkUp()) {
        onConnected(NetworkType::ETHERNET);
        return;
    }

    if (preferredType_ == NetworkType::WIFI && WiFi.status() == WL_CONNECTED) {
        onConnected(NetworkType::WIFI);
        return;
    }

    uint32_t elapsed = now - lastReconnectAttemptMs_;

    if (preferredType_ == NetworkType::ETHERNET && elapsed >= config::WIFI_CONNECT_TIMEOUT_MS) {
        Serial.printf("[Network] Ethernet timeout, falling back to WiFi\n");
        connectWiFi();
        return;
    }

    if (elapsed >= config::WIFI_RECONNECT_INTERVAL_MS) {
        reconnect();
    }
}

bool NetworkManager::isConnected() const {
    return activeType_ != NetworkType::NONE;
}

NetworkType NetworkManager::getActiveType() const {
    return activeType_;
}

std::string NetworkManager::getLocalIP() const {
    if (activeType_ == NetworkType::ETHERNET) {
        return ETH.localIP().toString().c_str();
    }
    if (activeType_ == NetworkType::WIFI) {
        return WiFi.localIP().toString().c_str();
    }
    return "0.0.0.0";
}

std::string NetworkManager::getMacAddress() const {
    if (activeType_ == NetworkType::ETHERNET) {
        return ETH.macAddress().c_str();
    }
    if (activeType_ == NetworkType::WIFI) {
        return WiFi.macAddress().c_str();
    }
    return "00:00:00:00:00:00";
}

void NetworkManager::setConnectCallback(ConnectCallback callback) {
    connectCallback_ = callback;
}

void NetworkManager::disconnect() {
    if (activeType_ == NetworkType::WIFI) {
        WiFi.disconnect(true);
    }
    activeType_ = NetworkType::NONE;
    Serial.printf("[Network] Disconnected\n");
}

bool NetworkManager::reconnect() {
    if (activeType_ != NetworkType::NONE) {
        disconnect();
    }

    if (config::ETHERNET_ENABLED) {
        connectEthernet();
    } else {
        connectWiFi();
    }
    return true;
}

void NetworkManager::connectWiFi() {
    Serial.printf("[Network] Connecting to WiFi SSID: %s\n", config::WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(config::WIFI_SSID, config::WIFI_PASSWORD);
    preferredType_ = NetworkType::WIFI;
    lastReconnectAttemptMs_ = millis();
}

void NetworkManager::connectEthernet() {
    if (!config::ETHERNET_ENABLED) return;
    Serial.printf("[Network] Initializing Ethernet\n");
    ETH.begin(config::ETH_CS_PIN, config::ETH_RST_PIN, config::ETH_IRQ_PIN);
    preferredType_ = NetworkType::ETHERNET;
    lastReconnectAttemptMs_ = millis();
}

void NetworkManager::checkConnectivity() {
    if (activeType_ == NetworkType::ETHERNET) {
        if (!ETH.linkUp()) {
            Serial.printf("[Network] Ethernet link lost\n");
            activeType_ = NetworkType::NONE;
        }
        return;
    }

    if (activeType_ == NetworkType::WIFI) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.printf("[Network] WiFi disconnected (status: %d)\n", WiFi.status());
            activeType_ = NetworkType::NONE;
        }
        return;
    }
}

void NetworkManager::onConnected(NetworkType type) {
    activeType_ = type;
    Serial.printf("[Network] Connected via %s\n",
        type == NetworkType::ETHERNET ? "Ethernet" : "WiFi");
    Serial.printf("[Network] IP: %s\n", getLocalIP().c_str());
    Serial.printf("[Network] MAC: %s\n", getMacAddress().c_str());

    if (connectCallback_) {
        connectCallback_(true, type);
    }
}
