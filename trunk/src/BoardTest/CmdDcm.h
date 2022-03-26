#ifndef __F69B3228_B1CA_43A8_BECC_B50886630984_CMDDCM_H__
#define __F69B3228_B1CA_43A8_BECC_B50886630984_CMDDCM_H__

#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

class CCmdDcm
{
public:
    CCmdDcm();
    ~CCmdDcm();

    uint32_t JudgeCmd(vector<string> vectCmdLine);
    uint32_t CmdDcmDummy(vector<string> vectCmdLine);
    uint32_t CmdDcmReset(vector<string> vectCmdLine);
    uint32_t CmdDcmStart(vector<string> vectCmdLine);
    uint32_t CmdRegWrite(vector<string> vectCmdLine);
    uint32_t CmdRegBatch(vector<string> vectCmdLine);
    uint32_t CmdRegRead(vector<string> vectCmdLine);
    uint32_t CmdBramWrite(vector<string> vectCmdLine);
    uint32_t CmdBramRead(vector<string> vectCmdLine);
    uint32_t CmdDramWrite(vector<string> vectCmdLine);
    uint32_t CmdDramRead(vector<string> vectCmdLine);
    uint32_t CmdRamFile(vector<string> vectCmdLine);
    uint32_t CmdATE305Write(vector<string> vectCmdLine);
    uint32_t CmdATE305Read(vector<string> vectCmdLine);
    uint32_t CmdATE305Batch(vector<string> vectCmdLine);
    uint32_t CmdAD7606Samp(vector<string> vectCmdLine);
	uint32_t CmdRelayCtrl(vector<string> vectCmdLine);
    uint32_t CmdHelpInf(vector<string> vectCmdLine);

private:
	uint32_t ReadFromFile(string strFile, uint8_t *pBuff, uint32_t maxSize, uint32_t startLine);
    uint32_t WriteToFile(string strFile, uint8_t *pBuff, uint32_t maxSize);
};

#endif /* __F69B3228_B1CA_43A8_BECC_B50886630984_CMDDCM_H__ */