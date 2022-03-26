#include "stdafx.h"
#include "CmdAcco8300Module.h"
#include "UtilLibrary.h"
#include <time.h>
#include <map>
#include "STSSP8201/STSSP8201.h"
#include "AT_CBB.h"
#include "CmdCfgIni.h"
#include <io.h>
#include <direct.h>

#pragma warning(disable : 4996)
using namespace std;

extern map<string, DWORD> g_map8300SlotAddress;
extern map<string, DWORD> g_mapModuleAddress;
extern map<string, int> g_map8300AddressType;
extern int g_systemType;
FLASH_PROPERTY g_flashProperty;
uint32_t g_regOfstVal1 = 0, g_regOfstVal2 = 0;
uint32_t g_ramBitWidth = 32, g_ramSinDepth = 0;
static uint32_t g_regRecord;

CCmdAcco8300Module::CCmdAcco8300Module()
{
	m_dataType.insert( {{ "0X00", 0 },{ "0X55", 1 },{ "0XAA", 2 },{ "0XFF", 3 },{ "0X5A", 4 },{ "0XA5", 5 },{ "INC", 6 },{ "DEC", 7 },{ "PRBS", 8 } });
}

CCmdAcco8300Module::~CCmdAcco8300Module()
{
}

uint32_t CCmdAcco8300Module::JudgeCmd(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

	do {
		if (vectCmdLine.size() < 2) {
			PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8300 command\n");
			retVal = 1;
			break;
		}

		ToUpperCase(vectCmdLine[1]);
		if ("HELP" == vectCmdLine[1])
		{
			retVal = CmdHelpInf(vectCmdLine);
		}
		else if ("OFS_ST" == vectCmdLine[1])
		{
			retVal = CmdOfstSet(vectCmdLine);
		}
		else if ("REG_WR" == vectCmdLine[1])
		{
			retVal = CmdRegWrite(vectCmdLine);
		}
		else if ("REG_RD" == vectCmdLine[1])
		{
			retVal = CmdRegRead(vectCmdLine);
		}
		else if ("G_WR" == vectCmdLine[1])
		{
			retVal = CmdGlobalRegWrite(vectCmdLine);
		}
		else if ("G_RD" == vectCmdLine[1])
		{
			retVal = CmdGlobalRegRead(vectCmdLine);
		}
		else if ("RAM_ST" == vectCmdLine[1])
		{
			retVal = CmdRamSet(vectCmdLine);
		}
		else if ("BRAM_WR" == vectCmdLine[1])
		{
			retVal = CmdBramWrite(vectCmdLine);
		}
		else if ("BRAM_RD" == vectCmdLine[1])
		{
			retVal = CmdBramRead(vectCmdLine);
		}
		else if ("DRAM_WR" == vectCmdLine[1])
		{
			retVal = CmdDramWrite(vectCmdLine);
		}
		else if ("DRAM_RD" == vectCmdLine[1])
		{
			retVal = CmdDramRead(vectCmdLine);
		}
		else if ("DCMTEMP" == vectCmdLine[1])
		{
			retVal = CmdDcmTemp(vectCmdLine);
		}
		else if ("FLASH_CFG" == vectCmdLine[1])
		{
			retVal = CmdFlashConfig(vectCmdLine);
		}
		else if ("FLASH_GETID" == vectCmdLine[1])
		{
			retVal = CmdFlashGetID(vectCmdLine);
		}
		else if ("FLASH_ERASE" == vectCmdLine[1])
		{
			retVal = CmdFlashErase(vectCmdLine);
		}
		else if ("FLASH_READ" == vectCmdLine[1])
		{
			retVal = CmdFlashRead(vectCmdLine);
		}
		else if ("FLASH_WRITE" == vectCmdLine[1])
		{
			retVal = CmdFlashWrite(vectCmdLine);
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8300 command\n");
			retVal = 1;
		}
	} while (0);

	return retVal;
}

uint32_t CCmdAcco8300Module::CmdOfstSet(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

	if (vectCmdLine.size() < 5)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8300 OFS_ST command\n");
		return 1;
	}

	ToUpperCase(vectCmdLine[2]);
	map<string, DWORD>::iterator iter = g_map8300SlotAddress.find(vectCmdLine[2]);
	DWORD chAddr = 0xFFFFFFFF;
	if (iter != g_map8300SlotAddress.end())
	{
		sscanf_s(vectCmdLine[3].c_str(), "%d", &g_regOfstVal1);
		sscanf_s(vectCmdLine[4].c_str(), "%d", &g_regOfstVal2);
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown module type\n");
		retVal = 1;
	}

	return retVal;
}

uint32_t CCmdAcco8300Module::CmdRegWrite(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

	if (vectCmdLine.size() < 5)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8300 REG_WR command\n");
		return 1;
	}

	ToUpperCase(vectCmdLine[2]);
	map<string, DWORD>::iterator iter = g_map8300SlotAddress.find(vectCmdLine[2]);
	DWORD chAddr = 0xFFFFFFFF;
	if (iter != g_map8300SlotAddress.end())
	{
		chAddr = iter->second;

		if (g_systemType == 8300)
		{
			DWORD regAddr = 0x00;
			DWORD regData = 0x00;

			sscanf_s(vectCmdLine[3].c_str(), "%X", &regAddr);
			sscanf_s(vectCmdLine[4].c_str(), "%X", &regData);

			DWORD inputAddr = GetAddress(iter->first, chAddr, regAddr);
			if (inputAddr != 0xFFFFFFFF)
			{
				write_dw(inputAddr, regData);
			}
			else
			{
				PrintLog(LOG_LEVEL_ERROR, "Unknown module type(Address=0xFFFFFFFF)\n");
				return 1;
			}
		}
		else if (g_systemType == 8200)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown REG_WR command(ACCO8200 module)\n");
			return 1;
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown REG_WR command(Unknown module)\n");
			return 1;
		}
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown module type\n");
		retVal = 1;
	}
	return retVal;
}

