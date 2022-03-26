#include "stdafx.h"
#include "CmdDcm.h"
#include "UtilLibrary.h"
#include "DcmLibrary.h"
#include <time.h>

#pragma warning(disable : 4996)

//typedef enum {
//    DATA_PATTERN_00,
//    DATA_PATTERN_55,
//    DATA_PATTERN_AA,
//    DATA_PATTERN_FF,
//    DATA_PATTERN_5A,
//    DATA_PATTERN_A5,
//    DATA_PATTERN_INC,
//    DATA_PATTERN_DEC,
//    DATA_PATTERN_RAND,
//}DATA_PATTERN_SETS;

CCmdDcm::CCmdDcm()
{
}

CCmdDcm::~CCmdDcm()
{
}

uint32_t CCmdDcm::JudgeCmd(vector<string> vectCmdLine)
{
    uint32_t retVal;

    do {
        if (vectCmdLine.size() < 2) {
            PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM command\n");
            retVal = 1;
            break;
        }

        ToUpperCase(vectCmdLine[1]);
        if ("HELP" == vectCmdLine[1]) {
            retVal = CmdHelpInf(vectCmdLine);
        }
        else if ("DUMMY" == vectCmdLine[1]) {
            retVal = CmdDcmDummy(vectCmdLine);
        }
        else if ("RESET" == vectCmdLine[1]) {
            retVal = CmdDcmReset(vectCmdLine);
        }
        else if ("START" == vectCmdLine[1]) {
            retVal = CmdDcmStart(vectCmdLine);
        }
        else if ("REG_WR" == vectCmdLine[1]) {
            retVal = CmdRegWrite(vectCmdLine);
        }
        else if ("REG_RD" == vectCmdLine[1]) {
            retVal = CmdRegRead(vectCmdLine);
        }
        else if ("REG_BATCH" == vectCmdLine[1]) {
            retVal = CmdRegBatch(vectCmdLine);
        }
        else if ("BRAM_WR" == vectCmdLine[1]) {
            retVal = CmdBramWrite(vectCmdLine);
        }
        else if ("BRAM_RD" == vectCmdLine[1]) {
            retVal = CmdBramRead(vectCmdLine);
        }
        else if ("DRAM_WR" == vectCmdLine[1]) {
            retVal = CmdDramWrite(vectCmdLine);
        }
        else if ("DRAM_RD" == vectCmdLine[1]) {
            retVal = CmdDramRead(vectCmdLine);
        }
        else if ("RAM_FILE" == vectCmdLine[1]) {
            retVal = CmdRamFile(vectCmdLine);
        }
        else if ("ATE305_WR" == vectCmdLine[1]) {
            retVal = CmdATE305Write(vectCmdLine);
        }
        else if ("ATE305_RD" == vectCmdLine[1]) {
            retVal = CmdATE305Read(vectCmdLine);
        }
        else if ("ATE305_BATCH" == vectCmdLine[1]) {
            retVal = CmdATE305Batch(vectCmdLine);
        }
        else if ("AD7606" == vectCmdLine[1]) {
            retVal = CmdAD7606Samp(vectCmdLine);
        }
		else if ("RELAY" == vectCmdLine[1]) {
			retVal = CmdRelayCtrl(vectCmdLine);
		}
        else {
            PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM command\n");
            retVal = 1;
        }
    } while (0);

    return retVal;
}

uint32_t CCmdDcm::CmdDcmDummy(vector<string> vectCmdLine)
{
    uint32_t slotAddr, fpgaAddr, timeVal;
    uint32_t regBurst[16], regValue;
    uint32_t index, retVal;
    double dbResult[512];

    if (vectCmdLine.size() < 4) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM DUMMY command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[2].c_str(), "%u", &slotAddr);
    sscanf_s(vectCmdLine[3].c_str(), "%u", &fpgaAddr);

    // DCM register write
    PrintLog(LOG_LEVEL_INFO, "DCM register write speed ... ");
    TickStart();
    for (index = 0; index < 10000; index++) {
        retVal = DcmWriteReg(slotAddr, fpgaAddr, 0x0840, index);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_INFO, "NG\n");
            return 1;
        }
    }
    TickEnd(timeVal);
    PrintLog(LOG_LEVEL_INFO, "%.6f us\n", timeVal / 10000.0);

    // DCM register read
    PrintLog(LOG_LEVEL_INFO, "DCM register read speed ... ");
    TickStart();
    for (index = 0; index < 10000; index++) {
        retVal = DcmReadReg(slotAddr, fpgaAddr, 0x0840, &regValue);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_INFO, "NG\n");
            return 1;
        }
    }
    TickEnd(timeVal);
    PrintLog(LOG_LEVEL_INFO, "%.6f us\n", timeVal / 10000.0);

    // ATE305 single register write
    PrintLog(LOG_LEVEL_INFO, "ATE305 single register write speed ... ");
    TickStart();
    for (index = 0; index < 10000; index++) {
        retVal = ATE305WriteReg(slotAddr, fpgaAddr, 0x0001, 0x06, index);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_INFO, "NG\n");
            return 1;
        }
    }
    TickEnd(timeVal);
    PrintLog(LOG_LEVEL_INFO, "%.6f us\n", timeVal / 10000.0);

    // ATE305 single register read
    PrintLog(LOG_LEVEL_INFO, "ATE305 single register read speed ... ");
    TickStart();
    for (index = 0; index < 10000; index++) {
        retVal = ATE305ReadReg(slotAddr, fpgaAddr, 0x0001, 0x06, &regValue);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_INFO, "NG\n");
            return 1;
        }
    }
    TickEnd(timeVal);
    PrintLog(LOG_LEVEL_INFO, "%.6f us\n", timeVal / 10000.0);

    // ATE305 multiple register write
    PrintLog(LOG_LEVEL_INFO, "ATE305 multiple register write speed ... ");
    TickStart();
    for (index = 0; index < 1000; index++) {
        retVal = ATE305MultWriteReg(slotAddr, fpgaAddr, 0xFFFF, 0x06, regBurst, 0);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_INFO, "NG\n");
            return 1;
        }
    }
    TickEnd(timeVal);
    PrintLog(LOG_LEVEL_INFO, "%.6f us\n", timeVal / 16000.0);

    // ATE305 multiple register read
    PrintLog(LOG_LEVEL_INFO, "ATE305 multiple register read speed ... ");
    TickStart();
    for (index = 0; index < 1000; index++) {
        retVal = ATE305MultReadReg(slotAddr, fpgaAddr, 0xFFFF, 0x06, regBurst);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_INFO, "NG\n");
            return 1;
        }
    }
    TickEnd(timeVal);
    PrintLog(LOG_LEVEL_INFO, "%.6f us\n", timeVal / 16000.0);

    // ATE305 burst register write
    PrintLog(LOG_LEVEL_INFO, "ATE305 burst register write speed ... ");
    TickStart();
    for (index = 0; index < 1000; index++) {
        retVal = ATE305MultWriteReg(slotAddr, fpgaAddr, 0xFFFF, 0x06, regBurst, 1);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_INFO, "NG\n");
            return 1;
        }
    }
    TickEnd(timeVal);
    PrintLog(LOG_LEVEL_INFO, "%.6f us\n", timeVal / 16000.0);

    // ATE305 burst register read
    PrintLog(LOG_LEVEL_INFO, "ATE305 burst register read speed ... ");
    TickStart();
    for (index = 0; index < 1000; index++) {
        retVal = ATE305MultReadReg(slotAddr, fpgaAddr, 0xFFFF, 0x06, regBurst);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_INFO, "NG\n");
            return 1;
        }
    }
    TickEnd(timeVal);
    PrintLog(LOG_LEVEL_INFO, "%.6f us\n", timeVal / 16000.0);

    // AD7606 single sample
    PrintLog(LOG_LEVEL_INFO, "AD7606 single sample speed ... ");
    TickStart();
    for (index = 0; index < 1000; index++) {
        retVal = AD7606Start(slotAddr, fpgaAddr, AD7606_MODE_SINGLE, 0);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_INFO, "NG\n");
            return 1;
        }

        retVal = AD7606Read(slotAddr, fpgaAddr, AD7606_MODE_SINGLE, 0, dbResult, NULL);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_INFO, "NG\n");
            return 1;
        }
    }
    TickEnd(timeVal);
    PrintLog(LOG_LEVEL_INFO, "%.6f us\n", timeVal / 1000.0);

    // AD7606 multiple sample
    PrintLog(LOG_LEVEL_INFO, "AD7606 multiple sample speed ... ");
    TickStart();
    for (index = 0; index < 1000; index++) {
        uint32_t sampDepth;

        retVal = AD7606Start(slotAddr, fpgaAddr, AD7606_MODE_MULTIPLE, 256);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_INFO, "NG\n");
            return 1;
        }

        sampDepth = 256;
        retVal = AD7606Read(slotAddr, fpgaAddr, AD7606_MODE_MULTIPLE, 0, dbResult, &sampDepth);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_INFO, "NG\n");
            return 1;
        }
    }
    TickEnd(timeVal);
    PrintLog(LOG_LEVEL_INFO, "%.6f us\n", timeVal / 1000.0);

    return 0;
}

