#ifndef __4B322E56_015E_42AB_B340_B085A8394F92_CMDACCO8300MODULE_H__
#define __4B322E56_015E_42AB_B340_B085A8394F92_CMDACCO8300MODULE_H__

#include <stdint.h>
#include <string>
#include <vector>
#include <map>

using namespace std;

/*!
* @brief	flash����
*/
typedef struct FlashProperty
{
	int flashType;							// flash�ͺŵ�ö��ֵ��1ΪGD25Q128E,
	int spiMode;								// flash SPIģʽ��1ΪX1��2ΪX4
	unsigned char	flashFuncAddr;            // flash�ĵ�һ�����ܵ�ַ
	string flashTypeString;                  //flash�ͺŵ��ַ���
	FlashProperty()
	{
		flashType = -1;
		spiMode = -1;
		flashFuncAddr = 0XFF;
		flashTypeString = "";
	}
}FLASH_PROPERTY;

class CCmdAcco8300Module
{
public:
    CCmdAcco8300Module();
    ~CCmdAcco8300Module();
	
	uint32_t JudgeCmd(vector<string> vectCmdLine);
	uint32_t CmdOfstSet(vector<string> vectCmdLine);
	uint32_t CmdRegWrite(vector<string> vectCmdLine);
	uint32_t CmdRegRead(vector<string> vectCmdLine);
	uint32_t CmdRamSet(vector<string> vectCmdLine);
	uint32_t CmdBramWrite(vector<string> vectCmdLine);
	uint32_t CmdBramRead(vector<string> vectCmdLine);
	uint32_t CmdDramWrite(vector<string> vectCmdLine);
	uint32_t CmdDramRead(vector<string> vectCmdLine);
	uint32_t CmdDcmTemp(vector<string> vectCmdLine);
	uint32_t CmdGlobalRegWrite(vector<string> vectCmdLine);
	uint32_t CmdGlobalRegRead(vector<string> vectCmdLine);
	uint32_t CmdFlashConfig(vector<string> vectCmdLine);
	uint32_t CmdFlashGetID(vector<string> vectCmdLine);
	uint32_t CmdFlashErase(vector<string> vectCmdLine);
	uint32_t CmdFlashWrite(vector<string> vectCmdLine);
	uint32_t CmdFlashRead(vector<string> vectCmdLine);
	uint32_t CmdHelpInf(vector<string> vectCmdLine);
	DWORD GetAddress(string mdtype, DWORD chAddr, DWORD funcAddr);
	//�����û������ַ������������͵Ķ�Ӧ��ϵ
	map<string, int> m_dataType;
	double CalcAd7606Voltage(uint16_t adcValue);
};
uint32_t CmdExceptionProcess(vector<string> vectCmdLine, int CommandSize, DWORD Flag);
#endif /* __4B322E56_015E_42AB_B340_B085A8394F92_CMDACCO8300MODULE_H__ */