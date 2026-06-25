#include "ota_updater.h"
#include <Arduino.h>
#include <Update.h>
#include <WiFi.h>
#include <HTTPClient.h>

OtaUpdater::OtaUpdater()
    : state_(OtaState::IDLE)
    , lastProgressMs_(0)
{}

void OtaUpdater::setCallbacks(OtaProgressCallback onProgress, OtaResultCallback onResult) {
    progressCallback_ = onProgress;
    resultCallback_ = onResult;
}

bool OtaUpdater::startUpdate(const std::string& firmwareUrl, const std::string& expectedVersion) {
    if (state_ != OtaState::IDLE) return false;

    if (!WiFi.isConnected()) {
        if (resultCallback_) resultCallback_(false, "No WiFi connection");
        state_ = OtaState::FAILED;
        return false;
    }

    firmwareUrl_ = firmwareUrl;
    expectedVersion_ = expectedVersion;
    state_ = OtaState::DOWNLOADING;
    return true;
}

void OtaUpdater::loop() {
    if (state_ != OtaState::DOWNLOADING && state_ != OtaState::APPLYING) return;

    if (state_ == OtaState::DOWNLOADING) {
        performUpdate();
    }
}

bool OtaUpdater::performUpdate() {
    state_ = OtaState::APPLYING;

    HTTPClient http;
    http.setTimeout(30000);
    http.begin(firmwareUrl_.c_str());

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        if (resultCallback_) resultCallback_(false, std::string("HTTP ") + std::to_string(httpCode));
        state_ = OtaState::FAILED;
        http.end();
        return false;
    }

    int totalSize = http.getSize();
    if (totalSize <= 0) {
        if (resultCallback_) resultCallback_(false, "Invalid content length");
        state_ = OtaState::FAILED;
        http.end();
        return false;
    }

    if (!Update.begin(totalSize, U_FLASH)) {
        if (resultCallback_) resultCallback_(false, std::string("Not enough space: ") + std::string(Update.errorString()));
        state_ = OtaState::FAILED;
        http.end();
        return false;
    }

    WiFiClient* stream = http.getStreamPtr();
    uint8_t buf[256];
    int written = 0;

    while (http.connected() && written < totalSize) {
        size_t available = stream->available();
        if (available > 0) {
            int bytesRead = stream->readBytes(buf, std::min((size_t)sizeof(buf), available));
            size_t bytesWritten = Update.write(buf, bytesRead);
            if (bytesWritten != bytesRead) {
                if (resultCallback_) resultCallback_(false, std::string("Write error: ") + std::string(Update.errorString()));
                Update.end();
                state_ = OtaState::FAILED;
                http.end();
                return false;
            }
            written += bytesWritten;

            uint32_t now = millis();
            if (now - lastProgressMs_ >= PROGRESS_INTERVAL_MS) {
                lastProgressMs_ = now;
                if (progressCallback_) progressCallback_(written, totalSize);
            }
        }
    }

    http.end();

    if (!Update.end()) {
        if (resultCallback_) resultCallback_(false, std::string("Update finalize failed: ") + std::string(Update.errorString()));
        state_ = OtaState::FAILED;
        return false;
    }

    if (resultCallback_) resultCallback_(true, "Update complete, rebooting");
    state_ = OtaState::REBOOTING;
    delay(500);
    ESP.restart();
    return true;
}

OtaState OtaUpdater::getState() const {
    return state_;
}

bool OtaUpdater::isUpdating() const {
    return state_ == OtaState::DOWNLOADING || state_ == OtaState::APPLYING;
}
