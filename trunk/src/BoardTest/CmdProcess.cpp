#include "stdafx.h"
#include "UtilLibrary.h"
#include "CmdProcess.h"
#include "CmdAcco8200Module.h"
#include "CmdAcco8300Module.h"
#include "CmdUtil.h"
#include "CmdCfgIni.h"
#include "Sts8100.h"
#include "STSSP8201/STSSP8201.h"
#include "STSSP8201/STSSP8201Def.h"
#include "STSSP8201/STSSPAPI.h"
#include "CmdAcco8200BERTest.h"
#include "CmdAcco8300BERTest.h"

#pragma comment(lib, "sts8100.lib")
#pragma comment(lib, "STSSP8201.lib")

#pragma warning(disable : 4996)

using namespace std;

//////////////////////////////////////////////////////////////////////////
// 扫描M8136对应的扩展板卡使用
#define EIB_MAX_EXTMD_CHANNEL_COUNT						8		// EIB板卡最大扩展通道数
#define EIB_MAX_COM_COUNT								4		// EIB最大COM口的数量

#define EIB_FUNC_R_CHANNELNUM_RESULT					0xFD	// 获取EIB扩展模块的通道数量
#define EIB_FUNC_RW_CABLECHEK							0xFE	// 启动电缆检查，获取CableCheck检测的结果

#define EIB_EXT_MD_FUNC_R_MODULE_INFO					0xFC	// 获取该模块的拨码、通道数量和模块的序号。对于需要多个COM口支持且有差异的硬件模块，需要获取对应COM口的模块序号
#define EIB_EXT_MD_FUNC_R_MODULE_TYPE					0xFE	// 获取扩展模块的类型，例如十六位数据对应为“1100”

#define OFFSET_2										2
//////////////////////////////////////////////////////////////////////////

map<string, DWORD> g_mapModuleAddress;
map<string, DWORD> g_map8300SlotAddress;
map<string, int> g_map8300AddressType;					// 0为常规的形式，1为CBITe的形式
int g_systemType = 0;			// 0是unknown

CCmdProcess::CCmdProcess()
{
    m_cmdStatePre = CMD_STATE_NON;
    m_cmdStateCur = CMD_STATE_NON;
    m_VectLoop.clear();
    m_LoopCnt = 0;
    m_PrintLoop = 0;
    m_BreakFlag = 0;
	g_mapModuleAddress.clear();
	g_map8300SlotAddress.clear();
	g_map8300AddressType.clear();
    SetEnvVariable("?", 0);
}

CCmdProcess::~CCmdProcess()
{
}

uint32_t CCmdProcess::CmdExecute()
{
	vector<string> vectCmdLine;
	char strInput[1024];
    m_runFlag = 1;
    vectCmdLine.clear();
    CmdHelpInf(vectCmdLine);
	CmdSystemCfg();

    while (m_runFlag) {
			fgets(strInput, 1024, stdin);
			strInput[strlen(strInput) - 1] = '\0';
			if (0 == strlen(strInput)) {
				continue;
			}
			ExecuteLine(strInput);
	}
    return 0;
}

