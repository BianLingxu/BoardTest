#include "stdafx.h"
#include "CmdUtil.h"
#include "UtilLibrary.h"
#include "STSSP8201/STSSP8201.h"
#include <io.h>
#include <direct.h>

#pragma warning(disable : 4996)
uint32_t g_length = 0;
uint32_t g_passCount = 0;
uint32_t g_failCount = 0;
FILE* fp_write = NULL;
string g_command[1024] = { "" };
bool g_blockFlag = false;
char g_fname[1024] = { "./BERTestFile/" };
char summaryName[1024] = { "./BERTestFile/" };

CCmdUtil::CCmdUtil()
{
}

CCmdUtil::~CCmdUtil()
{
}

uint32_t CCmdUtil::JudgeCmd(vector<string> vectCmdLine)
{
    uint32_t retVal;

    do {
        if (vectCmdLine.size() < 2) {
            PrintLog(LOG_LEVEL_ERROR, "Unknown UTIL command\n");
            retVal = 1;
            break;
        }

        ToUpperCase(vectCmdLine[1]);
        if ("HELP" == vectCmdLine[1]) {
            retVal = CmdHelpInf(vectCmdLine);
        }
		else if ("RAMS" == vectCmdLine[1]) {
			retVal = CmdRams(vectCmdLine);
		}
        else if ("ECHO" == vectCmdLine[1]) {
            retVal = CmdEcho(vectCmdLine);
        }
        else if ("SLEEP" == vectCmdLine[1]) {
            retVal = CmdSleep(vectCmdLine);
        }
        else if ("LOG" == vectCmdLine[1]) {
            retVal = CmdLog(vectCmdLine);
        }
        else if ("TIME" == vectCmdLine[1]) {
            retVal = CmdTime(vectCmdLine);
        }
		else if ("VERS" == vectCmdLine[1]) {
			retVal = CmdVers(vectCmdLine);
		}
		else if ("DEBUG" == vectCmdLine[1]) {
			retVal = CmdFileDebug(vectCmdLine);
		}
        else {
            PrintLog(LOG_LEVEL_ERROR, "Unknown UTIL command\n");
            retVal = 1;
        }
    } while (0);

    return retVal;
}

uint32_t CCmdUtil::CmdEcho(vector<string> vectCmdLine)
{
	string strText;
	uint32_t index;

	strText = "";
	for (index = 2; index < vectCmdLine.size(); index++) {
		strText += vectCmdLine[index];
		strText += " ";
	}
	strText.pop_back();
	PrintLog(LOG_LEVEL_INFO, "%s\n", strText.c_str());

	return 0;
}

uint32_t CCmdUtil::CmdRams(vector<string> vectCmdLine)
{
	uint32_t ramWidth, ramDepth, ramType, ramStep;
	uint32_t widthAlign;
	uint32_t *pBuff;

	if (vectCmdLine.size() < 6) {
		PrintLog(LOG_LEVEL_ERROR, "Unknown UTIL RAMS command\n");
		return 1;
	}

	sscanf_s(vectCmdLine[3].c_str(), "%d", &ramWidth);
	sscanf_s(vectCmdLine[4].c_str(), "%d", &ramDepth);
	sscanf_s(vectCmdLine[5].c_str(), "%d", &ramType);
	if (vectCmdLine.size() > 6) {
		sscanf_s(vectCmdLine[6].c_str(), "%d", &ramStep);
	}
	else {
		ramStep = 4;
	}

	widthAlign = (ramWidth + 31) / 32;
	pBuff = (uint32_t *)malloc(widthAlign * ramDepth * 4);
	if (NULL == pBuff) {
		PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
		return 1;
	}
	FillDataPattern((uint8_t *)pBuff, widthAlign * ramDepth * 4, DATA_PATTERN_SETS(ramType), ramStep);
	for (uint32_t depthIdx = 0; depthIdx < ramDepth; depthIdx++)
	{
		for (uint32_t widthIdx = 0; widthIdx < widthAlign; widthIdx++)
		{
			if ((0 == widthIdx) && (0 != (ramWidth % 32)))
			{
				pBuff[depthIdx * widthAlign + widthIdx] &= 0xFFFFFFFF >> (32 - (ramWidth % 32));
			}
		}
	}
	WriteTextFile(vectCmdLine[2], pBuff, ramWidth, ramDepth);
	free(pBuff);

	return 0;
}

