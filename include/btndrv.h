//button driver

#ifndef BTNDRV_H
#define BTNDRV_H

/* Hardware include. */
#include <xc.h>

/* Standard includes. */
#include <stdint.h>
#include <plib.h>

uint8_t initializeBTNDriver(void);
void readBTN(service_req btnval)



#endif