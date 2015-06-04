#ifndef UARTDRIVER_H
#define	UARTDRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <CommonIncludes.h>



    void InitUartDriver();
    void UartMessageOut(char * str);
    QueueHandle_t GetUartReceiveQueue();
    
#ifdef	__cplusplus
}
#endif

#endif	/* UARTDRIVER_H */

