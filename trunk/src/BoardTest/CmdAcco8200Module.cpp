#include "stdafx.h"
#include "CmdAcco8200Module.h"
#include "UtilLibrary.h"
#include <time.h>
#include <map>
#include "Sts8100.h"

#pragma warning(disable : 4996)

using namespace std;

extern map<string, DWORD> g_mapModuleAddress;
extern int g_systemType;

CCmdAcco8200Module::CCmdAcco8200Module()
{
}

CCmdAcco8200Module::~CCmdAcco8200Module()
{
}

uint32_t CCmdAcco8200Module::JudgeCmd(vector<string> vectCmdLine)
{
    uint32_t retVal = 0;

    do {
        if (vectCmdLine.size() < 2) {
            PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8200 command\n");
            retVal = 1;
            break;
        }

        ToUpperCase(vectCmdLine[1]);
        if ("HELP" == vectCmdLine[1])
		{
            retVal = CmdHelpInf(vectCmdLine);
        }
        else if ("REG_WR" == vectCmdLine[1])
		{
            retVal = CmdRegWrite(vectCmdLine);
        }
        else if ("REG_RD" == vectCmdLine[1])
		{
            retVal = CmdRegRead(vectCmdLine);
        }
		else if ("REG_WR(W)" == vectCmdLine[1])
		{
			retVal = Cmd8200RegWriteW(vectCmdLine);
		}
		else if ("REG_RD(W)" == vectCmdLine[1])
		{
			retVal = Cmd8200RegReadW(vectCmdLine);
		}
		else if ("G_WR" == vectCmdLine[1])
		{
			retVal = CmdGlobalRegWrite(vectCmdLine);
		}
		else if ("G_RD" == vectCmdLine[1])
		{
			retVal = CmdGlobalRegRead(vectCmdLine);
		}
		else if ("G_WR(W)" == vectCmdLine[1])
		{
			retVal = Cmd8200GlobalRegWriteW(vectCmdLine);
		}
		else if ("G_RD(W)" == vectCmdLine[1])
		{
			retVal = Cmd8200GlobalRegReadW(vectCmdLine);
		}
		else
		{
            PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8200 command\n");
            retVal = 1;
        }
    } while (0);

    return retVal;
}