uint32_t CCmdDcm::CmdDcmReset(vector<string> vectCmdLine)
{
    uint32_t slotAddr, fpgaAddr, retVal;

    if (vectCmdLine.size() < 4) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM RESET command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[2].c_str(), "%u", &slotAddr);
    sscanf_s(vectCmdLine[3].c_str(), "%u", &fpgaAddr);

    retVal = DcmWriteReg(slotAddr, fpgaAddr, 0x000D, 0x0001);
    if (0 != retVal) {
        PrintLog(LOG_LEVEL_ERROR, "DCM reset fail\n");
        return 1;
    }

    return 0;
}

uint32_t CCmdDcm::CmdDcmStart(vector<string> vectCmdLine)
{
    uint32_t slotAddr, fpgaAddr, retVal;

    if (vectCmdLine.size() < 3) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM START command\n");
        return 1;
    }

    ToUpperCase(vectCmdLine[2]);
    if ("SOFT" == vectCmdLine[2]) {
        if (vectCmdLine.size() < 5) {
            PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM START SOFT command\n");
            return 1;
        }

        sscanf_s(vectCmdLine[3].c_str(), "%u", &slotAddr);
        sscanf_s(vectCmdLine[4].c_str(), "%u", &fpgaAddr);

        retVal = DcmWriteReg(slotAddr, fpgaAddr, 0x000E, 0x0001);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "DCM soft start fail\n");
            return 1;
        }
    }
    else if ("TOTAL" == vectCmdLine[2]) {
        DcmTotalStart();
    }
    else {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM START command\n");
        return 1;
    }

    return 0;
}

uint32_t CCmdDcm::CmdRegWrite(vector<string> vectCmdLine)
{
    uint32_t slotAddr, fpgaAddr, regAddr, regValue, retVal;

    if (vectCmdLine.size() < 6) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM REG_WR command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[2].c_str(), "%u", &slotAddr);
    sscanf_s(vectCmdLine[3].c_str(), "%u", &fpgaAddr);
    sscanf_s(vectCmdLine[4].c_str(), "%X", &regAddr);
    sscanf_s(vectCmdLine[5].c_str(), "%X", &regValue);

    retVal = DcmWriteReg(slotAddr, fpgaAddr, regAddr, regValue);
    if (0 != retVal) {
        PrintLog(LOG_LEVEL_ERROR, "DCM register write fail\n");
        return 1;
    }

    return 0;
}

uint32_t CCmdDcm::CmdRegRead(vector<string> vectCmdLine)
{
	uint32_t slotAddr, fpgaAddr, regAddr, regValue, expValue, maskValue, retVal;

    if (vectCmdLine.size() < 5) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM REG_RD command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[2].c_str(), "%u", &slotAddr);
    sscanf_s(vectCmdLine[3].c_str(), "%u", &fpgaAddr);
    sscanf_s(vectCmdLine[4].c_str(), "%X", &regAddr);

    retVal = DcmReadReg(slotAddr, fpgaAddr, regAddr, &regValue);
    if (0 != retVal) {
        PrintLog(LOG_LEVEL_ERROR, "DCM register read fail\n");
        return 1;
    }

    if (vectCmdLine.size() > 5) {
        sscanf_s(vectCmdLine[5].c_str(), "%X", &expValue);

		if (vectCmdLine.size() > 6) {
			sscanf_s(vectCmdLine[6].c_str(), "%X", &maskValue);
		}
		else {
			maskValue = 0x00000000;
		}

		regValue &= ~maskValue;
		expValue &= ~maskValue;
        if (regValue != expValue) {
            PrintLog(LOG_LEVEL_ERROR, "Read Register Slot[%d] Fpga[%d] Addr[%04X] Value[%08X] Exp[%08X]\n", slotAddr, fpgaAddr, regAddr, regValue, expValue);
            return 1;
        }
    }
    else {
        PrintLog(LOG_LEVEL_INFO, "Read Register Slot[%d] Fpga[%d] Addr[%04X] Value[%08X]\n", slotAddr, fpgaAddr, regAddr, regValue);
    }

    return 0;
}

