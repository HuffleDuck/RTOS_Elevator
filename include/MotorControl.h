#define MOTORCONTROL
#ifndef MOTORCONTROL

enum STATES
{
	stopped = 0,
	accel_up,
	cruise_up,
	decel_up,
	accel_dn,
	cruise_dn,
	decel_dn
}

enum MOTOR_STATES
{
	idle,
	accel,
	cruise,
	decel,

}

struct State
{
	STATES state;
	int m_time_to_spend;
	bool m_emer_flag;
};

static State curr_state;
static

int GetFloordistance()
{
}

static void Motor_Control_Task(void)
{
	MOTOR_STATES STATE; 
	int i = 0; // count for seconds spend
	int speed = 0; // speed of the car
	 
	while(1)
	{

		switch(STATE)
		case idle: //motor not moving
		speed = 0;
		break;
		
		case accel: // motor speeding up
		
		//while m_time_to_spend
		for(i = 0; i< State->m_time_to_spend; i++)
		{
			if(speed  <= MAX_SPEED)
			{
				speed = speed + accel;
			}
			VTaskDelay(1000/portTICK_PERIOD_MS);
		}
		
		break;
		
		case cruise: // motor maintains speed
		
		//while m_time_to_spend
		vTaskDelay(State->m_time_to_spend);
		
		break;
		
		case:decel: // motor decreasing speed 
		
		//while m_time_to_spend
		for(i = 0; i< State->m_time_to_spend; i++)
		{
			VTaskDelay(1000/portTICK_PERIOD_MS);
			if(speed != accel)
			{
				speed = speed - accel;
			}
		}
		
		break;
		
	}
}
#endif