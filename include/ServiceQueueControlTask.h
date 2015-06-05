
#ifndef SERVICEQUEUECONTROLTASK_H
#define	SERVICEQUEUECONTROLTASK_H
#include <FreeRTOS.h>
#include "CommonIncludes.h"
#include "TypesAndGlobalVars.h"
#include "UartDriver.h"


void ServiceQueueControlTask(void *param_struct);
MotorMessage CreateNewMotorMessage(float current_max_speed,
                                            float current_acel,
                                            int current_floor,
                                            int requested_floor);
#endif	/* SERVICEQUEUECONTROLTASK_H */