uint32_t CCmdAcco8300Module::CmdRegRead(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

	if (vectCmdLine.size() < 4)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8300 REG_RD command\n");
		return 1;
	}

	ToUpperCase(vectCmdLine[2]);
	map<string, DWORD>::iterator iter = g_map8300SlotAddress.find(vectCmdLine[2]);
	DWORD chAddr = 0xFFFFFFFF;
	if (iter != g_map8300SlotAddress.end())
	{
		chAddr = iter->second;
		if (g_systemType == 8300)
		{
			DWORD regAddr = 0x00;

			sscanf_s(vectCmdLine[3].c_str(), "%X", &regAddr);

			DWORD inputAddr = GetAddress(iter->first, chAddr, regAddr);
			if (inputAddr != 0xFFFFFFFF)
			{
				DWORD getData = read_dw(inputAddr);
				g_regRecord = getData;
				if (vectCmdLine.size() > 4)
				{
					DWORD expData = 0x00;
					DWORD mskData = 0x00;
					sscanf_s(vectCmdLine[4].c_str(), "%X", &expData);
					if (vectCmdLine.size() > 5)
					{
						sscanf_s(vectCmdLine[5].c_str(), "%X", &mskData);
					}

					getData &= ~mskData;
					expData &= ~mskData;
					if (getData != expData)
					{
						PrintLog(LOG_LEVEL_ERROR, "Read Register Module[%s] RegAddr[0x%08X] Value[0x%08X] Exp[0x%08X]\n", vectCmdLine[2].c_str(), regAddr, getData, expData);
						return 1;
					}
				}
				else
				{
					PrintLog(LOG_LEVEL_INFO, "Read Register Module[%s] RegAddr[0x%08X] Value[0x%08X]\n", vectCmdLine[2].c_str(), regAddr, getData);
				}
			}
			else
			{
				PrintLog(LOG_LEVEL_ERROR, "Unknown module type(Address=0xFFFFFFFF)\n");
				return 1;
			}
		}
		else if (g_systemType == 8200)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown REG_RD command(ACCO8200 module)\n");
			return 1;
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown REG_RD command(Unknown module)\n");
			return 1;
		}
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown module type\n");
		retVal = 1;
	}
	return retVal;
}

uint32_t CCmdAcco8300Module::CmdGlobalRegWrite(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

	if (vectCmdLine.size() < 4)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8300 G_WR command\n");
		return 1;
	}

	DWORD regAddr = 0x00;
	DWORD regData = 0x00;

	sscanf_s(vectCmdLine[2].c_str(), "%X", &regAddr);
	sscanf_s(vectCmdLine[3].c_str(), "%X", &regData);
	write_dw(regAddr, regData);

	return retVal;
}

uint32_t CCmdAcco8300Module::CmdGlobalRegRead(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

	if (vectCmdLine.size() < 3)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8300 G_RD command\n");
		return 1;
	}

	DWORD regAddr = 0x00;
	sscanf_s(vectCmdLine[2].c_str(), "%X", &regAddr);
	DWORD getData = read_dw(regAddr);

	if (vectCmdLine.size() > 3)
	{
		DWORD expData = 0x00;
		sscanf_s(vectCmdLine[3].c_str(), "%X", &expData);

		if (getData != expData)
		{
			PrintLog(LOG_LEVEL_ERROR, "Read Register RegAddr[0x%08X] Value[0x%08X] Exp[0x%08X]\n", regAddr, getData, expData);
			return 1;
		}
	}
	else
	{
		PrintLog(LOG_LEVEL_INFO, "Read Register RegAddr[0x%08X] Value[0x%08X]\n", regAddr, getData);
	}
	return retVal;
}

uint32_t CCmdAcco8300Module::CmdRamSet(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

	if (vectCmdLine.size() < 5)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8300 RAM_ST command\n");
		return 1;
	}

	ToUpperCase(vectCmdLine[2]);
	map<string, DWORD>::iterator iter = g_map8300SlotAddress.find(vectCmdLine[2]);
	DWORD chAddr = 0xFFFFFFFF;
	if (iter != g_map8300SlotAddress.end())
	{
		sscanf_s(vectCmdLine[3].c_str(), "%d", &g_ramBitWidth);
		sscanf_s(vectCmdLine[4].c_str(), "%d", &g_ramSinDepth);
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown module type\n");
		retVal = 1;
	}

	return retVal;
}

uint32_t CCmdAcco8300Module::CmdBramWrite(vector<string> vectCmdLine)
{
	uint32_t addrRegVal, cmdsRegVal, bramDepth, startLine;
	uint32_t witdhAlign, timeVal, retVal;
	uint32_t *pBuff;
	double dbSpeed;

	retVal = 0;
	if (vectCmdLine.size() < 7)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8300 BRAM_WR command\n");
		return 1;
	}

	startLine = 0;
	sscanf_s(vectCmdLine[3].c_str(), "%X", &addrRegVal);
	sscanf_s(vectCmdLine[4].c_str(), "%X", &cmdsRegVal);
	sscanf_s(vectCmdLine[5].c_str(), "%X", &bramDepth);
	if (vectCmdLine.size() > 7) {
		sscanf_s(vectCmdLine[7].c_str(), "%d", &startLine);
	}

	witdhAlign = (g_ramBitWidth + 31) / 32;
	pBuff = (uint32_t*)malloc(witdhAlign * bramDepth * 4);
	if (NULL == pBuff)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
		return 1;
	}
	ReadTextFile(vectCmdLine[6], pBuff, g_ramBitWidth, bramDepth, startLine);

	ToUpperCase(vectCmdLine[2]);
	map<string, DWORD>::iterator iter = g_map8300SlotAddress.find(vectCmdLine[2]);
	DWORD chAddr = 0xFFFFFFFF;
	if (iter != g_map8300SlotAddress.end())
	{
		CCmdCfgIni* pConfig = CCmdCfgIni::GetInstance("AT_BoardTest");

		TickStart();
		chAddr = iter->second;
		if (0xFFFFFFFF != pConfig->m_vectBramRegs[0])
		{
			write_dw(GetAddress(iter->first, chAddr, pConfig->m_vectBramRegs[0]), addrRegVal);
		}

		if (0xFFFFFFFF != pConfig->m_vectBramRegs[1])
		{
			write_dw(GetAddress(iter->first, chAddr, pConfig->m_vectBramRegs[1]), cmdsRegVal);
		}

		for (uint32_t depthIdx = 0; depthIdx < bramDepth; depthIdx++)
		{
			for (uint32_t widthIdx = 0; widthIdx < witdhAlign; widthIdx++)
			{
				write_dw(GetAddress(iter->first, chAddr, pConfig->m_vectBramRegs[2]), pBuff[depthIdx * witdhAlign + witdhAlign - widthIdx - 1]);
			}
		}
		TickEnd(timeVal);
		if (0 != timeVal) {
			dbSpeed = (bramDepth * witdhAlign * 4) / 1024.0  * 1000000.0 / timeVal;
			PrintLog(LOG_LEVEL_INFO, "Bram write speed %0.2fKB/s\n", dbSpeed);
		}
		else {
			PrintLog(LOG_LEVEL_INFO, "Bram write speed --.--KB/s\n", retVal);
		}
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown module type\n");
		retVal = 1;
	}

	return retVal;
}

