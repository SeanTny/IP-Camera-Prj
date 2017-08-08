/*
 * main_page.c
 *
 *  Created on: 2017��5��30��
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
	/* 1)��ʾҳ�� */

	/* 2)����׼�����߳�Preapre */

	/* 3)����GetInputEvent ��������¼����������� */
	while(1)
	{
		InputEvent=MainPageGetInputEvent();
		switch (InputEvent)
		{
		case "���ģʽ" :
		{
			/* ���浱ǰҳ�� */
			StorePage();

			Page("explore")->Run();

			/* �ָ�֮ǰ��ҳ�� */
			RestorePage();

			break;
		}

		case "����ģʽ":
		{
			/* ���浱ǰҳ�� */
			StorePage();

			Page("auto")->Run();

			/* �ָ�֮ǰ��ҳ�� */
			RestorePage();

			break;
		}

		case "����ģʽ":
		{
			/* ���浱ǰҳ�� */
			StorePage();

			Page("setting")->Run();

			/* �ָ�֮ǰ��ҳ�� */
			RestorePage();

			break;
		}


		}
	}


}


int MainPageGetInputEvent()
{
	/* ���ԭʼ�ĵ���������
	 * ���ǵ���inputmanager�������κ������õ�ǰ�߳�����
	 * ���������̻߳�����ݺ󣬻��������
	 * */
	GetInputEvent();

	/* �������� */


}



int MainPageInit(void)
{


	return RegisterPageAction(&g_tMainPageAction);

}


