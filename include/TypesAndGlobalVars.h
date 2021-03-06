#ifndef TYPESANDGLOBALVARS_H
#define TYPESANDGLOBALVARS_H
#include "stdbool.h"
/////////////////////////////////////////////////////////////////////////////
// All common enum types, parameter structs, and global vars for inter-task
// Comunication
//////////////////////////////////////////////////////////////

static const float DISTANCE_FROM_GND_TO_P1 = 500;
static const float DISTANCE_FROM_P1_TO_P2 = 10;
static const float DISTANCE_FROM_GND_TO_P2 = 510;

//////////////////////// All Common Structs and Enums/////////////////////////////
////////////////For shoving inside message queues///////////////////////////
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
	ChangeMaxSpeedToN, //N will be an int packaged up with this bad boy
	ChangeMaxAccelToN,
                DoorWait,
} service_req;
//
//char [][] SERVICE_REQ
//{
//	"EmergStop",
//	"EmergClear",
//	"DoorInterference",
//	"OpenDoor",
//	"CloseDoor",
//	"CallToGNDInsideCar", // Requests inside the car get priority over requests outside the car.
//	"CallToP1InsideCar",
//	"CallToP2InsideCar",
//	"CallToGNDOutsideCar", //    treat both UART_TX requests for a floor outside a car and CLI
//	"CallToP1fromOutsideCar", //  requests for floor N as requests outside the car.
//	"CallToP2fromOutsideCar",
//	"ChangeMaxSpeedToN", // Before sending this message, update a global var  ChangeMaxAccelToN,
//	"ChangeMaxAccelToN"
//}


typedef struct _MotorMessage
{
        char state;
        float m_time_to_spend_in_accel;
	float m_time_to_spend_in_cruise;
	float m_time_to_spend_in_decel;
	bool m_emer_flag;
        bool m_start;
        bool m_up_true;
        float m_data;
}MotorMessage;

typedef struct ServiceQueueMessage
{
        service_req m_please_do_this;
        float m_data;
}ServiceQueueMessage;

///////////////////////////All Task Parameter Types////////////////////////
////////////////////For giving to Tasks///////////////////////////////////
typedef struct ServiceQueueControl_parameter {
     QueueHandle_t m_door_message_queue; // OUT to door control task.
     SemaphoreHandle_t m_motor_service_done; // IN from both motor and door control
     SemaphoreHandle_t m_door_service_done; // IN from both motor and door control
}  ServiceQueueControl_parameter;


typedef struct DummyListenerTask_parameter {
     QueueHandle_t m_service_request_message_queue; // OUT from "listener" tasks.
}  DummyListenerTask_parameter;

typedef struct MotorControl_parameter  {
    float m_current_speed;
    float m_current_distance;
} MotorControl_parameter;

typedef struct DoorControl_parameter  {
    //QueueHandle_t m_door_message_queue; // in from service queue
    SemaphoreHandle_t m_door_service_done;  // out to service queue
} DoorControl_parameter;


/////////////////////////






#endif