uint32_t CCmdAcco8300Module::CmdBramRead(vector<string> vectCmdLine)
{
	uint32_t addrRegVal, cmdsRegVal, bramDepth, startLine;
	uint32_t witdhAlign, *pTxBuff, *pRxBuff;
	uint32_t timeVal, retVal;
	double dbSpeed;

	retVal = 0;
	if (vectCmdLine.size() < 7)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8300 BRAM_WR command\n");
		return 1;
	}

	startLine = 0;
	sscanf_s(vectCmdLine[3].c_str(), "%X", &addrRegVal);
	sscanf_s(vectCmdLine[4].c_str(), "%X", &cmdsRegVal);
	sscanf_s(vectCmdLine[5].c_str(), "%X", &bramDepth);

	witdhAlign = (g_ramBitWidth + 31) / 32;
	pTxBuff = (uint32_t*)malloc(witdhAlign * bramDepth * 4);
	if (NULL == pTxBuff)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
		return 1;
	}
	pRxBuff = (uint32_t*)malloc(witdhAlign * bramDepth * 4);
	if (NULL == pRxBuff)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
		free(pTxBuff);
		return 1;
	}

	ToUpperCase(vectCmdLine[2]);
	map<string, DWORD>::iterator iter = g_map8300SlotAddress.find(vectCmdLine[2]);
	DWORD chAddr = 0xFFFFFFFF;
	if (iter != g_map8300SlotAddress.end())
	{
		CCmdCfgIni* pConfig = CCmdCfgIni::GetInstance("AT_BoardTest");

		TickStart();
		chAddr = iter->second;
		if (0xFFFFFFFF != pConfig->m_vectBramRegs[3])
		{
			write_dw(GetAddress(iter->first, chAddr, pConfig->m_vectBramRegs[3]), addrRegVal);
		}

		if (0xFFFFFFFF != pConfig->m_vectBramRegs[4])
		{
			write_dw(GetAddress(iter->first, chAddr, pConfig->m_vectBramRegs[4]), cmdsRegVal);
		}

		for (uint32_t depthIdx = 0; depthIdx < bramDepth; depthIdx++)
		{
			for (uint32_t widthIdx = 0; widthIdx < witdhAlign; widthIdx++)
			{
				pTxBuff[depthIdx * witdhAlign + witdhAlign - widthIdx - 1] = read_dw(GetAddress(iter->first, chAddr, pConfig->m_vectBramRegs[5]));
			}
		}
		TickEnd(timeVal);
		if (0 != timeVal) {
			dbSpeed = (bramDepth * witdhAlign * 4) / 1024.0  * 1000000.0 / timeVal;
			PrintLog(LOG_LEVEL_INFO, "Bram read speed %0.2fKB/s\n", dbSpeed);
		}
		else {
			PrintLog(LOG_LEVEL_INFO, "Bram read speed --.--KB/s\n", retVal);
		}

		WriteTextFile(vectCmdLine[6], pTxBuff, g_ramBitWidth, bramDepth);
		if (vectCmdLine.size() > 7)
		{
			if (vectCmdLine.size() > 8) {
				sscanf_s(vectCmdLine[8].c_str(), "%d", &startLine);
			}
			ReadTextFile(vectCmdLine[7], pRxBuff, g_ramBitWidth, bramDepth, startLine);
			if (0 != memcmp(pTxBuff, pRxBuff, witdhAlign * bramDepth * 4))
			{
				PrintLog(LOG_LEVEL_ERROR, "Data compare fail\n");
				retVal = 1;
			}
		}
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown module type\n");
		retVal = 1;
	}

	free(pTxBuff);
	free(pRxBuff);

	return retVal;
}

