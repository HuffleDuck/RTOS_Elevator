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
static const float RES_FREQ = 16;
static const float RES_PERIOD = 0.0625;
static const int RES_DELAY = 75;

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


QueueHandle_t motor_control_queue;
float speed = 0; // speed of the car
float distance_from_ground = 0;
/*************************************************
 * State Machine that
 * controls the motor
 *************************************************/
static void Motor_Control_Task()
{

        motor_control_queue = xQueueCreate(20, sizeof(MotorMessage));
        MOTOR_STATES STATE = idle;
        MotorMessage temp;
        MotorMessage read_in;
        char char_curr_speed[50];
        char char_curr_distfrom_grnd[50];

        bool start = false;
	float i = 0; // count for seconds spend
	
        float accel_t = 2;

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
            if (uxQueueMessagesWaiting(motor_control_queue) > 0)
            {
                xQueueReceive( motor_control_queue,
                            &read_in,
                            0);
                                // If state of the message is S,
                if ( read_in.state == 'S') // read in the new max speed.
                {
                    MAX_SPEED = read_in.m_data;
                    SignalMotorDone();
                }
                else if (read_in.state == 'A') // if the state of the message is A
                {                           // read in the new acel.
                    accel_t  = read_in.m_data;
                    if (accel_t > MAX_SPEED)
                        accel_t = MAX_SPEED;
                    SignalMotorDone();
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
                        SignalMotorDone();
                        STATE = idle;
                    }
                    if(speed  < MAX_SPEED)
                        speed = speed + (accel_t*RES_PERIOD);
                    else
                    {
                        speed = MAX_SPEED;
                        UartMessageOut("Accel Error! Adjusting!");
                    }
                    if ( up_true)
                        distance_from_ground = distance_from_ground + (speed*RES_PERIOD);
                    else
                        distance_from_ground = distance_from_ground - (speed*RES_PERIOD);
                    //PrintSpeedAndPosition(speed, distance_from_ground);

                    vTaskDelay(RES_DELAY/portTICK_PERIOD_MS);
                    //toggleLED(5);

                }
                STATE = cruise;

                break;

                case cruise: // motor maintains speed
                    if (speed != MAX_SPEED)
                    {
                        UartMessageOut("Idel Error! Adjusting!");
                        speed = MAX_SPEED;
                    }
                    i = 0;
                while( i < temp.m_time_to_spend_in_cruise)
                {

                     i += RES_PERIOD;
                    //while m_time_to_spend
                    if ( temp.m_emer_flag)
                    {
                           SignalMotorDone();
                           STATE = idle;
                    }
                    
                    if (up_true)
                        distance_from_ground = distance_from_ground + (speed*RES_PERIOD);
                    else
                        distance_from_ground = distance_from_ground - (speed*RES_PERIOD);

                    vTaskDelay( RES_DELAY/portTICK_PERIOD_MS  );

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
                           SignalMotorDone();
                           STATE = idle;
                    }



                   
                    vTaskDelay(RES_DELAY/portTICK_PERIOD_MS);

                        // Try this approach maybe?
//                    if (speed > 0)
//                    {
//                        speed = speed - (accel_t*RES_PERIOD);
//                        if (up_true)
//                            distance_from_ground = distance_from_ground + (speed*RES_PERIOD);
//                        else
//                            distance_from_ground = distance_from_ground - (speed*RES_PERIOD);
//                        parameters_for_you->m_current_speed = speed;
//                        parameters_for_you->m_current_distance = distance_from_ground;
//                    }
//                    else
//                    {
//                        speed = 0;
//                        UartMessageOut("Decel Error! Adjusting!");
//                    }
                    
                    if(speed > accel_t && speed > 1) ///?
                    {
                        speed = speed - (accel_t*RES_PERIOD);
                        if (up_true)
                            distance_from_ground = distance_from_ground + (speed*RES_PERIOD);
                        else
                            distance_from_ground = distance_from_ground - (speed*RES_PERIOD);



                    }
                    else
                    {
                        speed = 0;
                        UartMessageOut("Decel Error! Adjusting!");
                    }
                }

                SignalMotorDone();
                STATE = idle;
                start = false;
                break;

                default:
                 SignalMotorDone();
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

static void PrintSpeedAndPositionTask()
{

    float toggle_rate = 0;
    float cur_speed = 0;
    float cur_dist = 0;
    while(1)
    {
        cur_speed = speed;
        cur_dist = distance_from_ground;
        if (cur_speed > 0)
        {
            PrintSpeedAndPosition(cur_speed, cur_dist);
        }
        vTaskDelay(500/ portTICK_PERIOD_MS);
    }
}

static void Motor_LED_Toggle_Task()
{

    float toggle_rate = 0;
    float cur_speed = 0;
    while (1)
    {

        cur_speed = speed;
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

bool SendMessageToMotor( char state, float time_to_acel, float time_to_cruise, float time_to_decel,
                bool emerg_flag, bool start, bool up_true, float data_to_go_with_request)
{

     MotorMessage send_this = { state,
                                time_to_acel,
                                time_to_cruise,
                                time_to_decel,
                                emerg_flag,
                                start,
                                up_true,
                                data_to_go_with_request};
     return xQueueSendToBack(motor_control_queue, &send_this, 0);
    //return  xQueueSendToBack (motor_control_queue,
                            //&send_this,  0 );
}

void InitMotorControl()
{

    xTaskCreate(   Motor_Control_Task,
                    "MotorControlTask",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    1,
                    &MotorControlTask);

    xTaskCreate(   PrintSpeedAndPositionTask,
                    "MotorPrintTask",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    1,
                    NULL);

    xTaskCreate( Motor_LED_Toggle_Task,
                    "MotorLEDToggleTask",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    1,
                    NULL);
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