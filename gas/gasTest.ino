#include "cellular_hal.h"

// TELUS APN
STARTUP(cellular_credentials_set("isp.telus.com", "", "", NULL));

void setup() {
  Serial.begin(115200);
}

void loop() {
    float sensor_volt;
    float gas_ratio;
    float RS_air;
    float sensorValue;
    float R0;


    for (int i = 0; i < 100; i++){
      sensorValue = sensorValue + analogRead(A0);
    }

    sensorValue = sensorValue/100.0;
    sensor_volt = (float)sensorVal/1024*5.0;
    RS_air = (5.0-sensor_volt)/sensor_volt;
    R0 = RS_air/10                         // ratio is always 10 in clear air


    Serial.print("Sensor value= ")
    Serial.print(sensorValue);
    Serial.println("R0 = ");
    Serial.print(RO);

    Cellular.off();
    delay(120000);

    Cellular.on();
    delay(300000);

}
