#include "RadioConsole.h"
#include <string.h>
#include <stdio.h>
#include <Arduino.h>

RadioConsole* RadioConsole::activeInstance = 0;

void RadioConsole::begin(void) {
  activeInstance = this;
  enterReceiveMode();
}

void RadioConsole::run(void) {
  handleSerial();
  handleReceive();
  handleTransmitDone();
}

// ISR flags workaround
void RadioConsole::rxISR(void) {
  if(activeInstance != 0) {
    activeInstance->onRxInterrupt();
  }
}

void RadioConsole::txISR(void) {
  if(activeInstance != 0) {
    activeInstance->onTxInterrupt();
  }
}

void RadioConsole::onRxInterrupt(void) {
  rxFlag = true;
}

void RadioConsole::onTxInterrupt(void) {
  txFlag = true;
}

// Recieve
void RadioConsole::enterReceiveMode(void) {
  int state;

  radio->clearPacketSentAction();
  radio->clearPacketReceivedAction();
  radio->setPacketReceivedAction(rxISR);

  state = radio->startReceive();
  if(state != RADIOLIB_ERR_NONE) {
    Serial.print("startReceive failed, code ");
    Serial.println(state);
  } else {
    mode = MODE_RX;
    txInProgress = false;
  }
}

/*
Transmission
Transmission can either be done through serial input or automatically through buffer
*/
void RadioConsole::loadTxBuffer(const char* src) {
  if(src == 0) {
    txLen = 0;
    txBuf[0] = '\0';
    return;
  }

  size_t len = strlen(src);
  if(len > MSG_SIZE - 1) {
    len = MSG_SIZE - 1;
  }

  memcpy(txBuf, src, len);
  txBuf[len] = '\0';
  txLen = len;
}

void RadioConsole::transmitBuffer() {
  int state;

  radio->clearPacketReceivedAction();
  radio->clearPacketSentAction();
  radio->setPacketSentAction(txISR);

  state = radio->startTransmit(txBuf, txLen);
  if(state != RADIOLIB_ERR_NONE) {
    Serial.print("startTransmit failed, code ");
    Serial.println(state);
    enterReceiveMode();
  } else {
    mode = MODE_TX;
    txInProgress = true;
  }
}

void RadioConsole::handleSerial(void) {
  while(Serial.available() > 0) {
    char c = (char)Serial.read();

    if(c == '\n') {
      continue;
    }

    if(c == '\r') {
      if(inputPos > 0 && inputPos < INPUT_SIZE) {
        inputBuf[inputPos] = '\0';
      }
      break;
    }

    if(inputPos < (INPUT_SIZE - 1)) {
      inputBuf[inputPos] = c;
      inputPos++;
    }
  }

  if(!txInProgress && inputPos > 0 && inputBuf[inputPos] == '\0') {
    Serial.print("TX: ");
    Serial.println(inputBuf);

    loadTxBuffer(inputBuf);
    ackPending = false;
    transmitBuffer();
    clearInput();
  }
}

void RadioConsole::handleReceive(void) {
  String str;
  int state;

  if(!rxFlag) {
    return;
  }

  rxFlag = false;

  state = radio->readData(str);
  if(state == RADIOLIB_ERR_CRC_MISMATCH) {
    Serial.println("CRC mismatch");
    enterReceiveMode();
    return;
  }
  if(state != RADIOLIB_ERR_NONE) {
    Serial.print("readData failed, code ");
    Serial.println(state);
  }
  str.toCharArray(lastRxBuf, MSG_SIZE);

  Serial.print("RX: ");
  Serial.println(lastRxBuf);
  if(startsWithAck(lastRxBuf)){
    enterReceiveMode();
    return;
  }
  commandCallback(lastRxBuf);

  ackPending = true;
  char ackMsg[MSG_SIZE];
  snprintf(ackMsg, MSG_SIZE, "ACK: %s", lastRxBuf);
  Serial.print("TX: ");
  Serial.println(ackMsg);
  loadTxBuffer(ackMsg);
  transmitBuffer();
}

void RadioConsole::handleTransmitDone(void) {
  int state;

  if(!txFlag) {
    return;
  }

  txFlag = false;

  state = radio->finishTransmit();
  if(state == RADIOLIB_ERR_NONE) {
    if(ackPending) {
      Serial.println("ACK sent");
    } else {
      Serial.println("Transmit complete");
    }
  } else {
    Serial.print("finishTransmit failed, code ");
    Serial.println(state);
  }

  ackPending = false;
  enterReceiveMode();
}

void RadioConsole::clearInput(void) {
  inputPos = 0;
  inputBuf[0] = '\0';
}

bool RadioConsole::startsWithAck(const char* s) {
  if(s == 0) {
    return false;
  }
  return (strncmp(s, "ACK:", 4) == 0);
}