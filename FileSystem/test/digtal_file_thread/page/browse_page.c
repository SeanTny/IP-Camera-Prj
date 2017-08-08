/*
 * brows_page.c
 *
 *  Created on: 2017年5月30日
 *      Author: tusen
 */


#include <config.h>
#include <disp_manager.h>
#include <string.h>
#include <page/page_manager.h>


static T_PageAction g_tBrowsePageAction={
		.name="browse",
		.Run=BrowsePageRun,
		.GetInputEvent=BrowsePageGetInputEvent,
		.Prepare=BrowsePagePrepare,
};



static void BrowsePageRun()
{
	/* 1)显示页面 */

	/* 2)创建准备的线程Preapre */

	/* 3)调用GetInputEvent 获得输入事件，进而处理 */
	while(1)
	{
		InputEvent=BrowsePageGetInputEvent();
		switch (InputEvent)
		{
		case "返回" :
		{
			return 0;
			break;
		}

		case "缩小":
		{
			/* 显示缩小的页面 */

			break;
		}

		case "放大":
		{
			/* 显示放大的页面 */

			break;
		}

		case "下一幅":
		{
			/* 显示下一页 */

			break;
		}

		case "上一幅":
		{
			/* 显示上一页 */

			break;
		}

		case "连播":
		{
			/*  */
			Page("auto")->Run();
			break;
		}

		case "按住不放":
		{
			/* 显示你挪动的图片 */
			break;
		}


		}
	}


}



int BrowsePageInit(void)
{


	return RegisterPageAction(&g_tBrowsePageAction);

}



