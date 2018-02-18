#include "cellular_hal.h"

// TELUS APN
STARTUP(cellular_credentials_set("isp.telus.com", "", "", NULL));

int led = D7; // we will use D7 LED to monitor sensor activity
int pir = D0; //connect the PIR output to pin D0 of the Electron

void setup() {
    pinMode(D0, INPUT_PULLDOWN);
    pinMode(D7,OUTPUT);
}

void loop() {
    if (digitalRead(D0) == HIGH) {
      digitalWrite(D7,HIGH); //show the activity of the sensor with the onboard D7 LED
      Particle.publish("motion","high",60); //publish an event
      delay(1000);
        while (digitalRead(D0) == HIGH); // wait for the sensor to return back to normal
    }
    digitalWrite(D7,LOW);
}
