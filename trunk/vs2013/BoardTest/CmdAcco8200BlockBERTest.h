#ifndef __CMDACCO8200BLOCKBERTEST_H__
#define __CMDACCO8200BLOCKBERTEST_H__
#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

enum DATAYYPE
{
	DATATYPE_0X00_0XFF = 1,	// 0X00000000��0XFFFFFFFF
	DATATYPE_0X55_0XAA,		// 0X55555555��0XAAAAAAAA
	DATATYPE_PRBS,		    // α�����
	DATATYPE_INC_CLEAR_TO_ZERO,	// �ۼ�������0��ʼ��ÿ�β�����0
	DATATYPE_INC_NOT_CLEAR_TO_ZERO,	// �ۼ�������0��ʼ��ÿ�β��Բ���0
	DATATYPE_INC_CLEAR_TO_VALUE,	// �ۼ�����ָ����ʼֵ��ÿ�β��Իص���ʼֵ
	DATATYPE_INC_NOT_CLEAR_TO_VALUE	    // �ۼ�����ָ����ʼֵ��ÿ�β��Բ��ص���ʼֵ
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