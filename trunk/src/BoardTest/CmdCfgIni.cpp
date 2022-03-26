#include "stdafx.h"
#include "CmdCfgIni.h"
#include "UtilLibrary.h"

using namespace std;

CCmdCfgIni *CCmdCfgIni::m_instance = NULL;
string CCmdCfgIni::m_strIniPath = "";

string strSection = "MDTYPE";
string strTypeCnt = "TypeCnt";

string strTypeID = "TypeID";
string strSystem = "System";
string strMaxBoard = "MaxBoard";
string strMaxChannel = "MaxChannel";
string strActUseChannel = "ActUseChannel";
string strVerify = "Verify";
string strCfgAddressStep = "CfgAddressStep";


CCmdCfgIni::CCmdCfgIni(void)
{
	m_strMsg = "";
	if (m_vec8200SystemModule.size())
	{
		m_vec8200SystemModule.clear();
	}
}

CCmdCfgIni::~CCmdCfgIni(void)
{
    if (m_instance)
    {
        delete m_instance;
        m_instance = NULL;
    }
}
CCmdCfgIni *CCmdCfgIni::GetInstance(string iniName)
{
    if (NULL == m_instance)
    {
        string prjName = "BoardTest.exe";
        m_instance = new CCmdCfgIni();
        HMODULE hModule = GetModuleHandle(prjName.c_str());
        char modulePath[1024];
        int strLength;
        strLength = GetModuleFileName(hModule, modulePath, 1024);
        string path(modulePath);
        path = path.substr(0, path.length() - prjName.length());
        path += iniName;
        m_strIniPath = path.c_str();
    }
    return m_instance;
}