uint32_t CCmdDcm::CmdRegBatch(vector<string> vectCmdLine)
{
    uint32_t regAddr[8], regValue[8], regRead;
    uint32_t slotAddr, fpgaAddr, runTimes;
    uint32_t idx0, idx1, retVal;

    if (vectCmdLine.size() < 5) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM REG_BATCH command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[2].c_str(), "%u", &slotAddr);
    sscanf_s(vectCmdLine[3].c_str(), "%u", &fpgaAddr);
    sscanf_s(vectCmdLine[4].c_str(), "%u", &runTimes);

    regAddr[0] = 0x0840;
    regAddr[1] = 0x0841;
    regAddr[2] = 0x0842;
    regAddr[3] = 0x0843;
    regAddr[4] = 0x0844;
    regAddr[5] = 0x0845;
    regAddr[6] = 0x0846;
    regAddr[7] = 0x0847;

    srand((uint32_t)(time(NULL)));
    for (idx0 = 0; idx0 < runTimes; idx0++) {
        for (idx1 = 0; idx1 < 8; idx1++) {
            regValue[idx1] = rand();
        }

        for (idx1 = 0; idx1 < 8; idx1++) {
            retVal = DcmWriteReg(slotAddr, fpgaAddr, regAddr[idx1], regValue[idx1]);
            if (0 != retVal) {
                PrintLog(LOG_LEVEL_ERROR, "DCM register write fail\n");
                return 1;
            }
        }
        
        for (idx1 = 0; idx1 < 8; idx1++) {
            retVal = DcmReadReg(slotAddr, fpgaAddr, regAddr[idx1], &regRead);
            if (0 != retVal) {
                PrintLog(LOG_LEVEL_ERROR, "DCM register read fail\n");
                return 1;
            }
            if (regRead != regValue[idx1]) {
                PrintLog(LOG_LEVEL_ERROR, "DCM register compare fail, addr[%04X], write[%08X], read[%08X]\n", regAddr[idx1], regValue[idx1], regRead);
                return 1;
            }
        }

        if (0 == idx0 % 1000) {
            PrintLog(LOG_LEVEL_INFO, "Round %d ... \n", idx0 + 1);
        }
        else {
            if (idx0 == runTimes - 1) {
                PrintLog(LOG_LEVEL_INFO, "Round %d ... \n", idx0 + 1);
            }
        }
    }
    PrintLog(LOG_LEVEL_INFO, "DCM register batch test finished\n");

    return 0;
}

uint32_t CCmdDcm::CmdBramWrite(vector<string> vectCmdLine)
{
    uint32_t slotAddr, fpgaAddr, ramAddr, ramSize;
	uint32_t startLine, timeVal, retVal;
    uint8_t *pWrBuff;
    double dbSpeed;

    if (vectCmdLine.size() < 7) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM BRAM_WR command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[2].c_str(), "%u", &slotAddr);
    sscanf_s(vectCmdLine[3].c_str(), "%u", &fpgaAddr);
    sscanf_s(vectCmdLine[4].c_str(), "%X", &ramAddr);
    sscanf_s(vectCmdLine[5].c_str(), "%X", &ramSize);
    if (0 != ramSize % 4) {
        PrintLog(LOG_LEVEL_ERROR, "Size must be 4 byte align\n");
        return 1;
    }

	startLine = 0;
	if (vectCmdLine.size() > 7) {
		sscanf_s(vectCmdLine[7].c_str(), "%X", &startLine);
	}

    pWrBuff = (uint8_t *)malloc(ramSize);
    if (NULL == pWrBuff) {
        PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
        return 1;
    }
    memset(pWrBuff, 0x00, ramSize);

    retVal = ReadFromFile(vectCmdLine[6].c_str(), pWrBuff, ramSize, startLine);
    if (0 != retVal) {
        PrintLog(LOG_LEVEL_ERROR, "Unable to read file %s\n", vectCmdLine[6].c_str());
        free(pWrBuff);
        return 1;
    }

    TickStart();
    retVal = DcmBramWrite(slotAddr, fpgaAddr, ramAddr, pWrBuff, ramSize);
    TickEnd(timeVal);
    if (0 != retVal) {
        PrintLog(LOG_LEVEL_ERROR, "Bram write fail\n");
        free(pWrBuff);
        return 1;
    }    
    if (0 != timeVal) {
        dbSpeed = ramSize / 1024.0  * 1000000.0 / timeVal;
        PrintLog(LOG_LEVEL_INFO, "Bram write speed %0.2fKB/s\n", dbSpeed);
    }
    else {
        PrintLog(LOG_LEVEL_INFO, "Bram write speed --.--KB/s\n", retVal);
    }

    free(pWrBuff);

    return 0;
}

