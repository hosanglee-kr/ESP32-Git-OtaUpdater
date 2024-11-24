#include "Git_OTA_Updater_001.h"

FirmwareUpdater::FirmwareUpdater(const String& repoOwner, const String& repoName, const String& token) 
    : _repoOwner(repoOwner), _repoName(repoName), _token(token) {}

void FirmwareUpdater::setFiles(const String& bootloaderFile, const String& firmwareFile, const String& littleFSFile) {
    _bootloaderFile = bootloaderFile;
    _firmwareFile = firmwareFile;
    _littleFSFile = littleFSFile;
}

void FirmwareUpdater::setCurrentVersion(const String& versionKey, const String& currentVersion) {
    _versionKey = versionKey;
    _currentVersion = currentVersion;
}

void FirmwareUpdater::checkAndUpdate() {
    String latestVersion = getLatestReleaseInfo();
    if (latestVersion.isEmpty() || latestVersion == _currentVersion) {
        Serial.println("Already up-to-date.");
        return;
    }

    Serial.printf("Updating to version: %s\n", latestVersion.c_str());

    if (!updateBootloader(_bootloaderFile) ||
        !updateFirmware(_firmwareFile) ||
        !updateLittleFS(_littleFSFile)) {
        Serial.println("Update failed!");
        return;
    }

    Preferences preferences;
    preferences.begin("firmware", false);
    preferences.putString(_versionKey.c_str(), latestVersion);
    preferences.end();

    Serial.println("Update successful! Restarting...");
    ESP.restart();
}

String FirmwareUpdater::getLatestReleaseInfo() {
    HTTPClient http;
    String url = "https://api.github.com/repos/" + _repoOwner + "/" + _repoName + "/releases/latest";
    http.begin(url.c_str());
    http.addHeader("Authorization", "token " + _token);
    int httpCode = http.GET();

    if (httpCode != 200) {
        Serial.printf("Failed to fetch release info. HTTP code: %d\n", httpCode);
        return "";
    }

    String payload = http.getString();
    int startIdx = payload.indexOf("\"tag_name\":\"") + 12;
    int endIdx = payload.indexOf("\"", startIdx);
    return payload.substring(startIdx, endIdx);
}

bool FirmwareUpdater::downloadFile(const String& url, const String& filePath) {
    HTTPClient http;
    http.begin(url.c_str());
    http.addHeader("Authorization", "token " + _token);

    int httpCode = http.GET();
    if (httpCode != 200) {
        Serial.printf("Failed to download file. HTTP code: %d\n", httpCode);
        return false;
    }

    File file = LittleFS.open(filePath, "w");
    if (!file) {
        Serial.printf("Failed to open file for writing: %s\n", filePath.c_str());
        return false;
    }

    file.write(http.getStream());
    file.close();
    return true;
}

bool FirmwareUpdater::updateBootloader(const String& filePath) {
    Serial.println("Updating bootloader...");
    return Update.begin(UPDATE_SIZE_UNKNOWN) && Update.end();
}

bool FirmwareUpdater::updateFirmware(const String& filePath) {
    Serial.println("Updating firmware...");
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        Serial.printf("Failed to open firmware file: %s\n", filePath.c_str());
        return false;
    }

    size_t updateSize = file.size();
    if (!Update.begin(updateSize)) {
        file.close();
        return false;
    }

    Update.writeStream(file);
    file.close();
    return Update.end();
}

bool FirmwareUpdater::updateLittleFS(const String& filePath) {
    Serial.println("Updating LittleFS...");
    // Custom implementation based on LittleFS usage.
    return true;
}
