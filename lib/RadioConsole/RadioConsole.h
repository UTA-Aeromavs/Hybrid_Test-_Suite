#ifndef RADIO_CONSOLE_H
#define RADIO_CONSOLE_H

#include <Arduino.h>
#include <RadioLib.h>
#include "RemoteController.h"

class RadioConsole {
  public:
    RadioConsole(PhysicalLayer& radioRef, RemoteController* controllerPtr = 0);

    void begin(void);
    void run(void);

  private:
    enum Mode {
      MODE_RX = 0,
      MODE_TX
    };

    PhysicalLayer* radio;
    RemoteController* controller;
    Mode mode;

    volatile bool rxFlag;
    volatile bool txFlag;

    bool ackPending;
    bool txInProgress;

    static const int INPUT_SIZE = 128;
    static const int MSG_SIZE   = 128;

    char inputBuf[INPUT_SIZE];
    int inputPos;

    char txBuf[MSG_SIZE];
    char lastRxBuf[MSG_SIZE];

    static RadioConsole* activeInstance;

    static void rxISR(void);
    static void txISR(void);

    void onRxInterrupt(void);
    void onTxInterrupt(void);

    void enterReceiveMode(void);
    void enterTransmitMode(const char* msg);

    void handleSerial(void);
    void handleReceive(void);
    void handleTransmitDone(void);

    void clearInput(void);
    bool startsWithAck(const char* s);
};

#endif