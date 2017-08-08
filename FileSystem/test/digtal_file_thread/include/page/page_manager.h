/*
 * page_manager.h
 *
 *  Created on: 2017��5��30��
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