uint32_t CCmdProcess::JudgeCmd(vector<string> vectCmdLine)
{
    CCmdUtil cCmdUtil;
    CCmdAcco8200Module cCmdAcco8200Md;
	CCmdAcco8300Module cCmdAcco8300Md;
	CCmdAcco8200BERTest cCmdAcco8200BT;
	CCmdAcco8300BERTest cCmdAcco8300BT;
	uint32_t retVal;

	DebugPrint(vectCmdLine);

    ToUpperCase(vectCmdLine[0]);
	ToUpperCase(vectCmdLine[1]);
    if ("HELP" == vectCmdLine[0])
	{
        CmdHelpInf(vectCmdLine);
    }
    else if ("EXIT" == vectCmdLine[0])
	{
        m_runFlag = 0;
    }
    else if ("UTIL" == vectCmdLine[0])
	{
        retVal = cCmdUtil.JudgeCmd(vectCmdLine);
        SetEnvVariable("?", retVal);
    }
	else if ("ACCO8300" == vectCmdLine[0] || ("ACCO8200" == vectCmdLine[0] && "FLASH" == vectCmdLine[1].substr(0,5)))
	{
		retVal = cCmdAcco8300Md.JudgeCmd(vectCmdLine);
		SetEnvVariable("?", retVal);
	}
	else if ("ACCO8200" == vectCmdLine[0])
	{
		retVal = cCmdAcco8200Md.JudgeCmd(vectCmdLine);
		SetEnvVariable("?", retVal);
	}
	else if ("BER_ACCO8300" == vectCmdLine[0] || ("BER_ACCO8200" == vectCmdLine[0] && "FLASH" == vectCmdLine[1]))
	{
		retVal = cCmdAcco8300BT.JudgeCmd(vectCmdLine);
		SetEnvVariable("?", retVal);
	}
	else if ("BER_ACCO8200" == vectCmdLine[0])
	{
		retVal = cCmdAcco8200BT.JudgeCmd(vectCmdLine);
		SetEnvVariable("?", retVal);
	}
	else if ("BER" == vectCmdLine[0])
	{
		retVal = cCmdUtil.BlockCmd(vectCmdLine);
		SetEnvVariable("?", retVal);
	}
	else if ("SHEL" == vectCmdLine[0])
	{
        CmdShell(vectCmdLine);
    }
	else if ("CLEAR" == vectCmdLine[0])
	{
		CmdClearWindow();
	}
	else if ("CONFIG" == vectCmdLine[0])
	{
		CmdConfig();
	}
    else
	{
        PrintLog(LOG_LEVEL_ERROR, "Command Format Error\n");
    }

    return 0;
}

uint32_t CCmdProcess::CmdShell(vector<string> vectCmdLine)
{
    char strInput[1024];
    uint32_t idx0, idx1;
    FILE *fpScript;

    if (vectCmdLine.size() < 2) {
        PrintLog(LOG_LEVEL_ERROR, "Command Format Error\n");
        return 0;
    }

    fpScript = fopen(vectCmdLine[1].c_str(), "r");
    if (NULL == fpScript) {
        PrintLog(LOG_LEVEL_ERROR, "Unable to open file %s\n", vectCmdLine[1].c_str());
        return 0;
    }

    while (NULL != fgets(strInput, 1024, fpScript)) {
        if ('\n' == strInput[strlen(strInput) - 1]) {
            strInput[strlen(strInput) - 1] = '\0';
        }
        if (0 == strlen(strInput)) {
            continue;
        }
        if (0 == strncmp("#", strInput, 1)) {
            continue;
        }

		ToUpperCase(strInput);
        switch (m_cmdStateCur) {
        case CMD_STATE_NON:
            if (0 == strncmp("LOOP", strInput, 4)) {
                m_cmdStateCur = CMD_STATE_LOOP;
                m_BreakFlag = 0;
                SplitString(vectCmdLine, strInput, " ");
                if (vectCmdLine.size() < 2) {
                    m_LoopCnt = 0;
                }
                else {
                    sscanf_s(vectCmdLine[1].c_str(), "%ld", &m_LoopCnt);
                }

                if (vectCmdLine.size() < 3) {
                    m_PrintLoop = 0;
                }
                else {
                    sscanf_s(vectCmdLine[2].c_str(), "%ld", &m_PrintLoop);
                }
                continue;
            }

            ExecuteLine(strInput);
            break;
        case CMD_STATE_LOOP:
            if (0 == strncmp("ENDLOOP", strInput, 7)) {
                for (idx0 = 0; (idx0 < m_LoopCnt) || (0 == m_LoopCnt); idx0++) {
                    for (idx1 = 0; idx1 < m_VectLoop.size(); idx1++) {
                        ExecuteLine((char *)(m_VectLoop[idx1].c_str()));
                        if (0 != m_BreakFlag) {
                            break;
                        }
                    }
                    if (0 != m_BreakFlag) {
                        break;
                    }

                    if (0 != m_PrintLoop) {
                        PrintLog(LOG_LEVEL_INFO, "Loop Count %u ... \n", idx0+1);
                    }
                }
                m_cmdStateCur = CMD_STATE_NON;
                m_VectLoop.clear();
                m_LoopCnt = 0;
                m_BreakFlag = 0;
            }
            else {
                m_VectLoop.push_back(strInput);
            }
            break;
        default:
            break;
        }
    }

    fclose(fpScript);

    return 0;
}

