#ifndef DOORCONTROL_H
#define	DOORCONTROL_H

#ifdef	__cplusplus
extern "C" {
#endif

void InitDOORControl();
void SendToDoorControl(char * string);
#ifdef	__cplusplus
}
#endif

#endif	/* DOORCONTROL_H */
