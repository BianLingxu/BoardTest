#include "stdafx.h"
#include "CmdAcco8300FlashBERTest.h"
#include "CmdAcco8300BERTest.h"
#include "CmdAcco8300Module.h"
#include "UtilLibrary.h"
#include <time.h>
#include <map>
#include "STSSP8201/STSSP8201.h"
#include <io.h>
#include <direct.h>

#define THRESHOLD 1000
using namespace std;
extern map<string, DWORD> g_map8300SlotAddress;
extern map<string, DWORD> g_mapModuleAddress;
extern map<string, int> g_map8300AddressType;
extern int g_systemType;
extern FILE* fp_write;
extern bool g_blockFlag;
extern char g_fname[1024];
extern uint32_t g_passCount;
extern uint32_t g_failCount;
extern uint32_t g_length;
extern string g_command[1024];
extern FLASH_PROPERTY g_flashProperty;

CCmdAcco8300FlashBERTest::CCmdAcco8300FlashBERTest()
{
}

CCmdAcco8300FlashBERTest::~CCmdAcco8300FlashBERTest()
{
}

uint32_t CCmdAcco8300FlashBERTest::CmdFlashParse(vector<string> vectCmdLine)
{
	DWORD startAddr = 0x00;
	SYSTEMTIME sysTime;
	string result;
	map<string, DWORD>::iterator iter;
	AT_SYSTEM_TYPE systemType;
	int Length = 0;
	int retVal = 0;
	int ret = 0;
	char fnameTemp[1024] = { "" };
	char str[1024] = { "" };

	ToUpperCase(vectCmdLine[0]);
	if (("BER_ACCO8300" == vectCmdLine[0] && CmdExceptionProcess(vectCmdLine, 8, 0X00001111) == 0) || ("BER_ACCO8200" == vectCmdLine[0]&&CmdExceptionProcess(vectCmdLine, 8, 0X00001241) == 0))
	{
		ToUpperCase(vectCmdLine[2]);
		ToUpperCase(vectCmdLine[3]);  
		ToUpperCase(vectCmdLine[7]);
		sscanf_s(vectCmdLine[5].c_str(), "%X", &startAddr);
		sscanf_s(vectCmdLine[6].c_str(), "%d", &Length);
		if ("BER_ACCO8300" == vectCmdLine[0])
		{
			iter = g_map8300SlotAddress.find(vectCmdLine[2]);
			systemType = AT_STS8300;
		}
		else
		{
			iter = g_mapModuleAddress.find(vectCmdLine[2]);
			systemType = AT_STS8200;
		}
		if (("TIME" != vectCmdLine[3]) && ("NUM" != vectCmdLine[3]))
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown BERTest command\n");
			return -7;
		}
	
		if (!g_blockFlag)//非block指令，创建一个新的测试报告文件
		{
			GetLocalTime(&sysTime);
			sprintf_s(fnameTemp, STS_ARRAY_SIZE(fnameTemp), "BER_Test_%04d%02d%02d_%02d%02d%02d.txt", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
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
			fprintf_s(fp_write, "->Module:%s(0X%08X)\n", vectCmdLine[2].c_str(), iter->second);
			fprintf_s(fp_write, "->StartAddr:0X%08X\n", startAddr);
			fprintf_s(fp_write, "->Length:%d\n", Length);
			fprintf_s(fp_write, "-----------------------\n");
			fclose(fp_write);
			if ("TIME" == vectCmdLine[3])//时间测试
			{
				double Time = 0.0;
				Time = atof(vectCmdLine[4].c_str());
				ret = CmdFlashTimeTest(Time, startAddr, iter->second, Length, vectCmdLine[7], systemType);
			}
			else//次数测试
			{
				uint32_t Numbers = 0;
				sscanf_s(vectCmdLine[4].c_str(), "%d", &Numbers);
				ret = CmdFlashNumTest(Numbers, startAddr, iter->second, Length, vectCmdLine[7], systemType);
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
	}
	retVal = ret;
	return retVal;	
}

uint32_t CCmdAcco8300FlashBERTest::CmdFlashTimeTest(double Time, DWORD startAddr, DWORD chAddr, uint32_t Length, string dataType, AT_SYSTEM_TYPE systemType)
{
	DWORD eraseAddr = 0X00;
	AT_ERASE_MODE eraseMode;
	uint32_t TotalCnt = 0;
	uint32_t BlockCnt = 0;
	uint32_t ErrCnt = 0;
	uint32_t timeVal = 0;
	uint8_t eraseData = 0XFF;
	SYSTEMTIME sysTimeStart, sysTimeEnd;
	FILE *fpFile;
	int retVal = 0;
	//int cnt = 0;

	if ((startAddr & 0X000000FF) != 0)//判断起始地址低8位是否为0
	{
		PrintLog(LOG_LEVEL_ERROR, "Improper start address or data length\n");
		return -6;
	}
	CCmdAcco8300Module cCmdAcco8300Md;
	map<string, int>::iterator dataTypeIter = cCmdAcco8300Md.m_dataType.find(dataType);
	if (dataTypeIter == cCmdAcco8300Md.m_dataType.end() && Length == -1)
	{
		fpFile = fopen(dataType.c_str(), "r");
		if (NULL == fpFile)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unable to open the data file\n");
			retVal = -9;
		}
		else
		{
			Length = filelength(fileno(fpFile))/4;
			fclose(fpFile);
		}
	}
	if (Length < 0x00001000)//根据数据长度缺点擦除起始地址和擦除模式
	{
		eraseAddr = startAddr & 0XFFFFF000;
		eraseMode = AT_4K_SECTOR_ERASE;
	}
	else if (Length < 0x00008000)
	{
		eraseAddr = startAddr & 0XFFFF8000;
		eraseMode = AT_32K_BLOCK_ERASE;
	}
	else if (Length < 0x00010000)
	{
		eraseAddr = startAddr & 0XFFFF0000;
		eraseMode = AT_64K_BLOCK_ERASE;
	}
	else
	{
		eraseMode = AT_BULK_ERASE;
	}
	uint8_t* pSetBuff = (uint8_t*)malloc(Length);
	if (NULL == pSetBuff)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
		return -11;
	}
	uint8_t* pGetBuff = (uint8_t*)malloc(Length);
	if (NULL == pGetBuff)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
		free(pSetBuff);
		return -11;
	}
	
	AT_FLASH flash(systemType, chAddr);
	flash.SetFlashProperty(AT_FLASH_TYPE(g_flashProperty.flashType), AT_SPI_MODE(g_flashProperty.spiMode), g_flashProperty.flashFuncAddr);
	if (dataTypeIter == cCmdAcco8300Md.m_dataType.end())
	{
		fpFile = fopen(dataType.c_str(), "r");
		if (NULL == fpFile) {
			PrintLog(LOG_LEVEL_ERROR, "Unable to create a data file\n");
			retVal = -9;
		}
		else
		{
			for (int i = 0; i < Length; i++)
			{
				fscanf(fpFile, "%X", pSetBuff + i);
			}
			fclose(fpFile);
		}
	}
	else
	{
		FillDataPattern(pSetBuff, Length, DATA_PATTERN_SETS(dataTypeIter->second), 1);
	}
	Time *= 60000000;
	GetLocalTime(&sysTimeStart);
	TickStart();//开始计时
	while (timeVal < Time)
	{
			flash.SetFlashStartAddr(startAddr);
			retVal=flash.SetData((char*)pSetBuff, Length);
			if (retVal == 0)
			{
				retVal = flash.GetData((char*)pGetBuff, Length);
				if (retVal == 0)
				{
					//for (uint32_t i = 0; i < Length; i++)
					//{
						//cnt++;
						//if (cnt == 100)
						//{
						//	pGetBuff[i] += 1;
						//	cnt = 0;
						//}
						retVal = CmdDataCompare(TotalCnt, ErrCnt, BlockCnt, startAddr, Length, pSetBuff, pGetBuff);
						
						//if (pSetBuff[i] != pGetBuff[i])//数据比较
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
						//	fp_write = fopen(g_fname, "a");
						//	if (fp_write == NULL)
						//	{
						//		PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
						//		retVal = -13;
						//	}
						//	else
						//	{
						//		fprintf_s(fp_write, "[%d-%d]       0X%08X   0X%08X   (0X%08X)\n", BlockCnt + 1, i + 1, startAddr + i, pGetBuff[i], pSetBuff[i]);//写入错误信息，每次写入打开关闭文件
						//		fclose(fp_write);
						//		ErrCnt++;
						//		if (ErrCnt == THRESHOLD)
						//		{
						//			break;
						//		}
						//	}
						//}
						//TotalCnt = BlockCnt*Length + i + 1;
					//}
					flash.SetFlashStartAddr(eraseAddr);
					retVal = flash.EraseData(eraseMode);
					if (retVal == 0)
					{
						retVal = flash.GetData((char*)pGetBuff, Length);
						if (retVal == 0)
						{
							for (int i = 0; i < Length; i++)
							{
								if (pGetBuff[i] != eraseData)
								{
									PrintLog(LOG_LEVEL_ERROR, "Erase failed\n");
									retVal = -15;
									break;
								}
							}
						}
						else
						{
							PrintLog(LOG_LEVEL_ERROR, "Read failed\n");
							retVal = -14;
						}
					}
					else
					{
						PrintLog(LOG_LEVEL_ERROR, "Erase failed(Overtime)\n");
						retVal = -16;
					}
				}
				else
				{
					PrintLog(LOG_LEVEL_ERROR, "Read failed\n");
					retVal = -17;
				}
			}
			else
			{
				PrintLog(LOG_LEVEL_ERROR, "Write failed\n");
				retVal = -18;
			}
		BlockCnt++;
		TickEnd(timeVal);//计时结束
		if (retVal == -11)
		{
			break;
		}
	}
	GetLocalTime(&sysTimeEnd);
	if (!retVal)
	{
		retVal = CmdPrint(TotalCnt, ErrCnt, sysTimeStart, sysTimeEnd);
	}
	free(pSetBuff);
	free(pGetBuff);
	return retVal;
}

