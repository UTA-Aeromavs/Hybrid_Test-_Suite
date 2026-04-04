#ifndef RADIO_CONSOLE_H
#define RADIO_CONSOLE_H

#include <Arduino.h>
#include <RadioLib.h>
#include "RemoteController.h"

class RadioConsole {
  public:
    using commandHandler = void(*)(String);
    RadioConsole(PhysicalLayer& radioRef, commandHandler commands = noCommandHandler){
        radio = &radioRef;
        commandCallback = commands;
        mode = MODE_RX;
        rxFlag = false;
        txFlag = false;
        ackPending = false;
        txInProgress = false;
        inputPos = 0;
        inputBuf[0] = '\0';
        txBuf[0] = '\0';
        lastRxBuf[0] = '\0';
    }

    void begin(void);
    void run(void);
    bool sendMessage(const String& message);

  private:
    static void noCommandHandler(String s) {}

    enum Mode {
      MODE_RX = 0,
      MODE_TX
    };

    PhysicalLayer* radio;
    commandHandler commandCallback;
    Mode mode;

    volatile bool rxFlag;
    volatile bool txFlag;

    bool ackPending;
    bool txInProgress;
    unsigned long testTime = 9999999;

    static const int INPUT_SIZE = 128;
    static const int MSG_SIZE   = 128;

    char inputBuf[INPUT_SIZE];
    int inputPos;

    char txBuf[MSG_SIZE];
    int txLen = 0;
    char lastRxBuf[MSG_SIZE];

    static RadioConsole* activeInstance;

    static void rxISR(void);
    static void txISR(void);

    void onRxInterrupt(void);
    void onTxInterrupt(void);

    void enterReceiveMode(void);
    void handleReceive(void);

    void loadTxBuffer(const char* src);
    void transmitBuffer();
    void handleSerial(void);
    void handleTransmitDone(void);

    void clearInput(void);
    bool startsWithAck(const char* s);
};

#endif