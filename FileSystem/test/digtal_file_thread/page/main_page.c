/*
 * main_page.c
 *
 *  Created on: 2017年5月30日
 *      Author: tusen
 */

#include <config.h>
#include <disp_manager.h>
#include <string.h>
#include <page/page_manager.h>
#include <input_manager.h>

static T_PageAction g_tMainPageAction={
		.name="main",
		.Run=MainPageRun,
		.GetInputEvent=MainPageGetInputEvent,
		.Prepare=MainPagePrepare,
};



static void MainPageRun()
{
	/* 1)显示页面 */

	/* 2)创建准备的线程Preapre */

	/* 3)调用GetInputEvent 获得输入事件，进而处理 */
	while(1)
	{
		InputEvent=MainPageGetInputEvent();
		switch (InputEvent)
		{
		case "浏览模式" :
		{
			/* 保存当前页面 */
			StorePage();

			Page("explore")->Run();

			/* 恢复之前的页面 */
			RestorePage();

			break;
		}

		case "连播模式":
		{
			/* 保存当前页面 */
			StorePage();

			Page("auto")->Run();

			/* 恢复之前的页面 */
			RestorePage();

			break;
		}

		case "设置模式":
		{
			/* 保存当前页面 */
			StorePage();

			Page("setting")->Run();

			/* 恢复之前的页面 */
			RestorePage();

			break;
		}


		}
	}


}


int MainPageGetInputEvent()
{
	/* 获得原始的点坐标数据
	 * 这是调用inputmanager函数，次函数会让当前线程休眠
	 * 当触摸屏线程获得数据后，会把他唤醒
	 * */
	GetInputEvent();

	/* 处理数据 */


}



int MainPageInit(void)
{


	return RegisterPageAction(&g_tMainPageAction);

}


