#ifndef __2D0349F9_2B7E_4AFD_807E_9F585A2CDC91_CMDACCO8200MODULE_H__
#define __2D0349F9_2B7E_4AFD_807E_9F585A2CDC91_CMDACCO8200MODULE_H__

#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

class CCmdAcco8200Module
{
public:
    CCmdAcco8200Module();
    ~CCmdAcco8200Module();

    uint32_t JudgeCmd(vector<string> vectCmdLine);
	uint32_t CmdHelpInf(vector<string> vectCmdLine);
	uint32_t CmdRegWrite(vector<string> vectCmdLine);
    uint32_t CmdRegRead(vector<string> vectCmdLine);
	uint32_t Cmd8200RegWriteW(vector<string> vectCmdLine);
	uint32_t Cmd8200RegReadW(vector<string> vectCmdLine);
	uint32_t CmdGlobalRegWrite(vector<string> vectCmdLine);
	uint32_t CmdGlobalRegRead(vector<string> vectCmdLine);
	uint32_t Cmd8200GlobalRegWriteW(vector<string> vectCmdLine);
	uint32_t Cmd8200GlobalRegReadW(vector<string> vectCmdLine);
};

#endif /* __2D0349F9_2B7E_4AFD_807E_9F585A2CDC91_CMDACCO8200MODULE_H__ */