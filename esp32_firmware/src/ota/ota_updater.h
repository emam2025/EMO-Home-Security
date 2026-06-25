#pragma once

#include <cstdint>
#include <string>
#include <functional>

using OtaProgressCallback = std::function<void(int progress, int total)>;
using OtaResultCallback = std::function<void(bool success, const std::string& message)>;

enum class OtaState {
    IDLE,
    DOWNLOADING,
    APPLYING,
    REBOOTING,
    FAILED
};

class OtaUpdater {
public:
    OtaUpdater();

    void setCallbacks(OtaProgressCallback onProgress, OtaResultCallback onResult);

    bool startUpdate(const std::string& firmwareUrl, const std::string& expectedVersion);

    void loop();

    OtaState getState() const;
    bool isUpdating() const;

private:
    OtaState state_;
    std::string firmwareUrl_;
    std::string expectedVersion_;
    uint32_t lastProgressMs_;

    OtaProgressCallback progressCallback_;
    OtaResultCallback resultCallback_;

    static constexpr uint32_t PROGRESS_INTERVAL_MS = 500;

    bool performUpdate();
};
