#pragma once

#include "IRouterDriver.h"
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <map>

class HuaweiHG8145V5Driver : public IRouterDriver {
public:
    explicit HuaweiHG8145V5Driver(
        const std::string& ip = "192.168.1.1",
        const std::string& username = "admin",
        const std::string& password = "admin"
    );

    bool login() override;
    bool logout() override;

    std::vector<RouterDevice> getDevices() override;
    bool blockDevice(const std::string& mac) override;
    bool unblockDevice(const std::string& mac) override;

    bool setDNS(const std::string& primary, const std::string& secondary) override;
    bool enableWhitelist() override;
    bool disableWhitelist() override;

    RouterStatistics getStatistics() override;
    bool reboot() override;

    bool isAuthenticated() const { return authenticated_; }
    void setCredentials(const std::string& ip, const std::string& username, const std::string& password);

private:
    std::string buildUrl(const std::string& endpoint) const;
    bool httpPost(const std::string& url, const std::string& data, std::string& response, int maxRetries = 3);
    bool httpGet(const std::string& url, std::string& response, int maxRetries = 3);
    bool hasSessionCookie(const std::string& responseHeaders);
    std::string extractToken(const std::string& body, const std::string& tokenName);

    std::string ip_;
    std::string username_;
    std::string password_;
    std::string sessionCookie_;
    std::string csrfToken_;
    bool authenticated_;

    WiFiClient wifiClient_;
    HTTPClient httpClient_;

    static constexpr int HTTP_TIMEOUT_MS = 10000;
    static constexpr int RETRY_DELAY_MS = 1000;

    static constexpr const char* ENDPOINT_LOGIN = "login.cgi";
    static constexpr const char* ENDPOINT_DEVICES = "deviceInformation";
    static constexpr const char* ENDPOINT_MAC_FILTER = "wlmacflt.cgi";
    static constexpr const char* ENDPOINT_DNS = "dnsset.cgi";
};
