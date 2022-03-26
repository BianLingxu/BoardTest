#ifndef __4B322E56_015E_42AB_B340_B085A8394F92_CMDACCO8300MODULE_H__
#define __4B322E56_015E_42AB_B340_B085A8394F92_CMDACCO8300MODULE_H__

#include <stdint.h>
#include <string>
#include <vector>
#include <map>

using namespace std;

/*!
* @brief	flash属性
*/
typedef struct FlashProperty
{
	int flashType;							// flash型号的枚举值：1为GD25Q128E,
	int spiMode;								// flash SPI模式：1为X1，2为X4
	unsigned char	flashFuncAddr;            // flash的第一个功能地址
	string flashTypeString;                  //flash型号的字符串
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
	//保存用户输入字符串和数据类型的对应关系
	map<string, int> m_dataType;
	double CalcAd7606Voltage(uint16_t adcValue);
};
uint32_t CmdExceptionProcess(vector<string> vectCmdLine, int CommandSize, DWORD Flag);
#endif /* __4B322E56_015E_42AB_B340_B085A8394F92_CMDACCO8300MODULE_H__ */