uint32_t CCmdUtil::CmdSleep(vector<string> vectCmdLine)
{
	uint32_t timeVal, timeUnit;

    if( vectCmdLine.size() < 3 ) {
        PrintLog(LOG_LEVEL_ERROR, "Unknown UTIL SLEEP command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[2].c_str(), "%u", &timeVal);

	timeUnit = 0;
	if (vectCmdLine.size() > 3) {
		ToUpperCase(vectCmdLine[3]);
		if ("US" == vectCmdLine[3]) {
			timeUnit = 1;
		}
	}

	if (0 == timeUnit) {
		Sleep(timeVal);
	}
	else {
		DelayUs(timeVal);
	}

    return 0;
}

uint32_t CCmdUtil::CmdLog(vector<string> vectCmdLine)
{
    uint32_t level, retVal;

    ToUpperCase(vectCmdLine[2]);

    if ("OPEN" == vectCmdLine[2]) {
        if (vectCmdLine.size() < 4) {
            PrintLog(LOG_LEVEL_ERROR, "Unknown UTIL LOG OPEN command\n");
            return 1;
        }

        retVal = OpenLogFile(vectCmdLine[3]);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "Unable to open log file %s\n", vectCmdLine[3].c_str());
            return 1;
        }
    }
    else if ("LEVEL" == vectCmdLine[2]) {
        if (vectCmdLine.size() < 4) {
            PrintLog(LOG_LEVEL_ERROR, "Unknown UTIL LOG LEVEL command\n");
            return 1;
        }

		sscanf_s(vectCmdLine[3].c_str(), "%X", &level);
		SetLogLevel(level);
    }
    else if ("CLOSE" == vectCmdLine[2]) {
        CloseLogFile();
    }
    else {
        PrintLog(LOG_LEVEL_ERROR, "Unknown UTIL LOG command\n");
        return 1;
    }

    return 0;
}