uint32_t CCmdAcco8300Module::CmdDramWrite(vector<string> vectCmdLine)
{
	uint32_t addrRegVal, cmdsRegVal, statusRegVal, dramDepth, lastDepth;
	uint32_t widthAlign, startLine, regReadVal, timeVal, retVal;
	uint32_t *pBuff;
	double dbSpeed;

	retVal = 0;
	if (vectCmdLine.size() < 8)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8300 DRAM_WR command\n");
		return 1;
	}

	startLine = 0;
	sscanf_s(vectCmdLine[3].c_str(), "%X", &addrRegVal);
	sscanf_s(vectCmdLine[4].c_str(), "%X", &cmdsRegVal);
	sscanf_s(vectCmdLine[5].c_str(), "%X", &statusRegVal);
	sscanf_s(vectCmdLine[6].c_str(), "%X", &dramDepth);
	if (vectCmdLine.size() > 8) {
		sscanf_s(vectCmdLine[8].c_str(), "%d", &startLine);
	}

	if (0 != (dramDepth % g_ramSinDepth))
	{
		PrintLog(LOG_LEVEL_ERROR, "DRAM total size must be single size align\n");
		return 1;
	}

	widthAlign = (g_ramBitWidth + 31) / 32;
	pBuff = (uint32_t*)malloc(widthAlign * dramDepth * 4);
	if (NULL == pBuff)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
		return 1;
	}
	ReadTextFile(vectCmdLine[7], pBuff, g_ramBitWidth, dramDepth, startLine);

	ToUpperCase(vectCmdLine[2]);
	map<string, DWORD>::iterator iter = g_map8300SlotAddress.find(vectCmdLine[2]);
	DWORD chAddr = 0xFFFFFFFF;
	if (iter != g_map8300SlotAddress.end())
	{
		CCmdCfgIni* pConfig = CCmdCfgIni::GetInstance("AT_BoardTest");
		uint32_t *pBuffTemp;

		TickStart();
		chAddr = iter->second;
		lastDepth = dramDepth;
		pBuffTemp = pBuff;
		while (lastDepth > 0)
		{
			if (0xFFFFFFFF != pConfig->m_vectDramRegs[0])
			{
				write_dw(GetAddress(iter->first, chAddr, pConfig->m_vectDramRegs[0]), addrRegVal);
			}

			for (uint32_t depthIdx = 0; depthIdx < g_ramSinDepth; depthIdx++)
			{
				for (uint32_t widthIdx = 0; widthIdx < widthAlign; widthIdx++)
				{
					write_dw(GetAddress(iter->first, chAddr, pConfig->m_vectDramRegs[2]), pBuffTemp[widthAlign - widthIdx - 1]);
				}
				pBuffTemp += widthAlign;
			}

			if (0xFFFFFFFF != pConfig->m_vectDramRegs[1])
			{
				write_dw(GetAddress(iter->first, chAddr, pConfig->m_vectDramRegs[1]), cmdsRegVal);
			}

			if (0xFFFFFFFF != pConfig->m_vectDramRegs[3])
			{
				for (int tryCnt = 0; tryCnt < 500 * 1000; tryCnt++)
				{
					regReadVal = read_dw(GetAddress(iter->first, chAddr, pConfig->m_vectDramRegs[3]));
					if (statusRegVal == regReadVal)
					{
						break;
					}
				}
				if (statusRegVal != regReadVal)
				{
					break;
				}
			}
			addrRegVal += g_ramSinDepth * pConfig->m_vectDramRegs[8];
			lastDepth -= g_ramSinDepth;
		}
		if (lastDepth > 0)
		{
			PrintLog(LOG_LEVEL_ERROR, "DRAM write fail\n");
			retVal = 1;
		}
		else
		{
			TickEnd(timeVal);
			if (0 != timeVal) {
				dbSpeed = (dramDepth * widthAlign * 4) / 1024.0  * 1000000.0 / timeVal;
				PrintLog(LOG_LEVEL_INFO, "Dram write speed %0.2fKB/s\n", dbSpeed);
			}
			else {
				PrintLog(LOG_LEVEL_INFO, "Dram write speed --.--KB/s\n", retVal);
			}
		}
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown module type\n");
		retVal = 1;
	}

	return retVal;
}

uint32_t CCmdAcco8300Module::CmdDramRead(vector<string> vectCmdLine)
{
	uint32_t addrRegVal, cmdsRegVal, statusRegVal, dramDepth, lastDepth;
	uint32_t widthAlign, startLine, regReadVal, timeVal, retVal;
	uint32_t *pTxBuff, *pRxBuff;
	double dbSpeed;

	retVal = 0;
	if (vectCmdLine.size() < 8)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8300 BRAM_WR command\n");
		return 1;
	}

	startLine = 0;
	sscanf_s(vectCmdLine[3].c_str(), "%X", &addrRegVal);
	sscanf_s(vectCmdLine[4].c_str(), "%X", &cmdsRegVal);
	sscanf_s(vectCmdLine[5].c_str(), "%X", &statusRegVal);
	sscanf_s(vectCmdLine[6].c_str(), "%X", &dramDepth);
	if (0 != (dramDepth % g_ramSinDepth))
	{
		PrintLog(LOG_LEVEL_ERROR, "DRAM total size must be single size align\n");
		return 1;
	}

	widthAlign = (g_ramBitWidth + 31) / 32;
	pTxBuff = (uint32_t*)malloc(widthAlign * dramDepth * 4);
	if (NULL == pTxBuff)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
		return 1;
	}
	pRxBuff = (uint32_t*)malloc(widthAlign * dramDepth * 4);
	if (NULL == pRxBuff)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
		free(pTxBuff);
		return 1;
	}

	ToUpperCase(vectCmdLine[2]);
	map<string, DWORD>::iterator iter = g_map8300SlotAddress.find(vectCmdLine[2]);
	DWORD chAddr = 0xFFFFFFFF;
	if (iter != g_map8300SlotAddress.end())
	{
		CCmdCfgIni* pConfig = CCmdCfgIni::GetInstance("AT_BoardTest");
		uint32_t *pBuffTemp;

		TickStart();
		chAddr = iter->second;
		lastDepth = dramDepth;
		pBuffTemp = pTxBuff;
		while (lastDepth > 0)
		{
			if (0xFFFFFFFF != pConfig->m_vectDramRegs[4])
			{
				write_dw(GetAddress(iter->first, chAddr, pConfig->m_vectDramRegs[4]), addrRegVal);
			}

			if (0xFFFFFFFF != pConfig->m_vectDramRegs[5])
			{
				write_dw(GetAddress(iter->first, chAddr, pConfig->m_vectDramRegs[5]), cmdsRegVal);
			}

			if (0xFFFFFFFF != pConfig->m_vectDramRegs[7])
			{
				for (int tryCnt = 0; tryCnt < 500 * 1000; tryCnt++)
				{
					regReadVal = read_dw(GetAddress(iter->first, chAddr, pConfig->m_vectDramRegs[7]));
					if (statusRegVal == regReadVal)
					{
						break;
					}
				}
				if (statusRegVal != regReadVal)
				{
					break;
				}
			}

			for (uint32_t depthIdx = 0; depthIdx < g_ramSinDepth; depthIdx++)
			{
				for (uint32_t widthIdx = 0; widthIdx < widthAlign; widthIdx++)
				{
					pBuffTemp[widthAlign - widthIdx - 1] = read_dw(GetAddress(iter->first, chAddr, pConfig->m_vectDramRegs[6]));
				}
				pBuffTemp += widthAlign;
			}

			addrRegVal += g_ramSinDepth * pConfig->m_vectDramRegs[8];
			lastDepth -= g_ramSinDepth;
		}
		if (lastDepth > 0)
		{
			PrintLog(LOG_LEVEL_ERROR, "DRAM read fail\n");
			retVal = 1;
		}
		else
		{
			TickEnd(timeVal);
			if (0 != timeVal) {
				dbSpeed = (dramDepth * widthAlign * 4) / 1024.0  * 1000000.0 / timeVal;
				PrintLog(LOG_LEVEL_INFO, "Dram read speed %0.2fKB/s\n", dbSpeed);
			}
			else {
				PrintLog(LOG_LEVEL_INFO, "Dram read speed --.--KB/s\n", retVal);
			}
			WriteTextFile(vectCmdLine[7], pTxBuff, g_ramBitWidth, dramDepth);
		}

		if (vectCmdLine.size() > 8)
		{
			if (vectCmdLine.size() > 9) {
				sscanf_s(vectCmdLine[9].c_str(), "%d", &startLine);
			}
			ReadTextFile(vectCmdLine[8], pRxBuff, g_ramBitWidth, dramDepth, startLine);
			if (0 != memcmp(pTxBuff, pRxBuff, widthAlign * dramDepth * 4))
			{
				PrintLog(LOG_LEVEL_ERROR, "Data compare fail\n");
				retVal = 1;
			}
		}
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown module type\n");
		retVal = 1;
	}

	free(pTxBuff);
	free(pRxBuff);

	return retVal;
}

