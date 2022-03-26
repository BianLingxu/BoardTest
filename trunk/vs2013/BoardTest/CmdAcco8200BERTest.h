#ifndef __CMDACCO8200BERTEST_H__
#define __CMDACCO8200BERTEST_H__

#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

class CCmdAcco8200BERTest
{
public:
	CCmdAcco8200BERTest();
	~CCmdAcco8200BERTest();

	uint32_t JudgeCmd(vector<string> vectCmdLine);
	uint32_t CmdParse(vector<string> vectCmdLine);
	uint32_t CmdTimeTest(double Time, vector<string> vectCmdLine, DWORD Addr1, DWORD Addr2);
	uint32_t CmdNumTest(uint32_t Numbers, vector<string> vectCmdLine, DWORD Addr1, DWORD Addr2);
	uint32_t CmdHelpInf(vector<string> vectCmdLine);
};

#endif /* __CMDACCO8200BERTEST_H__ */