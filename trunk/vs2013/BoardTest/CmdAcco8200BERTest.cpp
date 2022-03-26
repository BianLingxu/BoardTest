#include "stdafx.h"
#include "UtilLibrary.h"
#include <time.h>
#include <map>
#include <io.h>
#include <direct.h>
#include "Sts8100.h"
#include "STSSP8201/STSSP8201.h"
#include "CmdAcco8200BlockBERTest.h"
#include "CmdAcco8200BERTest.h"
#include "CmdAcco8300BERTest.h"
#include "CmdAcco8300Module.h"


#pragma warning(disable : 4996)
#define THRESHOLD 1000
using namespace std;
extern map<string, DWORD> g_mapModuleAddress;
extern FILE* fp_write;
extern bool g_blockFlag;
extern char g_fname[1024];
extern uint32_t g_passCount;
extern uint32_t g_failCount;
extern uint32_t g_length;
extern string g_command[1024];

CCmdAcco8200BERTest::CCmdAcco8200BERTest()
{
}

CCmdAcco8200BERTest::~CCmdAcco8200BERTest()
{
}

uint32_t CCmdAcco8200BERTest::JudgeCmd(vector<string> vectCmdLine)
{
	int retVal;
	char folderName[] = "BERTestFile";
	CCmdAcco8200BlockBERTest CCmdAcco8200BBT;

	if (vectCmdLine.size() < 2)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown BERTest command\n");
		return -1;
	}
	// 文件夹不存在则创建文件夹
	if (_access(folderName, 0) == -1)
	{
		_mkdir(folderName);
	}
	ToUpperCase(vectCmdLine[1]);
	if ("HELP" == vectCmdLine[1]) {//帮助指令
		retVal = CmdHelpInf(vectCmdLine);
	}
	else if (("REG" == vectCmdLine[1]) || ("REGW" == vectCmdLine[1])) {//寄存器指令
		retVal = CmdParse(vectCmdLine);
	}
	else if ("BLOCK" == vectCmdLine[1]) {//block指令
		retVal = CCmdAcco8200BBT.CmdBlockParse(vectCmdLine);
	}
	else {
		PrintLog(LOG_LEVEL_ERROR, "Unknown BERTest command\n");
		retVal = -2;
	}
	return retVal;
}

uint32_t CCmdAcco8200BERTest::CmdParse(vector<string> vectCmdLine)
{
	DWORD inputAddr1 = 0x00;
	DWORD inputAddr2 = 0x00;
	SYSTEMTIME sysTime;
	char fnameTemp[1024] = { "" };
	char str[1024] = { "" };
	string result = "";
	int retVal = 0;
	int ret = 0;

	if (CmdExceptionProcess(vectCmdLine, 9, 0X00000281) == 0)//异常处理
	{
		ToUpperCase(vectCmdLine[6]);
		ToUpperCase(vectCmdLine[8]);
		sscanf_s(vectCmdLine[5].c_str(), "%X", &inputAddr1);
		sscanf_s(vectCmdLine[7].c_str(), "%X", &inputAddr2);

		if ((inputAddr1 > 0XFF) || (inputAddr2 > 0XFF))
		{
			PrintLog(LOG_LEVEL_ERROR, "Invalid address\n");
			return -6;
		}
		ToUpperCase(vectCmdLine[2]);
		ToUpperCase(vectCmdLine[4]);
		if (("TIME" != vectCmdLine[2]) && ("NUM" != vectCmdLine[2]))
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown BERTest command\n");
			return -7;
		}
		map<string, DWORD>::iterator iter = g_mapModuleAddress.find(vectCmdLine[4]);
		if (!g_blockFlag)//非block指令，创建一个新的测试报告文件
		{
			GetLocalTime(&sysTime);
			sprintf_s(fnameTemp, STS_ARRAY_SIZE(fnameTemp), "8200BER_Test_%04d%02d%02d_%02d%02d%02d.txt", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			strcpy(g_fname, "./BERTestFile/");
			strcat(g_fname, fnameTemp);
			fp_write = fopen(g_fname, "w");
			if (fp_write == NULL)
			{
				retVal = -8;
			}
		}
		else//block指令，打开之前创建的测试报告文件
		{
			fp_write = fopen(g_fname, "a");
			if (fp_write == NULL)
			{
				retVal = -8;
			}
		}
		if (retVal != -8)
		{
			fprintf_s(fp_write, "----------------------------CmdInfo---------------------------\n");
			fprintf_s(fp_write, "->");//保存并打印指令
			for (int i = 0; i < vectCmdLine.size(); i++)
			{
				g_command[g_length] += vectCmdLine[i];
				g_command[g_length] += " ";
			}
			fprintf_s(fp_write, "%s\n", g_command[g_length].c_str());

			fprintf_s(fp_write, "----------------------------DataInfo--------------------------\n");
			fprintf_s(fp_write, "->Module:%s(0X%08X)\n", vectCmdLine[4].c_str(), iter->second);
			fprintf_s(fp_write, "->Addr1:0X%08X\n", inputAddr1);
			fprintf_s(fp_write, "->Addr2:0X%08X\n", inputAddr2);
			fprintf_s(fp_write, "-----------------------\n");
			fclose(fp_write);
			if ("TIME" == vectCmdLine[2])//时间测试
			{
				double Time = 0.0;
				Time = atof(vectCmdLine[3].c_str());
				ret = CmdTimeTest(Time, vectCmdLine, inputAddr1, inputAddr2);
			}
			else//次数测试
			{
				uint32_t Numbers = 0;
				sscanf_s(vectCmdLine[3].c_str(), "%d", &Numbers);
				ret = CmdNumTest(Numbers, vectCmdLine, inputAddr1, inputAddr2);
			}

			if (!g_blockFlag)
			{
				PrintLog(LOG_LEVEL_INFO, "Test end(%s)\n", g_fname);
			}
			else//保存测试结果
			{
				switch (ret)
				{
				case 0:
					result = "PASS";
					g_passCount++;
					break;
				case 1:
					result = "FATEL ERR";
					g_failCount++;
					break;
				case 2:
					result = "FAIL";
					g_failCount++;
					break;
				default:
					result = "Other ERR";
					retVal = ret;
					g_failCount++;
					break;
				}
				sprintf_s(str, "%*s", 79 - g_command[g_length].length(), "");
				g_command[g_length] += str;
				g_command[g_length] += result;
			}
			g_length++;
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Unable to open the test report\n");
		}
	}
	return retVal;
}