uint32_t CCmdAcco8300Module::CmdDcmTemp(vector<string> vectCmdLine)
{
	double dbVolt0, dbVolt1;

	dbVolt0 = CalcAd7606Voltage((g_regRecord >> 0) & 0xFFFF);
	dbVolt1 = CalcAd7606Voltage((g_regRecord >> 16) & 0xFFFF);

	PrintLog(LOG_LEVEL_INFO, "%.3f\n", (dbVolt0 - dbVolt1) * 100 - 273.15);

	return 0;
}

uint32_t CCmdAcco8300Module::CmdFlashConfig(vector<string> vectCmdLine)
{
	int retVal = 0;
	ToUpperCase(vectCmdLine[0]);
	ToUpperCase(vectCmdLine[2]);
	CCmdCfgIni* pConfig = CCmdCfgIni::GetInstance("AT_BoardTest");
	map<string, int>::iterator iter = pConfig->m_strValueType.find(vectCmdLine[2]);
	if (iter != pConfig->m_strValueType.end())
	{
		if (("ACCO8300" == vectCmdLine[0] && CmdExceptionProcess(vectCmdLine, 5, 0X00000101) == 0) || ("ACCO8200" == vectCmdLine[0] && CmdExceptionProcess(vectCmdLine, 5, 0X00000201) == 0))
		{
			//保存flash属性信息到全局结构体变量
			sscanf_s(vectCmdLine[3].c_str(), "%d", &g_flashProperty.spiMode);
			sscanf_s(vectCmdLine[4].c_str(), "%X", &g_flashProperty.flashFuncAddr);
			g_flashProperty.flashType = iter->second;
			g_flashProperty.flashTypeString = iter->first;
		}
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown flash type\n");
		retVal = -2;
	}
	return retVal;
}

uint32_t CCmdAcco8300Module::CmdFlashGetID(vector<string> vectCmdLine)
{
	int retVal = 0;
	AT_SYSTEM_TYPE systemType;
	DWORD flashID = 0x00;
	map<string, DWORD>::iterator iter;
	ToUpperCase(vectCmdLine[0]);
	if (("ACCO8300" == vectCmdLine[0] && CmdExceptionProcess(vectCmdLine, 3, 0X00001111) == 0) || ("ACCO8200" == vectCmdLine[0] && CmdExceptionProcess(vectCmdLine, 3, 0X00001241) == 0))
	{
		ToUpperCase(vectCmdLine[2]);
		if ("ACCO8300" == vectCmdLine[0])
		{
			iter = g_map8300SlotAddress.find(vectCmdLine[2]);
			systemType = AT_STS8300;
		} 
		else
		{
			iter = g_mapModuleAddress.find(vectCmdLine[2]);
			systemType = AT_STS8200;
		}
		
		CCmdCfgIni* pConfig = CCmdCfgIni::GetInstance("AT_BoardTest");
		map<string, DWORD>::iterator iniIter = pConfig->m_strValueID.find(g_flashProperty.flashTypeString);
		AT_FLASH flash(systemType, iter->second);
		flash.SetFlashProperty(AT_FLASH_TYPE(g_flashProperty.flashType), AT_SPI_MODE(g_flashProperty.spiMode), g_flashProperty.flashFuncAddr);
		retVal = flash.GetFlashID(flashID);
		if (retVal == 0)
		{
			if (iniIter->second == flashID)//判断该flashID是否正确
			{
				PrintLog(LOG_LEVEL_INFO, "%s %08X\n", g_flashProperty.flashTypeString.c_str(), flashID);
			}
			else
			{
				PrintLog(LOG_LEVEL_INFO, "Actual ID:%08X Expect ID:%08X\n", flashID, iniIter->second);
				retVal = -7;
			}
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Read ID failed\n");
			retVal = -6;
		}
	}
	return retVal;
}

