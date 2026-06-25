#pragma once

#include <cstdint>

namespace config {

// Device identity
extern const char* DEVICE_ID;
extern const char* DEVICE_MODEL;
extern const char* FIRMWARE_VERSION;

// WiFi
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;
constexpr uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;
constexpr uint32_t WIFI_RECONNECT_INTERVAL_MS = 30000;

// Ethernet (W5500)
constexpr bool ETHERNET_ENABLED = true;
constexpr uint8_t ETH_CS_PIN = 5;
constexpr uint8_t ETH_RST_PIN = 14;
constexpr uint8_t ETH_IRQ_PIN = 4;

// MQTT
extern const char* MQTT_BROKER_HOST;
constexpr uint16_t MQTT_BROKER_PORT = 8883;
extern const char* MQTT_USERNAME;
extern const char* MQTT_PASSWORD;
constexpr bool MQTT_TLS_ENABLED = true;

// Topics (deviceId appended at runtime)
constexpr const char* MQTT_TOPIC_STATUS = "emo/%s/status";
constexpr const char* MQTT_TOPIC_EVENTS = "emo/%s/events";
constexpr const char* MQTT_TOPIC_ALERTS = "emo/%s/alerts";
constexpr const char* MQTT_TOPIC_COMMANDS = "emo/%s/commands";
constexpr const char* MQTT_TOPIC_POLICIES = "emo/%s/policies";
constexpr const char* MQTT_TOPIC_REGISTER = "emo/register";
constexpr const char* MQTT_TOPIC_USAGE = "emo/%s/usage";

// Serial
constexpr uint32_t MONITOR_SPEED = 115200;

// Timing (ms)
constexpr uint32_t HEARTBEAT_MS = 300000;  // SECURITY: Reduced from 30s to 300s to cut MQTT traffic by 90%
constexpr uint32_t DEVICE_POLL_MS = 60000;
constexpr uint32_t POLICY_CHECK_MS = 60000;
constexpr uint32_t USAGE_REPORT_MS = 300000; // 5 min
constexpr uint32_t MQTT_RECONNECT_BASE_MS = 1000;
constexpr uint32_t MQTT_RECONNECT_MAX_MS = 60000;

// Router defaults
extern const char* ROUTER_DEFAULT_IP;
extern const char* ROUTER_DEFAULT_USERNAME;
extern const char* ROUTER_DEFAULT_PASSWORD;

// NVS
constexpr const char* NVS_NAMESPACE = "emo-storage";
constexpr const char* NVS_KEY_DEVICE_ID = "device_id";
constexpr const char* NVS_KEY_PAIRING_CODE = "pairing_code";
constexpr const char* NVS_KEY_REGISTERED = "registered";

} // namespace config