uint32_t CCmdDcm::CmdBramRead(vector<string> vectCmdLine)
{
    uint32_t slotAddr, fpgaAddr, ramAddr, ramSize;
	uint32_t startLine, timeVal, retVal;
    uint8_t *pRdBuff, *pExpBuff;
    double dbSpeed;

    if (vectCmdLine.size() < 7) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM BRAM_RD command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[2].c_str(), "%u", &slotAddr);
    sscanf_s(vectCmdLine[3].c_str(), "%u", &fpgaAddr);
    sscanf_s(vectCmdLine[4].c_str(), "%X", &ramAddr);
    sscanf_s(vectCmdLine[5].c_str(), "%X", &ramSize);
    if (0 != ramSize % 4) {
        PrintLog(LOG_LEVEL_ERROR, "Size must be 4 byte align\n");
        return 1;
    }

    pRdBuff = (uint8_t *)malloc(ramSize);
    if (NULL == pRdBuff) {
        PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
        return 1;
    }
    memset(pRdBuff, 0x00, ramSize);

    TickStart();
    retVal = DcmBramRead(slotAddr, fpgaAddr, ramAddr, pRdBuff, ramSize);
    TickEnd(timeVal);
    if (0 != retVal) {
        PrintLog(LOG_LEVEL_ERROR, "Bram read fail\n");
        free(pRdBuff);
        return 1;
    }
    if (0 != timeVal) {
        dbSpeed = ramSize / 1024.0  * 1000000.0 / timeVal;
        PrintLog(LOG_LEVEL_INFO, "Bram read speed %0.2fKB/s\n", dbSpeed);
    }
    else {
        PrintLog(LOG_LEVEL_INFO, "Bram read speed --.--KB/s\n", retVal);
    }

    retVal = WriteToFile(vectCmdLine[6].c_str(), pRdBuff, ramSize);
    if (0 != retVal) {
        PrintLog(LOG_LEVEL_ERROR, "Unable to write file %s\n", vectCmdLine[6].c_str());
        free(pRdBuff);
        return 1;
    }

    if (vectCmdLine.size() > 7) {
        pExpBuff = (uint8_t *)malloc(ramSize);
        if (NULL == pExpBuff) {
            PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
            free(pRdBuff);
            return 1;
        }
        memset(pExpBuff, 0x00, ramSize);

		startLine = 0;
		if (vectCmdLine.size() > 8) {
			sscanf_s(vectCmdLine[8].c_str(), "%X", &startLine);
		}

        retVal = ReadFromFile(vectCmdLine[7].c_str(), pExpBuff, ramSize, startLine);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "Unable to read file %s\n", vectCmdLine[6].c_str());
            free(pRdBuff);
            free(pExpBuff);
            return 1;
        }

        retVal = memcmp(pRdBuff, pExpBuff, ramSize);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "File compare fail\n");
            free(pRdBuff);
            free(pExpBuff);
            return 1;
        }

        free(pExpBuff);
    }

    free(pRdBuff);

    return 0;
}

uint32_t CCmdDcm::CmdDramWrite(vector<string> vectCmdLine)
{
    uint32_t slotAddr, fpgaAddr, ramAddr, ramSize;
	uint32_t startLine, timeVal, retVal;
    uint8_t *pWrBuff;
    double dbSpeed;

    if (vectCmdLine.size() < 7) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM DRAM_WR command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[2].c_str(), "%u", &slotAddr);
    sscanf_s(vectCmdLine[3].c_str(), "%u", &fpgaAddr);
    sscanf_s(vectCmdLine[4].c_str(), "%X", &ramAddr);
    sscanf_s(vectCmdLine[5].c_str(), "%X", &ramSize);
    if (0 != ramSize % 4) {
        PrintLog(LOG_LEVEL_ERROR, "Size must be 4 byte align\n");
        return 1;
    }

	startLine = 0;
	if (vectCmdLine.size() > 7) {
		sscanf_s(vectCmdLine[7].c_str(), "%X", &startLine);
	}

    pWrBuff = (uint8_t *)malloc(ramSize);
    if (NULL == pWrBuff) {
        PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
        return 1;
    }

    retVal = ReadFromFile(vectCmdLine[6].c_str(), pWrBuff, ramSize, startLine);
    if (0 != retVal) {
        PrintLog(LOG_LEVEL_ERROR, "Unable to read file %s\n", vectCmdLine[6].c_str());
        free(pWrBuff);
        return 1;
    }

    TickStart();
    retVal = DcmDramWrite(slotAddr, fpgaAddr, ramAddr, pWrBuff, ramSize);
    TickEnd(timeVal);
    if (0 != retVal) {
        PrintLog(LOG_LEVEL_ERROR, "Dram write fail\n");
        free(pWrBuff);
        return 1;
    }
    if (0 != timeVal) {
        dbSpeed = ramSize / 1024.0  * 1000000.0 / timeVal;
        PrintLog(LOG_LEVEL_INFO, "Dram write speed %0.2fKB/s\n", dbSpeed);
    }
    else {
        PrintLog(LOG_LEVEL_INFO, "Dram write speed --.--KB/s\n", retVal);
    }

    free(pWrBuff);

    return 0;
}

uint32_t CCmdDcm::CmdDramRead(vector<string> vectCmdLine)
{
    uint32_t slotAddr, fpgaAddr, ramAddr, ramSize;
	uint32_t startLine, timeVal, retVal;
	uint8_t *pRdBuff, *pExpBuff;
    double dbSpeed;

    if (vectCmdLine.size() < 7) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM DRAM_RD command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[2].c_str(), "%u", &slotAddr);
    sscanf_s(vectCmdLine[3].c_str(), "%u", &fpgaAddr);
    sscanf_s(vectCmdLine[4].c_str(), "%X", &ramAddr);
    sscanf_s(vectCmdLine[5].c_str(), "%X", &ramSize);
    if (0 != ramSize % 4) {
        PrintLog(LOG_LEVEL_ERROR, "Size must be 4 byte align\n");
        return 1;
    }

    pRdBuff = (uint8_t *)malloc(ramSize);
    if (NULL == pRdBuff) {
        PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
        return 1;
    }

    TickStart();
    retVal = DcmDramRead(slotAddr, fpgaAddr, ramAddr, pRdBuff, ramSize);
    TickEnd(timeVal);
    if (0 != retVal) {
        PrintLog(LOG_LEVEL_ERROR, "Dram read fail\n");
        free(pRdBuff);
        return 1;
    }
    if (0 != timeVal) {
        dbSpeed = ramSize / 1024.0  * 1000000.0 / timeVal;
        PrintLog(LOG_LEVEL_INFO, "Dram read speed %0.2fKB/s\n", dbSpeed);
    }
    else {
        PrintLog(LOG_LEVEL_INFO, "Dram read speed --.--KB/s\n", retVal);
    }

    retVal = WriteToFile(vectCmdLine[6].c_str(), pRdBuff, ramSize);
    if (0 != retVal) {
        PrintLog(LOG_LEVEL_ERROR, "Unable to write file %s\n", vectCmdLine[6].c_str());
        free(pRdBuff);
        return 1;
    }

    if (vectCmdLine.size() > 7) {
        pExpBuff = (uint8_t *)malloc(ramSize);
        if (NULL == pExpBuff) {
            PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
            free(pRdBuff);
            return 1;
        }
        memset(pExpBuff, 0x00, ramSize);

		startLine = 0;
		if (vectCmdLine.size() > 8) {
			sscanf_s(vectCmdLine[8].c_str(), "%X", &startLine);
		}

        retVal = ReadFromFile(vectCmdLine[7].c_str(), pExpBuff, ramSize, startLine);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "Unable to read file %s\n", vectCmdLine[6].c_str());
            free(pRdBuff);
            free(pExpBuff);
            return 1;
        }

        retVal = memcmp(pRdBuff, pExpBuff, ramSize);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "File compare fail\n");
            free(pRdBuff);
            free(pExpBuff);
            return 1;
        }

        free(pExpBuff);
    }

    free(pRdBuff);

    return 0;
}

