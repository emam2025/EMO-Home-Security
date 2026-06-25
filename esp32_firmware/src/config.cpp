#include "config.h"

namespace config {

const char* DEVICE_ID = nullptr;                // Loaded from NVS at runtime
const char* DEVICE_MODEL = "EMO-ESP32-V1";
const char* FIRMWARE_VERSION = "1.0.0";

const char* WIFI_SSID = "EMO_Home_Security";
const char* WIFI_PASSWORD = "change_me";

const char* MQTT_BROKER_HOST = "broker.emo-home.local";
const char* MQTT_USERNAME = "";
const char* MQTT_PASSWORD = "";

const char* ROUTER_DEFAULT_IP = "192.168.1.1";
const char* ROUTER_DEFAULT_USERNAME = "admin";
const char* ROUTER_DEFAULT_PASSWORD = "admin";

} // namespace config
