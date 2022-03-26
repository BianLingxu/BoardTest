#ifndef __DB8E5120_CEB8_41B5_888B_07CE6B008B19_CMDCFGINI_H__
#define __DB8E5120_CEB8_41B5_888B_07CE6B008B19_CMDCFGINI_H__

#include <string>
#include <map>
#include <vector>
using namespace std;

typedef struct moduleInfo
{
	int typeID;							// 类型ID，如0x8136
	int system;							// 所属系统，如8200
	int maxBoard;						// 最大板数
	int maxChannel;						// 单板最大通道数
	int verify;							// 校验码，如0x89
	int cfgAddressStep;					// 配置步进值，一般为4

}MD_INFO;

class CCmdCfgIni
{
public:
    static CCmdCfgIni *GetInstance(string iniName);
private:
    static string m_strIniPath;            // ini文件路径
    static CCmdCfgIni *m_instance;
protected:
    CCmdCfgIni(void);
    ~CCmdCfgIni(void);
public:
    // Get settings from ini file
    int ReadINI();
public:
	string m_strMsg;						// 提示信息

	vector<string> m_vec8200SystemModule;
	vector<string> m_vec8300SystemModule;

	map<string, MD_INFO> m_mapSysModule;
	map<string, int> m_strValueType;
	map<string, DWORD> m_strValueID;
	vector<ULONG> m_vectRegBitOfst;
	vector<ULONG> m_vectBramRegs;
	vector<ULONG> m_vectDramRegs;
};

#endif /* __DB8E5120_CEB8_41B5_888B_07CE6B008B19_CMDCFGINI_H__ */