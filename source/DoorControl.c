#include <DoorControl.h>

#include <xc.h>
#include <sys/attribs.h>

#include <stdint.h>
#include <plib.h>

#include <CommonIncludes.h>
#include<TypesAndGlobalVars.h>

static TaskHandle_t DoorControlTaskID = 0;
static QueueHandle_t m_door_message_queue = 0;

#define MAX


typedef enum _DoorState 
{
    closed,
    open,
    opening,
    closing,
    emergency,
    

}DoorState;
/*********************************************
 *Contrls the door's opening and closing
 * STATES:
 *   closed - door will stay closed until otherwise told
 *   open - door will stay open for 5 seconds
 *  opening - door will open and go to open state unless it is an emergency
 *              which then will og to emergency state
 *  closing - goes straight to closed state when finished
 *  emergency - stays in emergency mode until emergency is cleared
 ********************************************/

static void DoorControlTask()
{

    DoorState curr_doorstate = closed;
    DoorState prev_doorstate = closed;
    char doormsg[15];
    int i = 0;

    SignalDoorDone();
    while(1)
    {
        switch(curr_doorstate)
        {
            case closed:
                setLED(1,1);
                setLED(2,1);
                setLED(3,1);
                setLED(4,1);

                if( xQueueReceive( m_door_message_queue , ((void*) &doormsg ) ,( TickType_t ) 10   ) )
                {
                    if(strcmp(doormsg,"EmergStop") == 0)
                    {
                        curr_doorstate = opening;
                    }
                    else if(strcmp(doormsg,"OpenDoor") == 0)
                    {
                        curr_doorstate = opening;
                    }
                    else if(strcmp(doormsg,"CloseDoor") == 0)
                    {
                        curr_doorstate = curr_doorstate;
                    }

                    prev_doorstate = closed;
                }
                SignalDoorDone();
                break;
            case open:
                setLED(1,0);
                setLED(2,0);
                setLED(3,0);
                setLED(4,0);
                
                //need door interference

                //if(prev_doorstate == opening)
                //{
                     //vTaskDelay(5000/portTICK_PERIOD_MS);
                     //curr_doorstate = closing;
                //}
                
                    if(strcmp(doormsg,"EmergStop") == 0)
                    {
                        curr_doorstate = closing;
                    }
                    else if(strcmp(doormsg,"OpenDoor") == 0)
                    {
                        curr_doorstate = curr_doorstate;
                    }
                    else if(strcmp(doormsg,"CloseDoor") == 0)
                    {
                        curr_doorstate = closing;
                    }



                if( xQueueReceive( m_door_message_queue , ((void*) &doormsg ) ,( TickType_t ) 10   ) )
                {
                    if(strcmp(doormsg,"EmergStop") == 0)
                    {
                        curr_doorstate = emergency;
                    }
                }

                prev_doorstate = open;
                SignalDoorDone();
                break;
            case opening:
                SignalJustKiddingDoorNotDone();
                
                for(i = 1; i <= 4; i++)
                {
                    vTaskDelay(500/portTICK_PERIOD_MS);
                    setLED(i,0);
                }

                if(strcmp(doormsg,"EmergStop") == 0)
                    curr_doorstate = emergency;
                
                else
                    curr_doorstate = open;

                 prev_doorstate = opening;
                 // SignalDoorDone();
                break;
                
            case closing:

                 for( i = 4; i > 0; i--)
                {
                    vTaskDelay(500/portTICK_PERIOD_MS);
                    setLED(i,1);
                }

                curr_doorstate = closed;
                prev_doorstate = curr_doorstate;
               // SignalDoorDone();
                
                break;

            case emergency:
                setLED(1,0);
                setLED(2,0);
                setLED(3,0);
                setLED(4,0);
                
                if( xQueueReceive( m_door_message_queue , ((void*) &doormsg ) ,( TickType_t ) 10   ) )
                {
                     if(strcmp(doormsg,"EmergClear") == 0)
                     {
                        curr_doorstate = closing;
                     }

                     else
                         curr_doorstate = emergency;
                }   
                
                break;
                
            default:
                curr_doorstate = open;
                break;

        }
    }
   
}
/*********************************************
 * initialzes the door control state machine
 * will build a messege queue to communicate with
 *
 ********************************************/
void InitDOORControl()
{

    char msg[25];

     m_door_message_queue = xQueueCreate(20,sizeof(msg));
     xTaskCreate(   DoorControlTask,
                    "DoorControlTask",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    1,
                    DoorControlTaskID);
}
/*********************************************
 * Used to send commands to door control
 * this will lose messages if it is full
 *
 ********************************************/
void SendToDoorControl(char * string)
{
     xQueueSendToBack(m_door_message_queue, string, ( TickType_t ) 0);
}
