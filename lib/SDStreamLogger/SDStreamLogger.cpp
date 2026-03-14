#include "SdStreamLogger.h"

SdStreamLogger::SdStreamLogger(SPIClass& spi, uint8_t csPin, uint32_t spiHz)
    : _spi(spi), _csPin(csPin), _spiHz(spiHz), _mounted(false) {}

bool SdStreamLogger::begin(int sckPin, int misoPin, int mosiPin) {
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);

    _spi.begin(sckPin, misoPin, mosiPin, _csPin);
    delay(20);

    _mounted = SD.begin(_csPin, _spi, _spiHz);
    return _mounted;
}

bool SdStreamLogger::append(const String& streamType, const String& data) {
    if (!_mounted) {
        return false;
    }

    if (!ensureStreamFolderExists(streamType)) {
        return false;
    }

    File f = SD.open(buildPath(streamType), FILE_APPEND);
    if (!f) {
        return false;
    }

    size_t written = f.print(data);
    f.close();

    return written == data.length();
}

bool SdStreamLogger::append(const String& streamType, const char* data) {
    if (data == nullptr) {
        return false;
    }
    return append(streamType, String(data));
}

bool SdStreamLogger::appendLine(const String& streamType, const String& line) {
    if (!_mounted) {
        return false;
    }

    if (!ensureStreamFolderExists(streamType)) {
        return false;
    }

    File f = SD.open(buildPath(streamType), FILE_APPEND);
    if (!f) {
        return false;
    }

    size_t written = f.println(line);
    f.close();

    return written > 0;
}

bool SdStreamLogger::appendTimestamped(const String& streamType, const String& payload) {
    const String line = String(millis()) + "," + payload;
    return appendLine(streamType, line);
}

bool SdStreamLogger::ensureHeader(const String& streamType, const String& headerLine) {
    if (!_mounted) {
        return false;
    }

    if (!ensureStreamFolderExists(streamType)) {
        return false;
    }

    const String path = buildPath(streamType);

    if (SD.exists(path)) {
        File existing = SD.open(path, FILE_READ);
        if (existing) {
            bool hasData = existing.size() > 0;
            existing.close();
            if (hasData) {
                return true;
            }
        }
    }

    File f = SD.open(path, FILE_APPEND);
    if (!f) {
        return false;
    }

    size_t written = f.println(headerLine);
    f.close();

    return written > 0;
}

String SdStreamLogger::readAll(const String& streamType) {
    if (!_mounted) {
        return "";
    }

    File f = SD.open(buildPath(streamType), FILE_READ);
    if (!f) {
        return "";
    }

    String out;
    while (f.available()) {
        out += static_cast<char>(f.read());
    }
    f.close();
    return out;
}

File SdStreamLogger::openRead(const String& streamType) {
    if (!_mounted) {
        return File();
    }
    return SD.open(buildPath(streamType), FILE_READ);
}

bool SdStreamLogger::clearCurrentTest(const String& streamType) {
    if (!_mounted) {
        return false;
    }

    const String path = buildPath(streamType);
    if (SD.exists(path)) {
        return SD.remove(path);
    }

    return true;
}

String SdStreamLogger::getPath(const String& streamType) const {
    return buildPath(streamType);
}

String SdStreamLogger::sanitize(const String& input) const {
    String s = input;
    s.replace("\\", "_");
    s.replace("/", "_");
    s.replace(":", "_");
    s.replace("*", "_");
    s.replace("?", "_");
    s.replace("\"", "_");
    s.replace("<", "_");
    s.replace(">", "_");
    s.replace("|", "_");
    s.trim();

    if (s.isEmpty()) {
        return "unnamed";
    }

    return s;
}

String SdStreamLogger::buildFolder(const String& streamType) const {
    return "/" + sanitize(streamType);
}

String SdStreamLogger::buildPath(const String& streamType) const {
    return buildFolder(streamType) + "/" + sanitize(g_testName) + ".csv";
}

bool SdStreamLogger::ensureStreamFolderExists(const String& streamType) {
    const String folder = buildFolder(streamType);
    if (SD.exists(folder)) {
        return true;
    }
    return SD.mkdir(folder);
}