uint32_t CCmdAcco8200BERTest::CmdTimeTest(double Time, vector<string> vectCmdLine, DWORD Addr1, DWORD Addr2)
{
	int retVal = 0;
	uint32_t timeVal = 0;
	uint32_t TotalCnt = 0;
	uint32_t ErrCnt = 0;
	double BER = 0.0;
	DWORD reg1Data = 0x00;
	DWORD reg2Data = 0x00;
	DWORD getData1 = 0x00;
	DWORD getData2 = 0x00;
	SYSTEMTIME sysTimeStart, sysTimeEnd;
	map<string, DWORD>::iterator iter = g_mapModuleAddress.find(vectCmdLine[4]);
	int cnt = 0;
	Time *= 60000000;
	GetLocalTime(&sysTimeStart);
	TickStart();//开始计时
	while (timeVal < Time)
	{
		
		if ("REG" == vectCmdLine[1])
		{
			if ("PRBS" == vectCmdLine[6])//产生伪随机数
			{
				reg1Data = (uint8_t)rand();
			}
			else
			{
				sscanf_s(vectCmdLine[6].c_str(), "%X", &reg1Data);
			}
			if ("PRBS" == vectCmdLine[8])
			{
				reg2Data = (uint8_t)rand();
			}
			else
			{
				sscanf_s(vectCmdLine[8].c_str(), "%X", &reg2Data);
			}

			_outp(iter->second, Addr1);//写两个寄存器
			_outp(iter->second + 2, reg1Data);
			_outp(iter->second, Addr2);
			_outp(iter->second + 2, reg2Data);

			_outp(iter->second, Addr1);//读两个寄存器
			getData1 = _inp(iter->second + 2);
			_outp(iter->second, Addr2);
			getData2 = _inp(iter->second + 2);
		}
		else
		{
			if ("PRBS" == vectCmdLine[6])//产生伪随机数
			{
				reg1Data = (uint8_t)rand() << 8 | (uint8_t)rand();
			}
			else
			{
				sscanf_s(vectCmdLine[6].c_str(), "%X", &reg1Data);
			}
			if ("PRBS" == vectCmdLine[8])
			{
				reg2Data = (uint8_t)rand() << 8 | (uint8_t)rand();
			}
			else
			{
				sscanf_s(vectCmdLine[8].c_str(), "%X", &reg2Data);
			}
			_outpw(iter->second, Addr1);//写两个寄存器
			_outpw(iter->second + 2, reg1Data);
			_outpw(iter->second, Addr2);
			_outpw(iter->second + 2, reg2Data);

			_outpw(iter->second, Addr1);//读两个寄存器
			getData1 = _inpw(iter->second + 2);
			_outpw(iter->second, Addr2);
			getData2 = _inpw(iter->second + 2);
		}
		cnt++;
		if (cnt == 100)
		{
			getData1 += 1;
			cnt = 0;
		}
		retVal = CmdDataCompare(TotalCnt, ErrCnt, getData1, reg1Data, getData2, reg2Data);
		if (retVal == -11)
		{
			break;
		}
		TotalCnt++;
		TickEnd(timeVal);//计时结束
	}
	GetLocalTime(&sysTimeEnd);
	if (!retVal)
	{
		retVal = CmdPrint(TotalCnt, ErrCnt, sysTimeStart, sysTimeEnd);
	}
	return retVal;
}

