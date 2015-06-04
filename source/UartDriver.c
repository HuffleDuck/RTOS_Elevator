#include <xc.h>
#include <sys/attribs.h>

#include <stdint.h>
#include <plib.h>


#include <UartDriver.h>

static TaskHandle_t rxTask = 0;
static TaskHandle_t txTask = 0;

static QueueHandle_t uartRxQueue = 0;
static QueueHandle_t uartTxQueue = 0;

static char uartIn = 0;
static char uartOut = 0;

static SemaphoreHandle_t uartRxMutex = 0;
static SemaphoreHandle_t uartTxMutex = 0;

#define MAX_INPUT_QUEUE     200
#define MAX_OUTPUT_QUEUE    20

/************************************************
 * UartISR: Unblocks tasks that manage the UART
 * driver when Rx/Tx signals are received
 ***********************************************/
void __ISR(_UART2_VECTOR, IPL2AUTO) UartISR(void) {
    static portBASE_TYPE taskSwitch;

    if (INTGetFlag(INT_U2RX))
    {
        INTEnable(INT_SOURCE_UART_RX(UART2), INT_DISABLED);
        INTClearFlag(INT_U2RX);
        uartIn = UARTGetDataByte(UART2);
        xSemaphoreGiveFromISR(uartRxMutex, &taskSwitch);
    }
    else
    {
        INTClearFlag(INT_U2TX);
        xSemaphoreGiveFromISR(uartTxMutex, &taskSwitch);
    }
    
    portEND_SWITCHING_ISR(taskSwitch);
}

/************************************************
 * taskTxUART: Listens for signals from ISR
 * and handles sending data out that is received
 * via queue
 ***********************************************/
static void taskTxUART(void * pParam) {
    char buffer[MAX_OUTPUT_QUEUE] = {0};
    char * isrOutString = 0;
    
    while (1)
    {
        isrOutString = buffer;
        if (xQueueReceive(uartTxQueue, &buffer, portMAX_DELAY))
        {
            while (*isrOutString != 0 && isrOutString < buffer + MAX_OUTPUT_QUEUE)
            {
                UARTSendDataByte(UART2, *isrOutString++);
                xSemaphoreTake(uartTxMutex, portMAX_DELAY);
            }
        }
    }
}

/************************************************
 * taskRxUART: Listens for signals from ISR
 * and handles parsing and receiving data
 ***********************************************/
static void taskRxUART(void * pParam) {
    static char rxMessage[MAX_INPUT_QUEUE] = {0};
    static char retMsg[2] = {0};
    uint8_t data = 0;
    char * strIter = rxMessage;

    while (1)
    {
        INTEnable(INT_SOURCE_UART_RX(UART2), INT_ENABLED);

        xSemaphoreTake(uartRxMutex, portMAX_DELAY);

        data = uartIn;

        if (data == '\r' || data == '\n')
        {
            *strIter = 0;
            if (strIter != rxMessage)
                xQueueSendToBack(uartRxQueue, rxMessage, 0);
            strIter = rxMessage;
        }
        else if (strIter != rxMessage + MAX_INPUT_QUEUE)
            (*strIter++) = data;

        *retMsg = data;
        UartMessageOut(retMsg);
    }
}

/************************************************
 * InitUartDriver: Sets statically defined data
 * required for the UartDriver to function.
 * This call is required prior to using any
 * API function
 ***********************************************/
void InitUartDriver()
{
    UARTConfigure(UART2, UART_ENABLE_PINS_TX_RX_ONLY);

    UARTSetFifoMode(UART2, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART2, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART2, (uint32_t)configPERIPHERAL_CLOCK_HZ, 9600);
    
    UARTEnable(UART2, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_TX | UART_RX));

    INTSetVectorPriority(INT_VECTOR_UART(UART2), INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_VECTOR_UART(UART2), INT_SUB_PRIORITY_LEVEL_0);
    //Read the status until it is clear
    while (U2STAbits.URXDA != 0);
    INTClearFlag(INT_U2RX);
    INTClearFlag(INT_U2TX);

    //UART task will enable this when ready
    INTEnable(INT_SOURCE_UART_RX(UART2), INT_DISABLED);
    INTEnable(INT_SOURCE_UART_TX(UART2), INT_ENABLED);

    if (rxTask == 0)
    {
        xTaskCreate(taskRxUART, "UART_RX", configMINIMAL_STACK_SIZE, NULL, 3, &rxTask);
        xTaskCreate(taskTxUART, "UART_TX", configMINIMAL_STACK_SIZE, NULL, 2, &txTask);

        uartRxMutex = xSemaphoreCreateMutex();
        xSemaphoreTake(uartRxMutex, portMAX_DELAY);
        uartTxMutex = xSemaphoreCreateMutex();
        xSemaphoreTake(uartTxMutex, portMAX_DELAY);

        uartTxQueue = xQueueCreate(5, MAX_OUTPUT_QUEUE);
        uartRxQueue = xQueueCreate(5, MAX_INPUT_QUEUE);
    }
}

/************************************************
 * GetUartReceiveQueue: Provides an interface for
 * non-API functions to listen to for messages
 * received through the UART
 ***********************************************/
QueueHandle_t GetUartReceiveQueue()
{
    return uartRxQueue;
}

/************************************************
 * UartMessageOut: Places a message into the
 * output queue. Will block if the queue is full
 * and continue once placed
 ***********************************************/
void UartMessageOut(char * string)
{
    char buffer[MAX_OUTPUT_QUEUE] = {0};

    int length = strlen(string);
    int numRuns = length / MAX_OUTPUT_QUEUE;
    int rem = length % MAX_OUTPUT_QUEUE;
    int i = 0;

    for (; i < numRuns; i++)
    {
        strncpy(buffer, &string[i * MAX_OUTPUT_QUEUE], MAX_OUTPUT_QUEUE);
        xQueueSendToBack(uartTxQueue, buffer, portMAX_DELAY);
    }
    if (rem > 0)
    {
        strcpy(buffer, &string[i * MAX_OUTPUT_QUEUE]);
        xQueueSendToBack(uartTxQueue, buffer, portMAX_DELAY);
    }
}