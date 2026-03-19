#include <Arduino.h>
// RASPBERRY PI PICO + ADAFRUIT FEATHERWING LORA
#include <RadioLib.h>
#include "RadioConsole.h"

// This is the Chip Select pin
#define RADIO_CS_PIN 10
// This pin is for irq/dio0 and tells the processor that there is data to read
#define RADIO_IRQ_PIN 11
// This pin resets the radio for new instruction
#define RADIO_RST_PIN 12
// This is the busy pin
#define RADIO_DIO1_PIN 2

void commands(String message){
  if(message == "Hello"){
  }
}

SX1276 radio = new Module(RADIO_CS_PIN, RADIO_IRQ_PIN, RADIO_RST_PIN, RADIO_DIO1_PIN);
RadioConsole console(radio, commands);
#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif

void setup(){
    Serial.begin(115200);
    Serial.println("RPICO");
    // freq=915 MHz, BW=125 kHz, SF=7, CR=4/5, sync word 0x12
    int bootState = radio.begin(915.0, 125.0, 7, 5, 0x12, 10, 8, 0);
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

void loop(){
    console.run();
}