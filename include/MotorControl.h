#ifndef MOTORCONTROL_H
#define	MOTORCONTROL_H

#ifdef	__cplusplus
extern "C" {
#endif

void InitMotorControl(void *pvParameters);
void PrintSpeedAndPosition(float cur_speed, float cur_distance);
int myRound(float round_this);
bool SendMessageToMotor( char state, float time_to_acel, float time_to_cruise, float time_to_decel,
                bool emerg_flag, bool start, bool up_true, float data_to_go_with_request);


#ifdef	__cplusplus
}
#endif

#endif	/* MOTORCONTROL_H */

