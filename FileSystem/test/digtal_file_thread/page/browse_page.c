/*
 * brows_page.c
 *
 *  Created on: 2017��5��30��
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
	/* 1)��ʾҳ�� */

	/* 2)����׼�����߳�Preapre */

	/* 3)����GetInputEvent ��������¼����������� */
	while(1)
	{
		InputEvent=BrowsePageGetInputEvent();
		switch (InputEvent)
		{
		case "����" :
		{
			return 0;
			break;
		}

		case "��С":
		{
			/* ��ʾ��С��ҳ�� */

			break;
		}

		case "�Ŵ�":
		{
			/* ��ʾ�Ŵ��ҳ�� */

			break;
		}

		case "��һ��":
		{
			/* ��ʾ��һҳ */

			break;
		}

		case "��һ��":
		{
			/* ��ʾ��һҳ */

			break;
		}

		case "����":
		{
			/*  */
			Page("auto")->Run();
			break;
		}

		case "��ס����":
		{
			/* ��ʾ��Ų����ͼƬ */
			break;
		}


		}
	}


}



int BrowsePageInit(void)
{


	return RegisterPageAction(&g_tBrowsePageAction);

}



