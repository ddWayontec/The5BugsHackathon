#include "cellular_hal.h"

//#define MAX_PHONE_NUMBER 14
#define CTRL_Z 0x1A
#define TIMEOUT 10000

unsigned char id;
boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds
unsigned int data1;
unsigned int data2;
//char szPhoneNumber[MAX_PHONE_NUMBER] = "14037972786";
int szPhoneNumber = 14037972786;

int lowerLimit = 10;
boolean smsSend = false;

int callback(int type, const char* buf, int len, char* param)
{
    Serial.print("Return: ");
    Serial.write((const uint8_t*)buf, len);
    Serial.println();

    return WAIT;
}

int setLowerLimit(String args)
{
    lowerLimit = args.toInt();

    return lowerLimit;
}

int sendMessage(char* pMessage){
    char szCmd[64];

    sprintf(szCmd, "AT+CMGS=\"+%s\",145\r\n", szPhoneNumber);

    Serial.print("Sending command ");
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
        Serial.println("+OK, Message Send");
    }
    else
    {
        Serial.println("+ERROR, error sending message");
    }

    return retVal;
}

STARTUP(cellular_credentials_set("isp.telus.com", "", "", NULL));

void setup()
{
    Serial.begin(115200);
    Spark.function("setmin", setLowerLimit);
}

void loop()
{

    Serial.println();
    Serial.println();

    char szMessage[64] = "Particle Electron Test Message 2";

    sendMessage(szMessage);

    delay(50000);
}