uint32_t CCmdProcess::ExecuteLine(char *strLine)
{
    static uint32_t condIf, condTrue;
    vector<string> vectCmdLine;
    uint32_t envValue, retVal;
    char strEnvName[32];

    vectCmdLine.clear();
    SplitString(vectCmdLine, strLine, " ");
    if (0 == condIf) {
        if ("IF" == vectCmdLine[0]) {
            strcpy(strEnvName, vectCmdLine[1].c_str());
            if ('!' == strEnvName[0]) {
                retVal = GetEnvVariable(strEnvName + 2, envValue);
            }
            else {
                retVal = GetEnvVariable(strEnvName + 1, envValue);
            }
            if (0 != retVal) {
                PrintLog(LOG_LEVEL_ERROR, "API GetEnvVariable Return With Error Code\n", retVal);
                envValue = 0;
                return 0;
            }
            condIf = 1;
            if ('!' == strEnvName[0]) {
                condTrue = (0 == envValue) ? 0 : 1;
            }
            else {
                condTrue = (0 == envValue) ? 1 : 0;
            }
        }
        else {
            JudgeCmd(vectCmdLine);
        }
    }
    else if (1 == condIf) {
        if ("ENDIF" == vectCmdLine[0]) {
            condIf = 0;
            condTrue = 0;
        }
        else if ("ELSE" == vectCmdLine[0]) {
            condIf = 2;
            condTrue = (0 == condTrue) ? 1 : 0;
        }
        else {
            if (0 != condTrue) {
                if ("BREAK" == vectCmdLine[0]) {
                    m_BreakFlag = 1;
                    condIf = 0;
                    condTrue = 0;
                }
                else {
                    JudgeCmd(vectCmdLine);
                }
            }
        }
    }
    else if (2 == condIf) {
        if ("ENDIF" == vectCmdLine[0]) {
            condIf = 0;
            condTrue = 0;
        }
        if (0 != condTrue) {
            JudgeCmd(vectCmdLine);
        }
    }
    else {
    }

    return 0;
}

uint32_t CCmdProcess::CmdHelpInf(vector<string> vectCmdLine)
{
    PrintLog(LOG_LEVEL_INFO, "#########################################################################################\n");
	PrintLog(LOG_LEVEL_INFO, "# UTIL HELP                                                                             #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8200 HELP                                                                         #\n");
	PrintLog(LOG_LEVEL_INFO, "# ACCO8300 HELP                                                                         #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8200 HELP                                                                     #\n");
	PrintLog(LOG_LEVEL_INFO, "# BER_ACCO8300 HELP                                                                               #\n");
	PrintLog(LOG_LEVEL_INFO, "# SHEL <File(S)>                                                                        #\n");
	PrintLog(LOG_LEVEL_INFO, "# CONFIG                                                                                #\n");
	PrintLog(LOG_LEVEL_INFO, "# CLEAR                                                                                 #\n");
	PrintLog(LOG_LEVEL_INFO, "# EXIT                                                                                  #\n");
	PrintLog(LOG_LEVEL_INFO, "# HELP                                                                                  #\n");
    PrintLog(LOG_LEVEL_INFO, "#########################################################################################\n");

    return 0;
}

uint32_t CCmdProcess::CmdClearWindow()
{
	system("cls");
	vector<string> vectCmdLine;
	CmdHelpInf(vectCmdLine);
	return 0;
}

uint32_t CCmdProcess::CmdConfig()
{
	CmdSystemCfg();
	return 0;
}