uint32_t CCmdAcco8300Module::CmdFlashErase(vector<string> vectCmdLine)
{
	int retVal = 0;
	uint32_t eraseMode = 0;
	uint32_t Length = 0;
	DWORD eraseAddr = 0x00;
	AT_SYSTEM_TYPE systemType;
	char eraseData = 0XFF;
	map<string, DWORD>::iterator iter;
	ToUpperCase(vectCmdLine[0]);
	if (("ACCO8300" == vectCmdLine[0] && CmdExceptionProcess(vectCmdLine, 5, 0X00001111) == 0) || ("ACCO8200" == vectCmdLine[0] && CmdExceptionProcess(vectCmdLine, 5, 0X00001241) == 0))
	{
		ToUpperCase(vectCmdLine[2]);
		if ("ACCO8300" == vectCmdLine[0])
		{
			iter = g_map8300SlotAddress.find(vectCmdLine[2]);
			systemType = AT_STS8300;
		}
		else
		{
			iter = g_mapModuleAddress.find(vectCmdLine[2]);
			systemType = AT_STS8200;
		}
		sscanf_s(vectCmdLine[3].c_str(), "%d", &eraseMode);
		sscanf_s(vectCmdLine[4].c_str(), "%X", &eraseAddr);
		AT_FLASH flash(systemType, iter->second);
		flash.SetFlashProperty(AT_FLASH_TYPE(g_flashProperty.flashType), AT_SPI_MODE(g_flashProperty.spiMode), g_flashProperty.flashFuncAddr);
		switch (eraseMode)//根据擦除模式判断回读的数据长度
		{
		case AT_BULK_ERASE:
			break;
		case AT_64K_BLOCK_ERASE:
			if ((eraseAddr & 0X0000FFFF) != 0)//判断擦除起始地址是否符合要求，不符合则报错并处理
			{
				eraseAddr &= 0XFFFF0000;
				retVal = -6;
			}
			Length = 0x00010000;
			break;
		case AT_32K_BLOCK_ERASE:
			if ((eraseAddr & 0X00007FFF) != 0)
			{
				eraseAddr &= 0XFFFF8000;
				retVal = -6;
			}
			Length = 0x00008000;
			break;
		case AT_4K_SECTOR_ERASE:
			if ((eraseAddr & 0X00000FFF) != 0)
			{
				eraseAddr &= 0XFFFFF000;
				retVal = -6;
			}
			Length = 0x00001000;
			break;
		default:
			eraseMode = AT_BULK_ERASE;
			break;
		}
		if (retVal == -6)
		{
			PrintLog(LOG_LEVEL_ERROR, "Improper erase address\n");
		}
		else
		{
			flash.SetFlashStartAddr(eraseAddr);
			retVal = flash.EraseData(AT_ERASE_MODE(eraseMode));
			if (retVal == 0 && eraseMode != 0)
			{
				char* pGetBuff = (char*)malloc(Length);
				if (NULL == pGetBuff)
				{
					PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
					retVal = -10;
				}
				else
				{
					retVal = flash.GetData(pGetBuff, Length);
					if (retVal == 0)
					{
						for (int i = 0; i < Length; i++)//判断回读的数据是否均为0XFF
						{
							if (pGetBuff[i] != eraseData)
							{
								PrintLog(LOG_LEVEL_ERROR, "Erase failed\n");
								retVal = -9;
								break;
							}
						}
					}
					else
					{
						PrintLog(LOG_LEVEL_ERROR, "Read failed\n");
						retVal = -8;
					}
				}
				free(pGetBuff);
			}
			else
			{
				if (eraseMode != 0)
				{
					PrintLog(LOG_LEVEL_ERROR, "Erase failed(Overtime)\n");
					retVal = -7;
				}
			}
		}
	}
	return retVal;
}

