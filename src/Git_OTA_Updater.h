#ifndef FIRMWARE_UPDATER_H
#define FIRMWARE_UPDATER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <Update.h>
#include "FS.h"
#include "LittleFS.h"

class FirmwareUpdater {
public:
    FirmwareUpdater(const String& repoOwner, const String& repoName, const String& token);
    void setFiles(const String& bootloaderFile, const String& firmwareFile, const String& littleFSFile);
    void setCurrentVersion(const String& versionKey, const String& currentVersion);
    void checkAndUpdate();

private:
    String getLatestReleaseInfo();
    bool downloadFile(const String& url, const String& filePath);
    bool updateBootloader(const String& filePath);
    bool updateFirmware(const String& filePath);
    bool updateLittleFS(const String& filePath);

    String _repoOwner;
    String _repoName;
    String _token;
    String _bootloaderFile;
    String _firmwareFile;
    String _littleFSFile;
    String _versionKey;
    String _currentVersion;
};

#endif
