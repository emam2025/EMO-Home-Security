#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <map>

struct TimeWindow {
    uint8_t startHour;
    uint8_t startMin;
    uint8_t endHour;
    uint8_t endMin;
};

struct DataQuota {
    uint64_t dailyBytes;
    uint64_t usedBytes;
    uint32_t lastResetDay;
};

struct Profile {
    std::string id;
    std::string name;
    std::vector<TimeWindow> allowedWindows;
    DataQuota quota;
    std::vector<std::string> deviceMacs;
    bool isActive;
};

struct PolicyAction {
    enum Type { BLOCK, UNBLOCK, NONE };
    Type type;
    std::string deviceMac;
    std::string profileId;
    std::string reason;
};

class IRouterDriver;

class PolicyEngine {
public:
    using PolicyUpdateCallback = std::function<void(const std::vector<Profile>&)>;

    PolicyEngine(IRouterDriver* driver);

    void addProfile(const Profile& profile);
    void removeProfile(const std::string& profileId);
    void updateProfile(const Profile& profile);
    const Profile* getProfile(const std::string& profileId) const;
    std::vector<Profile> getAllProfiles() const;

    std::vector<PolicyAction> evaluate();
    bool shouldBlock(const std::string& profileId);
    bool applyActions(const std::vector<PolicyAction>& actions);

    void updatePolicies(const std::string& jsonPayload);
    void setPolicyUpdateCallback(PolicyUpdateCallback callback);

    void clear();

private:
    bool isInAllowedWindow(const TimeWindow& window) const;
    bool isQuotaExhausted(const DataQuota& quota) const;
    void resetQuotaIfNeeded(DataQuota& quota);

    IRouterDriver* driver_;
    std::map<std::string, Profile> profiles_;
    PolicyUpdateCallback policyUpdateCallback_;
};