uint32_t CCmdDcm::CmdRamFile(vector<string> vectCmdLine)
{
    uint32_t fileSize, dataType, dataStep;
    uint32_t index, retVal;
    uint8_t *pBuff;

    if (vectCmdLine.size() < 6) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM RAM_FILE command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[3].c_str(), "%x", &fileSize);
    sscanf_s(vectCmdLine[4].c_str(), "%u", &dataType);
    sscanf_s(vectCmdLine[5].c_str(), "%u", &dataStep);

    if (dataType > DATA_PATTERN_RAND) {
        PrintLog(LOG_LEVEL_ERROR, "Unsupported type\n");
        return 1;
    }

    if ((1 != dataStep) && (2 != dataStep) && (4 != dataStep)) {
        PrintLog(LOG_LEVEL_ERROR, "Unsupported step\n");
        return 1;
    }

    pBuff = (uint8_t *)malloc(fileSize);
    if (NULL == pBuff) {
        PrintLog(LOG_LEVEL_ERROR, "Unable to allocate memory\n");
        return 1;
    }

    if (DATA_PATTERN_00 == dataType) {
        memset(pBuff, 0x00, fileSize);
    }
    else if (DATA_PATTERN_55 == dataType) {
        memset(pBuff, 0x55, fileSize);
    }
    else if (DATA_PATTERN_AA == dataType) {
        memset(pBuff, 0xAA, fileSize);
    }
    else if (DATA_PATTERN_FF == dataType) {
        memset(pBuff, 0xFF, fileSize);
    }
    else if (DATA_PATTERN_5A == dataType) {
        memset(pBuff, 0x5A, fileSize);
    }
    else if (DATA_PATTERN_A5 == dataType) {
        memset(pBuff, 0xA5, fileSize);
    }
    else if (DATA_PATTERN_INC == dataType) {
        for (index = 0; index < fileSize / dataStep; index++) {
            switch (dataStep) {
            case 1:
                ((uint8_t *)(pBuff))[index] = (uint8_t)index;
                break;
            case 2:
                ((uint16_t *)(pBuff))[index] = (uint16_t)index;
                break;
            case 4:
                ((uint32_t *)(pBuff))[index] = (uint32_t)index;
                break;
            default:
                break;
            }
        }
    }
    else if (DATA_PATTERN_DEC == dataType) {
        for (index = 0; index < fileSize / dataStep; index++) {
            switch (dataStep) {
            case 1:
                ((uint8_t *)(pBuff))[index] = 0xFF - (uint8_t)index;
                break;
            case 2:
                ((uint16_t *)(pBuff))[index] = 0xFFFF - (uint16_t)index;
                break;
            case 4:
                ((uint32_t *)(pBuff))[index] = 0xFFFFFFFF - (uint32_t)index;
                break;
            default:
                break;
            }
        }
    }
    else {
        srand((uint32_t)(time(NULL)));
        for (index = 0; index < fileSize; index++) {
            pBuff[index] = (uint8_t)rand();
        }
    }

    retVal = WriteToFile(vectCmdLine[2], pBuff, fileSize);
    if (0 != retVal) {
        PrintLog(LOG_LEVEL_ERROR, "Unable to write file %s\n", vectCmdLine[2].c_str());
        free(pBuff);
        return 1;
    }

    free(pBuff);

    return 0;
}

uint32_t CCmdDcm::ReadFromFile(string strFile, uint8_t *pBuff, uint32_t maxSize, uint32_t startLine)
{
    uint32_t fileType, fileSize, tempValue, index;
    char strLine[1024];
    string strTemp;
    FILE *fpFile;

    fileType = 0;
    if (strFile.size() > 4) {
        strTemp = strFile.substr(strFile.size() - 4, strFile.size());
        ToUpperCase(strTemp);
        if (".TXT" == strTemp) {
            fileType = 1;
        }
    }
    
    if (0 == fileType) {
        fpFile = fopen(strFile.c_str(), "r+b");
        if (NULL == fpFile) {
            return 1;
        }

        fseek(fpFile, 0, SEEK_END);
        fileSize = ftell(fpFile);
        fseek(fpFile, 0, SEEK_SET);

        if (fileSize >= maxSize) {
            fread(pBuff, maxSize, 1, fpFile);
        }
        else {
            fread(pBuff, fileSize, 1, fpFile);
        }

        fclose(fpFile);
    }
    else {
        fpFile = fopen(strFile.c_str(), "r");
        if (NULL == fpFile) {
            return 1;
        }

		while (startLine > 0) {
			if (NULL == fgets(strLine, 1024, fpFile)) {
				break;
			}
			startLine--;
		}

        for (index = 0; index < maxSize / 4; index++) {
            if (NULL == fgets(strLine, 1024, fpFile)) {
                break;
            }
            if ('\n' == strLine[strlen(strLine) - 1]) {
                strLine[strlen(strLine) - 1] = '\0';
            }
            if (0 == strlen(strLine)) {
                continue;
            }
            sscanf_s(strLine, "%x", &tempValue);
            memcpy(pBuff + index * 4, &tempValue, 4);
        }

        fclose(fpFile);
    }

    return 0;
}

uint32_t CCmdDcm::WriteToFile(string strFile, uint8_t *pBuff, uint32_t maxSize)
{
    uint32_t fileType, tempValue, index;
    string strTemp;
    FILE *fpFile;

    fileType = 0;
    if (strFile.size() > 4) {
        strTemp = strFile.substr(strFile.size() - 4, strFile.size());
        ToUpperCase(strTemp);
        if (".TXT" == strTemp) {
            fileType = 1;
        }
    }

    if (0 == fileType) {
        fpFile = fopen(strFile.c_str(), "w+b");
        if (NULL == fpFile) {
            return 1;
        }

        fwrite(pBuff, maxSize, 1, fpFile);

        fclose(fpFile);
    }
    else {
        fpFile = fopen(strFile.c_str(), "w");
        if (NULL == fpFile) {
            return 1;
        }

        for (index = 0; index < maxSize / 4; index++) {
            memcpy(&tempValue, pBuff + index * 4, 4);
            fprintf(fpFile, "%08X\n", tempValue);
        }

        fclose(fpFile);
    }

    return 0;
}

