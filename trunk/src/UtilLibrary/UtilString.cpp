// UtilLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "UtilLibrary.h"

#pragma warning(disable : 4996)

UTILLIBRARY_API uint32_t ToUpperCase(string &strInOut)
{
    uint32_t index;

    for (index = 0; index<strInOut.size(); index++) {
        if ((strInOut[index] >= 'a') && (strInOut[index] <= 'z')) {
            strInOut[index] -= 0x20;
        }
    }

    return 0;
}

UTILLIBRARY_API uint32_t ToLowerCase(string &strInOut)
{
    uint32_t index;

    for (index = 0; index<strInOut.size(); index++) {
        if ((strInOut[index] >= 'A') && (strInOut[index] <= 'Z')) {
            strInOut[index] += 0x20;
        }
    }

    return 0;
}

UTILLIBRARY_API uint32_t ToUpperCase(char *pStrInOut)
{
    uint32_t index;

    if (NULL == pStrInOut) {
        return ERR_UTIL_FUNC_PARM_NULL;
    }

    for (index = 0; index<strlen(pStrInOut); index++) {
        if ((pStrInOut[index] >= 'a') && (pStrInOut[index] <= 'z')) {
            pStrInOut[index] -= 0x20;
        }
    }

    return 0;
}

UTILLIBRARY_API uint32_t ToLowerCase(char *pStrInOut)
{
    uint32_t index;

    if (NULL == pStrInOut) {
        return ERR_UTIL_FUNC_PARM_NULL;
    }

    for (index = 0; index<strlen(pStrInOut); index++) {
        if ((pStrInOut[index] >= 'A') && (pStrInOut[index] <= 'Z')) {
            pStrInOut[index] += 0x20;
        }
    }

    return 0;
}

UTILLIBRARY_API uint32_t SplitString(vector<string> &vectStr, string strOrg, string strDelim)
{
    char strTemp[1024], *pStr;

    vectStr.clear();
    if ((0 == strOrg.size()) || (0 == strDelim.size())) {
        vectStr.push_back(strOrg);
        return 0;
    }

	// Ìæ»»tabÎª¿Õ¸ñ
	int pos = strOrg.find("\t");
	while (pos != -1){
		strOrg.replace(pos, string("\t").length(), " ");
		pos = strOrg.find("\t");
	}

    strcpy(strTemp, strOrg.c_str());
    pStr = strtok(strTemp, strDelim.c_str());
    while (NULL != pStr) {
        vectStr.push_back(string(pStr));
        pStr = strtok(NULL, strDelim.c_str());
    }

    return 0;
}
