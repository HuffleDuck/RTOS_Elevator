#ifndef LEDDRV_H
#define LEDDRV_H

/* Hardware include. */
#include <xc.h>

/* Standard includes. */
#include <stdint.h>
#include <plib.h>

uint8_t initializeLedDriver(void);
uint8_t readLed(uint8_t ledNum);
uint8_t setLED(uint8_t ledNum, uint8_t value);
uint8_t toggleLED(uint8_t ledNum);

#endif