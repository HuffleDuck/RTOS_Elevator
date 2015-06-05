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
static const float RES_FREQ = 8;
static const float RES_PERIOD = 0.125;
static const int RES_DELAY = 150;

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
	float i = 0; // count for seconds spend
	float speed = 0; // speed of the car
        float accel_t = 2;
        float distance_from_ground = 0;
        float MAX_SPEED = 20;
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
                    MAX_SPEED = read_in.m_data;
                    xSemaphoreGive(parameters_for_you->m_service_done);
                }
                else if (read_in.state == 'A') // if the state of the message is A
                {                           // read in the new acel.
                    accel_t  = read_in.m_data;
                    xSemaphoreGive(parameters_for_you->m_service_done);
                }
                else if(read_in.state == 'D')
                {
                    distance_from_ground = read_in.m_data;
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
                parameters_for_you->m_current_distance = distance_from_ground;
                //PrintSpeedAndPosition(speed, distance_from_ground);
                vTaskDelay(RES_DELAY/portTICK_PERIOD_MS);

                    
                if (temp.m_emer_flag)
                    STATE  = STATE;
                if(start)
                    STATE = accel;
                else
                    STATE = idle;
                break;

                case accel: // motor speeding up

                //while m_time_to_spend
                i = 0;
                while( i < temp.m_time_to_spend_in_accel)
                {
                     i += RES_PERIOD;
                    if (temp.m_emer_flag)
                    {
                        xSemaphoreGive(parameters_for_you->m_service_done);
                        STATE = idle;
                    }
                    if(speed  < MAX_SPEED)
                        speed = speed + (accel_t*RES_PERIOD);

                    if ( up_true)
                        distance_from_ground = distance_from_ground + (speed*RES_PERIOD);
                    else
                        distance_from_ground = distance_from_ground - (speed*RES_PERIOD);
                    parameters_for_you->m_current_speed = speed;
                    parameters_for_you->m_current_distance = distance_from_ground;
                    //PrintSpeedAndPosition(speed, distance_from_ground);

                    vTaskDelay(RES_DELAY/portTICK_PERIOD_MS);
                    //toggleLED(5);

                }
                STATE = cruise;

                break;

                case cruise: // motor maintains speed
                    i = 0;
                while( i < temp.m_time_to_spend_in_cruise)
                {

                     i += RES_PERIOD;
                    //while m_time_to_spend
                    if ( temp.m_emer_flag)
                    {
                           xSemaphoreGive(parameters_for_you->m_service_done);
                           STATE = idle;
                    }
                    
                    if (up_true)
                        distance_from_ground = distance_from_ground + (speed*RES_PERIOD);
                    else
                        distance_from_ground = distance_from_ground - (speed*RES_PERIOD);

                    vTaskDelay( RES_DELAY/portTICK_PERIOD_MS  );
                    parameters_for_you->m_current_speed = speed;
                    parameters_for_you->m_current_distance = distance_from_ground;
                    //toggleLED(5);
                }


                STATE = decel;

                break;

                case decel: // motor decreasing speed
                    i = 0;
                while( i< temp.m_time_to_spend_in_decel)
                {
                    i += RES_PERIOD;
                    if (temp.m_emer_flag)
                    {
                          xSemaphoreGive(parameters_for_you->m_service_done);
                           STATE = idle;
                    }



                    

                    vTaskDelay(RES_DELAY/portTICK_PERIOD_MS);
                    if(speed > accel_t && speed > 1)
                    {
                        speed = speed - (accel_t*RES_PERIOD);
                        if (up_true)
                            distance_from_ground = distance_from_ground + (speed*RES_PERIOD);
                        else
                            distance_from_ground = distance_from_ground - (speed*RES_PERIOD);
                        parameters_for_you->m_current_speed = speed;
                         parameters_for_you->m_current_distance = distance_from_ground;
                        //PrintSpeedAndPosition(speed, distance_from_ground);

                         //toggleLED(5);
                    }
                }

                xSemaphoreGive(parameters_for_you->m_service_done);
                STATE = idle;
                start = false;
                break;

                default:
                 xSemaphoreGive(parameters_for_you->m_service_done);
                STATE = idle;
                start = false;
                break;
            }
	}
}


void PrintSpeedAndPosition(float cur_speed, float cur_distance)
{
    char print_cur_speed[20];
    char print_cur_dist[20];
    sprintf(print_cur_speed, "%f", cur_speed);
    sprintf(print_cur_dist, "%f", cur_distance);
    strcat(print_cur_speed," ft/s   ");
    strcat(print_cur_dist, " ft \n\r");
    UartMessageOut(print_cur_speed);
    UartMessageOut(print_cur_dist);
};

static void PrintSpeedAndPositionTask(void *pvParameters)
{
    MotorControl_parameter *parameters_for_you;
    parameters_for_you = (MotorControl_parameter *) pvParameters;
    float toggle_rate = 0;
    float cur_speed = 0;
    float cur_dist = 0;
    while(1)
    {
        cur_speed = parameters_for_you->m_current_speed;
        cur_dist = parameters_for_you->m_current_distance;
        if (cur_speed > 0)
        {
            PrintSpeedAndPosition(cur_speed, cur_dist);
        }
        vTaskDelay(500/ portTICK_PERIOD_MS);
    }
}

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
//
//int myRound(float round_this)
//{
//    float temp = round_this;
//    int return_this = round_this;
//    temp = floor(temp);  // drop the decimals
//    temp = round_this - temp; // nothin' but the decimals.
//
//
//    if (temp >= 0.5 )
//        return_this = ceil(round_this);
//    else
//        return_this = floor(round_this);
//
//    return return_this;
//}

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

    xTaskCreate(   PrintSpeedAndPositionTask,
                    "MotorPrintTask",
                    configMINIMAL_STACK_SIZE,
                    (void *) parameters_for_you,
                    1,
                    NULL);

//    xTaskCreate( Motor_LED_Toggle_Task,
//                    "MotorLEDToggleTask",
//                    configMINIMAL_STACK_SIZE,
//                    (void *) parameters_for_you,
//                    1,
//                    NULL);
}
