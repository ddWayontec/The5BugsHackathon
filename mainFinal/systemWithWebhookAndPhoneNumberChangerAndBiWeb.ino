#define MAX_PHONE_NUMBER 14
#define CTRL_Z 0x1A
#define TIMEOUT 10000
#include "cellular_hal.h"

unsigned char id;
boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds
unsigned int data1;
unsigned int data2;
volatile int readPin = D7;

char szPhoneNumber[MAX_PHONE_NUMBER] = "14038001118"; //default phone number that will be used for SMS texts

int lowerLimit = 10;
boolean smsSend = false;

int prnToScr(String command);

int setMotion(String command);  //for the "call function "
int setMotionSMS = -1;

int callback(int type, const char* buf, int len, char* param)
{
    Serial.print("Return: ");
    Serial.write((const uint8_t*)buf, len);
    Serial.println();

    return WAIT;
}

char* getPhoneNumber()
{
    return szPhoneNumber;
}

int setLowerLimit(String args)
{
    lowerLimit = args.toInt();

    return lowerLimit;
}

void numberChanger(String num)  //phone number changer function
{
    char charBuf[MAX_PHONE_NUMBER];
    num.toCharArray(charBuf, MAX_PHONE_NUMBER);

    for (int i = 0; i < MAX_PHONE_NUMBER; i++)
    {
        szPhoneNumber[i] = charBuf[i];
    }
}

int sendMessage(char* pMessage) //for sending sms text message to cell phone
{
    char szCmd[64];

    sprintf(szCmd, "AT+CMGS=\"+%s\",145\r\n", szPhoneNumber);

    Serial.print("Sending command to SMS Centre...");
    Serial.print(szCmd);
    Serial.println();

    char szReturn[32] = "";

    Cellular.command(callback, szReturn, TIMEOUT, "AT+CMGF=1\r\n");
    Cellular.command(callback, szReturn, TIMEOUT, szCmd);
    Cellular.command(callback, szReturn, TIMEOUT, pMessage);

    sprintf(szCmd, "%c", CTRL_Z);

    int retVal = Cellular.command(callback, szReturn, TIMEOUT, szCmd);

    if(RESP_OK == retVal)
    {
        Serial.println("+OK, SMS text sent!");
    }
    else
    {
        Serial.println("+ERROR, error sending SMS text.");
    }
    return retVal;
}

int setMotion(String command) //function for the received POST from the Android App or Particle Cloud function call module
{
  if(command == "Disarm") //if Disarm "message" is received from the Android App or Particle Cloud function call module
  {
    setMotionOff(); //calls the function that turns the motion sensor off
    Serial.println("Motion disarmed."); //print message to screen (using Putty on Serial Port 9 at 115200)
    setMotionSMS = 0;
    return 0;
  }
  else if(command == "Arm") //if Arm "message" is received from the Android App or Particle Cloud function call module
  {
    setMotionOn(); //calls the function that turns the motion sensor off
    Serial.println("Motion ARMED!"); //print message to screen (using Putty on Serial Port 9 at 115200)
    setMotionSMS = 1;
    return 1;
  }
  else
  {
    setMotionSMS = -1;
    return -1;
  }
}

int prnToScr(String command)
{
    Serial.println("called func successfully");
    return 1;
}

void setMotionOn()  //turns the motion sensor on
{
    pinMode(readPin,INPUT);
    digitalRead(readPin);

}

void setMotionOff() //turns the motion sensor off
{
    pinMode(readPin,OUTPUT);
    digitalWrite(readPin, LOW);
}

STARTUP(cellular_credentials_set("isp.telus.com", "", "", NULL)); //required fer Telus SIM cards

void setup()
{
    // Subscribe to the integration response event:
    Particle.subscribe("hook-response/Motion Detected", myHandler, MY_DEVICES);

    Particle.function("test", prnToScr);
    Serial.begin(115200); //set serial baud rate for console
    Particle.function("setMotion", setMotion); //enables the Android app and Particle Console to turn the motion sensor off or on
    setMotionOff();
//    setMotionOn(); // ONLY FOR TESTING, DELETE WHEN APP IMPLEMENTED
}

void myHandler(const char *event, const char *data)
{
  // Handle the integration response
}

void loop()
{

  if(setMotionSMS == 0)
  {
    Serial.println("Sending disarmed SMS...");
    char szMotionOffMessage[64] = "Motion disarmed."; //sets the message to be sent through sms
    sendMessage(szMotionOffMessage);  //sends the sms text to the original number
    numberChanger("16044010082"); //calls the number changer function to change the phone number to this new number
    sendMessage(szMotionOffMessage);  //sends the sms text to the new number
    numberChanger("14038001118"); //calls the number changer function to change the phone number back to the number
    setMotionSMS = -1;
  }
  else if(setMotionSMS == 1)
  {
    Serial.println("Sending ARMED SMS...");
    char szMotionOnMessage[64] = "Motion ARMED!"; //sets the message to be sent through sms
    sendMessage(szMotionOnMessage);  //sends the sms text to the original number
    numberChanger("16044010082"); //calls the number changer function to change the phone number to this new number
    sendMessage(szMotionOnMessage);  //sends the sms text to the new number
    numberChanger("14038001118"); //calls the number changer function to change the phone number back to the number
    setMotionSMS = -1;
  }
  else
  {
    setMotionSMS = -1;
  }

  if (digitalRead(readPin) == HIGH) //if the pin is triggered by the motion sensor...
  {
    Serial.println("MOTION DETECTED"); //print message to screen (using Putty on Serial Port 9 at 115200)
    Serial.println();

    // Get some data:
    String data = String(10);
    // Trigger the integration onto the web through ThingSpeak:
    Particle.publish("Motion Detected", data, PRIVATE);

    char szMessage[64] = "Motion Detected!"; //sets the message to be sent to the customer
    sendMessage(szMessage); // sends message to the cell phone number on file

//    Serial.println(Time.format(TIME_FORMAT_ISO8601_FULL));

    delay(5000); //delay needed so that the Particle.io account deos not get "spammed" by this firmware. Can be set lower if customer needs more datapoints, but custower should then contact Particle Cloud to inform them of the higher rate requirement on their account.
  }
  else
  {
    Serial.println("No motion");  //print message to screen (using Putty on Serial Port 9 at 115200)
  }
    delay(1000);
}
