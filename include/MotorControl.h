#ifndef MOTORCONTROL_H
#define	MOTORCONTROL_H

#ifdef	__cplusplus
extern "C" {
#endif

void InitMotorControl(void *pvParameters);
void PrintSpeedAndPosition(float cur_speed, float cur_distance);
int myRound(float round_this);


#ifdef	__cplusplus
}
#endif

#endif	/* MOTORCONTROL_H */

