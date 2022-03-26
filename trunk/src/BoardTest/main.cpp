// BoardTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CmdProcess.h"
#include <windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	system("mode con cols=120 lines=50");//¸Ä±ä¿í¸ß
    CCmdProcess cCmdProcess;
    cCmdProcess.CmdExecute();
	system("pause");
    return 0;
}

