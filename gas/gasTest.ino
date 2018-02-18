#include "cellular_hal.h"
#include "math.h"

// TELUS APN
STARTUP(cellular_credentials_set("isp.telus.com", "", "", NULL));

#define RL_VALUE 5                    // load on the rensor
#define R0_CLEAN_AIR_FACTOR 9.83      // standard air/RO

#define CALIBRATION_TIMES 50            // number of samples when calibrating
#define CALIBRATION_INTERVAL 400        // time interval (milliseconds) between each sample

#define READ_INTERVAL 50                // number of samples in a normal run
#define READ_TIMES 5                    // time interval (milliseconds) between each sample

#define LPG_GAS 0
#define CO_GAS 1
#define SMOKE_GAS 2


float LPGCurve[3] = {2.3, 0.21, -0.47};
float COCurve[3] = {2.3, 0.72, -0.34};
float SmokeCurve[3] = {2.3, 0.53, -0.44};

float R0 = 10;

void setup()
{
    Serial.begin(115200);

    delay(5000);

    Serial.println("Calibrating");

    R0 = MQCalibration();

    Serial.println("Main calibration:");
    Serial.print("RO = ");
    Serial.println(R0);
    Serial.println("Main calibration done!");
    Serial.println();
}

void loop()
{
    R0 = MQCalibration();
    Serial.println("UPDATE");
    Serial.println("======");
    Serial.print("Air Purity (R0) [higher # = cleaner air]: ");
    Serial.println(R0);
    Serial.print("Liquified Petroleum Gas (LPG): ");
    Serial.print(getGasPercentage(MQRead()/R0, LPG_GAS) );
    Serial.println( " ppm" );
    Serial.print("Carbon Monoxide (CO): ");
    Serial.print(getGasPercentage(MQRead()/R0, CO_GAS) );
    Serial.println( " ppm" );
    Serial.print("SMOKE: ");
    Serial.print(getGasPercentage(MQRead()/R0, SMOKE_GAS) );
    Serial.println( " ppm" );
    Serial.println();
    delay(200);
}

// calibrates the sensor by using the normal air quality value
float MQCalibration()
{
    int i;
    float calibration = 0;
    for (i = 0; i < READ_TIMES; i++)
    {
      calibration += MQResistanceCalculation(analogRead(A0));
      delay(CALIBRATION_INTERVAL);
    }
    calibration = calibration/CALIBRATION_TIMES;
    calibration = calibration/R0_CLEAN_AIR_FACTOR;
    return calibration;
}

// reads from the sensor to get gas value
float MQRead()
{
    int i;
    float rs = 0;

    for (i = 0; i < READ_TIMES; i++) {
      rs += MQResistanceCalculation(analogRead(A0));
      delay(READ_INTERVAL);
    }
    rs = rs/READ_TIMES;
    return rs;
}

// calculates the sensor resistance
float MQResistanceCalculation(int adc) {
    return ( ((float)RL_VALUE*(1023-adc)/adc));
}


int getGasPercentage(float rs_ro_ratio, int gas_id)
{
    if (gas_id == LPG_GAS) {
      return MQGetPercentage(rs_ro_ratio,LPGCurve);
    }
    else if (gas_id == CO_GAS) {
      return MQGetPercentage(rs_ro_ratio,COCurve);
    }
    else if (gas_id == SMOKE_GAS) {
      return MQGetPercentage(rs_ro_ratio,SmokeCurve);
    }
    return 1;                 // the 1 is just to see if this effects anything
}


int MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
    return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
