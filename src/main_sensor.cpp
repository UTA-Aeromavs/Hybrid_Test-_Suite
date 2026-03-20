#include <Arduino.h>
//#include "Relay_Valve.h"
#include "HybridScale.h"
#include "Pressure_Transducer.h"

#define Pressure_Transducer_Pin_1 A5
#define Pressure_Transducer_Pin_2 A4
#define HX711_DAT 2
#define HX711_CLK 3
#define Read_Enable 13
#define Buzzer_Pin 4

//bool getPinState();

//Logic detect
HybridScale scale;
Pressure_Transducer Pressure_Transducer_1 = Pressure_Transducer(Pressure_Transducer_Pin_1);
Pressure_Transducer Pressure_Transducer_2 = Pressure_Transducer(Pressure_Transducer_Pin_1);
int enablePin; 

void setup(){
    Serial.begin(115200);
    scale.begin(HX711_DAT,HX711_CLK);
    scale.tare();
    enablePin = analogRead(Read_Enable);
    pinMode(Buzzer_Pin, OUTPUT);
    pinMode(Read_Enable, INPUT);
}
/*Note:
S=Scale
P1=Pressure Transducer 1
P2=Pressure Transducer 2
*/
void loop(){
    while(!(digitalRead(Read_Enable))){
        digitalWrite(Buzzer_Pin, LOW);
    }
    if((digitalRead(Read_Enable))){
        digitalWrite(Buzzer_Pin, HIGH);
    }
    
    //Scale
    Serial.print("S, ");
    Serial.print(scale.update());
    Serial.print(", ");
    Serial.println(scale.getRawForce());
    
    //Pressure Transducer 1
    Serial.print("P1, ");
    Serial.print(Pressure_Transducer_1.update());
    Serial.print(", ");
    Serial.println(Pressure_Transducer_1.getRawValue());
    
    //Pressure Transducer 2
    Serial.print("P2, ");
    Serial.print(Pressure_Transducer_2.update());
    Serial.print(", ");
    Serial.println(Pressure_Transducer_2.getRawValue());

}
