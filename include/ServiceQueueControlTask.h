
#ifndef SERVICEQUEUECONTROLTASK_H
#define	SERVICEQUEUECONTROLTASK_H
#include <FreeRTOS.h>
#include <queue.h>


void ServiceQueueControlTask(void *param_struct);





typedef struct ServiceQueueControl_parameter {
     QueueHandle_t m_service_request_message_queue; // IN from "listener" tasks.
     QueueHandle_t m_motor_message_queue; // OUT to motor control task.
     QueueHandle_t m_door_message_queue; // OUT to door control task.
}  ServiceQueueControl_parameter;


#endif	/* SERVICEQUEUECONTROLTASK_H */

