#include "stdafx.h"
#include "CmdAcco8200BlockBERTest.h"
#include "CmdAcco8300BERTest.h"
#include "CmdAcco8300Module.h"
#include "UtilLibrary.h"
#include <time.h>
#include <map>
#include "STSSP8201/STSSP8201.h"
#include "Sts8100.h"

#define THRESHOLD 1000
using namespace std;
extern map<string, DWORD> g_mapModuleAddress;
extern bool g_blockFlag;
extern char g_fname[1024];
extern FILE* fp_write;
extern uint32_t g_passCount;
extern uint32_t g_failCount;
extern uint32_t g_length;
extern string g_command[1024];

CCmdAcco8200BlockBERTest::CCmdAcco8200BlockBERTest()
{
}

CCmdAcco8200BlockBERTest::~CCmdAcco8200BlockBERTest()
{
}

uint32_t CCmdAcco8200BlockBERTest::CmdBlockParse(vector<string> vectCmdLine)
{
	DWORD inputAddr1 = 0x00;
	DWORD inputAddr2 = 0x00;
	DWORD inputAddr3 = 0x00;
	DWORD startAddr = 0x00;
	DWORD AccStartAddr = 0x00;
	uint32_t Length = 0;
	uint32_t Mode = 0;
	uint32_t strlength = 0;
	int retVal = 0;
	int ret = 0;
	SYSTEMTIME sysTime;
	char str[1024] = { 0 };
	char fnameTemp[1024] = { "" };
	string result = { "" };

	if (CmdExceptionProcess(vectCmdLine, 11, 0X00000281) == 0)
	{
		sscanf_s(vectCmdLine[5].c_str(), "%X", &inputAddr1);
		sscanf_s(vectCmdLine[6].c_str(), "%X", &startAddr);
		sscanf_s(vectCmdLine[7].c_str(), "%X", &inputAddr2);
		sscanf_s(vectCmdLine[8].c_str(), "%d", &Length);
		sscanf_s(vectCmdLine[9].c_str(), "%X", &inputAddr3);
		sscanf_s(vectCmdLine[10].c_str(), "%d", &Mode);
		if ((Mode == DATATYPE_INC_CLEAR_TO_VALUE || Mode == DATATYPE_INC_NOT_CLEAR_TO_VALUE) && (vectCmdLine.size() > 11))
		{
			sscanf_s(vectCmdLine[11].c_str(), "%X", &AccStartAddr);
		}
		if ((inputAddr1 > 0XFF) || (inputAddr2 > 0XFF) || (inputAddr3 > 0XFF))
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
			fprintf_s(fp_write, "->");
			for (int i = 0; i < vectCmdLine.size(); i++)//保存并打印指令
			{
				g_command[g_length] += vectCmdLine[i];
				g_command[g_length] += " ";
			}
			fprintf_s(fp_write, "%s\n", g_command[g_length].c_str());

			fprintf_s(fp_write, "----------------------------DataInfo--------------------------\n");
			fprintf_s(fp_write, "->Module:%s(0X%08X)\n", vectCmdLine[4].c_str(), iter->second);
			fprintf_s(fp_write, "->StartAddr:0X%08X\n", startAddr);
			fprintf_s(fp_write, "->Length:%d\n", Length);
			fprintf_s(fp_write, "-----------------------\n");
			fclose(fp_write);
			if ("TIME" == vectCmdLine[2])//时间测试
			{
				double Time = 0.0;
				Time = atof(vectCmdLine[3].c_str());
				ret = CmdBlockTimeTest(Time, inputAddr1, startAddr, inputAddr2, Length, inputAddr3, Mode, AccStartAddr, iter->second);
			}
			else//次数测试
			{
				uint32_t Numbers = 0;
				sscanf_s(vectCmdLine[3].c_str(), "%d", &Numbers);
				ret = CmdBlockNumTest(Numbers, inputAddr1, startAddr, inputAddr2, Length, inputAddr3, Mode, AccStartAddr, iter->second);
			}
			if (ret >= 0)
			{
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
				result = "Other ERR";
				g_failCount++;
			}
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Unable to open the test report\n");
		}
		retVal = ret;
	}
	return retVal;
}