uint32_t CCmdUtil::CmdTime(vector<string> vectCmdLine)
{
    SYSTEMTIME sysTime;

    GetLocalTime(&sysTime);
    PrintLog(LOG_LEVEL_INFO, "%04d-%02d-%02d %02d:%02d:%02d\n", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

    return 0;
}

uint32_t CCmdUtil::CmdVers(vector<string> vectCmdLine)
{
	string filename = "BoardTest.exe";
	string verStr = GetInfoFromExeAndDll(filename, FileVersion);

	PrintLog(LOG_LEVEL_INFO, "%s %s\n", filename.c_str(), verStr.c_str());

	return 0;
}

uint32_t CCmdUtil::CmdFileDebug(vector<string> vectCmdLine)
{
	uint32_t level = 0;
	uint32_t retVal = 0;

	ToUpperCase(vectCmdLine[2]);

	if ("OPEN" == vectCmdLine[2])
	{
		SetDebugStatus(true);
	}
	else if ("CLOSE" == vectCmdLine[2])
	{
		SetDebugStatus(false);
	}
	else if ("PAUSE" == vectCmdLine[2])
	{
		DebugPause();
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown UTIL DEBUG command\n");
		return 1;
	}
	return 0;
}

uint32_t CCmdUtil::BlockCmd(vector<string> vectCmdLine)
{
	SYSTEMTIME sysTime;
	uint32_t retVal = 0;
	FILE* fs_write = NULL;
	char fnameTemp[1024] = { "" };
	char folderName[] = "BERTestFile";

	do {
		if (vectCmdLine.size() < 3)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown BERTest command\n");
			retVal = -1;
			break;
		}
		ToUpperCase(vectCmdLine[1]);
		ToUpperCase(vectCmdLine[2]);
		if (("TEST" == vectCmdLine[1]) && ("BEGIN" == vectCmdLine[2]))
		{
			// 文件夹不存在则创建文件夹
			if (_access(folderName, 0) == -1)
			{
				_mkdir(folderName);
			}
			for (int i = 0; i < g_length; i++)
			{
				g_command[i].clear();
			}
			g_length = 0;
			GetLocalTime(&sysTime);
			sprintf_s(fnameTemp, STS_ARRAY_SIZE(fnameTemp), "BERTestReport_%04d%02d%02d_%02d%02d%02d.txt", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			strcat(g_fname, fnameTemp);
			fp_write = fopen(g_fname, "w");
			if (fp_write == NULL)
			{
				PrintLog(LOG_LEVEL_ERROR, "Unable to create a test report\n");
				retVal = -3;
				break;
			}
			fclose(fp_write);

			sprintf_s(fnameTemp, STS_ARRAY_SIZE(fnameTemp), "BERTestSummary_%04d%02d%02d_%02d%02d%02d.txt", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			strcat(summaryName, fnameTemp);
			fs_write = fopen(summaryName, "w");
			if (fs_write == NULL)
			{
				PrintLog(LOG_LEVEL_ERROR, "Unable to create a summary file\n");
				retVal = -5;
				break;
			}
			fprintf_s(fs_write, "---------------------------------------Total Summary----------------------------------------\n");
			fprintf_s(fs_write, "Command%*sResult\n", 72, " ");
			g_blockFlag = true;
			fclose(fs_write);
		}
		else if (("TEST" == vectCmdLine[1]) && ("END" == vectCmdLine[2]))
		{
			g_blockFlag = false;
			PrintLog(LOG_LEVEL_INFO, "Test end(%s)\n", g_fname);
			fs_write = fopen(summaryName, "a");
			if (fs_write == NULL)
			{
				PrintLog(LOG_LEVEL_ERROR, "Unable to open a summary file\n");
				retVal = -4;
				break;
			}
			for (int i = 0; i < g_length; i++)
			{
				fprintf_s(fs_write, "%s\n", g_command[i].c_str());
			}
			fprintf_s(fs_write, "--------------------------\n");
			fprintf_s(fs_write, "Command Count:%d\n", g_passCount + g_failCount);
			fprintf_s(fs_write, "Pass Count:%d\n", g_passCount);
			fprintf_s(fs_write, "Fail Count:%d\n", g_failCount);
			g_passCount = 0;
			g_failCount = 0;
			for (int i = 0; i < g_length; i++)
			{
				g_command[i].clear();
			}
			g_length = 0;
			fclose(fs_write);
			strcpy(summaryName, "./BERTestFile/");
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown BERTest command\n");
			retVal = -2;
			break;
		}
	} while (0);
	return retVal;
}

uint32_t CCmdUtil::CmdHelpInf(vector<string> vectCmdLine)
{
    PrintLog(LOG_LEVEL_INFO, "#########################################################################################\n");
	PrintLog(LOG_LEVEL_INFO, "# UTIL RAMS <File(S)> <Width(D)> <Depth(D)> <Type(D)> [Step(D)]                         #\n");
    PrintLog(LOG_LEVEL_INFO, "# UTIL ECHO <String(S)> ...                                                             #\n");
    PrintLog(LOG_LEVEL_INFO, "# UTIL SLEEP <Time(D)> [Unit(S)]                                                        #\n");
    PrintLog(LOG_LEVEL_INFO, "# UTIL LOG OPEN <File(S)>                                                               #\n");
    PrintLog(LOG_LEVEL_INFO, "# UTIL LOG LEVEL <Level(X)>                                                             #\n");
    PrintLog(LOG_LEVEL_INFO, "# UTIL LOG CLOSE                                                                        #\n");
	PrintLog(LOG_LEVEL_INFO, "# UTIL DEBUG OPEN                                                                       #\n");
	PrintLog(LOG_LEVEL_INFO, "# UTIL DEBUG CLOSE                                                                      #\n");
	PrintLog(LOG_LEVEL_INFO, "# UTIL DEBUG PAUSE                                                                      #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER TEST BEGIN  <COMMAND(S)>  ...  BER TEST END                                       #\n");
	PrintLog(LOG_LEVEL_INFO, "# UTIL TIME                                                                             #\n");
	PrintLog(LOG_LEVEL_INFO, "# UTIL VERS                                                                             #\n");
	PrintLog(LOG_LEVEL_INFO, "# UTIL HELP                                                                             #\n");
    PrintLog(LOG_LEVEL_INFO, "#########################################################################################\n");

    return 0;
}
