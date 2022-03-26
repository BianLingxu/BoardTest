// UtilLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "UtilLibrary.h"

#pragma warning(disable : 4996)

static uint32_t gDebugOutput = false;

UTILLIBRARY_API uint32_t DebugPrint(vector<string> vectCmdLine)
{
	if (gDebugOutput)
	{
		string strText = "";
		for (uint32_t index = 0; index < vectCmdLine.size(); index++)
		{
			strText += vectCmdLine[index];
			strText += " ";
		}
		strText.pop_back();
		PrintLog(LOG_LEVEL_INFO, "%s\n", strText.c_str());
	}
	return 0;
}

UTILLIBRARY_API uint32_t SetDebugStatus(bool flag)
{
	gDebugOutput = flag;
	return 0;
}

UTILLIBRARY_API uint32_t DebugPause()
{
	system("pause");
	return 0;
}