uint32_t CCmdProcess::CmdSystemCfg()
{
	int ret = 0;
	g_mapModuleAddress.clear();
	g_map8300SlotAddress.clear();
	g_map8300AddressType.clear();

	string iniName = "AT_BoardTest";
	CCmdCfgIni* pConfig = CCmdCfgIni::GetInstance(iniName);
	if (pConfig->ReadINI())
	{
		PrintLog(LOG_LEVEL_ERROR, pConfig->m_strMsg.c_str());				// 读取INI文件错误
	}
	else
	{
		StsSetStation(1);
		StsSetSitesStatus(0xFFFF);

		WORD startAddress = 0x010;
		int retVal1 = Cmd8200SystemCfg(pConfig, startAddress);
		int retVal2 = Cmd8136SystemCfg(pConfig, startAddress);
		if (retVal1 || retVal2)
		{
			g_systemType = 8200;
			PrintLog(LOG_LEVEL_INFO, "# ----------------------------- STS8200 Valid Module ---------------------------------- #\n");
		}
		else
		{
			if (Cmd8300SystemCfg(pConfig))
			{
				g_systemType = 8300;
				PrintLog(LOG_LEVEL_INFO, "# ----------------------------- STS8300 Valid Module ---------------------------------- #\n");
			}
			else
			{
				g_systemType = 0;
				PrintLog(LOG_LEVEL_INFO, "# ---------------------------------- Valid Module ------------------------------------- #\n");
			}
		}

		if (g_mapModuleAddress.size())
		{
			PrintLog(LOG_LEVEL_INFO, "#  No.    MODULE\n");
			int index = 0;
			map<string, DWORD>::iterator iter;
			for (iter = g_mapModuleAddress.begin(); iter != g_mapModuleAddress.end(); iter++)
			{
				PrintLog(LOG_LEVEL_INFO, "# -%2d-   %s\n", ++index, iter->first.c_str());
			}
		}
		else
		{
			PrintLog(LOG_LEVEL_INFO, "# No Module !!!\n");
			PrintLog(LOG_LEVEL_INFO, "# Please check the hardware status and the ini file !!!\n");
		}
		PrintLog(LOG_LEVEL_INFO, "# ------------------------------------------------------------------------------------- #\n");
	}
	return 0;
}

uint32_t CCmdProcess::Cmd8200SystemCfg(CCmdCfgIni* pCfg, WORD& startAddr)
{
	int ret = 0;
	// 8200系统
	int iMd = 0;
	int iBd = 0;
	int iCh = 0;
	int Module8200 = pCfg->m_vec8200SystemModule.size();
	for (iMd = 0; iMd < Module8200; iMd++)
	{
		if (pCfg->m_vec8200SystemModule[iMd] == "M8136")
		{
			continue;
		}
		map<string, MD_INFO>::iterator iter = pCfg->m_mapSysModule.find(pCfg->m_vec8200SystemModule[iMd]);
		if (iter != pCfg->m_mapSysModule.end())
		{
			DWORD typeID = iter->second.typeID;
			int maxBoard = iter->second.maxBoard;
			int maxChannel = iter->second.maxChannel;
			int verify = iter->second.verify;
			int step = iter->second.cfgAddressStep;
			char chModuleName[4] = { 0 };
			chModuleName[0] = 'M';
			chModuleName[1] = (char)((typeID >> 8) & 0xFF);
			chModuleName[2] = (char)(typeID & 0xFF);
			chModuleName[3] = (char)0;

			for (iBd = 0; iBd < maxBoard; iBd++)
			{
				int getByVer = StsGetModuleVersion(chModuleName, verify, iBd);
				if (getByVer)
				{
					BYTE* pBsIndex = new BYTE[maxChannel];
					BYTE* pChInBoard = new BYTE[maxChannel];
					WORD* pChAddress = new WORD[maxChannel];
					PSTS_BIND_INFO pbindInfo = new STS_BIND_INFO[maxChannel];
					for (iCh = 0; iCh < maxChannel; iCh++)
					{
						char strMd[256] = { 0 };
						sprintf_s(strMd, "%s_%d-%d", pCfg->m_vec8200SystemModule[iMd].c_str(), iBd + 1, iCh);
						pBsIndex[iCh] = iBd;
						pChInBoard[iCh] = iCh;
						pChAddress[iCh] = startAddr;
						g_mapModuleAddress.insert(pair<string, DWORD>(strMd, startAddr));
						startAddr += step;

						pbindInfo[iCh].lidx = iCh + iBd * maxChannel;
						pbindInfo[iCh].sidx = 0;
						pbindInfo[iCh].pidx = iCh + iBd * maxChannel;
					}
					BOOL bindFlag = StsGeneralBindAddr(chModuleName, pbindInfo, pBsIndex, pChInBoard, pChAddress, maxChannel, 0);

					delete[] pBsIndex;
					delete[] pChInBoard;
					delete[] pChAddress;
					delete[] pbindInfo;
					ret++;
				}
			}
		}
	}
	return ret;
}

