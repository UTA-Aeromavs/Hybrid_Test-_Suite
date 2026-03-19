#include <Arduino.h>
#include <RadioLib.h>

#define MESSAGE_BUFFER_SIZE 256

// Heltec ESP32 LoRa V3
#define RADIO_CS_PIN   8
#define RADIO_IRQ_PIN  14
#define RADIO_RST_PIN  12
#define RADIO_BUSY_PIN 13

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_IRQ_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#if defined(ESP8266) || defined(ESP32)
ICACHE_RAM_ATTR
#endif
void onTxInterrupt(void);

volatile bool txDoneFlag = false;
bool txInProgress = false;

char inputBuffer[MESSAGE_BUFFER_SIZE];
size_t inputLen = 0;

uint8_t txBuf[MESSAGE_BUFFER_SIZE];
size_t txLen = 0;

void onTxInterrupt(void) {
  txDoneFlag = true;
}

void resetInputBuffer() {
  inputLen = 0;
  inputBuffer[0] = '\0';
}

bool appendToInputBuffer(char c) {
  if (inputLen >= MESSAGE_BUFFER_SIZE - 1) {
    return false;
  }

  inputBuffer[inputLen++] = c;
  inputBuffer[inputLen] = '\0';
  return true;
}

bool copyInputToTxBuffer() {
  if (txInProgress) {
    return false;
  }

  txLen = inputLen;

  for (size_t i = 0; i < txLen; i++) {
    txBuf[i] = static_cast<uint8_t>(inputBuffer[i]);
  }

  return true;
}

void setup() {
  Serial.begin(115200);

  int bootState = radio.begin(915.0, 125.0, 7, 5, 0x12, 10, 8, 1.8, false);
  while (bootState != RADIOLIB_ERR_NONE) {
    Serial.println("Error Booting the radio");
    Serial.print("Code: ");
    Serial.println(bootState);
    delay(1000);
  }

  radio.setPacketSentAction(onTxInterrupt);
  resetInputBuffer();
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n') {
      continue;
    }

    if (c == '\r') {
      Serial.println("Return");

      if (inputLen == 0) {
        resetInputBuffer();
        break;
      }

      if (txInProgress) {
        Serial.println("ERROR: transmitter busy");
        resetInputBuffer();
        break;
      }

      if (!copyInputToTxBuffer()) {
        Serial.println("ERROR: failed to load TX buffer");
        resetInputBuffer();
        break;
      }

      int16_t state = radio.startTransmit(txBuf, txLen);
      if (state == RADIOLIB_ERR_NONE) {
        txInProgress = true;
        Serial.print("TX started: ");
        Serial.println(inputBuffer);
      } else {
        Serial.print("startTransmit failed, code: ");
        Serial.println(state);
      }

      resetInputBuffer();
      break;
    }

    if (!appendToInputBuffer(c)) {
      Serial.println("ERROR: message too long");
      resetInputBuffer();
      break;
    }
  }

  if (txDoneFlag) {
    txDoneFlag = false;

    int16_t state = radio.finishTransmit();
    if (state == RADIOLIB_ERR_NONE) {
      Serial.println("Transmission finished");
    } else {
      Serial.print("finishTransmit failed, code: ");
      Serial.println(state);
    }

    txInProgress = false;
    txLen = 0;
  }
}