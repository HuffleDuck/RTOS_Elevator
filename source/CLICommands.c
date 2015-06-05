#include "CLICommands.h"
#include "UartDriver.h"

const xCommandLineInput inputCommands[] = {
    {"ES", "ES: Emergency stop via CLI\r\n", EmergencyStop, 0},
    {"ER", "ER: Emergency clear via CLI\r\n", EmergencyClear, 0},
    {"AP", "AP [n]: Change acceleration in ft/s^2\r\n", ChangeAccel, 1},
    {"S", "S [n]: Change maximum speed in ft/s\r\n", ChangeMaxSpeed, 1},
    {"SF", "SF [n]: Send car to floor\r\n", SendToFloor, 1},
    {"TS", "TS: Display task stats\r\n", TaskStats, 0},
    {"RTS", "RTS: Display runtime stats\r\n", RunTimeStats, 0},
    {"q", "q: Ground floor call outside car\r\n", CallToGround, 0},
    {"w", "w: P1 DN floor call outside car\r\n", CallToP1, 0},
    {"e", "e: P1 UP floor call outside car\r\n", CallToP1, 0},
    {"r", "r: P2 floor call outside car\r\n", CallToP2, 0},
    {"t", "t: Ground floor call inside car\r\n", CallToGround, 0},
    {"y", "y: Emergency stop inside car\r\n", EmergencyStop, 0},
    {"u", "u: Emergency stop inside car\r\n", EmergencyClear, 0},
    {"i", "i: Door Interference\r\n", DoorInterference, 0}
};

static void taskCLI(void * vpParams)
{
    QueueHandle_t uartInQueue = GetUartReceiveQueue();
    
    char buffer[200] = {0};
    char outString[200] = {0};
    char resume = 0;

    while (1)
    {
        if (xQueueReceive(uartInQueue, &buffer, portMAX_DELAY))
        {
            do {
                resume = FreeRTOS_CLIProcessCommand(buffer, outString, 200);
                UartMessageOut(outString);
            } while(resume);
        }
    }
}


CLI_FORMAT(EmergencyStop)
{
    
}

CLI_FORMAT(EmergencyClear)
{

}

CLI_FORMAT(DoorInterference)
{
	
}

CLI_FORMAT(OpenDoor)
{

}

CLI_FORMAT(CloseDoor)
{
	
}

CLI_FORMAT(CallToGround)
{
	
}

CLI_FORMAT(CallToP1)
{
	
}

CLI_FORMAT(CallToP2)
{
	
}

CLI_FORMAT(ChangeMaxSpeed)
{
	
}

CLI_FORMAT(ChangeAccel)
{
	
}

CLI_FORMAT(SendToFloor)
{
	
}

CLI_FORMAT(TaskStats)
{
	
}

CLI_FORMAT(RunTimeStats)
{
	
}

void InitCLISystem()
{
    int sizeArray = sizeof(inputCommands) / sizeof(xCommandLineInput);
    int i = 0;

	xTaskCreate(taskCLI, "CLI", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	
    for (i = 0; i < sizeArray; i++)
    {
        FreeRTOS_CLIRegisterCommand(inputCommands + i);
    }
}