uint32_t CCmdDcm::CmdATE305Write(vector<string> vectCmdLine)
{
    uint32_t slotAddr, fpgaAddr, regAddr, chipChan;
    uint32_t regValue[16], retVal, isMult, index;
    uint32_t singleChan;

    if (vectCmdLine.size() < 7) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM ATE305_WR command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[2].c_str(), "%u", &slotAddr);
    sscanf_s(vectCmdLine[3].c_str(), "%u", &fpgaAddr);
    sscanf_s(vectCmdLine[4].c_str(), "%X", &chipChan);
    sscanf_s(vectCmdLine[5].c_str(), "%X", &regAddr);
    sscanf_s(vectCmdLine[6].c_str(), "%X", regValue);

    isMult = 0;
    for (index = 0; index < 16; index++) {
        if (0 != (chipChan & (1<<index))){
            isMult++;
            singleChan = index;
        }
    }
    isMult = (isMult > 1) ? 1 : 0;

    if (isMult) {
        retVal = ATE305MultWriteReg(slotAddr, fpgaAddr, chipChan, regAddr, regValue, 1);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "ATE305 register burst write fail\n");
            return 1;
        }
    }
    else {
        retVal = ATE305WriteReg(slotAddr, fpgaAddr, singleChan, regAddr, regValue[0]);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "ATE305 register single write fail\n");
            return 1;
        }
    }

    return 0;
}

uint32_t CCmdDcm::CmdATE305Read(vector<string> vectCmdLine)
{
    uint32_t slotAddr, fpgaAddr, regAddr, chipChan, expValue;
    uint32_t regValue[16], retVal, isMult, index;
    uint32_t singleChan;

    if (vectCmdLine.size() < 6) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM ATE305_RD command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[2].c_str(), "%u", &slotAddr);
    sscanf_s(vectCmdLine[3].c_str(), "%u", &fpgaAddr);
    sscanf_s(vectCmdLine[4].c_str(), "%X", &chipChan);
    sscanf_s(vectCmdLine[5].c_str(), "%X", &regAddr);

    isMult = 0;
    for (index = 0; index < 16; index++) {
        if (0 != (chipChan & (1 << index))){
            singleChan = index;
            isMult++;
        }
    }
    isMult = (isMult > 1) ? 1 : 0;

    if (isMult) {
        retVal = ATE305MultReadReg(slotAddr, fpgaAddr, chipChan, regAddr, regValue);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "ATE305 register burst read fail\n");
            return 1;
        }
    }
    else {
        retVal = ATE305ReadReg(slotAddr, fpgaAddr, singleChan, regAddr, regValue);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "ATE305 register single read fail\n");
            return 1;
        }
    }

    if (vectCmdLine.size() > 6) {
        sscanf_s(vectCmdLine[6].c_str(), "%X", &expValue);

        if (isMult) {
            for (index = 0; index < 16; index++) {
                if (0 != (chipChan & (1 << index))){
                    if (regValue[index] != expValue) {
                        PrintLog(LOG_LEVEL_ERROR, "Read Register Chan[%04X] Addr[%02X] Value[%04X] Exp[%04X]\n", (1 << index), regAddr, regValue[index], expValue);
                        return 1;
                    }
                }
            }
        }
        else {
            if (regValue[0] != expValue) {
                PrintLog(LOG_LEVEL_ERROR, "Read Register Chan[%04X] Addr[%02X] Value[%04X] Exp[%04X]\n", singleChan, regAddr, regValue[0], expValue);
                return 1;
            }
        }
    }
    else {
        if (isMult) {
            for (index = 0; index < 16; index++) {
                if (0 != (chipChan & (1 << index))){
                    PrintLog(LOG_LEVEL_INFO, "Read Register Chan[%04X] Addr[%02X] Value[%04X]\n", (1 << index), regAddr, regValue[index]);
                }
            }
        }
        else {
            PrintLog(LOG_LEVEL_INFO, "Read Register Chan[%04X] Addr[%02X] Value[%04X]\n", singleChan, regAddr, regValue[0]);
        }
    }

    return 0;
}