uint32_t CCmdAcco8200BERTest::CmdNumTest(uint32_t Numbers, vector<string> vectCmdLine, DWORD Addr1, DWORD Addr2)
{
	uint32_t TotalCnt;
	uint32_t ErrCnt = 0;
	uint32_t timeVal = 0;
	int retVal = 0;
	DWORD reg1Data = 0x00;
	DWORD reg2Data = 0x00;
	DWORD getData1 = 0x00;
	DWORD getData2 = 0x00;
	string Data1;
	string Data2;
	SYSTEMTIME sysTimeStart, sysTimeEnd;
	map<string, DWORD>::iterator iter = g_mapModuleAddress.find(vectCmdLine[4]);
	GetLocalTime(&sysTimeStart);
	TickStart();//开始计时
	for (TotalCnt = 0; TotalCnt < Numbers; TotalCnt++)
	{
		if ("REG" == vectCmdLine[1])
		{
			if ("PRBS" == vectCmdLine[6])//产生伪随机数
			{
				reg1Data = (uint8_t)rand();
			}
			else
			{
				sscanf_s(vectCmdLine[6].c_str(), "%X", &reg1Data);
			}
			if ("PRBS" == vectCmdLine[8])
			{
				reg2Data = (uint8_t)rand();
			}
			else
			{
				sscanf_s(vectCmdLine[8].c_str(), "%X", &reg2Data);
			}

			_outp(iter->second, Addr1);//写两个寄存器
			_outp(iter->second + 2, reg1Data);
			_outp(iter->second, Addr2);
			_outp(iter->second + 2, reg2Data);

			_outp(iter->second, Addr1);//读两个寄存器
			getData1 = _inp(iter->second + 2);
			_outp(iter->second, Addr2);
			getData2 = _inp(iter->second + 2);
		}
		else
		{
			if ("PRBS" == vectCmdLine[6])//产生伪随机数
			{
				reg1Data = (uint8_t)rand() << 8 | (uint8_t)rand();
			}
			else
			{
				sscanf_s(vectCmdLine[6].c_str(), "%X", &reg1Data);
			}
			if ("PRBS" == vectCmdLine[8])
			{
				reg2Data = (uint8_t)rand() << 8 | (uint8_t)rand();
			}
			else
			{
				sscanf_s(vectCmdLine[8].c_str(), "%X", &reg2Data);
			}
			_outpw(iter->second, Addr1);//写两个寄存器
			_outpw(iter->second + 2, reg1Data);
			_outpw(iter->second, Addr2);
			_outpw(iter->second + 2, reg2Data);

			_outpw(iter->second, Addr1);//读两个寄存器
			getData1 = _inpw(iter->second + 2);
			_outpw(iter->second, Addr2);
			getData2 = _inpw(iter->second + 2);
		}
		retVal = CmdDataCompare(TotalCnt, ErrCnt, getData1, reg1Data, getData2, reg2Data);
		if (retVal == -11)
		{
			break;
		}
		TickEnd(timeVal);//计时结束
	}
	GetLocalTime(&sysTimeEnd);
	if (!retVal)
	{
		retVal = CmdPrint(TotalCnt, ErrCnt, sysTimeStart, sysTimeEnd);
	}
	return retVal;
}

uint32_t CCmdAcco8200BERTest::CmdHelpInf(vector<string> vectCmdLine)
{
	PrintLog(LOG_LEVEL_INFO, "############################################################################################################################################\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8200 REG TIME <MINUTES(D)> <MODULE(S)> <Data0_Reg(X)> <Data0_Value(X/S)> <Data1_Reg(X)> <Data1_Value(X/S)>                       #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8200 REG NUM <NUMBERS(D)> <MODULE(S)> <Data0_Reg(X)> <Data0_Value(X/S)> <Data1_Reg(X)> <Data1_Value(X/S)>                        #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8200 REGW TIME <MINUTES(D)> <MODULE(S)> <Data0_Reg(X)> <Data0_Value(X/S)> <Data1_Reg(X)> <Data1_Value(X/S)>                      #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8200 REGW NUM < NUMBERS(D)> <MODULE(S)> <Data0_Reg(X)> <Data0_Value(X/S)> <Data1_Reg(X)> <Data1_Value(X/S)>                      #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8200 BLOCK TIME <MINUTES(D)> <MODULE(S)> <Reg1(X)> <StartAddr(X)> < Reg2(X)> <Length(D)> < Reg3(X)> <Mode (D)> <ACCStartData (X)>#\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8200 BLOCK NUM < NUMBERS(D)> <MODULE(S)> <Reg1(X)> <StartAddr(X)> < Reg2(X)> <Length(D)> < Reg3(X)> <Mode (D)> <ACCStartData (X)>#\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8200 FLASH TIME <MINUTES(D)> <MODULE(S)> <StartAddr(X)> <Length(D)> <DataType(S)>                                                #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8200 FLASH NUM <NUMBERS(D)> <MODULE(S)> <StartAddr(X)> <Length(D)> <DataType(S)>                                                 #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8200 HELP                                                                                                                        #\n");
	PrintLog(LOG_LEVEL_INFO, "############################################################################################################################################\n");
	return 0;
}

