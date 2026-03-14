#include <Arduino.h>
#include <RadioLib.h>
#include "RadioConsole.h"

// Heltec ESP32 LoRa V3
#define RADIO_CS_PIN   8
#define RADIO_IRQ_PIN  14
#define RADIO_RST_PIN  12
#define RADIO_BUSY_PIN 13

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_IRQ_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
RadioConsole console(radio);
#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif

void setup() {
    Serial.begin(115200);
    while(!Serial){delay(10);};

    Serial.println("ESP32");

    int bootState = radio.begin(915.0, 125.0, 7, 5, 0x12, 10, 8, 1.8, false);
    while(bootState != RADIOLIB_ERR_NONE){
        Serial.println("Error Booting the radio");
        Serial.print("Code: ");
        Serial.println(bootState);
        delay(1000);
    }

    console.begin();
    Serial.println("Ready.");
    Serial.println("Type a message and press Enter.");
}

void loop() {
  console.run();
}