uint32_t CCmdProcess::Cmd8136SystemCfg(CCmdCfgIni* pCfg, WORD& startAddr)
{
	int ret = 0;
	char strMd[256] = { 0 };
	// 8136
	int iBd = 0;
	char chModuleName[4] = { 0 };
	chModuleName[0] = 'M';
	chModuleName[1] = (char)0x81;
	chModuleName[2] = (char)0x36;
	chModuleName[3] = (char)0;
	for (iBd = 0; iBd < 16; iBd++)				// 最大16块板卡
	{
		int getByVer = StsGetModuleVersion(chModuleName, 0x89, iBd);				// 校验码是0x89
		if (getByVer)
		{
			sprintf_s(strMd, "M8136_%d-0", iBd + 1);
			BYTE BsIndex = iBd;
			BYTE ChInBoard = 0;
			WORD ChAddress = startAddr;
			STS_BIND_INFO bindInfo;
			bindInfo.lidx = iBd;
			bindInfo.sidx = 0;
			bindInfo.pidx = iBd;

			g_mapModuleAddress.insert(pair<string, DWORD>(strMd, startAddr));
			startAddr += 4;

			BOOL bindFlag = StsGeneralBindAddr(chModuleName, &bindInfo, &BsIndex, &ChInBoard, &ChAddress, 1, 0);
			if (bindFlag)
			{
				CmdControl8136Ext(iBd, ChAddress, startAddr);
			}
			ret++;
		}
	}
	return ret;
}

void CCmdProcess::CmdControl8136Ext(int bs8136, WORD addr8136, WORD& startAddr)
{
	int cableRes = 0;
	int channelStatus = 0;
	Cmd8136CableCheck(addr8136, cableRes, channelStatus);

	int arrEibChIndex[EIB_MAX_EXTMD_CHANNEL_COUNT] = { 0 };
	int actChannelCnt = 0;
	if ((cableRes != 0xFF) && (cableRes != 0x00))
	{
		// 先对4个通讯的COM口进行检测，两bit的数据：11表示没有通道存在，01表示1个通道，10表示2个通道，00暂未使用
		for (int iCom = 0; iCom < EIB_MAX_COM_COUNT; iCom++)
		{
			if (iCom * 2 > EIB_MAX_EXTMD_CHANNEL_COUNT)
			{
				break;
			}

			int tempChData = (channelStatus >> (iCom * 2)) & 0x03;
			if (cableRes & (0x01 << iCom))
			{
				if ((tempChData == 1) || (tempChData == 2))
				{
					if (tempChData == 1)
					{
						arrEibChIndex[actChannelCnt] = iCom * 2 + 1;
					}
					else
					{
						arrEibChIndex[actChannelCnt] = iCom * 2 + 1;
						arrEibChIndex[actChannelCnt + 1] = iCom * 2 + 2;
					}
					actChannelCnt += tempChData;
				}
			}
		}
	}

	if (actChannelCnt)
	{
		char chModuleName[4] = { 0 };
		chModuleName[0] = 'M';				// EIB = SM8136
		chModuleName[1] = (char)0x81;
		chModuleName[2] = (char)0x36;
		chModuleName[3] = (char)0;

		PSTS_BIND_INFO pBindInfo = new STS_BIND_INFO[actChannelCnt];
		BYTE* pBsInEIB = new BYTE[actChannelCnt];
		BYTE* pChInEIB = new BYTE[actChannelCnt];
		WORD* pAddr = new WORD[actChannelCnt];

		int iCh = 0;
		for (iCh = 0; iCh < actChannelCnt; iCh++)
		{
			pBindInfo[iCh].pidx = iCh;
			pBindInfo[iCh].sidx = 0;
			pBindInfo[iCh].lidx = iCh;
			pBsInEIB[iCh] = bs8136;
			pChInEIB[iCh] = arrEibChIndex[iCh];
			pAddr[iCh] = startAddr;
			startAddr += 4;
		}

		// 工位绑定操作
		StsGeneralBindAddr(chModuleName, pBindInfo, pBsInEIB, pChInEIB, pAddr, actChannelCnt, 0);

		char strMd[256] = { 0 };
		int channel = 0;
		for (iCh = 0; iCh < actChannelCnt; iCh++)
		{
			WORD exType = 0;
			BYTE exBs = 0;
			BYTE exChCnt = 0;
			BYTE exInnerIndex = 0;
			Cmd8136ExtInfo(pAddr[iCh], exType, exBs, exChCnt, exInnerIndex);
			if (exType != 0xFFFF)
			{
				if ((exChCnt > 1) && (channel == 0))
				{
					channel = exChCnt;
				}
				if (channel)
				{
					sprintf_s(strMd, "EX%X_%d-%d_B%d", exType, exBs + 1, exChCnt - channel, exInnerIndex + 1);
					channel--;
				}
				else
				{
					sprintf_s(strMd, "EX%X_%d-0_B%d", exType, exBs + 1, exInnerIndex + 1);
				}
				g_mapModuleAddress.insert(pair<string, DWORD>(strMd, pAddr[iCh]));
			}			
		}

		if (pBindInfo != NULL)
		{
			delete[] pBindInfo;
			pBindInfo = NULL;
		}
		if (pBsInEIB != NULL)
		{
			delete[] pBsInEIB;
			pBsInEIB = NULL;
		}
		if (pChInEIB != NULL)
		{
			delete[] pChInEIB;
			pChInEIB = NULL;
		}
		if (pAddr != NULL)
		{
			delete[] pAddr;
			pAddr = NULL;
		}
	}
}

