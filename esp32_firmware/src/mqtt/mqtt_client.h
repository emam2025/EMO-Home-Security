#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

class MqttClient {
public:
    using CommandCallback = std::function<void(const std::string& command, const std::string& payload)>;
    using ConnectCallback = std::function<void(bool connected)>;

    MqttClient();

    void setDeviceId(const std::string& deviceId);
    void setBroker(const std::string& host, uint16_t port);
    void setCredentials(const std::string& username, const std::string& password);
    void setCaCert(const char* caCert);  // PEM string, nullptr = insecure

    void onCommand(CommandCallback callback);
    void onConnect(ConnectCallback callback);

    bool connect();
    void disconnect();
    bool isConnected() const;

    void loop();

    bool publishRaw(const std::string& topic, const std::string& payload, bool retained = false);
    bool publishStatus(const std::string& payload, bool retained = true);
    bool publishEvent(const std::string& payload);
    bool publishUsage(const std::string& payload);
    bool publishAlert(const std::string& payload);

    void heartbeat();

private:
    static void mqttCallback(char* topic, uint8_t* payload, unsigned int length);
    void handleMessage(const std::string& topic, const std::string& payload);
    std::string topic(const std::string& pattern) const;

    std::string deviceId_;
    std::string host_;
    uint16_t port_;
    std::string username_;
    std::string password_;
    const char* caCert_;

    WiFiClientSecure wifiClient_;
    PubSubClient mqttClient_;

    CommandCallback commandCallback_;
    ConnectCallback connectCallback_;

    uint32_t reconnectBackoffMs_;
    uint32_t lastHeartbeatMs_;
    uint32_t lastReconnectAttemptMs_;
    bool wasConnected_;

    static MqttClient* instance_;
};