uint32_t CCmdAcco8300Module::CmdFlashWrite(vector<string> vectCmdLine)
{
	DWORD startAddr = 0x00;
	vector<string> pairValue;
	FILE *fpFile;
	map<string, DWORD>::iterator iter;
	AT_SYSTEM_TYPE systemType;
	int retVal = 0;
	int Length = 0;
	//_ASSERTE(_CrtCheckMemory());
	ToUpperCase(vectCmdLine[0]);
	if (("ACCO8300" == vectCmdLine[0] && CmdExceptionProcess(vectCmdLine, 6, 0X00001111) == 0) || ("ACCO8200" == vectCmdLine[0] && CmdExceptionProcess(vectCmdLine, 6, 0X00001241) == 0))
	{
		ToUpperCase(vectCmdLine[2]);
		ToUpperCase(vectCmdLine[5]);
		if ("ACCO8300" == vectCmdLine[0])
		{
			iter = g_map8300SlotAddress.find(vectCmdLine[2]);
			systemType = AT_STS8300;
		}
		else
		{
			iter = g_mapModuleAddress.find(vectCmdLine[2]);
			systemType = AT_STS8200;
		}
		map<string, int>::iterator dataTypeIter = m_dataType.find(vectCmdLine[5]);
		sscanf_s(vectCmdLine[3].c_str(), "%X", &startAddr);
		sscanf_s(vectCmdLine[4].c_str(), "%d", &Length);
		if (dataTypeIter == m_dataType.end() && Length == -1)
		{
			SplitString(pairValue, vectCmdLine[5].c_str(), ".");
			if (pairValue.size() == 2 && pairValue[1] == "BIN")//如果是bin文件
			{
				fpFile = fopen(vectCmdLine[5].c_str(), "rb");
			}
			else
			{
				fpFile = fopen(vectCmdLine[5].c_str(), "r");
			}
			if (NULL == fpFile)
			{
				PrintLog(LOG_LEVEL_ERROR, "Unable to open the data file\n");
				retVal = -6;
			}
			else
			{
				Length = filelength(fileno(fpFile));
				fclose(fpFile);
			}
		} 
		if ((startAddr & 0X000000FF) != 0)//判断起始地址低8位是否为0
		{
			PrintLog(LOG_LEVEL_ERROR, "Improper start address\n");
			return -7;
		}
		char* pGetBuff = (char*)malloc(Length);
		if (NULL == pGetBuff)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
			return -8;
		}
		AT_FLASH flash(systemType, iter->second);
		flash.SetFlashProperty(AT_FLASH_TYPE(g_flashProperty.flashType), AT_SPI_MODE(g_flashProperty.spiMode), g_flashProperty.flashFuncAddr);
		flash.SetFlashStartAddr(startAddr);
		if (dataTypeIter == m_dataType.end())//如果是文件
		{
			SplitString(pairValue, vectCmdLine[5].c_str(), ".");
			if (pairValue.size() == 2 && pairValue[1]=="BIN")//如果是bin文件
			{
				fpFile = fopen(vectCmdLine[5].c_str(), "rb");
				if (NULL == fpFile)
				{
					PrintLog(LOG_LEVEL_ERROR, "Unable to open the data file\n");
					retVal = -6;
				}
				else
				{
					fread(pGetBuff, 1, Length, fpFile);
					fclose(fpFile);
				}
			}
			else
			{
				fpFile = fopen(vectCmdLine[5].c_str(), "r");
				if (NULL == fpFile) {
					PrintLog(LOG_LEVEL_ERROR, "Unable to create a data file\n");
					retVal = -6;
				}
				else
				{
					for (int i = 0; i < Length; i++)
					{
						fscanf(fpFile, "%X", pGetBuff + i);
					}
					fclose(fpFile);
				}
			}
		}
		else//如果是指定数据类型
		{
			FillDataPattern((uint8_t *)pGetBuff, Length, DATA_PATTERN_SETS(dataTypeIter->second), 1);
		}
		retVal = flash.SetData(pGetBuff, Length);
		if (retVal != 0)
		{
			PrintLog(LOG_LEVEL_ERROR, "Write failed\n");
			retVal = -9;
		}
		free(pGetBuff);
	}
	return retVal;
}

uint32_t CCmdAcco8300Module::CmdFlashRead(vector<string> vectCmdLine)
{
	uint32_t Length = 0;
	DWORD startAddr = 0x00;
	SYSTEMTIME sysTime;
	string fname = "./RunData/";
	FILE *fpFile;
	uint8_t* pBuff;
	map<string, DWORD>::iterator iter;
	AT_SYSTEM_TYPE systemType;
	int retVal = 0;
	char folderName[] = "RunData";
	char fnameTemp[1024];

	ToUpperCase(vectCmdLine[0]);
	if (("ACCO8300" == vectCmdLine[0] && CmdExceptionProcess(vectCmdLine, 5, 0X00001111) == 0) || ("ACCO8200" == vectCmdLine[0] && CmdExceptionProcess(vectCmdLine, 5, 0X00001241) == 0))
	{
		ToUpperCase(vectCmdLine[2]);
		if ("ACCO8300" == vectCmdLine[0])
		{
			iter = g_map8300SlotAddress.find(vectCmdLine[2]);
			systemType = AT_STS8300;
		}
		else
		{
			iter = g_mapModuleAddress.find(vectCmdLine[2]);
			systemType = AT_STS8200;
		}
		sscanf_s(vectCmdLine[3].c_str(), "%X", &startAddr);
		sscanf_s(vectCmdLine[4].c_str(), "%d", &Length);
		//起始地址低8位是否为0
		if ((startAddr & 0X000000FF) !=0)
		{
			PrintLog(LOG_LEVEL_ERROR, "Improper start address\n");
			return -6;
		}
		char* pGetBuff = (char*)malloc(Length);
		if (NULL == pGetBuff)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
			//free(pBuff);
			return -7;
		}
	
		AT_FLASH flash(systemType, iter->second);
		flash.SetFlashProperty(AT_FLASH_TYPE(g_flashProperty.flashType), AT_SPI_MODE(g_flashProperty.spiMode), g_flashProperty.flashFuncAddr);
		flash.SetFlashStartAddr(startAddr);
		retVal = flash.GetData(pGetBuff, Length);
		if (retVal == 0)
		{
			// 文件夹不存在则创建文件夹
			if (_access(folderName, 0) == -1)
			{
				_mkdir(folderName);
			}
			if (vectCmdLine.size() == 6)//若输入文件名
			{
				ToUpperCase(vectCmdLine[5]);
				fname += vectCmdLine[5];
			}
			else//生成默认文件名
			{
				GetLocalTime(&sysTime);
				sprintf_s(fnameTemp, STS_ARRAY_SIZE(fnameTemp), "8300FLASH_READ_%04d%02d%02d_%02d%02d%02d.txt", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
				fname += fnameTemp;
			}
			fpFile = fopen(fname.c_str(), "w");
			if (NULL == fpFile) {
				PrintLog(LOG_LEVEL_ERROR, "Unable to create a data file\n");
				retVal = -8;
			}
			else//写入表头信息
			{
				fprintf_s(fpFile, "----------------------------DataInfo--------------------------\n");
				fprintf_s(fpFile, "->Module:%s\n", vectCmdLine[2].c_str());
				fprintf_s(fpFile, "->StartAddr:0X%08X\n", startAddr);
				fprintf_s(fpFile, "->Length:%d\n", Length);
				fprintf_s(fpFile, "-----------------------\n");
				fclose(fpFile);
			}
			fpFile = fopen(fname.c_str(), "a");
			if (NULL == fpFile) {
				PrintLog(LOG_LEVEL_ERROR, "Unable to create a data file\n");
				retVal = -8;
			}
			else
			{
				pBuff = (uint8_t*)pGetBuff;
				for (int i = 0; i < Length; i++)
				{
					fprintf(fpFile, "0X%02X\n", *(pBuff + i));
				}
				fclose(fpFile);
			}
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Read failed\n");
			retVal = -9;
		}
		free(pGetBuff);
	}
	return retVal;
}

