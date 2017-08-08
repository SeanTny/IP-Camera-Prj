#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H
#include <sys/time.h>
#include <pthread.h>


#define INPUT_TYPE_STDIN        0
#define INPUT_TYPE_TOUCHSCREEN  1

#define INPUT_VALUE_UP          0   
#define INPUT_VALUE_DOWN        1
#define INPUT_VALUE_EXIT        2
#define INPUT_VALUE_UNKNOWN     -1


typedef struct InputEvent {
	struct timeval tTime;
	int iType;  /* stdin, touchsceen */
	int iVal;   /*  */
}T_InputEvent, *PT_InputEvent;


typedef struct InputOpr {
	char *name;
	pthread_t tThreadID;
	int (*DeviceInit)(void);
	int (*DeviceExit)(void);
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	struct InputOpr *ptNext;
}T_InputOpr, *PT_InputOpr;


int InputInit(void);
int RegisterInputOpr(PT_InputOpr ptInputOpr);
void ShowInputOpr(void);
int AllInputDevicesInit(void);
int GetInputEvent(PT_InputEvent ptInputEvent);

#endif /* _INPUT_MANAGER_H */

