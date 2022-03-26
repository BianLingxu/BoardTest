#ifndef __DB8E5120_CEB8_41B5_888B_07CE6B008B19_CMDCFGINI_H__
#define __DB8E5120_CEB8_41B5_888B_07CE6B008B19_CMDCFGINI_H__

#include <string>
#include <map>
#include <vector>
using namespace std;

typedef struct moduleInfo
{
	int typeID;							// ����ID����0x8136
	int system;							// ����ϵͳ����8200
	int maxBoard;						// ������
	int maxChannel;						// �������ͨ����
	int verify;							// У���룬��0x89
	int cfgAddressStep;					// ���ò���ֵ��һ��Ϊ4

}MD_INFO;

class CCmdCfgIni
{
public:
    static CCmdCfgIni *GetInstance(string iniName);
private:
    static string m_strIniPath;            // ini�ļ�·��
    static CCmdCfgIni *m_instance;
protected:
    CCmdCfgIni(void);
    ~CCmdCfgIni(void);
public:
    // Get settings from ini file
    int ReadINI();
public:
	string m_strMsg;						// ��ʾ��Ϣ

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