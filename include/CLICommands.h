#ifndef CLICOMMANDS_H
#define	CLICOMMANDS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <CommonIncludes.h>

//CLI shortcut - cuts down on time writing out the lengthy signature
#define CLI_FORMAT(name) BaseType_t name(char * wB, size_t bL, const char * cS)

//Command line function struct
extern const xCommandLineInput inputCommands[];

//Initialize CLI and register commands
void InitCLISystem();

//CLI Command definitions
CLI_FORMAT(EmergencyStop);
CLI_FORMAT(EmergencyClear);
CLI_FORMAT(DoorInterference);
CLI_FORMAT(OpenDoor);
CLI_FORMAT(CloseDoor);
CLI_FORMAT(CallToGround);
CLI_FORMAT(CallToP1);
CLI_FORMAT(CallToP2);
CLI_FORMAT(ChangeMaxSpeed);
CLI_FORMAT(ChangeAccel);
CLI_FORMAT(SendToFloor);
CLI_FORMAT(TaskStats);
CLI_FORMAT(RunTimeStats);

#ifdef	__cplusplus
}
#endif

#endif	/* CLICOMMANDS_H */

