#ifndef __51A1DC23_9431_4CF6_B8C1_5C7AA5031369_CMDUTIL_H__
#define __51A1DC23_9431_4CF6_B8C1_5C7AA5031369_CMDUTIL_H__

#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

class CCmdUtil
{
public:
    CCmdUtil();
    ~CCmdUtil();

    uint32_t JudgeCmd(vector<string> vectCmdLine);
	uint32_t CmdRams(vector<string> vectCmdLine);
    uint32_t CmdEcho(vector<string> vectCmdLine);
    uint32_t CmdSleep(vector<string> vectCmdLine);
    uint32_t CmdLog(vector<string> vectCmdLine);
    uint32_t CmdTime(vector<string> vectCmdLine);
    uint32_t CmdHelpInf(vector<string> vectCmdLine);
	uint32_t CmdVers(vector<string> vectCmdLine);
	uint32_t CmdFileDebug(vector<string> vectCmdLine);
	uint32_t BlockCmd(vector<string> vectCmdLine);
};

#endif /* __51A1DC23_9431_4CF6_B8C1_5C7AA5031369_CMDUTIL_H__ */