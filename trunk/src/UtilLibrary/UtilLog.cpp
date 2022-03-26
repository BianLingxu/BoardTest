// UtilLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "UtilLibrary.h"

#pragma warning(disable : 4996)

static FILE *gLogFile = NULL;
static uint32_t gLogLevel = LOG_LEVEL_ERROR | LOG_LEVEL_WARN | LOG_LEVEL_INFO;

UTILLIBRARY_API uint32_t OpenLogFile(string strFile)
{
    if (NULL != gLogFile) {
        CloseLogFile();
    }

    gLogFile = fopen(strFile.c_str(), "a+");
    if (NULL == gLogFile) {
        return ERR_UTIL_OPEN_LOG_FILE;
    }

    return 0;
}

UTILLIBRARY_API uint32_t CloseLogFile()
{
    if (NULL != gLogFile) {
        fclose(gLogFile);
        gLogFile = NULL;
    }

    return 0;
}

UTILLIBRARY_API uint32_t SetLogLevel(uint32_t logLevel)
{
    gLogLevel = logLevel & 0x0F;

    return 0;
}

UTILLIBRARY_API uint32_t PrintLog(uint32_t logLevel, const char *fmt, ...)
{
    char strText[1024];
    va_list ap;

    if (0 == (gLogLevel & logLevel)) {
        return 0;
    }

    if (LOG_LEVEL_ERROR & logLevel) {
        strcpy(strText, "ERR:");
    }
    else if (LOG_LEVEL_WARN & logLevel) {
        strcpy(strText, "WRN:");
    }
	else if (LOG_LEVEL_DEBUG & logLevel) {
		strcpy(strText, "DBG:");
	}
    else {
        strcpy(strText, "");
    }

    va_start(ap, fmt);
    vsprintf(strText + strlen(strText), fmt, ap);
    if (NULL != gLogFile) {
        fprintf(gLogFile, strText);
        fflush(gLogFile);
    }
    fprintf(stdout, strText);
    fflush(stdout);
    va_end(ap);

    return 0;
}