uint32_t CCmdAcco8300FlashBERTest::CmdFlashNumTest(uint32_t Numbers, DWORD startAddr, DWORD chAddr, uint32_t Length, string dataType, AT_SYSTEM_TYPE systemType)
{
	DWORD eraseAddr = 0X00;
	AT_ERASE_MODE eraseMode;
	uint32_t TotalCnt = 0;
	uint32_t BlockCnt = 0;
	uint32_t ErrCnt = 0;
	uint32_t timeVal = 0;
	uint8_t eraseData = 0XFF;
	SYSTEMTIME sysTimeStart, sysTimeEnd;
	FILE *fpFile;
	int retVal = 0;
	//int cnt = 0;

	if ((startAddr & 0X000000FF) != 0)//判断起始地址低8位是否为0
	{
		startAddr &= 0XFFFFFF00;
		PrintLog(LOG_LEVEL_ERROR, "Improper start address or data length\n");
		return -6;
	}
	CCmdAcco8300Module cCmdAcco8300Md;
	map<string, int>::iterator dataTypeIter = cCmdAcco8300Md.m_dataType.find(dataType);
	if (dataTypeIter == cCmdAcco8300Md.m_dataType.end() && Length == -1)
	{
		fpFile = fopen(dataType.c_str(), "r");
		if (NULL == fpFile)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unable to open the data file\n");
			retVal = -9;
		}
		else
		{
			Length = filelength(fileno(fpFile)) / 4;
			fclose(fpFile);
		}
	}
	if (Length < 0x00001000)//根据数据长度缺点擦除起始地址和擦除模式
	{
		eraseAddr = startAddr & 0XFFFFF000;
		eraseMode = AT_4K_SECTOR_ERASE;
	}
	else if (Length < 0x00008000)
	{
		eraseAddr = startAddr & 0XFFFF8000;
		eraseMode = AT_32K_BLOCK_ERASE;
	}
	else if (Length < 0x00010000)
	{
		eraseAddr = startAddr & 0XFFFF0000;
		eraseMode = AT_64K_BLOCK_ERASE;
	}
	else
	{
		eraseMode = AT_BULK_ERASE;
	}
	uint8_t* pSetBuff = (uint8_t*)malloc(Length+1);
	if (NULL == pSetBuff)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
		return -11;
	}
	uint8_t* pGetBuff = (uint8_t*)malloc(Length);
	if (NULL == pGetBuff)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
		free(pSetBuff);
		return -11;
	}
	AT_FLASH flash(systemType, chAddr);
	flash.SetFlashProperty(AT_FLASH_TYPE(g_flashProperty.flashType), AT_SPI_MODE(g_flashProperty.spiMode), g_flashProperty.flashFuncAddr);
	if (dataTypeIter == cCmdAcco8300Md.m_dataType.end())
	{
		fpFile = fopen(dataType.c_str(), "r");
		if (NULL == fpFile) {
			PrintLog(LOG_LEVEL_ERROR, "Unable to create a data file\n");
			retVal = -9;
		}
		else
		{
			for (int i = 0; i < Length; i++)
			{
				fscanf(fpFile, "%hX", pSetBuff + i);
			}
			fclose(fpFile);
		}
	}
	else
	{
		FillDataPattern((uint8_t *)pSetBuff, Length, DATA_PATTERN_SETS(dataTypeIter->second), 1);
	}
	//pSetBuff = (char*)pBuff;
	GetLocalTime(&sysTimeStart);
	TickStart();//开始计时
	for (BlockCnt = 0; BlockCnt < Numbers; BlockCnt++)
	{
			flash.SetFlashStartAddr(startAddr);
			retVal = flash.SetData((char*)pSetBuff, Length);
			if (retVal == 0)
			{
				flash.SetFlashStartAddr(startAddr);
				retVal = flash.GetData((char*)pGetBuff, Length);
				if (retVal == 0)
				{
					//for (uint32_t i = 0; i < Length; i++)
					//{
						//cnt++;
						//if (cnt == 100)
						//{
						//	pGetBuff[i] += 1;
						//	cnt = 0;
						//}
						retVal = CmdDataCompare(TotalCnt, ErrCnt, BlockCnt, startAddr, Length, pSetBuff, pGetBuff);
						//if (pSetBuff[i] != pGetBuff[i])//数据比较
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
						//	fp_write = fopen(g_fname, "a");
						//	if (fp_write == NULL)
						//	{
						//		PrintLog(LOG_LEVEL_ERROR, "Unable to open a test report\n");
						//		retVal = -13;
						//	}
						//	else
						//	{
						//		fprintf_s(fp_write, "[%d-%d]       0X%08X   0X%08X   (0X%08X)\n", BlockCnt + 1, i + 1, startAddr + i, pGetBuff[i], pSetBuff[i]);//写入错误信息，每次写入打开关闭文件
						//		fclose(fp_write);
						//		TotalCnt = BlockCnt*Length + i + 1;
						//		ErrCnt++;
						//		if (ErrCnt == THRESHOLD)
						//		{
						//			break;
						//		}
						//	}
						//}
					//}
					flash.SetFlashStartAddr(eraseAddr);
					retVal = flash.EraseData(eraseMode);
					if (retVal == 0)
					{
						retVal = flash.GetData((char*)pGetBuff, Length);
						if (retVal == 0)
						{
							for (int i = 0; i < Length; i++)
							{
								if (pGetBuff[i] != eraseData)
								{
									PrintLog(LOG_LEVEL_ERROR, "Erase failed\n");
									retVal = -15;
									break;
								}
							}
						}
						else
						{
							PrintLog(LOG_LEVEL_ERROR, "Read failed\n");
							retVal = -14;
						}
					}
					else
					{
						PrintLog(LOG_LEVEL_ERROR, "Erase failed(Overtime)\n");
						retVal = -16;
					}
				}
				else
				{
					PrintLog(LOG_LEVEL_ERROR, "Read failed\n");
					retVal = -17;
				}
			}
			else
			{
				PrintLog(LOG_LEVEL_ERROR, "Write failed\n");
				retVal = -18;
			}
		TickEnd(timeVal);//计时结束
		if (retVal == -11)
		{
			break;
		}
	}
	GetLocalTime(&sysTimeEnd);
	if (!retVal)
	{
		retVal = CmdPrint(TotalCnt, ErrCnt, sysTimeStart, sysTimeEnd);
	}
	free(pSetBuff);
	free(pGetBuff);
	return retVal;
}
