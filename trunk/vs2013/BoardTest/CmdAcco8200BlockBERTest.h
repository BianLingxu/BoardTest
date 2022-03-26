#ifndef __CMDACCO8200BLOCKBERTEST_H__
#define __CMDACCO8200BLOCKBERTEST_H__
#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

enum DATAYYPE
{
	DATATYPE_0X00_0XFF = 1,	// 0X00000000和0XFFFFFFFF
	DATATYPE_0X55_0XAA,		// 0X55555555和0XAAAAAAAA
	DATATYPE_PRBS,		    // 伪随机数
	DATATYPE_INC_CLEAR_TO_ZERO,	// 累加数，从0开始，每次测试清0
	DATATYPE_INC_NOT_CLEAR_TO_ZERO,	// 累加数，从0开始，每次测试不清0
	DATATYPE_INC_CLEAR_TO_VALUE,	// 累加数，指定初始值，每次测试回到初始值
	DATATYPE_INC_NOT_CLEAR_TO_VALUE	    // 累加数，指定初始值，每次测试不回到初始值
};

class CCmdAcco8200BlockBERTest
{
public:
	CCmdAcco8200BlockBERTest();
	~CCmdAcco8200BlockBERTest();

	uint32_t CmdBlockParse(vector<string> vectCmdLine);
	uint32_t CmdBlockTimeTest(double Time, DWORD Addr1, DWORD startAddr, DWORD Addr2, uint32_t Length, DWORD Addr3, uint32_t Mode, DWORD AccStartAddr, DWORD chAddr);
	uint32_t CmdBlockNumTest(uint32_t Numbers, DWORD Addr1, DWORD startAddr, DWORD Addr2, uint32_t Length, DWORD Addr3, uint32_t Mode, DWORD AccStartAddr, DWORD chAddr);
	void ModeJudge(uint32_t Mode, uint32_t Length, DWORD AccStartAddr);
private:
	vector<DWORD>m_regData;
	vector<DWORD>m_getData;
	uint32_t m_totalCnt = 0;
};

#endif /* __CMDACCO8200BLOCKBERTEST_H__ */