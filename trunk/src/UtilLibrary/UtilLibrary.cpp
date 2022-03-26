// UtilLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <time.h>
#include "UtilLibrary.h"

#pragma warning(disable : 4996)

#pragma comment(lib, "version.lib")

/********************************************************************************
local structure define
********************************************************************************/
typedef struct {
    string strName;
    string strValue;
}ST_ENV_VARIABLE;

/********************************************************************************
local variable define
********************************************************************************/
static vector<ST_ENV_VARIABLE> gVectEnvVariable;

UTILLIBRARY_API uint32_t SetEnvVariable(string strName, string strValue) 
{
    vector<ST_ENV_VARIABLE>::iterator itor;
    ST_ENV_VARIABLE stEnvVariable;

    for (itor = gVectEnvVariable.begin(); itor != gVectEnvVariable.end(); itor++) {
        if (strName == itor->strName) {
            itor->strValue = strValue;
            break;
        }
    }
    if (itor == gVectEnvVariable.end()) {
        stEnvVariable.strName = strName;
        stEnvVariable.strValue = strValue;
        gVectEnvVariable.push_back(stEnvVariable);
    }

    return 0;
}

UTILLIBRARY_API uint32_t SetEnvVariable(string strName, uint32_t value)
{
    vector<ST_ENV_VARIABLE>::iterator itor;
    ST_ENV_VARIABLE stEnvVariable;
    char strValue[32];

    sprintf(strValue, "%d", value);
    for (itor = gVectEnvVariable.begin(); itor != gVectEnvVariable.end(); itor++) {
        if (strName == itor->strName) {
            itor->strValue = strValue;
            break;
        }
    }
    if (itor == gVectEnvVariable.end()) {
        stEnvVariable.strName = strName;
        stEnvVariable.strValue = strValue;
        gVectEnvVariable.push_back(stEnvVariable);
    }

    return 0;
}

UTILLIBRARY_API uint32_t GetEnvVariable(string strName, string &strValue)
{
    vector<ST_ENV_VARIABLE>::iterator itor;

    for (itor = gVectEnvVariable.begin(); itor != gVectEnvVariable.end(); itor++) {
        if (strName == itor->strName) {
            strValue = itor->strValue;
            break;
        }
    }

    if (itor == gVectEnvVariable.end()) {
        return ERR_UTIL_ENV_NEXIST;
    }

    return 0;
}

UTILLIBRARY_API uint32_t GetEnvVariable(string strName, uint32_t &value)
{
    vector<ST_ENV_VARIABLE>::iterator itor;
    

    for (itor = gVectEnvVariable.begin(); itor != gVectEnvVariable.end(); itor++) {
        if (strName == itor->strName) {
            value = atoi(itor->strValue.c_str());
            break;
        }
    }

    if (itor == gVectEnvVariable.end()) {
        return ERR_UTIL_ENV_NEXIST;
    }

    return 0;
}

UTILLIBRARY_API uint32_t ClrEnvVariable(string strName)
{
    vector<ST_ENV_VARIABLE>::iterator itor;

    for (itor = gVectEnvVariable.begin(); itor != gVectEnvVariable.end(); itor++) {
        if (strName == itor->strName) {
            gVectEnvVariable.erase(itor);
            break;
        }
    }

    return 0;
}

UTILLIBRARY_API uint32_t FillDataPattern(uint8_t *pBuff, uint32_t fSize, DATA_PATTERN_SETS dataType, uint8_t dataStep)
{
	uint32_t index;

	if (DATA_PATTERN_00 == dataType) {
		memset(pBuff, 0x00, fSize);
	}
	else if (DATA_PATTERN_55 == dataType) {
		memset(pBuff, 0x55, fSize);
	}
	else if (DATA_PATTERN_AA == dataType) {
		memset(pBuff, 0xAA, fSize);
	}
	else if (DATA_PATTERN_FF == dataType) {
		memset(pBuff, 0xFF, fSize);
	}
	else if (DATA_PATTERN_5A == dataType) {
		memset(pBuff, 0x5A, fSize);
	}
	else if (DATA_PATTERN_A5 == dataType) {
		memset(pBuff, 0xA5, fSize);
	}
	else if (DATA_PATTERN_INC == dataType) {
		for (index = 0; index < fSize / dataStep; index++) {
			switch (dataStep) {
			case 1:
				((uint8_t *)(pBuff))[index] = (uint8_t)index;
				break;
			case 2:
				((uint16_t *)(pBuff))[index] = (uint16_t)index;
				break;
			case 4:
				((uint32_t *)(pBuff))[index] = (uint32_t)index;
				break;
			default:
				break;
			}
		}
	}
	else if (DATA_PATTERN_DEC == dataType) {
		for (index = 0; index < fSize / dataStep; index++) {
			switch (dataStep) {
			case 1:
				((uint8_t *)(pBuff))[index] = 0xFF - (uint8_t)index;
				break;
			case 2:
				((uint16_t *)(pBuff))[index] = 0xFFFF - (uint16_t)index;
				break;
			case 4:
				((uint32_t *)(pBuff))[index] = 0xFFFFFFFF - (uint32_t)index;
				break;
			default:
				break;
			}
		}
	}
	else {
		srand((uint32_t)(time(NULL)));
		for (index = 0; index < fSize; index++) {
			pBuff[index] = (uint8_t)rand();
		}
	}

	return 0;
}

