#include "btndrv.h"
/*
Explorer 16 Buttons
SW1 – P2 Call button inside car
SW2 – P1 Call button inside car
SW3 – Open Door inside car
SW4 – Close Door inside car
*/

static QueueHandle_t btnQueue;



/*
pass in a string and front of button queue will get copied in there
*/
void readBTN(service_req btnval)
{
	service_req Queuerecive;
	if(xQueueReceive( btnQueue , (void*) &Queuerecive  , (10/ portTICK_RATE_MS)))
		btnval = Queuerecive;
}

static void BTNtask(void)
{
	//uh i am bad please add the other button
    service_req message = 0;
    while (1)
    {
	 if ( (!mPORTDReadBits( BIT_6 ) |
                 !mPORTDReadBits( BIT_7 ) |
                 !mPORTDReadBits( BIT_13 ) |
                 !mPORTAReadBits(BIT_7) ))
        {
			vTaskDelay(10/ portTICK_RATE_MS);
                         //Delay_ms(100);
			//vTaskDelay( xDelay10ms );

              if( !mPORTDReadBits(BIT_6 ))
              {
                  vTaskDelay(10/ portTICK_RATE_MS);
                  //decrease led delay time
                  //message = CallToP1InsideCar;
                  //xQueueSendToBack(btnQueue, (void*)&message,( TickType_t ) 0);
                  QueueServiceRequest( CallToP1InsideCar, 0);
                  while(!mPORTDReadBits(BIT_6));
              }
              if( !mPORTDReadBits(BIT_7 ))
              {
                 vTaskDelay(10/ portTICK_RATE_MS);
                  //increase led delay time
                  //message = CallToP2InsideCar;
                  //xQueueSendToBack(btnQueue, (void*)&message,( TickType_t ) 0);
                 QueueServiceRequest( CallToP2InsideCar, 0);
                 while(!mPORTDReadBits(BIT_7));
              }
              if( !mPORTDReadBits(BIT_13 ))
              {
                   vTaskDelay(10/ portTICK_RATE_MS);
//					message = OpenDoor;
//					xQueueSendToBack(btnQueue, (void*)&message,( TickType_t ) 0);
                   QueueServiceRequest( OpenDoor, 0);
                    while(!mPORTDReadBits(BIT_13));
            }
			  
	     if( !mPORTAReadBits(BIT_7))
              {
                    vTaskDelay(10/ portTICK_RATE_MS);
					message = CloseDoor;
					xQueueSendToBack(btnQueue, (void*)&message,( TickType_t ) 0);
                                       QueueServiceRequest( CloseDoor, 0);
                    while(!mPORTAReadBits(BIT_7));
             }
        }
    }
	
}

uint8_t initializeBTNDriver(void)
{
	service_req message;
	btnQueue =  xQueueCreate(5,sizeof(message));
	//ports d as input should be dedfault

            xTaskCreate(BTNtask,
               "BTN_task",
            configMINIMAL_STACK_SIZE,
            NULL,
            1, NULL);
            // Configure that one port a as input.

       mPORTAOpenDrainOpen(BIT_7);
       mPORTASetPinsDigitalIn(  BIT_7  );

}