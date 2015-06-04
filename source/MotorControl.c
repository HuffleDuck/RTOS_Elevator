#include <xc.h>
#include <sys/attribs.h>

#include <stdint.h>
#include <plib.h>
#include <leddrv.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <CommonIncludes.h>

#include <UartDriver.h>
#include <MotorControl.h>

const int MAX_SPEED = 20;
static TaskHandle_t MotorControlTask;

/*enum STATES
{
	stopped = 0,
	accel_up,
	cruise_up,
	decel_up,
	accel_dn,
	cruise_dn,
	decel_dn
}*/

typedef enum _MOTOR_STATES
{
    idle,
    accel,
    cruise,
    decel,

} MOTOR_STATES;

typedef struct _State
{
	MOTOR_STATES state;
	int m_time_to_spend_in_accel;
	int m_time_to_spend_in_cruise;
	int m_time_to_spend_in_decel;
	bool m_emer_flag;
        bool m_start;
}State;

/*************************************************
 * State Machine that
 * controls the motor
 *************************************************/
static void Motor_Control_Task(void *pvParameters)
{
	MOTOR_STATES STATE = idle;
        State temp;
        char char_curr_speed[50];
        char char_curr_distfrom_grnd[50];
        bool start = true;
	int i = 0; // count for seconds spend
	int speed = 0; // speed of the car
        int accel_t = 2;

        temp.m_time_to_spend_in_accel = 5;
        temp.m_time_to_spend_in_cruise = 3;
        temp.m_time_to_spend_in_decel = 5;

	while(1)
	{

            switch(STATE)
            {
                case idle: //motor not moving
                speed = 0;
                itoa(char_curr_speed,speed,10);
                strcat(char_curr_speed," ft/s \n\r");
                UartMessageOut(char_curr_speed);
                    
                if (temp.m_emer_flag)
                    STATE  = STATE;
                if(start)
                    STATE = accel;
                else
                    STATE = idle;
                break;

                case accel: // motor speeding up

                //while m_time_to_spend
                for(i = 0; i < (temp.m_time_to_spend_in_accel); i++)
                {
                    if (temp.m_emer_flag)
                        STATE = idle;

                    if(speed  < MAX_SPEED)
                        speed = speed + accel_t;
                    itoa(char_curr_speed,speed,10);
                    strcat(char_curr_speed," ft/s \n\r");
                    UartMessageOut(char_curr_speed);

                    vTaskDelay(1000/portTICK_PERIOD_MS);
                    toggleLED(5);
                    toggleLED(5);
                }
                STATE = cruise;

                break;

                case cruise: // motor maintains speed
                setLED(5,0); //testing states

                setLED(7,1);//testing states
                for(i = 0; i < (temp.m_time_to_spend_in_cruise); i++)
                {
                    //while m_time_to_spend
                    if (temp.m_emer_flag)
                           STATE = idle;
                    vTaskDelay( 1000/portTICK_PERIOD_MS  );
                    itoa(char_curr_speed,speed,10);
                    strcat(char_curr_speed," ft/s \n\r");
                    UartMessageOut(char_curr_speed);
                }


                STATE = decel;

                break;

                case decel: // motor decreasing speed
                setLED(7,0);//testing states
                //while m_time_to_spend
                for(i = 0; i< temp.m_time_to_spend_in_decel; i++)
                {
                    toggleLED(5);
                    if (temp.m_emer_flag)
                           STATE = idle;

                    vTaskDelay(1000/portTICK_PERIOD_MS);
                    if(speed > accel_t && speed > 1)
                    {
                        speed = speed - accel_t;
                        itoa(char_curr_speed,speed,10);
                        strcat(char_curr_speed," ft/s \n\r");
                        UartMessageOut(char_curr_speed);
                    }
                     toggleLED(5);
                }
                STATE = idle;
                break;

                default:
                STATE = idle;
                start = false;
                break;
            }
	}
}

void InitMotorControl(void)
{
    xTaskCreate(   Motor_Control_Task,
                    "MotorControlTask",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    1,
                    &MotorControlTask);

}
