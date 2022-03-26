// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the UTILLIBRARY_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// UTILLIBRARY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef UTILLIBRARY_EXPORTS
#define UTILLIBRARY_API __declspec(dllexport)
#else
#define UTILLIBRARY_API __declspec(dllimport)
#endif

#include <stdint.h>
#include <string>
#include <vector>
using namespace std;

/********************************************************************************
error number define
********************************************************************************/
#define ERR_UTIL_FUNC_PARM_NULL    (0x0001)
#define ERR_UTIL_OPEN_LOG_FILE     (0x0002)

#define ERR_UTIL_FILE_OPEN    (0x00001003)
#define ERR_UTIL_FILE_CREATE  (0x00001004)
#define ERR_UTIL_FILE_COMP    (0x00001005)
#define ERR_UTIL_MALLOC_MEM   (0x00001006)
#define ERR_UTIL_ENV_NEXIST   (0x00001007)

/********************************************************************************
data type define
********************************************************************************/
typedef enum {
	DATA_PATTERN_00,
	DATA_PATTERN_55,
	DATA_PATTERN_AA,
	DATA_PATTERN_FF,
	DATA_PATTERN_5A,
	DATA_PATTERN_A5,
	DATA_PATTERN_INC,
	DATA_PATTERN_DEC,
	DATA_PATTERN_RAND,
}DATA_PATTERN_SETS;

/********************************************************************************
log level define
********************************************************************************/
typedef enum {
	LOG_LEVEL_ERROR = 0x01,
	LOG_LEVEL_WARN = 0x02,
	LOG_LEVEL_INFO = 0x04,
	LOG_LEVEL_DEBUG = 0x08,
}LOG_LEVEL_SETS;

/********************************************************************************
file info define
********************************************************************************/
enum FILEINFO_LIST
{
	CompanyName,
	FileDescription,
	FileVersion,
	InternalName,
	LegalCopyright,
	OriginalFilename,
	ProductName,
	ProductVersion,
	Comments,
	LegalTrademarks,
	PrivateBuild,
	SpecialBuild,
};

/********************************************************************************
log function declaration
********************************************************************************/
UTILLIBRARY_API uint32_t OpenLogFile(string strFile);
UTILLIBRARY_API uint32_t CloseLogFile();
UTILLIBRARY_API uint32_t SetLogLevel(uint32_t logLevel);
UTILLIBRARY_API uint32_t PrintLog(uint32_t logLevel, const char *fmt, ...);

/********************************************************************************
string function declaration
********************************************************************************/
UTILLIBRARY_API uint32_t ToUpperCase(string &strInOut);
UTILLIBRARY_API uint32_t ToLowerCase(string &strInOut);
UTILLIBRARY_API uint32_t ToUpperCase(char *pStrInOut);
UTILLIBRARY_API uint32_t ToLowerCase(char *pStrInOut);
UTILLIBRARY_API uint32_t SplitString(vector<string> &vectStr, string strOrg, string strDelim);

/********************************************************************************
file function declaration
********************************************************************************/
UTILLIBRARY_API uint32_t SetEnvVariable(string strName, string strValue);
UTILLIBRARY_API uint32_t SetEnvVariable(string strName, uint32_t value);
UTILLIBRARY_API uint32_t GetEnvVariable(string strName, string &strValue);
UTILLIBRARY_API uint32_t GetEnvVariable(string strName, uint32_t &value);
UTILLIBRARY_API uint32_t ClrEnvVariable(string strName);
UTILLIBRARY_API uint32_t FillDataPattern(uint8_t *pBuff, uint32_t fSize, DATA_PATTERN_SETS dataType, uint8_t dataStep);
UTILLIBRARY_API uint32_t WriteTextFile(string strFile, uint32_t *pBuff, uint32_t bitWidth, uint32_t ramDepth);
UTILLIBRARY_API uint32_t ReadTextFile(string strFile, uint32_t *pBuff, uint32_t bitWidth, uint32_t ramDepth, uint32_t startLine);

/********************************************************************************
time function declaration
********************************************************************************/
UTILLIBRARY_API uint32_t TickStart();
UTILLIBRARY_API uint32_t TickEnd(uint32_t &tickTime);
UTILLIBRARY_API uint32_t DelayUs(double dbTimeUs);
UTILLIBRARY_API uint32_t DelayMs(double dbTimeMs);

/********************************************************************************
Debug function declaration
********************************************************************************/
UTILLIBRARY_API uint32_t DebugPrint(vector<string> vectCmdLine);
UTILLIBRARY_API uint32_t SetDebugStatus(bool flag);
UTILLIBRARY_API uint32_t DebugPause();

UTILLIBRARY_API string GetInfoFromExeAndDll(string fileName, FILEINFO_LIST infoSel);