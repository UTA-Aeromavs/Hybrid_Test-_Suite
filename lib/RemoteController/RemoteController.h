#pragma once

class RemoteController {
  public:
    virtual ~RemoteController() {}

    // Called when a full line is typed on serial.
    // Return true if handled, false if RadioConsole should do default TX behavior.
    virtual bool handleSerialCommand(const char* line) = 0;

    // Called when a radio packet is received.
    // Return true if handled, false if RadioConsole should do default behavior.
    virtual bool handleRadioPacket(const char* packet) = 0;
};