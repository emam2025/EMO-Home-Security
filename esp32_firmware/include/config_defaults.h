#pragma once

enum class RouterModel {
    UNKNOWN,
    HG8145V5,
    HG8245,
    ZTE_H188A
};

enum class DnsProvider {
    DNS_DEFAULT,
    CLOUDFLARE_FAMILY,
    OPENDNS_FAMILY
};

enum class DeviceStatus {
    PENDING,
    APPROVED,
    BLOCKED,
    UNKNOWN
};

namespace dns_presets {
    constexpr const char* CLOUDFLARE_PRIMARY = "1.1.1.3";
    constexpr const char* CLOUDFLARE_SECONDARY = "1.0.0.3";
    constexpr const char* OPENDNS_PRIMARY = "208.67.222.123";
    constexpr const char* OPENDNS_SECONDARY = "208.67.220.123";
}