uint32_t CCmdAcco8200Module::CmdRegWrite(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

    if (vectCmdLine.size() < 5)
	{
        PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8200 REG_WR command\n");
        return 1;
    }

	ToUpperCase(vectCmdLine[2]);
	map<string, DWORD>::iterator iter = g_mapModuleAddress.find(vectCmdLine[2]);
	if (iter != g_mapModuleAddress.end())
	{	
		if (g_systemType == 8200)
		{
			DWORD chAddr = iter->second;
			DWORD offset = 0x00;
			DWORD setData = 0x00;

			sscanf_s(vectCmdLine[3].c_str(), "%X", &offset);
			sscanf_s(vectCmdLine[4].c_str(), "%X", &setData);

			_outp(chAddr + offset, setData);
		}
		else if (g_systemType == 8300)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown REG_WR command(ACCO8300 module)\n");
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

uint32_t CCmdAcco8200Module::CmdRegRead(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

    if (vectCmdLine.size() < 4)
	{
        PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8200 REG_RD command\n");
        return 1;
    }

	ToUpperCase(vectCmdLine[2]);
	map<string, DWORD>::iterator iter = g_mapModuleAddress.find(vectCmdLine[2]);
	if (iter != g_mapModuleAddress.end())
	{
		if (g_systemType == 8200)
		{
			DWORD chAddr = iter->second;
			DWORD offset = 0x00;
			DWORD expData = 0x00;

			sscanf_s(vectCmdLine[3].c_str(), "%X", &offset);
			BYTE getData = _inp(chAddr + offset);

			if (vectCmdLine.size() > 4)
			{
				sscanf_s(vectCmdLine[4].c_str(), "%X", &expData);

				if (getData != expData)
				{
					PrintLog(LOG_LEVEL_ERROR, "Read Register Module[%s] Offset[0x%02X] Value[0x%02X] Exp[0x%02X]\n", vectCmdLine[2].c_str(), offset, getData, expData);
					return 1;
				}
			}
			else
			{
				PrintLog(LOG_LEVEL_INFO, "Read Register Module[%s] Offset[0x%02X] Value[0x%02X]\n", vectCmdLine[2].c_str(), offset, getData);
			}
		}
		else if (g_systemType == 8300)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown REG_RD command(ACCO8300 module)\n");
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


uint32_t CCmdAcco8200Module::Cmd8200RegWriteW(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

	if (vectCmdLine.size() < 5)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8200 REG_WR(w) command\n");
		return 1;
	}

	ToUpperCase(vectCmdLine[2]);
	map<string, DWORD>::iterator iter = g_mapModuleAddress.find(vectCmdLine[2]);
	if (iter != g_mapModuleAddress.end())
	{
		if (g_systemType == 8200)
		{
			DWORD chAddr = iter->second;
			DWORD offset = 0x00;
			DWORD setData = 0x00;

			sscanf_s(vectCmdLine[3].c_str(), "%X", &offset);
			sscanf_s(vectCmdLine[4].c_str(), "%X", &setData);

			_outpw(chAddr + offset, setData);
		}
		else if (g_systemType == 8300)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown REG_WR(w) command(ACCO8300 module)\n");
			return 1;
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown REG_WR(w) command(Unknown module)\n");
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

uint32_t CCmdAcco8200Module::Cmd8200RegReadW(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

	if (vectCmdLine.size() < 4)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8200 REG_RD(w) command\n");
		return 1;
	}

	ToUpperCase(vectCmdLine[2]);
	map<string, DWORD>::iterator iter = g_mapModuleAddress.find(vectCmdLine[2]);
	if (iter != g_mapModuleAddress.end())
	{
		if (g_systemType == 8200)
		{
			DWORD chAddr = iter->second;
			DWORD offset = 0x00;
			DWORD expData = 0x00;

			sscanf_s(vectCmdLine[3].c_str(), "%X", &offset);
			WORD getData = _inpw(chAddr + offset);

			if (vectCmdLine.size() > 4)
			{
				sscanf_s(vectCmdLine[4].c_str(), "%X", &expData);

				if (getData != expData)
				{
					PrintLog(LOG_LEVEL_ERROR, "Read Register Module[%s] Offset[0x%02X] Value[0x%04X] Exp[0x%04X]\n", vectCmdLine[2].c_str(), offset, getData, expData);
					return 1;
				}
			}
			else
			{
				PrintLog(LOG_LEVEL_INFO, "Read Register Module[%s] Offset[0x%02X] Value[0x%04X]\n", vectCmdLine[2].c_str(), offset, getData);
			}
		}
		else if (g_systemType == 8300)
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown REG_RD(w) command(ACCO8300 module)\n");
			return 1;
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Unknown REG_RD(w) command(Unknown module)\n");
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

uint32_t CCmdAcco8200Module::CmdGlobalRegWrite(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

	if (vectCmdLine.size() < 4)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8200 G_WR command\n");
		return 1;
	}

	if (g_systemType == 8200)
	{
		DWORD regAddr = 0x00;
		DWORD setData = 0x00;

		sscanf_s(vectCmdLine[2].c_str(), "%X", &regAddr);
		sscanf_s(vectCmdLine[3].c_str(), "%X", &setData);

		if (regAddr >= 0x300)
		{
			_outp(regAddr, setData);
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Global Register address(0x%04X) must be greater than 0x02FF!\n", regAddr);
			return 1;
		}
	}
	else if (g_systemType == 8300)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown G_WR command(ACCO8300 module)\n");
		return 1;
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown G_WR command(Unknown module)\n");
		return 1;
	}
	return retVal;
}

uint32_t CCmdAcco8200Module::CmdGlobalRegRead(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

	if (vectCmdLine.size() < 3)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8200 G_RD command\n");
		return 1;
	}

	if (g_systemType == 8200)
	{
		DWORD regAddr = 0x00;
		DWORD expData = 0x00;

		sscanf_s(vectCmdLine[2].c_str(), "%X", &regAddr);
		if (regAddr >= 0x300)
		{
			BYTE getData = _inp(regAddr);

			if (vectCmdLine.size() > 3)
			{
				sscanf_s(vectCmdLine[3].c_str(), "%X", &expData);

				if (getData != expData)
				{
					PrintLog(LOG_LEVEL_ERROR, "Read Global Register Address[0x%04X] Value[0x%02X] Exp[0x%02X]\n", regAddr, getData, expData);
					return 1;
				}
			}
			else
			{
				PrintLog(LOG_LEVEL_INFO, "Read Global Register Address[0x%04X] Value[0x%02X]\n", regAddr, getData);
			}
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Global Register address(0x%04X) must be greater than 0x02FF!\n", regAddr);
			return 1;
		}
	}
	else if (g_systemType == 8300)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown G_RD command(ACCO8300 module)\n");
		return 1;
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown G_RD command(Unknown module)\n");
		return 1;
	}
	return retVal;
}


uint32_t CCmdAcco8200Module::Cmd8200GlobalRegWriteW(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

	if (vectCmdLine.size() < 4)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8200 G_WR(w) command\n");
		return 1;
	}

	if (g_systemType == 8200)
	{
		DWORD regAddr = 0x00;
		DWORD setData = 0x00;

		sscanf_s(vectCmdLine[2].c_str(), "%X", &regAddr);
		sscanf_s(vectCmdLine[3].c_str(), "%X", &setData);

		if (regAddr >= 0x300)
		{
			_outpw(regAddr, setData);
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Global Register address(0x%04X) must be greater than 0x02FF!\n", regAddr);
			return 1;
		}
	}
	else if (g_systemType == 8300)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown G_WR(w) command(ACCO8300 module)\n");
		return 1;
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown G_WR(w) command(Unknown module)\n");
		return 1;
	}

	return retVal;
}

