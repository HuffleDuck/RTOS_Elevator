#ifndef LEDDRV_H
#define LEDDRV_H

/* Hardware include. */
#include <xc.h>
#include "CommonIncludes.h"
#include "FreeRTOS.h"
/* Standard includes. */
#include <stdint.h>
#include <plib.h>
#include <TypesAndGlobalVars.h>
uint8_t initializeLedDriver(void);
bool readLed(uint8_t ledNum);
uint8_t setLED(uint8_t ledNum, uint8_t value);
uint8_t toggleLED(uint8_t ledNum);

#endif