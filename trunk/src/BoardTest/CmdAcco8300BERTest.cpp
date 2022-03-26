#include "stdafx.h"
#include "UtilLibrary.h"
#include <time.h>
#include <map>
#include "STSSP8201/STSSP8201.h"
#include "CmdAcco8300BlockBERTest.h"
#include "CmdAcco8300FlashBERTest.h"
#include "CmdAcco8300BERTest.h"
#include "CmdAcco8300Module.h"
#include <io.h>
#include <direct.h>

#pragma warning(disable : 4996)
#define THRESHOLD 1000
using namespace std;
extern map<string, DWORD> g_map8300SlotAddress;
extern map<string, int> g_map8300AddressType;
extern int g_systemType;
extern FILE* fp_write;
extern bool g_blockFlag;
extern char g_fname[1024];
extern uint32_t g_passCount;
extern uint32_t g_failCount;
extern uint32_t g_length;
extern string g_command[1024];
 
CCmdAcco8300BERTest::CCmdAcco8300BERTest()
{
}

CCmdAcco8300BERTest::~CCmdAcco8300BERTest()
{
}

uint32_t CCmdAcco8300BERTest::JudgeCmd(vector<string> vectCmdLine)
{
	uint32_t retVal;
	char folderName[] = "BERTestFile";
	CCmdAcco8300BlockBERTest CCmdAcco8300BBT;
	CCmdAcco8300FlashBERTest CCmdAcco8300FBT;
	
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
	else if (("REG" == vectCmdLine[1])||("REG_G" == vectCmdLine[1])) {//寄存器指令
		retVal = CmdParse(vectCmdLine);
	}
	else if (("BLOCK" == vectCmdLine[1]) || ("BLOCK_G" == vectCmdLine[1])) {//block指令
		retVal = CCmdAcco8300BBT.CmdBlockParse(vectCmdLine);
	}
	else if ("FLASH" == vectCmdLine[1]){//flash指令
		retVal = CCmdAcco8300FBT.CmdFlashParse(vectCmdLine);
	}
	else {
		PrintLog(LOG_LEVEL_ERROR, "Unknown BERTest command\n");
		retVal = -2;
	}
	return retVal;
}

