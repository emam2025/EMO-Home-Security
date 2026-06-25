#include "tamper_detector.h"
#include <Arduino.h>

TamperDetector::TamperDetector()
    : lastCheckMs_(0)
    , knownUptimeSec_(0)
    , hadValidUptime_(false)
    , consecutiveLoginFailures_(0)
    , lastMqttConnected_(false)
    , mqttDisconnectStartMs_(0)
    , routerUnreachableReported_(false)
{}

void TamperDetector::setCallback(TamperCallback callback) {
    callback_ = callback;
}

void TamperDetector::reportRouterLoginResult(bool success) {
    if (success) {
        if (consecutiveLoginFailures_ >= MAX_LOGIN_FAILURES) {
            Serial.println("[Tamper] Router login recovered after failures");
        }
        consecutiveLoginFailures_ = 0;
        routerUnreachableReported_ = false;
    } else {
        consecutiveLoginFailures_++;
        if (consecutiveLoginFailures_ >= MAX_LOGIN_FAILURES && !routerUnreachableReported_) {
            fire(TamperEvent::ROUTER_CREDENTIALS_FAILED, "Router login failed " + String(consecutiveLoginFailures_) + " times");
        }
    }
}

void TamperDetector::reportRouterUptime(uint32_t uptimeSec) {
    if (uptimeSec > 0) {
        if (hadValidUptime_ && uptimeSec < knownUptimeSec_ - 60) {
            fire(TamperEvent::ROUTER_RESTARTED,
                "Router uptime dropped from " + String(knownUptimeSec_) + "s to " + String(uptimeSec_) + "s");
        }
        knownUptimeSec_ = uptimeSec;
        hadValidUptime_ = true;
    }
}

void TamperDetector::reportMqttConnected(bool connected) {
    if (!connected && lastMqttConnected_) {
        mqttDisconnectStartMs_ = millis();
    }
    if (connected && !lastMqttConnected_) {
        mqttDisconnectStartMs_ = 0;
    }
    lastMqttConnected_ = connected;
}

void TamperDetector::loop() {
    uint32_t now = millis();
    if (now - lastCheckMs_ < CHECK_INTERVAL_MS) return;
    lastCheckMs_ = now;

    checkRouterHealth();
    checkMqttHealth();
}

void TamperDetector::checkRouterHealth() {
    if (consecutiveLoginFailures_ >= MAX_LOGIN_FAILURES) {
        uint32_t elapsed = millis() - mqttDisconnectStartMs_;
        if (elapsed >= ROUTER_UNREACHABLE_TIMEOUT_MS && !routerUnreachableReported_) {
            fire(TamperEvent::ROUTER_UNREACHABLE, "Router unreachable for " + String(elapsed / 1000) + "s");
            routerUnreachableReported_ = true;
        }
    }
}

void TamperDetector::checkMqttHealth() {
    if (!lastMqttConnected_ && mqttDisconnectStartMs_ > 0) {
        uint32_t elapsed = millis() - mqttDisconnectStartMs_;
        if (elapsed >= MQTT_OFFLINE_ALERT_MS) {
            fire(TamperEvent::MQTT_CONNECTION_LOST, "MQTT offline for " + String(elapsed / 1000) + "s");
        }
    }
}

void TamperDetector::fire(TamperEvent event, const std::string& detail) {
    if (callback_) {
        callback_(event, detail);
    }
}
