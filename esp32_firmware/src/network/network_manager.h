#pragma once
#include <cstdint>
#include <string>
#include <functional>

enum class NetworkType {
    NONE,
    ETHERNET,
    WIFI
};

class NetworkManager {
public:
    using ConnectCallback = std::function<void(bool success, NetworkType type)>;

    NetworkManager();
    
    bool begin();
    void loop();
    
    bool isConnected() const;
    NetworkType getActiveType() const;
    std::string getLocalIP() const;
    std::string getMacAddress() const;
    
    void setConnectCallback(ConnectCallback callback);
    
    void disconnect();
    bool reconnect();

private:
    void connectWiFi();
    void connectEthernet();
    void checkConnectivity();
    void onConnected(NetworkType type);

    NetworkType activeType_;
    NetworkType preferredType_;
    ConnectCallback connectCallback_;
    
    uint32_t lastReconnectAttemptMs_;
    uint32_t lastConnectivityCheckMs_;
    bool initialized_;
    bool ethernetAvailable_;
};