uint32_t CCmdAcco8300BERTest::CmdParse(vector<string> vectCmdLine)
{
	DWORD chAddr = 0xFFFFFFFF;
	DWORD reg1Addr = 0x00;
	DWORD reg2Addr = 0x00;
	DWORD inputAddr1 = 0x00;
	DWORD inputAddr2 = 0x00;
	SYSTEMTIME sysTime;
	char fnameTemp[1024] = { "" };
	char str[1024] = { "" };
	string result = "" ;
	uint32_t retVal = 0;
	uint32_t ret = 0;

	if ("REG" == vectCmdLine[1] && CmdExceptionProcess(vectCmdLine, 9, 0X00000121) == 0)
	{
		ToUpperCase(vectCmdLine[6]);
		ToUpperCase(vectCmdLine[8]);
		sscanf_s(vectCmdLine[5].c_str(), "%X", &reg1Addr);
		sscanf_s(vectCmdLine[7].c_str(), "%X", &reg2Addr);
			
		if ((reg1Addr > 0XFF) || (reg2Addr > 0XFF))
		{
			PrintLog(LOG_LEVEL_ERROR, "Invalid address\n");
			return -6;
		}
	}
	else if ("REG_G" == vectCmdLine[1] && CmdExceptionProcess(vectCmdLine, 8, 0X00000101) == 0)
	{
		ToUpperCase(vectCmdLine[5]);
		ToUpperCase(vectCmdLine[7]);
		sscanf_s(vectCmdLine[4].c_str(), "%X", &inputAddr1);
		sscanf_s(vectCmdLine[6].c_str(), "%X", &inputAddr2);
	}
	else
	{
		retVal = -7;
	}
	if (retVal != -7)
	{
		ToUpperCase(vectCmdLine[2]);
		ToUpperCase(vectCmdLine[4]);
		if (("TIME" != vectCmdLine[2]) && ("NUM" != vectCmdLine[2]))
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown BERTest command\n");
			return -9;
		}
		map<string, DWORD>::iterator iter = g_map8300SlotAddress.find(vectCmdLine[4]);
		if (!g_blockFlag)//非block指令，创建一个新的测试报告文件
		{
			GetLocalTime(&sysTime);
			sprintf_s(fnameTemp, STS_ARRAY_SIZE(fnameTemp), "8300BER_Test_%04d%02d%02d_%02d%02d%02d.txt", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
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
			if ("REG" == vectCmdLine[1])//寄存器地址处理
			{
				chAddr = iter->second;
				inputAddr1 = GetAddress(iter->first, chAddr, reg1Addr);
				inputAddr2 = GetAddress(iter->first, chAddr, reg2Addr);
				fprintf_s(fp_write, "->Module:%s(0X%08X)\n", vectCmdLine[4].c_str(), chAddr);
			}
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



uint32_t CCmdAcco8300BERTest::CmdTimeTest(double Time, vector<string> vectCmdLine, DWORD Addr1, DWORD Addr2)
{
	uint32_t retVal = 0;
	uint32_t timeVal = 0;
	uint32_t TotalCnt = 0;
	uint32_t ErrCnt = 0;
	double BER = 0.0;
	//int cnt = 0;
	DWORD reg1Data = 0x00;
	DWORD reg2Data = 0x00;
	DWORD getData1 = 0x00;
	DWORD getData2 = 0x00;
	string Data1;
	string Data2;
	SYSTEMTIME sysTimeStart, sysTimeEnd;

	Time *= 60000000;
	if ((Addr1 == 0xFFFFFFFF) || (Addr2 == 0xFFFFFFFF))
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown module type(Address=0xFFFFFFFF)\n");
		return -1;
	}
	if ("REG" == vectCmdLine[1])
	{
		Data1 = vectCmdLine[6];
		Data2 = vectCmdLine[8];
	}
	else
	{
		Data1 = vectCmdLine[5];
		Data2 = vectCmdLine[7];
	}
	GetLocalTime(&sysTimeStart);
	TickStart();//开始计时
	while (timeVal < Time)
	{
		if ("PRBS" == Data1)
		{
			reg1Data = (uint8_t)rand() << 24 | (uint8_t)rand() << 16 | (uint8_t)rand() << 8 | (uint8_t)rand();
		}
		else
		{
			sscanf_s(Data1.c_str(), "%X", &reg1Data);
		}
		if ("PRBS" == Data2)
		{
			reg2Data = (uint8_t)rand() << 24 | (uint8_t)rand() << 16 | (uint8_t)rand() << 8 | (uint8_t)rand();
		}
		else
		{
			sscanf_s(Data2.c_str(), "%X", &reg2Data);
		}
		write_dw(Addr1, reg1Data);;//写两个寄存器
		write_dw(Addr2, reg2Data);
		getData1 = read_dw(Addr1);//读两个寄存器
		getData2 = read_dw(Addr2);

		/*cnt++;
		if (cnt == 100)
		{
			getData1 += 1;
			cnt = 0;
		}*/
		retVal = CmdDataCompare(TotalCnt, ErrCnt, getData1, reg1Data, getData2, reg2Data);
		//if ((getData1 != reg1Data) || (getData2 != reg2Data))//数据比较
		//{
		//	if (ErrCnt==0)//只写一次表头
		//	{
		//		fp_write = fopen(g_fname, "a");
		//		if (fp_write == NULL)
		//		{
		//			PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
		//			retVal = -9;
		//		}
		//		else
		//		{
		//			fprintf_s(fp_write, "[No]reg1Actual  (reg1Expect)  reg2Actual  (reg2Expect)\n");
		//			fclose(fp_write);
		//		}
		//	}
		//	fp_write = fopen(g_fname, "a");
		//	if (fp_write == NULL)
		//	{
		//		PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
		//		retVal = -3;
		//	}
		//	else
		//	{
		//		fprintf_s(fp_write, "[%d]0X%08X  (0X%08X)  0X%08X  (0X%08X)\n", TotalCnt + 1, getData1, reg1Data, getData2, reg2Data);//写入错误信息，每次写入打开关闭文件
		//		fclose(fp_write);
		//	}
		//	ErrCnt++;
		//	if (ErrCnt == THRESHOLD)
		//	{
		//		TotalCnt++;
		//		break;
		//	}
		//}
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

uint32_t CCmdAcco8300BERTest::CmdNumTest(uint32_t Numbers, vector<string> vectCmdLine, DWORD Addr1, DWORD Addr2)
{
	uint32_t TotalCnt;
	uint32_t ErrCnt = 0;
	uint32_t timeVal = 0;
	uint32_t retVal = 0;
	//int cnt = 0;
	DWORD reg1Data = 0x00;
	DWORD reg2Data = 0x00;
	DWORD getData1 = 0x00;
	DWORD getData2 = 0x00;
	string Data1;
	string Data2;
	SYSTEMTIME sysTimeStart, sysTimeEnd;

	if ((Addr1 == 0xFFFFFFFF) || (Addr2 == 0xFFFFFFFF))
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown module type(Address=0xFFFFFFFF)\n");
		return -1;
	}
	if ("REG" == vectCmdLine[1])
	{
		Data1 = vectCmdLine[6];
		Data2 = vectCmdLine[8];
	}
	else
	{
		Data1 = vectCmdLine[5];
		Data2 = vectCmdLine[7];
	}
	GetLocalTime(&sysTimeStart);
	TickStart();//开始计时
	for (TotalCnt = 0; TotalCnt < Numbers; TotalCnt++)
	{
		if ("PRBS" == Data1)
		{
			reg1Data = (uint8_t)rand() << 24 | (uint8_t)rand() << 16 | (uint8_t)rand() << 8 | (uint8_t)rand();
		}
		else
		{
			sscanf_s(Data1.c_str(), "%X", &reg1Data);
		}
		if ("PRBS" == Data2)
		{
			reg2Data = (uint8_t)rand() << 24 | (uint8_t)rand() << 16 | (uint8_t)rand() << 8 | (uint8_t)rand();
		}
		else
		{
			sscanf_s(Data1.c_str(), "%X", &reg2Data);
		}
		write_dw(Addr1, reg1Data);//写两个寄存器
		write_dw(Addr2, reg2Data);
		getData1 = read_dw(Addr1);//读两个寄存器
		getData2 = read_dw(Addr2);

		//cnt++;
		//if (cnt == 50)
		//{
		//	getData2 += 1;
		//	cnt = 0;
		//}

		retVal = CmdDataCompare(TotalCnt, ErrCnt, getData1, reg1Data, getData2, reg2Data);
		//if ((getData1 != reg1Data) || (getData2 != reg2Data))//数据比较
		//{
		//	if (ErrCnt==0)//只写一次表头
		//	{
		//		fp_write = fopen(g_fname, "a");
		//		if (fp_write == NULL)
		//		{
		//			PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
		//			retVal = -9;
		//		}
		//		else
		//		{
		//			fprintf_s(fp_write, "[No]reg1Actual  (reg1Expect)  reg2Actual  (reg2Expect)\n");
		//			fclose(fp_write);
		//		}
		//	}
		//	fp_write = fopen(g_fname, "a");
		//	if (fp_write == NULL)
		//	{
		//		PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
		//		retVal = -3;
		//	}
		//	else
		//	{
		//		fprintf_s(fp_write, "[%d]0X%08X  (0X%08X)  0X%08X  (0X%08X)\n", TotalCnt + 1, getData1, reg1Data, getData2, reg2Data);//写入错误信息，每次写入打开关闭文件
		//		fclose(fp_write);
		//	}
		//	ErrCnt++;
		//	if (ErrCnt == THRESHOLD)
		//	{
		//		TotalCnt++;
		//		break;
		//	}
		//}
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

uint32_t CmdPrint(uint32_t TotalCnt, uint32_t ErrCnt, SYSTEMTIME sysTimeStart, SYSTEMTIME sysTimeEnd)
{
	uint32_t retVal = 0;
	double BER = 0.0;
	fp_write = fopen(g_fname, "a");
	if (fp_write == NULL)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
		return -12;
	}
	if (TotalCnt != 0)//除数不能为0，为0BER设为-1
	{
		BER = (double)ErrCnt / TotalCnt;
		if (ErrCnt == 0)//测试通过
		{
			fprintf_s(fp_write, "->ALL PASS !!\n");
			PrintLog(LOG_LEVEL_INFO, "ALL PASS !!\n");
		}
		else if (ErrCnt < THRESHOLD)//测试失败但未超限
		{
			PrintLog(LOG_LEVEL_INFO, "TEST FAIL !!\n");
			retVal = 2;
		}
		else//测试失败且超限
		{
			fprintf_s(fp_write, "...\n");
			PrintLog(LOG_LEVEL_INFO, "FATEL ERR !!\n");
			retVal = 1;
		}
		uint32_t timeVal = (sysTimeEnd.wDay - sysTimeStart.wDay) * 86400000 + (sysTimeEnd.wHour - sysTimeStart.wHour) * 3600000 + (sysTimeEnd.wMinute - sysTimeStart.wMinute) * 60000 + (sysTimeEnd.wSecond - sysTimeStart.wSecond) * 1000 + (sysTimeEnd.wMilliseconds - sysTimeStart.wMilliseconds);
		uint32_t Day = timeVal / 86400000;//计算时间间隔并转换为各时间单位
		uint32_t Hour = timeVal % 86400000 / 3600000;
		uint32_t Minute = timeVal % 86400000 % 3600000 / 60000;
		uint32_t Second = timeVal % 86400000 % 3600000 % 60000 / 1000;
		uint32_t MilliSecond = timeVal % 86400000 % 3600000 % 60000 % 1000;

		fprintf_s(fp_write, "----------------------------Summary---------------------------\n");
		fprintf_s(fp_write, "TotalCount:%d\n", TotalCnt);
		fprintf_s(fp_write, "FailCount:%d\n", ErrCnt);
		fprintf_s(fp_write, "BER:%.02f%%\n", BER * 100);
		fprintf_s(fp_write, "BeginTime:%04d-%02d-%02d %02d:%02d:%02d.%02d\n", sysTimeStart.wYear, sysTimeStart.wMonth, sysTimeStart.wDay, sysTimeStart.wHour, sysTimeStart.wMinute, sysTimeStart.wSecond, sysTimeStart.wMilliseconds);
		fprintf_s(fp_write, "EndTime:%04d-%02d-%02d %02d:%02d:%02d.%02d\n", sysTimeEnd.wYear, sysTimeEnd.wMonth, sysTimeEnd.wDay, sysTimeEnd.wHour, sysTimeEnd.wMinute, sysTimeEnd.wSecond, sysTimeEnd.wMilliseconds);
		fprintf_s(fp_write, "TotalTime:%02dDays %02dHours %02dMinutes %02dSeconds %03dMilliseconds\n", Day, Hour, Minute, Second, MilliSecond);
		fprintf_s(fp_write, "\n\n");
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "No tests have been performed!\n");
		retVal = -13;
		BER = -1;
	}
	fclose(fp_write);
	return retVal;
}

DWORD CCmdAcco8300BERTest::GetAddress(string mdtype, DWORD chAddr, DWORD funcAddr)
{
	DWORD address = 0x00;
	map<string, int>::iterator iter = g_map8300AddressType.find(mdtype);
	if (iter != g_map8300AddressType.end())
	{
		int type = iter->second;
		if (type == 0)
		{
			address = (chAddr << 10) | (funcAddr << 2);
		}
		else if (type == 1)
		{
			address = (chAddr << 18) | (funcAddr << 2);
		}
		else
		{
			address = 0xFFFFFFFF;
		}
	}
	else
	{
		address = 0xFFFFFFFF;
	}
	return address;
}

uint32_t CmdDataCompare(uint32_t &TotalCnt, uint32_t &ErrCnt, uint32_t BlockCnt, DWORD startAddr, uint32_t Length, DWORD* pSetBuff, DWORD*pGetBuff)
{
	int retVal = 0;

	for (uint32_t i = 0; i < Length; i++)
	{
		if (pSetBuff[i] != pGetBuff[i])//数据比较
		{
			if (ErrCnt == 0)//只写一次表头
			{
				fp_write = fopen(g_fname, "a");
				if (fp_write == NULL)
				{
					PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
					retVal = -10;
				}
				else
				{
					fprintf_s(fp_write, "[No]           errAddress     actValue       (expValue)\n");
					fclose(fp_write);
				}
			}
			ErrCnt++;
			fp_write = fopen(g_fname, "a");
			if (fp_write == NULL)
			{
				PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
				retVal = -10;
			}
			else
			{
				fprintf_s(fp_write, "[%d-%d]     0X%08X     0X%08X     (0X%08X)\n", BlockCnt + 1, i + 1, startAddr + i, pGetBuff[i], pSetBuff[i]);//写入错误信息，每次写入打开关闭文件
				fclose(fp_write);
			}
			if (ErrCnt == THRESHOLD)
			{
				retVal = -11;
				break;
			}
		}
		TotalCnt = BlockCnt*Length + i + 1;
	}
	return retVal;
}

uint32_t CmdDataCompare(uint32_t &TotalCnt, uint32_t &ErrCnt, uint32_t BlockCnt, DWORD startAddr, uint32_t Length, uint8_t* pSetBuff, uint8_t* pGetBuff)
{
	int retVal = 0;

	for (uint32_t i = 0; i < Length; i++)
	{
		if (pSetBuff[i] != pGetBuff[i])//数据比较
		{
			if (ErrCnt == 0)//只写一次表头
			{
				fp_write = fopen(g_fname, "a");
				if (fp_write == NULL)
				{
					PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
					retVal = -10;
				}
				else
				{
					fprintf_s(fp_write, "[No]           errAddress     actValue       (expValue)\n");
					fclose(fp_write);
				}
			}
			ErrCnt++;
			fp_write = fopen(g_fname, "a");
			if (fp_write == NULL)
			{
				PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
				retVal = -10;
			}
			else
			{
				fprintf_s(fp_write, "[%d-%d]     0X%08X     0X%08X     (0X%08X)\n", BlockCnt + 1, i + 1, startAddr + i, pGetBuff[i], pSetBuff[i]);//写入错误信息，每次写入打开关闭文件
				fclose(fp_write);
			}
			if (ErrCnt == THRESHOLD)
			{
				retVal = -11;
				break;
			}
		}
		TotalCnt = BlockCnt*Length + i + 1;
	}
	return retVal;
}

uint32_t CmdDataCompare(uint32_t &TotalCnt, uint32_t &ErrCnt, DWORD getData1, DWORD reg1Data, DWORD getData2, DWORD reg2Data)
{
	int retVal = 0;

	if ((getData1 != reg1Data) || (getData2 != reg2Data))//数据比较
	{
		if (ErrCnt == 0)//只写一次表头
		{
			fp_write = fopen(g_fname, "a");
			if (fp_write == NULL)
			{
				PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
				retVal = -10;
			}
			else
			{
				fprintf_s(fp_write, "[No]reg1Actual  (reg1Expect)  reg2Actual  (reg2Expect)\n");
				fclose(fp_write);
			}
		}
		fp_write = fopen(g_fname, "a");
		if (fp_write == NULL)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
			retVal = -10;
		}
		else
		{
			fprintf_s(fp_write, "[%d]0X%08X  (0X%08X)  0X%08X  (0X%08X)\n", TotalCnt + 1, getData1, reg1Data, getData2, reg2Data);//写入错误信息，每次写入打开关闭文件
			fclose(fp_write);
		}
		ErrCnt++;
		if (ErrCnt == THRESHOLD)
		{
			TotalCnt++;
			retVal = -11;
		}
	}
	return retVal;
}


uint32_t CCmdAcco8300BERTest::CmdHelpInf(vector<string> vectCmdLine)
{
	PrintLog(LOG_LEVEL_INFO, "############################################################################################################################################\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8300 REG TIME <MINUTES(D)> <MODULE(S)> <Data0_Reg(X)> <Data0_Value(X/S)> <Data1_Reg(X)> <Data1_Value(X/S)>                       #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8300 REG NUM <NUMBERS(D)> <MODULE(S)> <Data0_Reg(X)> <Data0_Value(X/S)> <Data1_Reg(X)> <Data1_Value(X/S)>                        #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8300 REG_G TIME <MINUTES(D)> <Data0_Reg(X)> <Data0_Value(X/S)> <Data1_Reg(X)> <Data1_Value(X/S)>                                 #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8300 REG_G NUM <NUMBERS(D)> <Data0_Reg(X)> <Data0_Value(X/S)> <Data1_Reg(X)> <Data1_Value(X/S)>                                  #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8300 BLOCK TIME <MINUTES(D)> <MODULE(S)> <Reg1(X)> <StartAddr(X)> < Reg2(X)> <Length(D)> < Reg3(X)> <Mode (D)> <ACCStartData (X)>#\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8300 BLOCK NUM < NUMBERS(D)> <MODULE(S)> <Reg1(X)> <StartAddr(X)> < Reg2(X)> <Length(D)> < Reg3(X)> <Mode (D)> <ACCStartData (X)>#\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8300 BLOCK_G TIME <MINUTES(D)> <Reg1(X)> <StartAddr(X)> < Reg2(X)> <Length(D)> < Reg3(X)> <Mode (D)> <ACCStartData (X)>          #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8300 BLOCK_G NUM < NUMBERS(D)> <Reg1(X)> <StartAddr(X)> < Reg2(X)> <Length(D)> < Reg3(X)> <Mode (D)> <ACCStartData (X)>          #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8300 FLASH TIME <MINUTES(D)> <MODULE(S)> <StartAddr(X)> <Length(D)> <DataType(S)>                                                #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8300 FLASH NUM <NUMBERS(D)> <MODULE(S)> <StartAddr(X)> <Length(D)> <DataType(S)>                                                 #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8300 HELP                                                                                                                        #\n");
	PrintLog(LOG_LEVEL_INFO, "############################################################################################################################################\n");
	return 0;
}
