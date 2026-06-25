#include "policy_engine.h"
#include "IRouterDriver.h"
#include <ArduinoJson.h>
#include <Arduino.h>

PolicyEngine::PolicyEngine(IRouterDriver* driver)
    : driver_(driver) {}

void PolicyEngine::addProfile(const Profile& profile) {
    profiles_[profile.id] = profile;
}

void PolicyEngine::removeProfile(const std::string& profileId) {
    profiles_.erase(profileId);
}

void PolicyEngine::updateProfile(const Profile& profile) {
    profiles_[profile.id] = profile;
}

const Profile* PolicyEngine::getProfile(const std::string& profileId) const {
    auto it = profiles_.find(profileId);
    if (it != profiles_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Profile> PolicyEngine::getAllProfiles() const {
    std::vector<Profile> result;
    result.reserve(profiles_.size());
    for (const auto& pair : profiles_) {
        result.push_back(pair.second);
    }
    return result;
}

bool PolicyEngine::isInAllowedWindow(const TimeWindow& window) const {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);

    uint8_t currentHour = timeinfo->tm_hour;
    uint8_t currentMin = timeinfo->tm_min;

    uint16_t current = currentHour * 60 + currentMin;
    uint16_t start = window.startHour * 60 + window.startMin;
    uint16_t end = window.endHour * 60 + window.endMin;

    if (start <= end) {
        return current >= start && current < end;
    }
    return current >= start || current < end;
}

bool PolicyEngine::isQuotaExhausted(const DataQuota& quota) const {
    if (quota.dailyBytes == 0) return false;
    return quota.usedBytes >= quota.dailyBytes;
}

void PolicyEngine::resetQuotaIfNeeded(DataQuota& quota) {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    uint32_t currentDay = timeinfo->tm_yday;

    if (currentDay != quota.lastResetDay) {
        quota.usedBytes = 0;
        quota.lastResetDay = currentDay;
    }
}

bool PolicyEngine::shouldBlock(const std::string& profileId) {
    auto it = profiles_.find(profileId);
    if (it == profiles_.end() || !it->second.isActive) return false;
    Profile& profile = it->second;

    if (profile.allowedWindows.empty() && profile.quota.dailyBytes == 0) {
        return false;
    }

    bool inWindow = false;
    for (const auto& window : profile.allowedWindows) {
        if (isInAllowedWindow(window)) {
            inWindow = true;
            break;
        }
    }

    resetQuotaIfNeeded(profile.quota);
    bool quotaExhausted = isQuotaExhausted(profile.quota);

    if (!profile.allowedWindows.empty() && profile.quota.dailyBytes == 0) {
        return !inWindow;
    }

    return !inWindow || quotaExhausted;
}

std::vector<PolicyAction> PolicyEngine::evaluate() {
    std::vector<PolicyAction> actions;

    for (auto& pair : profiles_) {
        Profile& profile = pair.second;
        if (!profile.isActive) continue;

        resetQuotaIfNeeded(profile.quota);
        bool block = shouldBlock(profile.id);

        for (const auto& mac : profile.deviceMacs) {
            PolicyAction action;
            action.deviceMac = mac;
            action.profileId = profile.id;

            if (block) {
                action.type = PolicyAction::BLOCK;
                action.reason = "Outside schedule or quota exhausted";
            } else {
                action.type = PolicyAction::UNBLOCK;
                action.reason = "Within allowed window and quota available";
            }

            actions.push_back(action);
        }
    }

    return actions;
}

bool PolicyEngine::applyActions(const std::vector<PolicyAction>& actions) {
    if (!driver_) return false;

    bool allSuccess = true;
    for (const auto& action : actions) {
        bool result = false;
        switch (action.type) {
            case PolicyAction::BLOCK:
                result = driver_->blockDevice(action.deviceMac);
                break;
            case PolicyAction::UNBLOCK:
                result = driver_->unblockDevice(action.deviceMac);
                break;
            case PolicyAction::NONE:
                result = true;
                break;
        }
        if (!result) allSuccess = false;
    }

    return allSuccess;
}

void PolicyEngine::updatePolicies(const std::string& jsonPayload) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonPayload);
    if (error) return;

    JsonArray arr = doc["profiles"].as<JsonArray>();
    for (JsonObject obj : arr) {
        Profile profile;
        profile.id = obj["id"].as<std::string>();
        profile.name = obj["name"].as<std::string>();
        profile.isActive = obj["is_active"].as<bool>();

        JsonArray windows = obj["allowed_windows"].as<JsonArray>();
        for (JsonObject win : windows) {
            TimeWindow tw;
            tw.startHour = win["start_hour"].as<uint8_t>();
            tw.startMin = win["start_min"].as<uint8_t>();
            tw.endHour = win["end_hour"].as<uint8_t>();
            tw.endMin = win["end_min"].as<uint8_t>();
            profile.allowedWindows.push_back(tw);
        }

        profile.quota.dailyBytes = obj["quota"]["daily_bytes"].as<uint64_t>();
        profile.quota.usedBytes = obj["quota"]["used_bytes"].as<uint64_t>();
        profile.quota.lastResetDay = obj["quota"]["last_reset_day"].as<uint32_t>();

        JsonArray macs = obj["device_macs"].as<JsonArray>();
        for (const char* mac : macs) {
            profile.deviceMacs.push_back(mac);
        }

        profiles_[profile.id] = profile;
    }

    if (policyUpdateCallback_) {
        policyUpdateCallback_(getAllProfiles());
    }
}

void PolicyEngine::setPolicyUpdateCallback(PolicyUpdateCallback callback) {
    policyUpdateCallback_ = callback;
}

void PolicyEngine::clear() {
    profiles_.clear();
}
