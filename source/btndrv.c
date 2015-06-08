/*
Explorer 16 Buttons
SW1 – P2 Call button inside car
SW2 – P1 Call button inside car
SW3 – Open Door inside car
SW4 – Close Door inside car
*/

static QueueHandle_t btnQueue;

uint8_t initializeBTNDriver(void)
{
	service_req message;
	btnQueue =  xQueueCreate(5,sizeof(message));
	//ports d as input should be dedfault
}

/*
pass in a string and front of button queue will get copied in there
*/
void readBTN(char * btnval)
{
	service_req Queuerecive;
	if(xQueueReceive( btnQueue , ((void*) &Queuerecive ) ,( TickType_t10)
		btnval = Queuerecive;
}

static void BTNtask(void)
{
	//uh i am bad please add the other button
	 if ( (!mPORTDReadBits( BIT_6 ) | !mPORTDReadBits( BIT_7 ) | !mPORTDReadBits( BIT_13 )) )
        {
			Delay_ms(100);
			
			vTaskDelay( xDelay10ms );

              if( !mPORTDReadBits(BIT_6 ))
              {
                  vTaskDelay( xDelay10ms );
                  //decrease led delay time
                  message = CallToP1InsideCar;
                  xQueueSendToBack(btnQueue, (void*)&message,( TickType_t ) 0);
                  while(!mPORTDReadBits(BIT_6));
              }
              if( !mPORTDReadBits(BIT_7 ))
              {
                  vTaskDelay( xDelay10ms );
                  //increase led delay time
                  message = CallToP2InsideCar;
                  xQueueSendToBack(btnQueue, (void*)&message,( TickType_t ) 0);
                  while(!mPORTDReadBits(BIT_7));
              }
              if( !mPORTDReadBits(BIT_13 ))
              {
                    vTaskDelay( xDelay10ms );
					message = OpenDoor;
					xQueueSendToBack(btnQueue, (void*)&message,( TickType_t ) 0);
                    while(!mPORTDReadBits(BIT_13));
			  }
			  
			    if( !mPORTDReadBits(/*BIT_ */))
              {
                    vTaskDelay( xDelay10ms );
					message = CloseDoor;
					xQueueSendToBack(btnQueue, (void*)&message,( TickType_t ) 0);
                    while(!mPORTDReadBits(BIT_13));
			  }
		}
	
}