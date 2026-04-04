#include <Arduino.h>
#include <RadioLib.h>
#include <Wire.h>
#include <cstring>
#include <stdlib.h>

// Heltec ESP32 LoRa V3
#define RADIO_CS_PIN   8
#define RADIO_IRQ_PIN  14
#define RADIO_RST_PIN  12
#define RADIO_BUSY_PIN 13

const int LINE_LIM = 255;
char serial_buffer[LINE_LIM];
int serial_cursor=0;

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_IRQ_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif

void ISRRecieve();
void ISRTransmit();
void executeCommand(char* buffer);

void setup(){
    Serial.begin(115200);
    Serial.println("ENGINE CODE");
    // freq=915 MHz, BW=125 kHz, SF=7, CR=4/5, sync word 0x12
    int bootState = radio.begin(915.0, 125.0, 7, 5, 0x12, 10, 8, 1.8, false);
    while(bootState != RADIOLIB_ERR_NONE){
        Serial.println("Error Booting the radio");
        Serial.print("Code: ");
        Serial.println(bootState);
        delay(1000);
    }
    // Initialize the periferal Devices
    radio.setPacketReceivedAction(ISRRecieve);
    radio.startReceive();
}


// Indicates that the radio is sending a packet
volatile bool transmitFlag = false;
// Indicates that a packet is available to read
volatile bool recieveFlag = false;
// bool if the radio is busy
bool radioBusy = false;
char radioBuffer[LINE_LIM];
int transmitResult = RADIOLIB_ERR_NONE;

void loop(){
  // we finished a transmission
  if(transmitFlag){
    radioBusy = false;
    transmitFlag = false;
    radio.finishTransmit();
    radio.setPacketReceivedAction(ISRRecieve);
    int state = radio.startReceive();

  }
  // we got mail
  if(recieveFlag){
    recieveFlag = false;
    String str;
    int state = radio.readData(str);
    if(state == RADIOLIB_ERR_NONE){
      char command[LINE_LIM];
      Serial.print("RX: ");
      Serial.println(str);
      str.toCharArray(command, sizeof(command));
      executeCommand(command);
    } else if(state == RADIOLIB_ERR_CRC_MISMATCH){
      Serial.println(F("CRC error!"));
    }
  }
  // check for new serial commands, dont execute if not current on ack
  while(Serial.available() > 0 && !radioBusy){
    char incoming_byte = Serial.read();
    if((int)incoming_byte == 13){
      // convert to a null terminated string
      serial_buffer[serial_cursor] = '\0';
      // int number = atoi(serial_buffer+1);
      // Serial.print("Extracted Num:");
      // Serial.println(number);
      executeCommand(serial_buffer);
      // clear and process buffer
      serial_cursor = 0;
      return;
    }
    // Serial.println((int)incoming_byte);
    // check that the index is in bounds
    // add to buffer
    serial_buffer[serial_cursor] = incoming_byte;
    serial_cursor++;
  }
}


void ISRRecieve(){
  recieveFlag = true;
}

void ISRTransmit(){
  transmitFlag = true;
}

void executeCommand(char* buffer){
  if(memcmp(buffer, "SEND", 4)==0){
    char* sendbuffer = buffer + 5;
    String message = sendbuffer;
    Serial.print("TX: ");
    Serial.println(sendbuffer);
    radio.setPacketSentAction(ISRTransmit);
    radio.startTransmit(message);
  }
  if(memcmp(buffer, "ACK", 4)==0){
    Serial.println("Message Confirmed exiting transmit");
  }
  if(memcmp(buffer, "ARM", 4)==0){
    Serial.println("Engine Armed!");
  }
  if(memcmp(buffer, "FIRE", 4)==0){
    int duration = atoi(buffer+5);
    Serial.print("Firing Engine for ");
    Serial.print(duration);
    Serial.println(" s");
  }
}