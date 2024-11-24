#include <Arduino.h>
#include <LittleFS.h>

#include "Git_OTA_Updater_001.h"

void setup() {
    Serial.begin(115200);
    LittleFS.begin();

    FirmwareUpdater updater("repoOwner", "repoName", "your-personal-access-token");
    updater.setFiles("/bootloader.bin", "/firmware.bin", "/littlefs.bin");
    updater.setCurrentVersion("fw_version", "1.0.0");

    xTaskCreate(
        [](void*) {
            updater.checkAndUpdate();
            vTaskDelete(nullptr);
        },
        "UpdateTask", 4096, nullptr, 1, nullptr
    );
}

void loop() {
    // Main loop code
}
