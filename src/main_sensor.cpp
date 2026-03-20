#include <Arduino.h>
//#include "Relay_Valve.h"
#include "HybridScale.h"
#include "Pressure_Transducer.h"

#define Pressure_Transducer_Pin_1 A5
#define Pressure_Transducer_Pin_2 A4
#define HX711_DAT 2
#define HX711_CLK 3

//bool getPinState();

//Logic detect
HybridScale scale;
Pressure_Transducer Pressure_Transducer_1 = Pressure_Transducer(Pressure_Transducer_Pin_1);
Pressure_Transducer Pressure_Transducer_2 = Pressure_Transducer(Pressure_Transducer_Pin_1);

void setup(){
    Serial.begin(115200);
    scale.begin(HX711_DAT,HX711_CLK);
    scale.tare();
}
/*Note:
S=Scale
P1=Pressure Transducer 1
P2=Pressure Transducer 2
*/
void loop(){
    //Scale
    Serial.print("S, ");
    Serial.print(scale.update());
    Serial.print(", ");
    Serial.println(scale.getRawForce());
    
    //Pressure Transducer 1
    Serial.print("P1, ");
    Serial.print(Pressure_Transducer_1.update());
    Serial.print(", ");
    Serial.println(Pressure_Transducer_1.getRawVoltage());
    
    //Pressure Transducer 2
    Serial.print("P2, ");
    Serial.print(Pressure_Transducer_2.update());
    Serial.print(", ");
    Serial.println(Pressure_Transducer_2.getRawVoltage());

}
