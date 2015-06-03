
#ifndef SERVICEQUEUECONTROLTASK_H
#define	SERVICEQUEUECONTROLTASK_H
#include <FreeRTOS.h>
#include <queue.h>


void ServiceQueueControlTask(void *param_struct);




typedef enum
{
	EmergStop,
	EmergClear,
	DoorInterference,
	OpenDoor,
	CloseDoor,
	CallToGNDInsideCar, // Requests inside the car get priority over requests outside the car.
	CallToP1InsideCar,
	CallToP2InsideCar,
	CallToGNDOutsideCar, //    treat both UART_TX requests for a floor outside a car and CLI
	CallToP1fromOutsideCar, //  requests for floor N as requests outside the car.
	CallToP2fromOutsideCar,
	ChangeMaxSpeedToN, // Before sending this message, update a global var  ChangeMaxAccelToN,
	// "NEW_VEL_VALUE" with the new speed/acel. CLI will be the only
	// task to write to this space. The Service_queue_controller_task will be the only task to
	// read this var.
} service_req;

typedef struct ServiceQueueControl_parameter {
     QueueHandle_t m_service_request_message_queue; // IN from "listener" tasks.
     QueueHandle_t m_motor_message_queue; // OUT to motor control task.
     QueueHandle_t m_door_message_queue; // OUT to door control task.
}  ServiceQueueControl_parameter;


#endif	/* SERVICEQUEUECONTROLTASK_H */

