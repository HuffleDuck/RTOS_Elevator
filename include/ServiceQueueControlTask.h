
#ifndef SERVICEQUEUECONTROLTASK_H
#define	SERVICEQUEUECONTROLTASK_H
#include <FreeRTOS.h>
#include "CommonIncludes.h"
#include "TypesAndGlobalVars.h"
#include "UartDriver.h"


void ServiceQueueControlTask(void *param_struct);
bool SignalDoorDone();
bool SignalMotorDone();
void SignalJustKiddingDoorNotDone();
MotorMessage CreateNewMotorMessage(float current_max_speed,
                                            float current_acel,
                                            int current_floor,
                                            int requested_floor);

/////Place Data into the SericeQueue here////////////////
bool QueueServiceRequest( service_req request_for_service,
                            float data_to_go_with_request);

#endif	/* SERVICEQUEUECONTROLTASK_H */

