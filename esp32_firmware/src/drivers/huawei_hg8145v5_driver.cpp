#include "huawei_hg8145v5_driver.h"
#include <ArduinoJson.h>

HuaweiHG8145V5Driver::HuaweiHG8145V5Driver(
    const std::string& ip,
    const std::string& username,
    const std::string& password
) : ip_(ip), username_(username), password_(password), authenticated_(false) {}

void HuaweiHG8145V5Driver::setCredentials(
    const std::string& ip,
    const std::string& username,
    const std::string& password
) {
    ip_ = ip;
    username_ = username;
    password_ = password;
    authenticated_ = false;
    sessionCookie_.clear();
    csrfToken_.clear();
}

std::string HuaweiHG8145V5Driver::buildUrl(const std::string& endpoint) const {
    return "http://" + ip_ + "/" + endpoint;
}

bool HuaweiHG8145V5Driver::httpPost(
    const std::string& url, const std::string& data,
    std::string& response, int maxRetries
) {
    for (int attempt = 0; attempt < maxRetries; ++attempt) {
        httpClient_.setTimeout(HTTP_TIMEOUT_MS);
        httpClient_.begin(wifiClient_, url.c_str());

        if (!sessionCookie_.empty()) {
            httpClient_.addHeader("Cookie", sessionCookie_.c_str());
        }
        if (!csrfToken_.empty()) {
            httpClient_.addHeader("X-CSRF-Token", csrfToken_.c_str());
        }

        int httpCode = httpClient_.POST(data.c_str());
        if (httpCode > 0) {
            response = httpClient_.getString().c_str();

            std::string headers = httpClient_.header("Set-Cookie").c_str();
            if (!headers.empty()) {
                sessionCookie_ = headers;
            }

            httpClient_.end();
            return true;
        }

        httpClient_.end();
        if (attempt < maxRetries - 1) {
            delay(RETRY_DELAY_MS);
        }
    }
    return false;
}

bool HuaweiHG8145V5Driver::httpGet(
    const std::string& url, std::string& response, int maxRetries
) {
    for (int attempt = 0; attempt < maxRetries; ++attempt) {
        httpClient_.setTimeout(HTTP_TIMEOUT_MS);
        httpClient_.begin(wifiClient_, url.c_str());

        if (!sessionCookie_.empty()) {
            httpClient_.addHeader("Cookie", sessionCookie_.c_str());
        }
        if (!csrfToken_.empty()) {
            httpClient_.addHeader("X-CSRF-Token", csrfToken_.c_str());
        }

        int httpCode = httpClient_.GET();
        if (httpCode > 0) {
            response = httpClient_.getString().c_str();

            std::string headers = httpClient_.header("Set-Cookie").c_str();
            if (!headers.empty()) {
                sessionCookie_ = headers;
            }

            httpClient_.end();
            return true;
        }

        httpClient_.end();
        if (attempt < maxRetries - 1) {
            delay(RETRY_DELAY_MS);
        }
    }
    return false;
}

bool HuaweiHG8145V5Driver::hasSessionCookie(const std::string& responseHeaders) {
    return responseHeaders.find("SessionID=") != std::string::npos ||
           responseHeaders.find("Cookie=") != std::string::npos;
}

std::string HuaweiHG8145V5Driver::extractToken(
    const std::string& body, const std::string& tokenName
) {
    std::string search = "name=\"" + tokenName + "\" value=\"";
    size_t start = body.find(search);
    if (start == std::string::npos) return "";

    start += search.length();
    size_t end = body.find("\"", start);
    if (end == std::string::npos) return "";

    return body.substr(start, end - start);
}

bool HuaweiHG8145V5Driver::login() {
    std::string response;
    std::string postData = "username=" + username_ + "&password=" + password_;

    if (!httpPost(buildUrl(ENDPOINT_LOGIN), postData, response)) {
        authenticated_ = false;
        return false;
    }

    csrfToken_ = extractToken(response, "csrf_token");
    authenticated_ = true;
    return true;
}

bool HuaweiHG8145V5Driver::logout() {
    if (!authenticated_) return true;

    std::string response;
    bool result = httpGet(buildUrl("logout.cgi"), response);
    authenticated_ = false;
    sessionCookie_.clear();
    csrfToken_.clear();
    return result;
}

std::vector<RouterDevice> HuaweiHG8145V5Driver::getDevices() {
    std::vector<RouterDevice> devices;

    if (!authenticated_ && !login()) {
        return devices;
    }

    std::string response;
    if (!httpGet(buildUrl(ENDPOINT_DEVICES), response)) {
        return devices;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, response);
    if (error) {
        return devices;
    }

    JsonArray arr = doc["devices"].as<JsonArray>();
    for (JsonObject obj : arr) {
        RouterDevice dev;
        dev.mac = obj["mac"].as<std::string>();
        dev.ip = obj["ip"].as<std::string>();
        dev.hostname = obj["hostname"].as<std::string>();
        dev.vendor = obj["vendor"].as<std::string>();
        dev.isOnline = obj["online"].as<bool>();
        devices.push_back(dev);
    }

    return devices;
}

bool HuaweiHG8145V5Driver::blockDevice(const std::string& mac) {
    if (!authenticated_ && !login()) return false;

    std::string postData = "action=add&mac=" + mac + "&filter=block";
    std::string response;
    return httpPost(buildUrl(ENDPOINT_MAC_FILTER), postData, response);
}

bool HuaweiHG8145V5Driver::unblockDevice(const std::string& mac) {
    if (!authenticated_ && !login()) return false;

    std::string postData = "action=del&mac=" + mac;
    std::string response;
    return httpPost(buildUrl(ENDPOINT_MAC_FILTER), postData, response);
}

bool HuaweiHG8145V5Driver::setDNS(
    const std::string& primary, const std::string& secondary
) {
    if (!authenticated_ && !login()) return false;

    std::string postData = "primary=" + primary + "&secondary=" + secondary;
    std::string response;
    return httpPost(buildUrl(ENDPOINT_DNS), postData, response);
}

bool HuaweiHG8145V5Driver::enableWhitelist() {
    if (!authenticated_ && !login()) return false;

    std::string postData = "action=enable&mode=whitelist";
    std::string response;
    return httpPost(buildUrl(ENDPOINT_MAC_FILTER), postData, response);
}

bool HuaweiHG8145V5Driver::disableWhitelist() {
    if (!authenticated_ && !login()) return false;

    std::string postData = "action=disable";
    std::string response;
    return httpPost(buildUrl(ENDPOINT_MAC_FILTER), postData, response);
}

RouterStatistics HuaweiHG8145V5Driver::getStatistics() {
    RouterStatistics stats = {0, 0, 0};

    if (!authenticated_ && !login()) return stats;

    std::string response;
    if (!httpGet(buildUrl(ENDPOINT_DEVICES), response)) return stats;

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, response);
    if (error) return stats;

    auto devices = getDevices();
    stats.totalDevices = devices.size();
    for (const auto& dev : devices) {
        if (dev.isOnline) stats.onlineDevices++;
    }
    stats.uptime = doc["uptime"].as<uint32_t>();

    return stats;
}

bool HuaweiHG8145V5Driver::reboot() {
    if (!authenticated_ && !login()) return false;

    std::string response;
    bool result = httpGet(buildUrl("reboot.cgi"), response);
    authenticated_ = false;
    return result;
}
