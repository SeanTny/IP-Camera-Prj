/*
 * expole_page.c
 *
 *  Created on: 2017��5��30��
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
	/* 1)��ʾҳ�� */

	/* 2)����׼�����߳�Preapre */

	/* 3)����GetInputEvent ��������¼����������� */
	while(1)
	{
		InputEvent=ExploePageGetInputEvent();
		switch (InputEvent)
		{
		case "����" :
		{
		/* �ж��Ƿ��Ѿ��Ƕ��� */
			if(isTopLevel)
			{
				return 0;
			}
			else
			{
				/* ��ʾ��һ��Ŀ¼��ҳ�� */
			}
			break;
		}

		case "ѡ��":
		{
			if(isSelectDir)
			{
				/* ��ʾ��һ��Ŀ¼ */
			}
			else
			{
				/* ���浱ǰҳ�� */
				StorePage();

				Page("browse")->Run();

				/* �ָ�֮ǰ��ҳ�� */
				RestorePage();
			}


			break;
		}

		case "��һҳ":
		{
			/* ��ʾ��һҳ */

			break;
		}

		case "��һҳ":
		{
			/* ��ʾ��һҳ */

			break;
		}


		}
	}


}



int ExploePageInit(void)
{


	return RegisterPageAction(&g_tExploePageAction);

}