uint32_t CCmdDcm::CmdATE305Batch(vector<string> vectCmdLine)
{
    uint32_t regAddr[2], regValue[16], regRead[16];
    uint32_t slotAddr, fpgaAddr, runTimes;
    uint32_t idx0, idx1, retVal;

    if (vectCmdLine.size() < 5) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM ATE305_BATCH command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[2].c_str(), "%u", &slotAddr);
    sscanf_s(vectCmdLine[3].c_str(), "%u", &fpgaAddr);
    sscanf_s(vectCmdLine[4].c_str(), "%u", &runTimes);

    regAddr[0] = 0x06;
    regAddr[1] = 0x07;

    srand((uint32_t)(time(NULL)));
    for (idx0 = 0; idx0 < runTimes; idx0++) {
        // register single write read
        for (idx1 = 0; idx1 < 2; idx1++) {
            regValue[idx1] = rand() & 0x3FFF;
        }

        for (idx1 = 0; idx1 < 2; idx1++) {
            retVal = ATE305WriteReg(slotAddr, fpgaAddr, idx0%16, regAddr[idx1], regValue[idx1]);
            if (0 != retVal) {
                PrintLog(LOG_LEVEL_ERROR, "ATE305 register write fail\n");
                return 1;
            }
        }

        for (idx1 = 0; idx1 < 2; idx1++) {
            retVal = ATE305ReadReg(slotAddr, fpgaAddr, idx0 % 16, regAddr[idx1], regRead);
            if (0 != retVal) {
                PrintLog(LOG_LEVEL_ERROR, "ATE305 read write fail\n");
                return 1;
            }
            if (regRead[0] != regValue[idx1]) {
                PrintLog(LOG_LEVEL_ERROR, "ATE305 register compare fail, chan[%04X], addr[%02X], write[%04X], read[%04X]\n", 1 << (idx0 % 16), regAddr[idx1], regValue[idx1], regRead[0]);
                return 1;
            }
        }

        // register multiple write read without burst
        for (idx1 = 0; idx1 < 16; idx1++) {
            regValue[idx1] = rand() & 0x3FFF;
        }

        retVal = ATE305MultWriteReg(slotAddr, fpgaAddr, 0xFFFF, regAddr[0], regValue, 0);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "ATE305 read write fail\n");
            return 1;
        }

        memset(regRead, 0x00, 16);
        retVal = ATE305MultReadReg(slotAddr, fpgaAddr, 0xFFFF, regAddr[0], regRead);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "ATE305 read write fail\n");
            return 1;
        }

        if (0 != memcmp(regRead, regValue, 16)) {
            PrintLog(LOG_LEVEL_ERROR, "ATE305 register compare fail when 16 channel multiple mode\n");
            PrintLog(LOG_LEVEL_INFO, "Write: ");
            for (idx1 = 0; idx1 < 16; idx1++) {
                PrintLog(LOG_LEVEL_INFO, "0x%04X ", regValue[idx1]);
                if (idx1 == 7) {
                    PrintLog(LOG_LEVEL_INFO, "\n       ");
                }
            }
            PrintLog(LOG_LEVEL_INFO, "\n");

            PrintLog(LOG_LEVEL_INFO, "Read:  ");
            for (idx1 = 0; idx1 < 16; idx1++) {
                PrintLog(LOG_LEVEL_INFO, "0x%04X ", regRead[idx1]);
                if (idx1 == 7) {
                    PrintLog(LOG_LEVEL_INFO, "\n       ");
                }
            }
            PrintLog(LOG_LEVEL_INFO, "\n");
            return 1;
        }

        // register multiple write read with burst
        regValue[0] = rand() & 0x3FFF;
        for (idx1 = 1; idx1 < 16; idx1++) {
            regValue[idx1] = regValue[0];
        }

        retVal = ATE305MultWriteReg(slotAddr, fpgaAddr, 0xFFFF, regAddr[0], regValue, 1);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "ATE305 read write fail\n");
            return 1;
        }

        memset(regRead, 0x00, 16);
        retVal = ATE305MultReadReg(slotAddr, fpgaAddr, 0xFFFF, regAddr[0], regRead);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "ATE305 read write fail\n");
            return 1;
        }

        if (0 != memcmp(regRead, regValue, 16)) {
            PrintLog(LOG_LEVEL_ERROR, "ATE305 register compare fail when 16 channel multiple mode\n");
            PrintLog(LOG_LEVEL_INFO, "Write: ");
            for (idx1 = 0; idx1 < 16; idx1++) {
                PrintLog(LOG_LEVEL_INFO, "0x%04X ", regValue[idx1]);
                if (idx1 == 7) {
                    PrintLog(LOG_LEVEL_INFO, "\n       ");
                }
            }
            PrintLog(LOG_LEVEL_INFO, "\n");

            PrintLog(LOG_LEVEL_INFO, "Read:  ");
            for (idx1 = 0; idx1 < 16; idx1++) {
                PrintLog(LOG_LEVEL_INFO, "0x%04X ", regRead[idx1]);
                if (idx1 == 7) {
                    PrintLog(LOG_LEVEL_INFO, "\n       ");
                }
            }
            PrintLog(LOG_LEVEL_INFO, "\n");
            return 1;
        }

        if (0 == idx0 % 10) {
            PrintLog(LOG_LEVEL_INFO, "Round %d ... \n", idx0 + 1);
        }
        else {
            if (idx0 == runTimes - 1) {
                PrintLog(LOG_LEVEL_INFO, "Round %d ... \n", idx0 + 1);
            }
        }
    }
    PrintLog(LOG_LEVEL_INFO, "ATE305 register batch test finished\n");

    return 0;
}

uint32_t CCmdDcm::CmdAD7606Samp(vector<string> vectCmdLine)
{
    uint32_t slotAddr, fpgaAddr, sampChan;
    uint32_t sampDepth, sampTime;
    uint32_t retVal, index;
    double dbResult[512];

    if (vectCmdLine.size() < 6) {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM AD7606 command\n");
        return 1;
    }

    sscanf_s(vectCmdLine[3].c_str(), "%u", &slotAddr);
    sscanf_s(vectCmdLine[4].c_str(), "%u", &fpgaAddr);
    sscanf_s(vectCmdLine[5].c_str(), "%u", &sampChan);

    retVal = AD7606Stop(slotAddr, fpgaAddr);
    if (0 != retVal) {
        PrintLog(LOG_LEVEL_ERROR, "AD7606 start fail\n");
        return 1;
    }

    ToUpperCase(vectCmdLine[2]);
    if ("SINGLE" == vectCmdLine[2]) {
        retVal = AD7606Start(slotAddr, fpgaAddr, AD7606_MODE_SINGLE, 0);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "AD7606 start fail\n");
            return 1;
        }

        retVal = AD7606Read(slotAddr, fpgaAddr, AD7606_MODE_SINGLE, sampChan, dbResult, NULL);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "AD7606 read fail\n");
            return 1;
        }

        PrintLog(LOG_LEVEL_INFO, "Voltage = %2.6fV\n", dbResult[0]);
    }
    else if ("MULTIPLE" == vectCmdLine[2]) {
        if (vectCmdLine.size() < 7) {
            PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM AD7606 command\n");
            return 1;
        }

        sscanf_s(vectCmdLine[6].c_str(), "%u", &sampDepth);
        if ((sampDepth < 1) || (sampDepth > 256)) {
            PrintLog(LOG_LEVEL_ERROR, "Sample depth mus be [1 ~ 256]\n");
            return 1;
        }

        retVal = AD7606Start(slotAddr, fpgaAddr, AD7606_MODE_MULTIPLE, sampDepth);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "AD7606 start fail\n");
            return 1;
        }

        retVal = AD7606Read(slotAddr, fpgaAddr, AD7606_MODE_MULTIPLE, sampChan, dbResult, &sampDepth);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "AD7606 read fail\n");
            return 1;
        }

        PrintLog(LOG_LEVEL_INFO, "Voltage = %2.6fV\n", dbResult[0]);
    }
    else if ("CONTINOUS" == vectCmdLine[2]) {
        if (vectCmdLine.size() < 7) {
            PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM AD7606 command\n");
            return 1;
        }

        sscanf_s(vectCmdLine[6].c_str(), "%u", &sampTime);

        retVal = AD7606Start(slotAddr, fpgaAddr, AD7606_MODE_CONTINOUS, 0);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "AD7606 start fail\n");
            return 1;
        }

        Sleep(sampTime);

        retVal = AD7606Stop(slotAddr, fpgaAddr);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "AD7606 start fail\n");
            return 1;
        }

        retVal = AD7606Read(slotAddr, fpgaAddr, AD7606_MODE_CONTINOUS, sampChan, dbResult, &sampDepth);
        if (0 != retVal) {
            PrintLog(LOG_LEVEL_ERROR, "AD7606 read fail\n");
            return 1;
        }

        PrintLog(LOG_LEVEL_INFO, "In total %u points is sampled, the last %u voltage:\n", sampDepth, (sampDepth > 512) ? 512 : sampDepth);
        sampDepth = (sampDepth > 512) ? 512 : sampDepth;

        for (index = 0; index < sampDepth; index++) {
            PrintLog(LOG_LEVEL_INFO, "index = %03u  Voltage = %2.6fV\n", index, dbResult[index]);
        }
    }
    else {
        PrintLog(LOG_LEVEL_ERROR, "Unkonwn DCM AD7606 command\n");
        return 1;
    }

    return 0;
}

