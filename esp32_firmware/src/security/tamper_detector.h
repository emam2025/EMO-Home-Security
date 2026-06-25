#pragma once

#include <cstdint>
#include <functional>
#include <string>

enum class TamperEvent {
    ROUTER_RESTARTED,
    ROUTER_CREDENTIALS_FAILED,
    ROUTER_UNREACHABLE,
    MQTT_CONNECTION_LOST,
    EMO_OFFLINE
};

using TamperCallback = std::function<void(TamperEvent event, const std::string& detail)>;

class TamperDetector {
public:
    TamperDetector();

    void setCallback(TamperCallback callback);

    void reportRouterLoginResult(bool success);
    void reportRouterUptime(uint32_t uptimeSec);
    void reportMqttConnected(bool connected);

    void loop();

private:
    TamperCallback callback_;

    uint32_t lastCheckMs_;
    uint32_t knownUptimeSec_;
    bool hadValidUptime_;
    int consecutiveLoginFailures_;
    bool lastMqttConnected_;
    uint32_t mqttDisconnectStartMs_;
    bool routerUnreachableReported_;

    static constexpr uint32_t CHECK_INTERVAL_MS = 10000;
    static constexpr int MAX_LOGIN_FAILURES = 3;
    static constexpr uint32_t ROUTER_UNREACHABLE_TIMEOUT_MS = 120000;
    static constexpr uint32_t MQTT_OFFLINE_ALERT_MS = 60000;

    void checkRouterHealth();
    void checkMqttHealth();
    void fire(TamperEvent event, const std::string& detail);
};