void CCmdProcess::Cmd8136CableCheck(WORD chAddr, int& cableRes, int& channelStatus)
{
	// 启动电缆检测
	_outp(chAddr, EIB_FUNC_RW_CABLECHEK);
	_outp(chAddr + OFFSET_2, 0x01);
	wait_ms(1);
	// 获取EIB板卡电缆状态检测的结果
	_outp(chAddr, EIB_FUNC_RW_CABLECHEK);
	cableRes = _inp(chAddr + OFFSET_2);
	// 获取EIB板卡扩展的通道信息
	_outp(chAddr, EIB_FUNC_R_CHANNELNUM_RESULT);
	channelStatus = _inp(chAddr + OFFSET_2);
}

void CCmdProcess::Cmd8136ExtInfo(WORD chAddr, WORD& mdType, BYTE& mdBs, BYTE& mdChCnt, BYTE& mdInnerIndex)
{
	_outp(chAddr, EIB_EXT_MD_FUNC_R_MODULE_TYPE);
	mdType = _inpw(chAddr + OFFSET_2);

	_outp(chAddr, EIB_EXT_MD_FUNC_R_MODULE_INFO);
	WORD getData = _inpw(chAddr + OFFSET_2);
	mdBs = getData & 0x0F;
	mdChCnt = ((getData >> 4) & 0x0F) + 1;
	mdInnerIndex = (getData >> 8) & 0x07;
}