UTILLIBRARY_API uint32_t WriteTextFile(string strFile, uint32_t *pBuff, uint32_t ramWidth, uint32_t ramDepth)
{
	uint32_t widthAlign;
	FILE *fpFile;

	fpFile = fopen(strFile.c_str(), "w");
	if (NULL == fpFile) {
		return 1;
	}

	widthAlign = (ramWidth + 31) / 32;
	for (uint32_t depthIdx = 0; depthIdx < ramDepth; depthIdx++)
	{
		for (uint32_t widthIdx = 0; widthIdx < widthAlign; widthIdx++)
		{
			if (widthIdx == widthAlign - 1)
			{
				fprintf(fpFile, "%08X", *pBuff);
			}
			else
			{
				fprintf(fpFile, "%08X_", *pBuff);
			}
			pBuff++;
		}
		fprintf(fpFile, "\n");
	}

	fclose(fpFile);

	return 0;
}

UTILLIBRARY_API uint32_t ReadTextFile(string strFile, uint32_t *pBuff, uint32_t ramWidth, uint32_t ramDepth, uint32_t startLine)
{
    uint32_t widthAlign;
    char strLine[1024];
    FILE *fpFile;

    fpFile = fopen(strFile.c_str(), "r");
    if (NULL == fpFile) {
        return 1;
    }
	while (startLine > 0) {
		if (NULL == fgets(strLine, 1024, fpFile)) {
			break;
		}
		startLine--;
	}
	widthAlign = (ramWidth + 31) / 32;
    memset(pBuff, 0x00, sizeof(uint32_t) * ramDepth * widthAlign);
    for (uint32_t depthIdx = 0; depthIdx < ramDepth; depthIdx++)
    {
        vector<string> vectSplit;

        if (NULL == fgets(strLine, 1024, fpFile))
        {
            break;
        }
        SplitString(vectSplit, strLine, "_");
        for (uint32_t widthIdx = 0; widthIdx < widthAlign; widthIdx++)
        {
            if (vectSplit.size() > widthIdx)
            {
                sscanf_s(vectSplit[widthIdx].c_str(), "%X", pBuff + widthAlign * depthIdx + widthIdx);
            }
        }
    }

    fclose(fpFile);

    return 0;
}

/*!
	* @brief	回读模块的属性信息
	* @return   回读的信息
*/
UTILLIBRARY_API string GetInfoFromExeAndDll(string fileName, FILEINFO_LIST infoSel)
{
	string retStr = "N/A";
	struct LANGANDCODEPAGE
	{
		WORD wLanguage;
		WORD wCodePage;
	};

	char tmpPath[1024] = { 0 };
	GetModuleFileName(NULL, (LPSTR)tmpPath, sizeof(tmpPath));
	string strFullPath(tmpPath);
	int pos = strFullPath.rfind("\\");
	strFullPath.erase(pos + 1);
	strFullPath += fileName;

	DWORD dwSize = 0;
	UINT uiSize = GetFileVersionInfoSize(strFullPath.c_str(), &dwSize);
	if (0 == uiSize)
	{
		return retStr;
	}
	PTSTR pBuffer = new TCHAR[uiSize];
	if (NULL == pBuffer)
	{
		return retStr;
	}
	memset((void*)pBuffer, 0, uiSize);

	// 获取exe或DLL的资源信息，存放在pBuffer内
	if (!GetFileVersionInfo(strFullPath.c_str(), 0, uiSize, (PVOID)pBuffer))
	{
		return retStr;
	}
	LANGANDCODEPAGE* pLanguage = NULL;		// 这里这样设置没关系了。
	UINT  uiOtherSize = 0;
	// 获取资源相关的codepage和language 
	if (!VerQueryValue(pBuffer, "\\VarFileInfo\\Translation", (PVOID*)&pLanguage, &uiOtherSize))
	{
		return retStr;
	}
	// 重点
	char* pTmp = NULL;
	TCHAR SubBlock[1024] = { 0 };
	memset((void*)SubBlock, 0, sizeof(SubBlock));

	string fileInfo = "";
	// 在字符串格式化的\\最后一个字符串的名称就是我们想要获取的属性名称，它们可以是下述内容
	switch (infoSel)
	{
	case CompanyName:
		fileInfo = "CompanyName";
		break;
	case FileDescription:
		fileInfo = "FileDescription";
		break;
	case FileVersion:
		fileInfo = "FileVersion";
		break;
	case InternalName:
		fileInfo = "InternalName";
		break;
	case LegalCopyright:
		fileInfo = "LegalCopyright";
		break;
	case OriginalFilename:
		fileInfo = "OriginalFilename";
		break;
	case ProductName:
		fileInfo = "ProductName";
		break;
	case ProductVersion:
		fileInfo = "ProductVersion";
		break;
	case Comments:
		fileInfo = "Comments";
		break;
	case LegalTrademarks:
		fileInfo = "LegalTrademarks";
		break;
	case PrivateBuild:
		fileInfo = "PrivateBuild";
		break;
	case SpecialBuild:
		fileInfo = "SpecialBuild";
		break;
	default:
		fileInfo = "CompanyName";
		break;
	}
	sprintf_s(SubBlock, 1024, TEXT("\\StringFileInfo\\%04x%04x\\%s"), pLanguage[0].wLanguage, pLanguage[0].wCodePage, fileInfo.c_str());

	UINT uLength = 0;
	int ret = uiOtherSize / sizeof(LANGANDCODEPAGE);
	if (ret > 0)
	{
		if (VerQueryValue(pBuffer, SubBlock, (PVOID*)&pTmp, &uLength))
		{
			if (strlen(pTmp) > 0)
			{
				retStr = pTmp;
			}
		}
	}

	if (NULL != pBuffer)
	{
		delete[] pBuffer;
		pBuffer = NULL;
	}
	return retStr;
}