// Get settings from ini file
int CCmdCfgIni::ReadINI()
{
	int ret = 0;
	int index = 0;
	int returnSize = 0;
	const int strLen = 256;
	char strSec[strLen] = { 0 };
	char strKey[strLen] = { 0 };
	char getStr[strLen] = { 0 };
	char errMsg[strLen] = { 0 };
	char flashType[strLen] = { 0 };
	char flashID[strLen] = { 0 };
	vector<string> supportType;
	vector<string> pairValue;
	m_strValueType.clear();
	m_strValueID.clear();
	m_vectRegBitOfst.clear();
	m_vectBramRegs.clear();
	m_vectDramRegs.clear();

	GetPrivateProfileString("OfstDefine", "regOfst", "", getStr, strLen, m_strIniPath.c_str());
	if (0 != strcmp(getStr, ""))
	{
		ULONG ulTemp;

		sprintf_s(strSec, "%s_OFST", getStr);
		for (index = 1; index < 255; index++)
		{
			sprintf_s(strKey, "REGOFST%d", index);
			GetPrivateProfileString(strSec, strKey, "", getStr, strLen, m_strIniPath.c_str());
			if (0 == strlen(getStr))
			{
				break;
			}
			sscanf_s(getStr, "%u", &ulTemp);
			m_vectRegBitOfst.push_back(ulTemp);
		}

		string strKeyBram[6] = { "BRAM_WR_ADDR", "BRAM_WR_CMDS", "BRAM_WR_DATA", "BRAM_RD_ADDR", "BRAM_RD_CMDS", "BRAM_RD_DATA" };
		for (index = 0; index < 6; index++)
		{
			GetPrivateProfileString(strSec, strKeyBram[index].c_str(), "", getStr, strLen, m_strIniPath.c_str());
			if (0 == strlen(getStr))
			{
				ulTemp = 0xFFFFFFFF;
			}
			else
			{
				sscanf_s(getStr, "%X", &ulTemp);
			}
			m_vectBramRegs.push_back(ulTemp);
		}

		string strKeyDram[9] = { "DRAM_WR_ADDR", "DRAM_WR_CMDS", "DRAM_WR_DATA", "DRAM_WR_STAT", "DRAM_RD_ADDR", "DRAM_RD_CMDS", "DRAM_RD_DATA", "DRAM_RD_STAT", "DRAM_ADDR_INC" };
		for (index = 0; index < 9; index++)
		{
			GetPrivateProfileString(strSec, strKeyDram[index].c_str(), "", getStr, strLen, m_strIniPath.c_str());
			if (0 == strlen(getStr))
			{
				ulTemp = 0xFFFFFFFF;
			}
			else
			{
				sscanf_s(getStr, "%X", &ulTemp);
			}
			m_vectDramRegs.push_back(ulTemp);
		}
	}

	// 1、读取是否进行自检
	if (GetPrivateProfileString(strSection.c_str(), strTypeCnt.c_str(), "", getStr, strLen, m_strIniPath.c_str()))
	{
		int typeCnt = atoi(getStr);
		if (typeCnt > 0)
		{
			for (index = 0; index < typeCnt; index++)
			{
				char str[32] = { 0 };
				sprintf_s(str, 32, "type%d", index + 1);
				if (GetPrivateProfileString(strSection.c_str(), str, "", getStr, strLen, m_strIniPath.c_str()))
				{
					supportType.push_back(getStr);
				}
				else
				{
					sprintf_s(errMsg, strLen, "INI Error! \"%s\"[MDTYPE] is error!\n", str);
					m_strMsg = errMsg;
					ret = 3;
					break;
				}
			}
		}
		else
		{
			m_strMsg = "INI Error! \"TypeCnt\"[MDTYPE] is less than 1 or error!\n";
			ret = 2;			// INI错误，TypeCnt错误或小于1。
		}
	}
	else
	{
		m_strMsg = "INI Error! \"TypeCnt\"[MDTYPE] is error!\n";
		ret = 1;				// INI错误，TypeCnt不存在
	}

	if (!ret)
	{
		for (index = 0; index < supportType.size(); index++)
		{
			MD_INFO mdinfo;
			// 读取系统
			if (GetPrivateProfileString(supportType[index].c_str(), strSystem.c_str(), "", getStr, strLen, m_strIniPath.c_str()))
			{
				mdinfo.system = atoi(getStr);
				if (mdinfo.system == 8200)
				{
					m_vec8200SystemModule.push_back(supportType[index]);
				}
				else if (mdinfo.system == 8300)
				{
					m_vec8300SystemModule.push_back(supportType[index]);
				}
				else
				{
					sprintf_s(errMsg, strLen, "INI Error! \"%s\"[%s] is error!\n", strSystem.c_str(), supportType[index].c_str());
					m_strMsg = errMsg;
					ret = 4;
					break;
				}
			}
			else
			{
				sprintf_s(errMsg, strLen, "INI Error! \"%s\"[%s] is error!\n", strSystem.c_str(), supportType[index].c_str());
				m_strMsg = errMsg;
				ret = 4;
				break;
			}

			// 读取TypeID
			if (GetPrivateProfileString(supportType[index].c_str(), strTypeID.c_str(), "", getStr, strLen, m_strIniPath.c_str()))
			{
				sscanf_s(getStr, "%X", &mdinfo.typeID);
			}
			else
			{
				sprintf_s(errMsg, strLen, "INI Error! \"%s\"[%s] is error!\n", strTypeID.c_str(), supportType[index].c_str());
				m_strMsg = errMsg; 
				ret = 5;
				break;
			}

			// 读取支持板数
			if (GetPrivateProfileString(supportType[index].c_str(), strMaxBoard.c_str(), "", getStr, strLen, m_strIniPath.c_str()))
			{
				mdinfo.maxBoard = atoi(getStr);
				if (mdinfo.maxBoard <= 0)
				{
					sprintf_s(errMsg, strLen, "INI Error! \"%s\"[%s] is error!\n", strMaxBoard.c_str(), supportType[index].c_str());
					m_strMsg = errMsg;
					ret = 6;
					break;
				}
			}
			else
			{
				sprintf_s(errMsg, strLen, "INI Error! \"%s\"[%s] is error!\n", strMaxBoard.c_str(), supportType[index].c_str());
				m_strMsg = errMsg;
				ret = 6;
				break;
			}

			// 读取单板最大通道数
			if (GetPrivateProfileString(supportType[index].c_str(), strMaxChannel.c_str(), "", getStr, strLen, m_strIniPath.c_str()))
			{
				mdinfo.maxChannel = atoi(getStr);
				if (mdinfo.maxChannel <= 0)
				{
					sprintf_s(errMsg, strLen, "INI Error! \"%s\"[%s] is error!\n", strMaxChannel.c_str(), supportType[index].c_str());
					m_strMsg = errMsg;
					ret = 7;
					break;
				}
			}
			else
			{
				sprintf_s(errMsg, strLen, "INI Error! \"%s\"[%s] is error!\n", strMaxChannel.c_str(), supportType[index].c_str());
				m_strMsg = errMsg;
				ret = 7;
				break;
			}

			// 读取校验码
			if (GetPrivateProfileString(supportType[index].c_str(), strVerify.c_str(), "", getStr, strLen, m_strIniPath.c_str()))
			{
				sscanf_s(getStr, "%X", &mdinfo.verify);
			}
			else
			{
				sprintf_s(errMsg, strLen, "INI Error! \"%s\"[%s] is error!\n", strVerify.c_str(), supportType[index].c_str());
				m_strMsg = errMsg;
				ret = 8;
				break;
			}

			// 读取配置地址的步进
			if (GetPrivateProfileString(supportType[index].c_str(), strCfgAddressStep.c_str(), "", getStr, strLen, m_strIniPath.c_str()))
			{
				mdinfo.cfgAddressStep = atoi(getStr);
				if (mdinfo.cfgAddressStep < 0)
				{
					sprintf_s(errMsg, strLen, "INI Error! \"%s\"[%s] is error!\n", strCfgAddressStep.c_str(), supportType[index].c_str());
					m_strMsg = errMsg;
					ret = 9;
					break;
				}
			}
			else
			{
				sprintf_s(errMsg, strLen, "INI Error! \"%s\"[%s] is error!\n", strCfgAddressStep.c_str(), supportType[index].c_str());
				m_strMsg = errMsg;
				ret = 9;
				break;
			}

			m_mapSysModule.insert(pair<string, MD_INFO>(supportType[index], mdinfo));
		}
	}


	returnSize = GetPrivateProfileSection("FLASHINFO", flashType, strLen, m_strIniPath.c_str());
	if (returnSize)
	{
		int count = 0;
		int countPrivate = 0;
		int tempFlashType = 0;
		char strValue[strLen] = { 0 };
		DWORD tempFlashID = 0X00;
		while (count < returnSize)
		{
			memset(strValue, 0, sizeof(strValue));
			memcpy(strValue, &flashType[count], strlen(&flashType[count]));
			countPrivate = strlen(&flashType[count]) + 1;
			count += countPrivate;
			SplitString(pairValue, strValue, "=");
			tempFlashType = atoi(pairValue[2].c_str());
			tempFlashID = strtoul(pairValue[1].c_str(),NULL,16);
			m_strValueType.insert({ pairValue[0], tempFlashType });
			m_strValueID.insert({ pairValue[0], tempFlashID });
		}
	}

	return ret;
}