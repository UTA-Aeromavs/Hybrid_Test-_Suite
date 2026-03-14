#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

// Define this in main.cpp
extern String g_testName;

class SdStreamLogger {
public:
    SdStreamLogger(SPIClass& spi, uint8_t csPin, uint32_t spiHz = 10000000UL);

    bool begin(int sckPin, int misoPin, int mosiPin);

    bool append(const String& streamType, const String& data);
    bool append(const String& streamType, const char* data);
    bool appendLine(const String& streamType, const String& line);
    bool appendTimestamped(const String& streamType, const String& payload);

    bool ensureHeader(const String& streamType, const String& headerLine);

    String readAll(const String& streamType);
    File openRead(const String& streamType);

    bool clearCurrentTest(const String& streamType);
    String getPath(const String& streamType) const;

private:
    SPIClass& _spi;
    uint8_t _csPin;
    uint32_t _spiHz;
    bool _mounted;

    String sanitize(const String& input) const;
    String buildFolder(const String& streamType) const;
    String buildPath(const String& streamType) const;
    bool ensureStreamFolderExists(const String& streamType);
};