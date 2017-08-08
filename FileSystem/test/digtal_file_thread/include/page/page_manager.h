/*
 * page_manager.h
 *
 *  Created on: 2017Äê5ÔÂ30ÈÕ
 *      Author: tusen
 */

#ifndef INCLUDE_PAGE_PAGE_MANAGER_H_
#define INCLUDE_PAGE_PAGE_MANAGER_H_


typedef struct PageAction{
	char  *name;
	int (*Run)(void);
	int (*GetInputEvent)();
	int (*Prepare)();
	struct PageAction *ptNext;
}T_PageAction,*PT_PageAction;





#endif /* INCLUDE_PAGE_PAGE_MANAGER_H_ */
