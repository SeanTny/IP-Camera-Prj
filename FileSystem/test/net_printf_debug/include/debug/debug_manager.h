/*
 * debug_manager.h
 *
 *  Created on: 2017年5月28日
 *      Author: tusen
 */

#ifndef INCLUDE_DEBUG_MANAGER_H_
#define INCLUDE_DEBUG_MANAGER_H_


#define APP_EMERG     	"<0>"  	/* system is unusable */
#define APP_ALERT		"<1>"  	/* action must be taken immediately */
#define APP_CRIT		"<2>"	/* critical conditions 	*/
#define APP_ERR			"<3>"	/* error conditions	*/
#define APP_WARNING		"<4>"	/* warining conditions */
#define APP_NOTICE		"<5>"	/* normal but significal condition */
#define APP_INFO		"<6>"	/* informational  */
#define APP_DEBUG		"<7>"	/* debug-levle messages */

#define DEFAULT_DEBUG_LEVEL  4


/* 声明一个类 */
typedef struct DebugClass{
	char *pcName;
	int  iIsCanUse;
	int (*DebugInit)(void);
	int (*DebugExit)(void);
	int (*DebugPrint)(const char *strData);
	struct DebugClass *ptNext;
}T_DebugClass,*PT_DebugClass;




/********************************************************************************/




/* 构造 */
int DebugObjInit(void);


/* 使用 */
int ShowAllDebugObj(void);
PT_DebugClass GetBebugObjp(char *pcName);
int  SetDebugLeve(char *strBuf);
int SetDebugChanel(char *strBuf);
int managerDebugPrint(const char *pcFormat, ... );
int InitDebugChanel(void);


#endif /* INCLUDE_DEBUG_MANAGER_H_ */