uint32_t CCmdAcco8200Module::Cmd8200GlobalRegReadW(vector<string> vectCmdLine)
{
	uint32_t retVal = 0;

	if (vectCmdLine.size() < 3)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown ACCO8200 G_RD(w) command\n");
		return 1;
	}

	if (g_systemType == 8200)
	{
		DWORD regAddr = 0x00;
		DWORD expData = 0x00;

		sscanf_s(vectCmdLine[2].c_str(), "%X", &regAddr);
		if (regAddr >= 0x300)
		{
			WORD getData = _inpw(regAddr);

			if (vectCmdLine.size() > 3)
			{
				sscanf_s(vectCmdLine[3].c_str(), "%X", &expData);

				if (getData != expData)
				{
					PrintLog(LOG_LEVEL_ERROR, "Read Global Register Address[0x%04X] Value[0x%04X] Exp[0x%04X]\n", regAddr, getData, expData);
					return 1;
				}
			}
			else
			{
				PrintLog(LOG_LEVEL_INFO, "Read Global Register Address[0x%04X] Value[0x%04X]\n", regAddr, getData);
			}
		}
		else
		{
			PrintLog(LOG_LEVEL_ERROR, "Global Register address(0x%04X) must be greater than 0x02FF!\n", regAddr);
			return 1;
		}
	}
	else if (g_systemType == 8300)
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown G_RD(w) command(ACCO8300 module)\n");
		return 1;
	}
	else
	{
		PrintLog(LOG_LEVEL_ERROR, "Unknown G_RD(w) command(Unknown module)\n");
		return 1;
	}
	return retVal;
}

uint32_t CCmdAcco8200Module::CmdHelpInf(vector<string> vectCmdLine)
{
    PrintLog(LOG_LEVEL_INFO, "#########################################################################################\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 REG_WR <MODULE(S)> <Offset(X)> <Value(X)>                                    #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 REG_RD <MODULE(S)> <Offset(X)> [ExpValue(X)]                                 #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 REG_WR(w) <MODULE(S)> <Offset(X)> <Value(X)>                                 #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 REG_RD(w) <MODULE(S)> <Offset(X)> [ExpValue(X)]                              #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 G_WR <RegAddr(X)> <Value(X)>                                                 #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 G_RD <RegAddr(X)> [ExpValue(X)]                                              #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 G_WR(w) <RegAddr(X)> <Value(X)>                                              #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 G_RD(w) <RegAddr(X)> [ExpValue(X)]                                           #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 FLASH_CFG <FlashType(S)> <SPIMode(D)> <FuncAddr(X)>                          #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 FLASH_GETID <MODULE(S)>                                                      #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 FLASH_ERASE <MODULE(S)> <EraseMode(D)> <StartAddr(X)>                        #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 FLASH_WRITE <MODULE(S)> <StartAddr(X)> <Length(D)> <DataType(S)>             #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 FLASH_ READ <MODULE(S)> <StartAddr(X)> <Length(D)> [File(S)]                 #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 HELP                                                                         #\n");
    PrintLog(LOG_LEVEL_INFO, "#########################################################################################\n");

    return 0;
}
