#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <draw.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <string.h>
#include <debug/debug_manager.h>
#include <page/page_manager.h>


int main(int argc, char **argv)
{
	/*  ³õÊ¼»¯  */

	page("main")->Run();

	return 0;
}

