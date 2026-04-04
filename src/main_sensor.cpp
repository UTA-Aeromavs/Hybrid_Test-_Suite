#include <Wire.h>

struct Telemetry {
  int16_t p1;
  int16_t p2;
  int16_t w1;
};

// Internal values
volatile int16_t g_p1 = 100;
volatile int16_t g_p2 = 200;
volatile int16_t g_w1 = 300;

// Function you can call anywhere in your code
Telemetry getTelemetry() {
  Telemetry t;
  t.p1 = g_p1;
  t.p2 = g_p2;
  t.w1 = g_w1;
  return t;
}

// Called when I2C master asks for data
void onI2CRequest() {
  Telemetry t = getTelemetry();
  Wire.write((uint8_t*)&t, sizeof(t));
}

void setup() {
  Wire.begin(0x12);   // slave address
  Wire.onRequest(onI2CRequest);
}

void loop() {
  // Example updates
  g_p1++;
  g_p2 += 2;
  g_w1 += 3;
  delay(100);
}