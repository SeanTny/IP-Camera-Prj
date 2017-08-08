/*
 * expole_page.c
 *
 *  Created on: 2017年5月30日
 *      Author: tusen
 */



#include <config.h>
#include <disp_manager.h>
#include <string.h>
#include <page/page_manager.h>


static T_PageAction g_tExploePageAction={
		.name="explore",
		.Run=ExploePageRun,
		.GetInputEvent=ExploePageGetInputEvent,
		.Prepare=ExploePagePrepare,
};



static void ExploePageRun()
{
	/* 1)显示页面 */

	/* 2)创建准备的线程Preapre */

	/* 3)调用GetInputEvent 获得输入事件，进而处理 */
	while(1)
	{
		InputEvent=ExploePageGetInputEvent();
		switch (InputEvent)
		{
		case "向上" :
		{
		/* 判断是否已经是顶层 */
			if(isTopLevel)
			{
				return 0;
			}
			else
			{
				/* 显示上一个目录的页面 */
			}
			break;
		}

		case "选择":
		{
			if(isSelectDir)
			{
				/* 显示下一级目录 */
			}
			else
			{
				/* 保存当前页面 */
				StorePage();

				Page("browse")->Run();

				/* 恢复之前的页面 */
				RestorePage();
			}


			break;
		}

		case "下一页":
		{
			/* 显示下一页 */

			break;
		}

		case "上一页":
		{
			/* 显示上一页 */

			break;
		}


		}
	}


}



int ExploePageInit(void)
{


	return RegisterPageAction(&g_tExploePageAction);

}



