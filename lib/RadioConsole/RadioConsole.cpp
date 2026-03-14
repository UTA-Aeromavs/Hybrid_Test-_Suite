#include "RadioConsole.h"
#include <string.h>
#include <stdio.h>

RadioConsole* RadioConsole::activeInstance = 0;

RadioConsole::RadioConsole(PhysicalLayer& radioRef, RemoteController* controllerPtr) {
  radio = &radioRef;
  controller = controllerPtr;
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

void RadioConsole::begin(void) {
  activeInstance = this;
  enterReceiveMode();
}

void RadioConsole::run(void) {
  handleSerial();
  handleReceive();
  handleTransmitDone();
}

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

void RadioConsole::enterTransmitMode(const char* msg) {
  int state;

  radio->clearPacketReceivedAction();
  radio->clearPacketSentAction();
  radio->setPacketSentAction(txISR);

  strncpy(txBuf, msg, MSG_SIZE - 1);
  txBuf[MSG_SIZE - 1] = '\0';

  state = radio->startTransmit(txBuf);
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
  char c;
  bool handled;

  while(Serial.available() > 0) {
    c = (char)Serial.read();

    if(c == '\r' || c == '\n') {
      if(inputPos > 0) {
        inputBuf[inputPos] = '\0';

        handled = false;
        if(controller != 0) {
          handled = controller->handleSerialCommand(inputBuf);
        }

        if(!handled) {
          if(!txInProgress) {
            Serial.print("TX: ");
            Serial.println(inputBuf);

            ackPending = false;
            enterTransmitMode(inputBuf);
          } else {
            Serial.println("Busy transmitting, input dropped");
          }
        }
      }

      clearInput();
    }
    else {
      if(inputPos < (INPUT_SIZE - 1)) {
        inputBuf[inputPos] = c;
        inputPos++;
      }
    }
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
  if(state == RADIOLIB_ERR_NONE) {
    str.toCharArray(lastRxBuf, MSG_SIZE);

    Serial.print("RX: ");
    Serial.println(lastRxBuf);

    if(!startsWithAck(lastRxBuf) && !txInProgress) {
      snprintf(txBuf, MSG_SIZE, "ACK:%s", lastRxBuf);
      ackPending = true;
      Serial.print("TX ACK: ");
      Serial.println(txBuf);
      enterTransmitMode(txBuf);
      return;
    }
  } else if(state == RADIOLIB_ERR_CRC_MISMATCH) {
    Serial.println("CRC mismatch");
  } else {
    Serial.print("readData failed, code ");
    Serial.println(state);
  }

  enterReceiveMode();
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