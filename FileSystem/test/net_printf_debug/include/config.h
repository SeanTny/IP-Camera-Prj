
#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include <debug/debug_manager.h>


#define FB_DEVICE_NAME "/dev/fb0"

#define COLOR_BACKGROUND   0xE7DBB5  /* ·º»ÆµÄÖ½ */
#define COLOR_FOREGROUND   0x514438  /* ºÖÉ«×ÖÌå */


#define  TS_PRINT
//#define  TS_PRINT   printf


#define DBG_PRINTF(...)
//#define DBG_PRINTF printf
//#define  DBG_PRINTF  managerDebugPrint


#endif /* _CONFIG_H */
