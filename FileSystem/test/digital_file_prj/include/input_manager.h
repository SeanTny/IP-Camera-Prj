/*
 * input_manager.h
 *
 *  Created on: 2017Äê4ÔÂ6ÈÕ
 *      Author: tusen
 */

#ifndef INCLUDE_INPUT_MANAGER_H_
#define INCLUDE_INPUT_MANAGER_H_

#include <sys/time.h>

#define INPUT_TYPE_STDIN  		0
#define INPUT_TYPE_TOUCHSCREEN  1

#define INPUT_VALUE_UNKNOWN     -1
#define INPUT_VALUE_UP 		    0
#define INPUT_VALUE_DOWN        1
#define INPUT_VALUE_EXIT        2


typedef struct InputEvent{
	struct timeval tTime;
	int            iType; /* stdin touchscreen */
	int            iVal;
}T_InputEvent,*PT_InputEvent;



typedef struct InputOpr{
	char * name;
	int  iFd;
	int (*DeviceInit)(void);
	int (*DeviceExit)(void);
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	struct InputOpr * ptNext;
}T_InputOpr,*PT_InputOpr;


int RegisterInputOpr(PT_InputOpr ptInputOpr);
void ShowInputOpr(void);
PT_InputOpr GetInputpOpr(char *pcName);
int  AllInputDeviceInit(void );

int TouchScreenInit(void);
int StdinInit(void);
int InputInit(void);

int GetInputEvevt(PT_InputEvent ptInputEvent);

#endif /* INCLUDE_INPUT_MANAGER_H_ */







