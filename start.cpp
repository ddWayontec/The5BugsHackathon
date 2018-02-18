#include "cellular_hal.h"

// TELUS APN
STARTUP(cellular_credentials_set("isp.telus.com", "", "", NULL));

void setup()
{
  // Set the keep-alive value for TELUS SIM card
  Particle.keepAlive(30);
}

void loop ()
{
  Cellular.off();
}
