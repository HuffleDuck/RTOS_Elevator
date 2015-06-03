#include "leddrv.h"

uint8_t initializeLedDriver(void)
{
    uint8_t success = 1;


    ODCACLR = 0xFF;
    TRISACLR = 0xFF;
    PORTA = 0x00;

 
    success = 0;
    

    return success;
}
uint8_t readLed(uint8_t ledNum)
{
    uint8_t success = 2;
    uint8_t TEMPread = 0;
    if(ledNum == 1)
    {
        if(LATA & 0x01)
           success =  1;
        if(LATA ^ 0x01)
            success = 0;
    }
    if(ledNum == 2)
    {
        if(LATA & 0x02)
           success =  1;
        if(LATA ^ 0x02)
            success = 0;
    }
    if(ledNum == 3)
    {
        if(LATA & 0x04)
           success =  1;
        if(LATA ^ 0x04)
            success = 0;
    }
	if(ledNum == 7)
    {
        if(LATA & 0x40)
           success =  1;
        if(LATA ^ 0x40)
            success = 0;
    }
	if(ledNum == 8)
    {
        if(LATA & 0x80)
           success =  1;
        if(LATA ^ 0x80)
            success = 0;
    }
	
    return success;
}
//sets the state of the LED 
// (LED to set, 1 == on)
uint8_t setLED(uint8_t ledNum, uint8_t value)
{
    uint8_t success = 1;

      if( ledNum == 1)
    {
          if(value == 0)//off
          {
              PORTACLR = 0x01;
              success = 0;
          }
          else if(value == 1)//on
          {
             PORTASET = 0x01;
             success = 0;
          }
    }
    else if ( ledNum == 2)
    {
        if(value == 0)
          {
            PORTACLR = 0x02;
            success = 0;
          }

        else if(value == 1)
          {
             PORTASET = 0x02;
             success = 0;
          }
    }
    else if( ledNum == 3)
    {    if(value == 0)
          {
              PORTACLR = 0x04;
              success = 0;
          }
          else if(value == 1)
          {
             PORTASET = 0x04;
             success = 0;
          }
    }
	else if( ledNum == 4)
	{    if(value == 0)
		  {
			  PORTACLR = 0x08;
			  success = 0;
		  }
		  else if(value == 1)
		  {
			 PORTASET = 0x08;
			 success = 0;
		  }
	}
	else if( ledNum == 5)
	{    if(value == 0)
		  {
			  PORTACLR = 0x10;
			  success = 0;
		  }
		  else if(value == 1)
		  {
			 PORTASET = 0x10;
			 success = 0;
		  }
	}
	else if( ledNum == 6)
	{    if(value == 0)
		  {
			  PORTACLR = 0x20;
			  success = 0;
		  }
		  else if(value == 1)
		  {
			 PORTASET = 0x20;
			 success = 0;
		  }
	}
	else if( ledNum == 7)
	{    if(value == 0)
		  {
			  PORTACLR = 0x40;
			  success = 0;
		  }
		  else if(value == 1)
		  {
			 PORTASET = 0x40;
			 success = 0;
		  }
	}
	else if( ledNum == 8)
    {    if(value == 0)
          {
              PORTACLR = 0x80;
              success = 0;
          }
          else if(value == 1)
          {
             PORTASET = 0x80;
             success = 0;
          }
    }
    return success;
}
//toggles the state of the LED passed in
//returns zero if successful
uint8_t toggleLED(uint8_t ledNum)
{
    uint8_t success = 1;

    if( ledNum == 1)
    {
        PORTAINV = 0x01;
        success = 0;
    }
    else if ( ledNum == 2)
    {   PORTAINV = 0x02;
        success = 0;
    }
    else if( ledNum == 3)
    {   PORTAINV = 0x04;
        success = 0;
    }
	else if( ledNum == 4)
    {   PORTAINV = 0x08;
        success = 0;
    }
	else if( ledNum == 5)
    {   PORTAINV = 0x10;
        success = 0;
    }
	else if( ledNum == 6)
    {   PORTAINV = 0x20;
        success = 0;
    }
	else if( ledNum == 7)
    {   PORTAINV = 0x40;
        success = 0;
    }
	else if( ledNum == 8)
    {   PORTAINV = 0x80;
        success = 0;
    }
    return success;
    
}