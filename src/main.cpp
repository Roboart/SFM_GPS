#include <Arduino.h>
//#include <sapi.h>
//#include <sapi_delay.h>
#include <sapi_gpsParser.h>

#define ledGPS 15

//gpsDatos_t datosGPS;

uint32_t message = 0;



void gpsInit(Stream &serialport){
    serialport.println("Hola Mundo");
}

void setup() {
    
    Serial.begin(115200);
    Serial2.begin(9600);

    pinMode(ledGPS, OUTPUT);
    digitalWrite(ledGPS,LOW);
  
    Serial.println("sfmGPS_VR210408_1.0");
    gpsInit(Serial);
}



void loop() {
    //gpsDataUpdate();
    //if(gpsNewData())
    if(getDataSerial(Serial2)){
        Serial.println(getDataGPS());
        cleanBuff();
    }
}

