#ifndef __CMDACCO8300FLASHBERTEST_H__
#define __CMDACCO8300FLASHBERTEST_H__
#include <stdint.h>
#include <string>
#include <vector>
#include "AT_CBB.h"

using namespace std;

class CCmdAcco8300FlashBERTest
{
public:
	CCmdAcco8300FlashBERTest();
	~CCmdAcco8300FlashBERTest();

	uint32_t CmdFlashParse(vector<string> vectCmdLine);
	uint32_t CmdFlashTimeTest(double Time, DWORD startAddr, DWORD chAddr, uint32_t Length, string File, AT_SYSTEM_TYPE systemType);
	uint32_t CmdFlashNumTest(uint32_t Numbers, DWORD startAddr, DWORD chAddr, uint32_t Length, string File, AT_SYSTEM_TYPE systemType);
};
#endif /* __CMDACCO8300FLASHBERTEST_H__ */