uint32_t CCmdDcm::CmdRelayCtrl(vector<string> vectCmdLine)
{
	uint32_t slotAddr, relay0To31, relay32To63, relayHvch;
	uint32_t relayOnOff, retVal;

	if (vectCmdLine.size() < 7) {
		PrintLog(LOG_LEVEL_ERROR, "Unknown DCM RELAY command\n");
		return 1;
	}

	sscanf_s(vectCmdLine[2].c_str(), "%u", &slotAddr);
	sscanf_s(vectCmdLine[3].c_str(), "%x", &relay0To31);
	sscanf_s(vectCmdLine[4].c_str(), "%x", &relay32To63);
	sscanf_s(vectCmdLine[5].c_str(), "%x", &relayHvch);

	ToUpperCase(vectCmdLine[6]);
	if ("ON" == vectCmdLine[6]) {
		relayOnOff = 1;
	}
	else if ("OFF" == vectCmdLine[6]) {
		relayOnOff = 0;
	}
	else {
		PrintLog(LOG_LEVEL_ERROR, "Unknown DCM RELAY command\n");
		return 1;
	}

	retVal = DcmRelayCtrl(slotAddr, relay0To31, relay32To63, relayHvch, relayOnOff);
	if (0 != retVal) {
		PrintLog(LOG_LEVEL_ERROR, "Relay control fail\n");
		return 1;
	}

	return 0;
}

uint32_t CCmdDcm::CmdHelpInf(vector<string> vectCmdLine)
{
	PrintLog(LOG_LEVEL_INFO, "#########################################################################################\n");
#if 0
	PrintLog(LOG_LEVEL_INFO, "# DCM DUMMY <Slot(D)> <Fpga(D)>                                                         #\n");
#endif
	PrintLog(LOG_LEVEL_INFO, "# DCM RESET <Slot(D)> <Fpga(D)>                                                         #\n");
	PrintLog(LOG_LEVEL_INFO, "# DCM START <SOFT/TOTAL> <Slot(D)> <Fpga(D)>                                            #\n");
	PrintLog(LOG_LEVEL_INFO, "# DCM REG_WR <Slot(D)> <Fpga(D)> <Addr(X)> <Value(X)>                                   #\n");
	PrintLog(LOG_LEVEL_INFO, "# DCM REG_RD <Slot(D)> <Fpga(D)> <Addr(X)> [ExpValue(X)] [<MaskValue>(X)]               #\n");
#if 0
	PrintLog(LOG_LEVEL_INFO, "# DCM REG_BATCH <Slot(D)> <Fpga(D)> <Times(D)>                                          #\n");
#endif
	PrintLog(LOG_LEVEL_INFO, "# DCM BRAM_WR <Slot(D)> <Fpga(D)> <Addr(X)> <Size(X)> <File(S)> [Line(X)]               #\n");
	PrintLog(LOG_LEVEL_INFO, "# DCM BRAM_RD <Slot(D)> <Fpga(D)> <Addr(X)> <Size(X)> <File(S)> [ExpFile(S)] [Line(X)]  #\n");
	PrintLog(LOG_LEVEL_INFO, "# DCM DRAM_WR <Slot(D)> <Fpga(D)> <Addr(X)> <Size(X)> <File(S)> [Line(X)]               #\n");
	PrintLog(LOG_LEVEL_INFO, "# DCM DRAM_RD <Slot(D)> <Fpga(D)> <Addr(X)> <Size(X)> <File(S)> [ExpFile(S)] [Line(X)]  #\n");
	PrintLog(LOG_LEVEL_INFO, "# DCM RAM_FILE <File(S)> <Size(X)> <Type(D)> <Step(D)>                                  #\n");
	PrintLog(LOG_LEVEL_INFO, "# DCM ATE305_WR <Slot(D)> <Fpga(D)> <Chan(X)> <Addr(X)> <Value(X)>                      #\n");
	PrintLog(LOG_LEVEL_INFO, "# DCM ATE305_RD <Slot(D)> <Fpga(D)> <Chan(X)> <Addr(X)> [ExpValue(X)]                   #\n");
#if 0
	PrintLog(LOG_LEVEL_INFO, "# DCM ATE305_BATCH <Slot(D)> <Fpga(D)> <Times(D)>                                       #\n");
#endif
	PrintLog(LOG_LEVEL_INFO, "# DCM AD7606 SINGLE <Slot(D)> <Fpga(D)> <Chan(D)>                                       #\n");
	PrintLog(LOG_LEVEL_INFO, "# DCM AD7606 MULTIPLE <Slot(D)> <Fpga(D)> <Chan(D)> <Depth(D)>                          #\n");
	PrintLog(LOG_LEVEL_INFO, "# DCM AD7606 CONTINOUS <Slot(D)> <Fpga(D)> <Chan(D)> <SampTime(D)>                      #\n");
	PrintLog(LOG_LEVEL_INFO, "# DCM RELAY <Slot(D)> <CH[0:31](X)> <CH[32:63](X)> <HVCH(X)> <ON/OFF>                   #\n");
	PrintLog(LOG_LEVEL_INFO, "# DCM HELP                                                                              #\n");
	PrintLog(LOG_LEVEL_INFO, "#########################################################################################\n");

	return 0;
}
