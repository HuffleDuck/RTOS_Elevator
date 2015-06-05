#ifndef MOTORCONTROL_H
#define	MOTORCONTROL_H

#ifdef	__cplusplus
extern "C" {
#endif

void InitMotorControl(void *pvParameters);
void PrintSpeedAndPosition(int cur_speed, int cur_distance);

#ifdef	__cplusplus
}
#endif

#endif	/* MOTORCONTROL_H */