uint32_t CCmdAcco8200BlockBERTest::CmdBlockTimeTest(double Time, DWORD Addr1, DWORD startAddr, DWORD Addr2, uint32_t Length, DWORD Addr3, uint32_t Mode, DWORD AccStartAddr, DWORD chAddr)
{
	uint32_t TotalCnt = 0;
	uint32_t BlockCnt = 0;
	uint32_t ErrCnt = 0;
	uint32_t timeVal = 0;
	int retVal = 0;
	SYSTEMTIME sysTimeStart, sysTimeEnd;
	DWORD startAddrRead = 0x00;
	Time *= 60000000;
	GetLocalTime(&sysTimeStart);
	TickStart();//开始计时
	while (timeVal < Time)
	{
		ModeJudge(Mode, Length, AccStartAddr);

		//_outp(AccStartAddr, 0x27);
		//_outp(AccStartAddr + 2, 1024);
		//_outp(AccStartAddr, 0x28);
		//_outp(AccStartAddr + 2, 1024);
		//_outp(AccStartAddr, 0x29);
		//_outp(AccStartAddr + 2, 1);
		//_outp(AccStartAddr, 0x39);
		//_outp(AccStartAddr + 2, 5);

		_outpw(chAddr, Addr1);
		_outpw(chAddr + 2, startAddr);//写起始地址
		if (Addr2 != 0xFF)
		{
			_outpw(chAddr, Addr2);
			_outpw(chAddr + 2, Length);//写地址长度
		}
		for (uint32_t i = 0; i < Length; i++)
		{
			_outpw(chAddr, Addr3);
			_outpw(chAddr + 2, m_regData[i] & 0x0FFFF);//写length个数据

			_outpw(chAddr, 0x08);
			_outpw(chAddr + 2, m_regData[i] >> 16 & 0xFFFF);
		}
		DelayUs(Length / 10);
		_outpw(chAddr, Addr1);
		_outpw(chAddr + 2, startAddr);//写起始地址
		if (Addr2 != 0xFF)
		{
			_outpw(chAddr, Addr2);
			_outpw(chAddr + 2, Length);//写地址长度
		}
		for (uint32_t i = 0; i < Length; i++)
		{
			DWORD data = 0;
			_outpw(chAddr, Addr3);
			data = _inpw(chAddr + 2);
			_outpw(chAddr, 0x08);
			data |= _inpw(chAddr + 2) << 16;

			m_getData.push_back(data);//读length个数据
		}
		retVal = CmdDataCompare(TotalCnt, ErrCnt, BlockCnt, startAddr, Length, m_regData.data(), m_getData.data());
		//_outp(Addr1, startAddr);//写起始地址
		//if (Addr2 != 0xFFFFFFFF)
		//{
		//	_outp(Addr2, Length);//写地址长度
		//}
		//for (uint32_t i = 0; i < Length; i++)
		//{
		//	_outp(Addr3, m_regData[i]);//写length个数据
		//}
		//DelayUs(Length / 10);
		//_outp(Addr1, startAddrRead);//写起始地址
		//if (Addr2 != 0xFFFFFFFF)
		//{
		//	_outp(Addr2, Length);//写地址长度
		//}
		//for (uint32_t i = 0; i < Length; i++)
		//{
		//	m_getData.push_back(_inp(Addr3));//读length个数据
			//cnt++;
			//if (cnt == 100)
			//{
			//	m_getData[i] += 1;
			//	cnt = 0;
			//}

			//if (m_getData[i] != m_regData[i])//数据比较
			//{
			//	if (ErrCnt == 0)//只写一次表头
			//	{
			//		fp_write = fopen(g_fname, "a");
			//		if (fp_write == NULL)
			//		{
			//			PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
			//			retVal = -14;
			//		}
			//		else
			//		{
			//			fprintf_s(fp_write, "[No]        errAddress   actValue     (expValue)\n");
			//			fclose(fp_write);
			//		}
			//	}
			//	ErrCnt++;
			//	fp_write = fopen(g_fname, "a");
			//	if (fp_write == NULL)
			//	{
			//		PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
			//		retVal = -3;
			//	}
			//	else
			//	{
			//		fprintf_s(fp_write, "[%d-%d]       0X%08X   0X%08X   (0X%08X)\n", BlockCnt + 1, i + 1, startAddr + i, m_getData[i], m_regData[i]);//写入错误信息，每次写入打开关闭文件
			//		fclose(fp_write);
			//	}
			//	TotalCnt = BlockCnt*Length + i + 1;
			//	if (ErrCnt == THRESHOLD)
			//	{
			//		retVal = -4;
			//		break;
			//	}
			//}
		//}
		m_regData.clear();
		m_getData.clear();
		if (retVal == -11)
		{
			break;
		}
		BlockCnt++;
		TickEnd(timeVal);//计时结束
	}

	GetLocalTime(&sysTimeEnd);
	if (!retVal)
	{
		retVal = CmdPrint(TotalCnt, ErrCnt, sysTimeStart, sysTimeEnd);
	}
	
	return retVal;
}

