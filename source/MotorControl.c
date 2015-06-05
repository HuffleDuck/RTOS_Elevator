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
#include <TypesAndGlobalVars.h>


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



/*************************************************
 * State Machine that
 * controls the motor
 *************************************************/
static void Motor_Control_Task(void *pvParameters)
{
	MotorControl_parameter *parameters_for_you;
        parameters_for_you = (MotorControl_parameter *) pvParameters;


        MOTOR_STATES STATE = idle;
        MotorMessage temp;
        MotorMessage read_in;
        char char_curr_speed[50];
        char char_curr_distfrom_grnd[50];

        bool start = false;
	int i = 0; // count for seconds spend
	int speed = 0; // speed of the car
        int accel_t = 2;
        int distance_from_ground = 0;
        int MAX_SPEED = 20;
        bool up_true;

        temp.m_time_to_spend_in_accel = 5;
        temp.m_time_to_spend_in_cruise = 3;
        temp.m_time_to_spend_in_decel = 5;
        temp.m_up_true = false;

        read_in.state = 0x00;
        read_in.m_time_to_spend_in_accel  = 0;
        read_in.m_time_to_spend_in_cruise = 0;
        read_in.m_time_to_spend_in_decel = 0;
        read_in.m_up_true = false;
        read_in.m_emer_flag = false;

	while(1)
	{
            if (uxQueueMessagesWaiting(parameters_for_you->m_motor_message_queue) > 0)
            {
                xQueueReceive( parameters_for_you->m_motor_message_queue,
                            &read_in,
                            0);
                                // If state of the message is S,
                if ( read_in.state == 'S') // read in the new max speed.
                {
                    MAX_SPEED = read_in.m_time_to_spend_in_accel;
                    xSemaphoreGive(parameters_for_you->m_service_done);
                }
                else if (read_in.state == 'A') // if the state of the message is A
                {                           // read in the new acel.
                    accel_t  = read_in.m_time_to_spend_in_accel;
                    xSemaphoreGive(parameters_for_you->m_service_done);
                }
                else
                {
                    temp = read_in;
                    start = temp.m_start;
                    up_true = temp.m_up_true;
                }
            }

            switch(STATE)
            {
                case idle: //motor not moving
                speed = 0;
                parameters_for_you->m_current_speed = speed;
                PrintSpeedAndPosition(speed, distance_from_ground);
                vTaskDelay(1000/portTICK_PERIOD_MS);

                    
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
                    {
                        xSemaphoreGive(parameters_for_you->m_service_done);
                        STATE = idle;
                    }
                    if(speed  < MAX_SPEED)
                        speed = speed + accel_t;

                    if ( up_true)
                        distance_from_ground = distance_from_ground + speed;
                    else
                        distance_from_ground = distance_from_ground - speed;
                    parameters_for_you->m_current_speed = speed;
                    PrintSpeedAndPosition(speed, distance_from_ground);

                    vTaskDelay(1000/portTICK_PERIOD_MS);
                    //toggleLED(5);

                }
                STATE = cruise;

                break;

                case cruise: // motor maintains speed

                for(i = 0; i < (temp.m_time_to_spend_in_cruise); i++)
                {
                    //while m_time_to_spend
                    if ( temp.m_emer_flag)
                    {
                           xSemaphoreGive(parameters_for_you->m_service_done);
                           STATE = idle;
                    }
                    
                    if (up_true)
                        distance_from_ground = distance_from_ground + speed;
                    else
                        distance_from_ground = distance_from_ground - speed;

                    vTaskDelay( 1000/portTICK_PERIOD_MS  );
                    parameters_for_you->m_current_speed = speed;
                    PrintSpeedAndPosition(speed, distance_from_ground);
                    //toggleLED(5);
                }


                STATE = decel;

                break;

                case decel: // motor decreasing speed

                for(i = 0; i< temp.m_time_to_spend_in_decel; i++)
                {

                    if (temp.m_emer_flag)
                    {
                          xSemaphoreGive(parameters_for_you->m_service_done);
                           STATE = idle;
                    }



                    

                    vTaskDelay(1000/portTICK_PERIOD_MS);
                    if(speed > accel_t && speed > 1)
                    {
                        speed = speed - accel_t;
                        if (up_true)
                            distance_from_ground = distance_from_ground + speed;
                        else
                            distance_from_ground = distance_from_ground - speed;
                        parameters_for_you->m_current_speed = speed;
                        PrintSpeedAndPosition(speed, distance_from_ground);

                         //toggleLED(5);
                    }
                }

                xSemaphoreGive(parameters_for_you->m_service_done);
                STATE = idle;
                break;

                default:
                 xSemaphoreGive(parameters_for_you->m_service_done);
                STATE = idle;
                start = false;
                break;
            }
	}
}


void PrintSpeedAndPosition(int cur_speed, int cur_distance)
{
    char print_cur_speed[15], print_cur_dist[15];
    itoa(print_cur_speed,cur_speed,10);
    itoa(print_cur_dist, cur_distance,10);
    strcat(print_cur_speed," ft/s   ");
    strcat(print_cur_dist," ft \n\r");
    UartMessageOut(print_cur_speed);
    UartMessageOut(print_cur_dist);
};


static void Motor_LED_Toggle_Task(void *pvParameters)
{
    MotorControl_parameter *parameters_for_you;
    parameters_for_you = (MotorControl_parameter *) pvParameters;
    float toggle_rate = 0;
    float cur_speed = 0;
    while (1)
    {

        cur_speed = parameters_for_you->m_current_speed;
        if (cur_speed > 0)
        {
            toggle_rate = (cur_speed / 10) * 1000;
            toggle_rate = 1/ toggle_rate;
            vTaskDelay(toggle_rate/ portTICK_PERIOD_MS);
            toggleLED(5);
        }
        else
           setLED(5, 0);

    }

}
void InitMotorControl(void *pvParameters)
{
    MotorControl_parameter *parameters_for_you;
    parameters_for_you = (MotorControl_parameter *) pvParameters;

    xTaskCreate(   Motor_Control_Task,
                    "MotorControlTask",
                    configMINIMAL_STACK_SIZE,
                    (void *) parameters_for_you,
                    1,
                    &MotorControlTask);
                    
//    xTaskCreate( Motor_LED_Toggle_Task,
//                    "MotorLEDToggleTask",
//                    configMINIMAL_STACK_SIZE,
//                    (void *) parameters_for_you,
//                    1,
//                    NULL);
}
