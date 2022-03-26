#ifndef __DAF8CC17_525E_4803_B424_45FC54FFC007_CMDPROCESS_H__
#define __DAF8CC17_525E_4803_B424_45FC54FFC007_CMDPROCESS_H__

#include <stdint.h>
#include <string>
#include <vector>
using namespace std;

#define CMD_STATE_NON    (0)
#define CMD_STATE_LOOP   (1)

class CCmdCfgIni;
class CCmdProcess
{
public:
    CCmdProcess();
    ~CCmdProcess();

    uint32_t CmdExecute();
    uint32_t JudgeCmd(vector<string> vectCmdLine);
    uint32_t CmdShell(vector<string> vectCmdLine);
    uint32_t CmdHelpInf(vector<string> vectCmdLine);
	uint32_t CmdClearWindow();
	uint32_t CmdConfig();
	uint32_t CmdSystemCfg();
	uint32_t Cmd8200SystemCfg(CCmdCfgIni* pCfg, WORD& startAddr);
	uint32_t Cmd8136SystemCfg(CCmdCfgIni* pCfg, WORD& startAddr);
	uint32_t Cmd8300SystemCfg(CCmdCfgIni* pCfg);

	void CmdControl8136Ext(int bs8136, WORD addr8136, WORD& startAddr);
	void Cmd8136CableCheck(WORD chAddr, int& cableRes, int& channelStatus);
	void Cmd8136ExtInfo(WORD chAddr, WORD& mdType, BYTE& mdBs, BYTE& mdChCnt, BYTE& mdInnerIndex);

	DWORD CmdBind(int slotID);

private:
    uint32_t m_runFlag;
    uint32_t m_cmdStatePre;
    uint32_t m_cmdStateCur;

    vector<string> m_VectLoop;
    uint32_t m_LoopCnt;
    uint32_t m_PrintLoop;
    uint32_t m_BreakFlag;

    uint32_t ExecuteLine(char *strLine);
};

#endif /* __DAF8CC17_525E_4803_B424_45FC54FFC007_CMDPROCESS_H__ */