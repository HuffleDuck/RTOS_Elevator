#ifndef TYPESANDGLOBALVARS_H
#define TYPESANDGLOBALVARS_H
/////////////////////////////////////////////////////////////////////////////
// All common enum types, parameter structs, and global vars for inter-task
// Comunication
//
//////////////////////// All Common Structs and Enums/////////////////////////////
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
	ChangeMaxAccelToN,// "NEW_VEL_VALUE" with the new speed/acel. CLI will be the only
	// task to write to this space. The Service_queue_controller_task will be the only task to
	// read this var.
} service_req;

typedef struct ServiceQueueControl_parameter {
     QueueHandle_t m_service_request_message_queue; // IN from "listener" tasks.
     QueueHandle_t m_motor_message_queue; // OUT to motor control task.
     QueueHandle_t m_door_message_queue; // OUT to door control task.
}  ServiceQueueControl_parameter;


typedef struct DummyListenerTask_parameter {
     QueueHandle_t m_service_request_message_queue; // OUT from "listener" tasks.
}  DummyListenerTask_parameter;

typedef struct MotorControl_parameter  {
    QueueHandle_t m_motor_message_queue;
};
typedef struct DoorControl_parameter  {
    QueueHandle_t m_motor_message_queue;
};

typedef struct _MotorMessage
{
	char state[15];
	int m_time_to_spend_in_accel;
	int m_time_to_spend_in_cruise;
	int m_time_to_spend_in_decel;
	bool m_emer_flag;
        bool m_start;
}MotorMessage;
//////// Global Vars Here/////////////////////////////////////////////////////
// Cli dumps data here, serviceQueueControlTaskReads it.
static int NEW_VEL_VALUE;

//Current Emergancy State, ServiceQueueControlTask writes here, and MotorControl reads it.
static char EMER_FLAG;

// Motor Control writes. Service Control reads and sends as messages to UART_TX
static int CUR_SPEED;
static int CUR_DISTANCE;

/////////////////////////







#endif