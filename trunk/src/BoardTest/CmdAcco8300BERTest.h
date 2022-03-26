#ifndef __CMDACCO8300BERTEST_H__
#define __CMDACCO8300BERTEST_H__

#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

class CCmdAcco8300BERTest
{
public:
	CCmdAcco8300BERTest();
	~CCmdAcco8300BERTest();

	uint32_t JudgeCmd(vector<string> vectCmdLine);
	uint32_t CmdParse(vector<string> vectCmdLine);
	uint32_t CmdTimeTest(double Time, vector<string> vectCmdLine, DWORD Addr1, DWORD Addr2);
	uint32_t CmdNumTest(uint32_t Numbers, vector<string> vectCmdLine, DWORD Addr1, DWORD Addr2);
	uint32_t CmdHelpInf(vector<string> vectCmdLine);
	DWORD GetAddress(string mdtype, DWORD chAddr, DWORD funcAddr);
};
uint32_t CmdDataCompare(uint32_t &TotalCnt, uint32_t &ErrCnt, DWORD getData1, DWORD reg1Data, DWORD getData2, DWORD reg2Data);
uint32_t CmdDataCompare(uint32_t &TotalCnt, uint32_t &ErrCnt, uint32_t BlockCnt, DWORD startAddr, uint32_t Length, DWORD* pSetBuff, DWORD*pGetBuff);
uint32_t CmdDataCompare(uint32_t &TotalCnt, uint32_t &ErrCnt, uint32_t BlockCnt, DWORD startAddr, uint32_t Length, uint8_t* pSetBuff, uint8_t* pGetBuff);
uint32_t CmdPrint(uint32_t TotalCnt, uint32_t ErrCnt, SYSTEMTIME sysTimeStart, SYSTEMTIME sysTimeEnd);
#endif /* __CMDACCO8300BERTEST_H__ */