uint32_t CCmdProcess::Cmd8300SystemCfg(CCmdCfgIni* pCfg)
{
	int ret = 0;
	// 8300系统
	// 常规8300资源
	DWORD startAddr = 0x2500;					// 8300板卡操作RAM的地址指针，下一次操作，该地址加1（正序）
	StsInterfaceBoardLogicSelect(0x00);			// 使用旧逻辑，0x00使用旧逻辑
	short systemMdCnt = StsScanBoard();
	systemMdCnt = 3;
	if (systemMdCnt > 0)
	{
		bool calFlag = false;
		bool dcmFlag = false;
		bool cbiteFlag = false;
		int iBd = 0;
		int iCh = 0;
		DWORD typeID = 0x00;
		int maxBoard = 0;
		int maxChannel = 0;
		int slotID = 0;
		DWORD logicSel = 0x00;
		HEAD_BOARD_INFO strbdinfo;
		for (iBd = 0; iBd < systemMdCnt; iBd++)
		{		
			StsGetBoardInfo(iBd, &strbdinfo);

			if (strbdinfo.BoardInfo.boardtype == 0x8204)				// CAL
			{
				calFlag = true;
				continue;
			}
			if (strbdinfo.BoardInfo.boardtype == 0x8213)				// DCM
			{
				dcmFlag = true;
				continue;
			}
			if (strbdinfo.BoardInfo.boardtype == 0x8205)				// CBITe
			{
				cbiteFlag = true;
				continue;
			}
			char tempName[256] = { 0 };
			sprintf_s(tempName, 256, "M%X", strbdinfo.BoardInfo.boardtype);

			map<string, MD_INFO>::iterator iter = pCfg->m_mapSysModule.find(tempName);
			if (iter != pCfg->m_mapSysModule.end())
			{
				typeID = strbdinfo.BoardInfo.boardtype;
				maxBoard = iter->second.maxBoard;
				maxChannel = iter->second.maxChannel;
				slotID = strbdinfo.BoardInfo.slot;

				logicSel |= (0x01 << (slotID - 1));

				DWORD* inputChannel = new DWORD[maxChannel];
				DWORD* inputSlot = new DWORD[maxChannel];
				for (iCh = 0; iCh < maxChannel; iCh++)
				{
					inputChannel[iCh] = startAddr;
					inputSlot[iCh] = 0x01 << (slotID - 1);

					char strMd[256] = { 0 };
					sprintf_s(strMd, 256, "S%d_%d (M%X)", slotID, iCh, typeID);
					g_mapModuleAddress.insert(pair<string, DWORD>(strMd, startAddr));
					sprintf_s(strMd, 256, "S%d_%d", slotID, iCh);
					g_map8300SlotAddress.insert(pair<string, DWORD>(strMd, startAddr));
					g_map8300AddressType.insert(pair<string, int>(strMd, 0));
					startAddr++;
					ret++;
				}

				// 配置接口板信息
				StsInterfaceBoardRamConfig(inputChannel[0], inputSlot, maxChannel);
				// 配置单通道起始通道代号寄存器
				StsFeatureBoardSingleChannelRegCfg(slotID, inputChannel[0]);
				// 擦除该板卡的RAM路由表中所有内容
				StsFeatureBoardRamClear(slotID);
				wait_us(50);

				delete[] inputChannel;
				delete[] inputSlot;				
			}
		}
		StsInterfaceBoardLogicSelect(logicSel);
		// DCM
		if (dcmFlag)
		{
			for (iBd = 0; iBd < systemMdCnt; iBd++)
			{
				StsGetBoardInfo(iBd, &strbdinfo);

				if (strbdinfo.BoardInfo.boardtype != 0x8213)					// DCM
				{
					continue;
				}

				slotID = strbdinfo.BoardInfo.slot;
				char strMd[256] = { 0 };
				sprintf_s(strMd, 256, "S%d (DCM)", slotID);
				g_mapModuleAddress.insert(pair<string, DWORD>(strMd, slotID));
				g_map8300SlotAddress.insert(pair<string, DWORD>(strMd, slotID));
				g_map8300AddressType.insert(pair<string, int>(strMd, 2));
				ret++;
			}
		}

		// CBITe/CAL
		if (cbiteFlag || calFlag)
		{
			for (iBd = 0; iBd < systemMdCnt; iBd++)
			{
				StsGetBoardInfo(iBd, &strbdinfo);

				if ((strbdinfo.BoardInfo.boardtype != 0x8205) &&				// CBITe
					(strbdinfo.BoardInfo.boardtype != 0x8204))				// CAL
				{
					continue;
				}
				char tempName[256] = { 0 };
				sprintf_s(tempName, 256, "M%X", strbdinfo.BoardInfo.boardtype);

				map<string, MD_INFO>::iterator iter = pCfg->m_mapSysModule.find(tempName);
				if (iter != pCfg->m_mapSysModule.end())
				{
					typeID = strbdinfo.BoardInfo.boardtype;
					maxBoard = iter->second.maxBoard;
					maxChannel = iter->second.maxChannel;
					slotID = strbdinfo.BoardInfo.slot;

					DWORD address = CmdBind(slotID);				// 工位配置

					char strMd[256] = { 0 };
					sprintf_s(strMd, 256, "S%d_0 (M%X)", slotID, typeID);
					g_mapModuleAddress.insert(pair<string, DWORD>(strMd, address));
					sprintf_s(strMd, 256, "S%d_0", slotID);
					g_map8300SlotAddress.insert(pair<string, DWORD>(strMd, address));
					g_map8300AddressType.insert(pair<string, int>(strMd, 1));

					ret++;
				}
			}
		}
	}

	return ret;
}

DWORD CCmdProcess::CmdBind(int slotID)
{
	DWORD bind_addr = (0x3F1 << 14) + ((slotID - 1) << 2);
	write_dw(bind_addr, 0x00);
	return slotID;
}