uint32_t CCmdAcco8200BlockBERTest::CmdBlockNumTest(uint32_t Numbers, DWORD Addr1, DWORD startAddr, DWORD Addr2, uint32_t Length, DWORD Addr3, uint32_t Mode, DWORD AccStartAddr, DWORD chAddr)
{
	uint32_t TotalCnt = 0;
	uint32_t BlockCnt = 0;
	uint32_t ErrCnt = 0;
	uint32_t timeVal = 0;
	int retVal = 0;
	int cnt = 0;
	SYSTEMTIME sysTimeStart, sysTimeEnd;
	DWORD startAddrRead = 0x00;

	//if (Addr2 == 0xFFFFFFFF)
	//{
	//	startAddrRead = startAddr + 0x10000000;
	//}
	//else
	//{
	//	startAddrRead = startAddr;
	//}
	GetLocalTime(&sysTimeStart);
	TickStart();//开始计时
	for (BlockCnt = 0; BlockCnt < Numbers; BlockCnt++)
	{
		ModeJudge(Mode, Length, AccStartAddr);
		//write_dw(Addr1, startAddr);//写起始地址
		//if (Addr2 != 0xFFFFFFFF)
		//{
		//	write_dw(Addr2, Length);//写地址长度
		//}
		//for (uint32_t i = 0; i < Length; i++)
		//{
		//	write_dw(Addr3, m_regData[i]);//写length个数据
		//}
		//DelayUs(Length / 10);
		//write_dw(Addr1, startAddrRead);//写起始地址
		//if (Addr2 != 0xFFFFFFFF)
		//{
		//	write_dw(Addr2, Length);//写地址长度
		//}
		//for (uint32_t i = 0; i < Length; i++)
		//{
		//	m_getData.push_back(read_dw(Addr3));//读length个数据
		//	cnt++;
		//	if (cnt == 50)
		//	{
		//		m_getData[i] += 1;
		//		cnt = 0;
		//	}
		//	if (m_getData[i] != m_regData[i])//数据比较
		//	{
		//		if (ErrCnt == 0)//只写一次表头
		//		{
		//			fp_write = fopen(g_fname, "a");
		//			if (fp_write == NULL)
		//			{
		//				PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
		//				retVal = -14;
		//			}
		//			else
		//			{
		//				fprintf_s(fp_write, "[No]        errAddress   actValue     (expValue)\n");
		//				fclose(fp_write);
		//			}
		//		}
		//		ErrCnt++;
		//		fp_write = fopen(g_fname, "a");
		//		if (fp_write == NULL)
		//		{
		//			PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
		//			retVal = -3;
		//		}
		//		else
		//		{
		//			fprintf_s(fp_write, "[%d-%d]       0X%08X   0X%08X   (0X%08X)\n", BlockCnt + 1, i + 1, startAddr + i, m_getData[i], m_regData[i]);//写入错误信息，每次写入打开关闭文件
		//			fclose(fp_write);
		//		}
		//		TotalCnt = BlockCnt*Length + i + 1;
		//		if (ErrCnt == THRESHOLD)
		//		{
		//			retVal = -4;
		//			break;
		//		}
		//	}
		//}
		_outpw(chAddr, Addr1);
		_outpw(chAddr + 2, startAddr);//写起始地址
		if (Addr2 != 0xFF)
		{
			_outpw(chAddr, Addr2);
			_outpw(chAddr + 2, Length);//写地址长度
		}
		for (uint32_t i = 0; i < Length; i++)
		{
			_outpw(chAddr, Addr3);
			_outpw(chAddr + 2, m_regData[i] & 0x0FFFF);//写length个数据

			//_outpw(chAddr, 0x08);
			//_outpw(chAddr + 2, m_regData[i] >> 16 & 0xFFFF);
		}
		DelayUs(Length / 10);
		_outpw(chAddr, Addr1);
		_outpw(chAddr + 2, startAddr);//写起始地址
		if (Addr2 != 0xFF)
		{
			_outpw(chAddr, Addr2);
			_outpw(chAddr + 2, Length);//写地址长度
		}
		for (uint32_t i = 0; i < Length; i++)
		{
			DWORD data = 0;
			_outpw(chAddr, Addr3);
			data = _inpw(chAddr + 2);
			//_outpw(chAddr, 0x08);
			//data |= _inpw(chAddr + 2) << 16;

			m_getData.push_back(data);//读length个数据
		}
		retVal = CmdDataCompare(TotalCnt, ErrCnt, BlockCnt, startAddr, Length, m_regData.data(), m_getData.data());
		m_regData.clear();
		m_getData.clear();
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

void CCmdAcco8200BlockBERTest::ModeJudge(uint32_t Mode, uint32_t Length, DWORD AccStartAddr)
{
	m_regData.reserve(Length);//预先分配内存
	m_getData.reserve(Length);
	bool flag = false;
	uint32_t temp = 0;
	switch (Mode)
	{
	case DATATYPE_0X00_0XFF://模式1：0X00000000和0XFFFFFFFF轮流写入
		while (m_regData.size()<Length)
		{
			m_regData.push_back(0X00000000);
			m_regData.push_back(0XFFFFFFFF);
		}
		break;
	case DATATYPE_0X55_0XAA://模式2：0X55555555和0XAAAAAAAA轮流写入
		while (m_regData.size() < Length)
		{
			m_regData.push_back(0X55555555);
			m_regData.push_back(0XAAAAAAAA);
		}
		break;
	case DATATYPE_PRBS://模式3：length个伪随机数
		for (uint32_t i = 0; i < Length; i++)
		{
			DWORD PRBSData = 0x00;
			PRBSData = (uint8_t)rand() << 24 | (uint8_t)rand() << 16 | (uint8_t)rand() << 8 | (uint8_t)rand();
			m_regData.push_back(PRBSData);
		}
		break;
	case DATATYPE_INC_CLEAR_TO_ZERO://模式4：累加数，从0开始，每次测试清0
	case DATATYPE_INC_NOT_CLEAR_TO_ZERO://模式5：累加数，从0开始，每次测试不清0
		if (Mode == 4)
		{
			m_totalCnt = 0;
		}
		temp = m_totalCnt;
		while (m_totalCnt < Length + temp)
		{
			DWORD AccData = 0x00;
			DWORD OutputData = 0x00;
			if (Length <= 0XFF)//分段累加
			{
				AccData = m_totalCnt & 0XFF;
				OutputData = (AccData << 24) | (AccData << 16) | (AccData << 8) | (AccData << 0);
			}
			else if ((Length > 0XFF) && (Length <= 0XFFFF))
			{
				AccData = m_totalCnt & 0XFFFF;
				OutputData = (AccData << 16) | (AccData << 0);
			}
			m_regData.push_back(OutputData);
			m_totalCnt++;
		}
		break;
	case DATATYPE_INC_CLEAR_TO_VALUE://模式6：累加数，指定初始值，每次测试回到初始值
	case DATATYPE_INC_NOT_CLEAR_TO_VALUE://模式7：累加数，指定初始值，每次测试不回到初始值
		if (Mode == 6)
		{
			m_totalCnt = AccStartAddr;
		}
		else
		{
			if (!flag)
			{
				m_totalCnt = AccStartAddr;
				flag = true;
			}
		}
		temp = m_totalCnt;
		while (m_totalCnt < Length + temp)
		{
			m_regData.push_back(m_totalCnt);
			m_totalCnt++;
		}
		break;
	default://默认为模式1
		while (m_regData.size() < Length + 1)
		{
			m_regData.push_back(0X00000000);
			m_regData.push_back(0XFFFFFFFF);
		}
	}
}