uint32_t CCmdAcco8300Module::CmdHelpInf(vector<string> vectCmdLine)
{
    PrintLog(LOG_LEVEL_INFO, "#########################################################################################\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 OFS_ST <MODULE(S)> <Ofst1(D)> <Ofst2(D)>                                     #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 REG_WR <MODULE(S)> <RegAddr(X)> <Value(X)>                                   #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 REG_RD <MODULE(S)> <RegAddr(X)> [ExpValue(X)]                                #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 G_WR <RegAddr(X)> <Value(X)>                                                 #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 G_RD <RegAddr(X)> [ExpValue(X)]                                              #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 RAM_ST <MODULE(S)> <bitWidth(D)> <singleDepth(D)>                            #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 BRAM_WR <MODULE(S)> <CmdVal1(X)> <CmdVal2(X)> <Depth(X)> <File(S)> [Line(D)] #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 BRAM_RD <MODULE(S)> <CmdVal1(X)> <CmdVal2(X)> <Depth(X)> <File(S)>           #\n");
	PrintLog(LOG_LEVEL_INFO, "                   [ExpFile(S)] [Line(D)]                                               #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 DRAM_WR <MODULE(S)> <AddrVal(X)> <CmdVal(X)> <StsVal(X)> <Depth(X)> <File(S)>#\n");
	PrintLog(LOG_LEVEL_INFO, "                   [Line(D)]                                                            #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 DRAM_RD <MODULE(S)> <AddrVal(X)> <CmdVal(X)> <StsVal(X)> <Depth(X)> <File(S)>#\n");
	PrintLog(LOG_LEVEL_INFO, "                   [ExpFile(S)] [Line(D)]                                               #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 DCMTEMP                                                                      #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 FLASH_CFG <FlashType(S)> <SPIMode(D)> <FuncAddr(X)>                          #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 FLASH_GETID <MODULE(S)>                                                      #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 FLASH_ERASE <MODULE(S)> <EraseMode(D)> <StartAddr(X)>                        #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 FLASH_WRITE <MODULE(S)> <StartAddr(X)> <Length(D)> <DataType(S)>             #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 FLASH_ READ <MODULE(S)> <StartAddr(X)> <Length(D)> [File(S)]                 #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 HELP                                                                         #\n");
    PrintLog(LOG_LEVEL_INFO, "#########################################################################################\n");

    return 0;
}

uint32_t CmdExceptionProcess(vector<string> vectCmdLine, int CommandSize, DWORD Flag)
{
	if (vectCmdLine.size() < CommandSize&&Flag & 0X00000001)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown command\n");
		return -1;
	}

	if (Flag & 0X00000010)
	{
		ToUpperCase(vectCmdLine[2]);
		map<string, DWORD>::iterator iter = g_map8300SlotAddress.find(vectCmdLine[2]);
		if (iter == g_map8300SlotAddress.end())
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown module type\n");
			return -2;
		}
	} 
	else if (Flag & 0X00000020)
	{
		ToUpperCase(vectCmdLine[4]);
		map<string, DWORD>::iterator iter = g_map8300SlotAddress.find(vectCmdLine[4]);
		if (iter == g_map8300SlotAddress.end())
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown module type\n");
			return -2;
		}
	}
	else if (Flag & 0X00000040)
	{
		ToUpperCase(vectCmdLine[2]);
		map<string, DWORD>::iterator iter = g_mapModuleAddress.find(vectCmdLine[2]);
		if (iter == g_mapModuleAddress.end())
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown module type\n");
			return -2;
		}
	}
	else if (Flag & 0X00000080)
	{
		ToUpperCase(vectCmdLine[4]);
		map<string, DWORD>::iterator iter = g_mapModuleAddress.find(vectCmdLine[4]);
		if (iter == g_mapModuleAddress.end())
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown module type\n");
			return -2;
		}
	}
	
	if (g_systemType != 8300 && Flag & 0X00000100)
	{
		if (g_systemType == 8200)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown command(ACCO8200 module)\n");
			return -3;
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown command(Unknown module)\n");
			return -4;
		}
	}
	else if (g_systemType != 8200 && Flag & 0X00000200)
	{
		if (g_systemType == 8300)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown command(ACCO8300 module)\n");
			return -3;
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown command(Unknown module)\n");
			return -4;
		}
	}
	
	if ((g_flashProperty.flashType == -1 || g_flashProperty.spiMode == -1 || g_flashProperty.flashFuncAddr == 0XFF) && Flag & 0X00001000)//判断是否设置flash属性
	{
		PrintLog(LOG_LEVEL_ERROR, "No flash property information\n");
		return -5;
	}
	return 0;
}

DWORD CCmdAcco8300Module::GetAddress(string mdtype, DWORD chAddr, DWORD funcAddr)
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
		else if (type == 2)
		{
			string iniName = "AT_BoardTest";
			CCmdCfgIni* pConfig = CCmdCfgIni::GetInstance(iniName);

			address = (chAddr << 16) + funcAddr;
			if (pConfig->m_vectRegBitOfst.size() > 0)
			{
				address += g_regOfstVal1 << pConfig->m_vectRegBitOfst[0];
			}
			if (pConfig->m_vectRegBitOfst.size() > 1)
			{
				address += g_regOfstVal2 << pConfig->m_vectRegBitOfst[1];
			}
			address <<= 2;
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

double CCmdAcco8300Module::CalcAd7606Voltage(uint16_t adcValue)
{
	double fLsb, dbResult;

	fLsb = 10.0 / 32768;
	if (adcValue >= 0x8000) {
		dbResult = (adcValue - 0x8000) * fLsb - 10.0 + fLsb / 2;
	}
	else {
		dbResult = adcValue * fLsb - fLsb / 2;
	}

	return dbResult;
}