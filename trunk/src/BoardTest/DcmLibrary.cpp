// DcmLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "UtilLibrary.h"
#include "DcmLibrary.h"
#include "STSSP8201/STSSP8201.h"

/********************************************************************************
relay control macro
********************************************************************************/
#define MAX_CH_PER_BOARD	64
#define MAX_HVCH_PER_BOARD	8
#define TOTAL_CH_PER_BOARD	72
#define BITS_PER_DWORD		32

#define CH0_KFUNC	63
#define CH1_KFUNC	56
#define CH2_KFUNC	62
#define CH3_KFUNC	57
#define CH4_KFUNC	61
#define CH5_KFUNC	58
#define CH6_KFUNC	60
#define CH7_KFUNC	59
#define CH8_KFUNC	67
#define CH9_KFUNC	68
#define CH10_KFUNC	66
#define CH11_KFUNC	69
#define CH12_KFUNC	65
#define CH13_KFUNC	70
#define CH14_KFUNC	64
#define CH15_KFUNC	71
#define CH16_KFUNC	44
#define CH17_KFUNC	39
#define CH18_KFUNC	45
#define CH19_KFUNC	38
#define CH20_KFUNC	42
#define CH21_KFUNC	41
#define CH22_KFUNC	43
#define CH23_KFUNC	40
#define CH24_KFUNC	49
#define CH25_KFUNC	50
#define CH26_KFUNC	48
#define CH27_KFUNC	51
#define CH28_KFUNC	46
#define CH29_KFUNC	52
#define CH30_KFUNC	47
#define CH31_KFUNC	53
#define CH32_KFUNC	26
#define CH33_KFUNC	33
#define CH34_KFUNC	27
#define CH35_KFUNC	32
#define CH36_KFUNC	28
#define CH37_KFUNC	31
#define CH38_KFUNC	29
#define CH39_KFUNC	30
#define CH40_KFUNC	22
#define CH41_KFUNC	21
#define CH42_KFUNC	23
#define CH43_KFUNC	20
#define CH44_KFUNC	24
#define CH45_KFUNC	19
#define CH46_KFUNC	25
#define CH47_KFUNC	18
#define CH48_KFUNC	9
#define CH49_KFUNC	14
#define CH50_KFUNC	8
#define CH51_KFUNC	15
#define CH52_KFUNC	11
#define CH53_KFUNC	12
#define CH54_KFUNC	10
#define CH55_KFUNC	13
#define CH56_KFUNC	5
#define CH57_KFUNC	2
#define CH58_KFUNC	4
#define CH59_KFUNC	3
#define CH60_KFUNC	7
#define CH61_KFUNC	0
#define CH62_KFUNC	6
#define CH63_KFUNC	1
#define HVCH0_KFUNC	54
#define HVCH1_KFUNC	55
#define HVCH2_KFUNC	37
#define HVCH3_KFUNC	36
#define HVCH4_KFUNC	35
#define HVCH5_KFUNC	34
#define HVCH6_KFUNC	16
#define HVCH7_KFUNC	17

#define CAL_DUTG0K	79
#define CAL_DUTG1K	78
#define CAL_DUTG2K	77
#define CAL_DUTG3K	76

#define DCM_RELAY_ON  1
#define DCM_RELAY_OFF 0

/********************************************************************************
local variable define
********************************************************************************/
static uint32_t g_tlc_func_reg_data[3];

BYTE g_ch_kfunc[TOTAL_CH_PER_BOARD] =
{
	CH0_KFUNC, CH1_KFUNC, CH2_KFUNC, CH3_KFUNC, CH4_KFUNC, CH5_KFUNC, CH6_KFUNC, CH7_KFUNC,
	CH8_KFUNC, CH9_KFUNC, CH10_KFUNC, CH11_KFUNC, CH12_KFUNC, CH13_KFUNC, CH14_KFUNC, CH15_KFUNC,
	CH16_KFUNC, CH17_KFUNC, CH18_KFUNC, CH19_KFUNC, CH20_KFUNC, CH21_KFUNC, CH22_KFUNC, CH23_KFUNC,
	CH24_KFUNC, CH25_KFUNC, CH26_KFUNC, CH27_KFUNC, CH28_KFUNC, CH29_KFUNC, CH30_KFUNC, CH31_KFUNC,
	CH32_KFUNC, CH33_KFUNC, CH34_KFUNC, CH35_KFUNC, CH36_KFUNC, CH37_KFUNC, CH38_KFUNC, CH39_KFUNC,
	CH40_KFUNC, CH41_KFUNC, CH42_KFUNC, CH43_KFUNC, CH44_KFUNC, CH45_KFUNC, CH46_KFUNC, CH47_KFUNC,
	CH48_KFUNC, CH49_KFUNC, CH50_KFUNC, CH51_KFUNC, CH52_KFUNC, CH53_KFUNC, CH54_KFUNC, CH55_KFUNC,
	CH56_KFUNC, CH57_KFUNC, CH58_KFUNC, CH59_KFUNC, CH60_KFUNC, CH61_KFUNC, CH62_KFUNC, CH63_KFUNC,
	HVCH0_KFUNC, HVCH1_KFUNC, HVCH2_KFUNC, HVCH3_KFUNC, HVCH4_KFUNC, HVCH5_KFUNC, HVCH6_KFUNC, HVCH7_KFUNC,
};

uint32_t DcmTotalStart()
{
    write_dw(0x00FC0000, 0x00000000);

    return 0;
}

uint32_t DcmWriteReg(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t regAddr, uint32_t regValue)
{
    uint32_t barOffset = ((slotAddr & 0x3F) << 18) | ((fpgaAddr & 0x07) << 15) | ((regAddr & 0x1FFF) << 2);
	write_dw(barOffset, regValue);

	PrintLog(LOG_LEVEL_DEBUG, "Write register Slot[%d] Fpga[%d] Addr[%04X] Value[%08X]\n", slotAddr, fpgaAddr, regAddr, regValue);
	return 0;
}

uint32_t DcmReadReg(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t regAddr, uint32_t *pRegValue)
{
	uint32_t barOffset;

	if (NULL == pRegValue)
	{
		return ERR_DCM_FUNC_PARM_ILLEGAL;
	}

	barOffset = ((slotAddr & 0x3F) << 18) | ((fpgaAddr & 0x07) << 15) | ((regAddr & 0x1FFF) << 2);
	*pRegValue = read_dw(barOffset);

	PrintLog(LOG_LEVEL_DEBUG, "Read  register Slot[%d] Fpga[%d] Addr[%04X] Value[%08X]\n", slotAddr, fpgaAddr, regAddr, *pRegValue);
	return 0;
}

uint32_t DcmBramWrite(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t bramAddr, uint8_t *pBuff, uint32_t ramSize)
{
    uint32_t index, retVal;
    uint32_t *pBuff32;

    if (NULL == pBuff)
	{
        return ERR_DCM_FUNC_PARM_ILLEGAL;
    }

    retVal = DcmWriteReg(slotAddr, fpgaAddr, 0x0407, bramAddr);
    if (0 != retVal)
	{
        return retVal;
	}

    pBuff32 = (uint32_t *)pBuff;
    for (index = 0; index < ramSize / 4; index++)
	{
        retVal = DcmWriteReg(slotAddr, fpgaAddr, 0x0408, pBuff32[index]);
		if (0 != retVal)
		{
            return retVal;
        }
    }
    return 0;
}

uint32_t DcmBramRead(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t bramAddr, uint8_t *pBuff, uint32_t ramSize)
{
    uint32_t index, retVal;
    uint32_t *pBuff32;

    if (NULL == pBuff)
	{
        return ERR_DCM_FUNC_PARM_ILLEGAL;
    }

	retVal = DcmWriteReg(slotAddr, fpgaAddr, 0x0409, bramAddr);
	if (0 != retVal)
	{
		return retVal;
	}

	pBuff32 = (uint32_t *)pBuff;
	for (index = 0; index < ramSize / 4; index++)
	{
		retVal = DcmReadReg(slotAddr, fpgaAddr, 0x040A, pBuff32 + index);
		if (0 != retVal)
		{
			return retVal;
		}
	}

    return 0;
}

uint32_t DcmDramWrite(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t dramAddr, uint8_t *pBuff, uint32_t ramSize)
{
    uint32_t lastSize, writeSize, regValue, timeVal;
    uint32_t index, retVal;
    uint32_t *pBuff32;

    if (NULL == pBuff)
	{
        return ERR_DCM_FUNC_PARM_ILLEGAL;
    }

	lastSize = ramSize;
	pBuff32 = (uint32_t *)pBuff;

	while (lastSize > 0)
	{
		writeSize = (lastSize > 1024) ? 1024 : lastSize;

		/* set dram access address */
		retVal = DcmWriteReg(slotAddr, fpgaAddr, 0x0400, dramAddr);
		if (0 != retVal)
		{
			return retVal;
		}

		/* write dram data to fpga buffer */
		for (index = 0; index < writeSize / 4; index++)
		{
			retVal = DcmWriteReg(slotAddr, fpgaAddr, 0x0401, *pBuff32);
			if (0 != retVal)
			{
				return retVal;
			}
			pBuff32++;
		}

		/* send request for flush buffer to DRAM */
		retVal = DcmWriteReg(slotAddr, fpgaAddr, 0x0405, writeSize | (1 << 16));
		if (0 != retVal)
		{
			return retVal;
		}

		/* check if request finished */
		for (timeVal = 0; timeVal < 0x100; timeVal++)
		{
			retVal = DcmReadReg(slotAddr, fpgaAddr, 0x0406, &regValue);
			if (0 != retVal)
			{
				return retVal;
			}
			if (0 == (regValue & 0x01))
			{
				break;
			}
		}
		if (0 != (regValue & 0x01))
		{
			return ERR_DCM_DRAM_WR_TIMEOUT;
		}

		dramAddr += writeSize / 4;
		lastSize -= writeSize;
	}

    return 0;
}

uint32_t DcmDramRead(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t dramAddr, uint8_t *pBuff, uint32_t ramSize)
{
    uint32_t lastSize, readSize, regValue, timeVal;
    uint32_t index, retVal;
    uint32_t *pBuff32;

    if (NULL == pBuff)
	{
        return ERR_DCM_FUNC_PARM_ILLEGAL;
    }

	lastSize = ramSize;
	pBuff32 = (uint32_t *)pBuff;

	while (lastSize > 0)
	{
		readSize = (lastSize > 1024) ? 1024 : lastSize;

		/* set dram access address */
		retVal = DcmWriteReg(slotAddr, fpgaAddr, 0x0400, dramAddr);
		if (0 != retVal)
		{
			return retVal;
		}

		/* send request for flush DRAM to buffer */
		retVal = DcmWriteReg(slotAddr, fpgaAddr, 0x0405, readSize | (1 << 17));
		if (0 != retVal)
		{
			return retVal;
		}

		/* check if request finished */
		for (timeVal = 0; timeVal < 0x100; timeVal++)
		{
			retVal = DcmReadReg(slotAddr, fpgaAddr, 0x0406, &regValue);
			if (0 != retVal)
			{
				return retVal;
			}
			if (0 == (regValue & 0x02))
			{
				break;
			}
		}
		if (0 != (regValue & 0x02))
		{
			return ERR_DCM_DRAM_RD_TIMEOUT;
		}

		/* read dram data from fpga buffer */
		for (index = 0; index < readSize / 4; index++)
		{
			retVal = DcmReadReg(slotAddr, fpgaAddr, 0x0402, pBuff32);
			if (0 != retVal)
			{
				return retVal;
			}
			pBuff32++;
		}

		dramAddr += readSize / 4;
		lastSize -= readSize;
	}

    return 0;
}

uint32_t ATE305ReadReg(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t channel, uint32_t regAddr, uint32_t *pRegValue)
{
    uint32_t fpgaRegAddr, fpgaRegValue;
    uint32_t retVal;

    fpgaRegAddr = 0x860 + channel;
    fpgaRegValue = (regAddr << 16) + (1 << 21);
    retVal = DcmWriteReg(slotAddr, fpgaAddr, fpgaRegAddr, fpgaRegValue);
    if (0 != retVal)
	{
        return retVal;
    }

#if 0
    uint32_t timeVal;

    for (timeVal = 0; timeVal < 0x100; timeVal++) {
        retVal = DcmReadReg(slotAddr, fpgaAddr, fpgaRegAddr, &fpgaRegValue);
        if (0 != retVal) {
            return retVal;
        }
        if (0 != (fpgaRegValue & (1<<22))) {
            break;
        }
    }
    if (0 == (fpgaRegValue & (1 << 22))) {
        return ERR_ATE305_RD_TIMEOUT;
    }
#else
    DelayUs(2.5);

    retVal = DcmReadReg(slotAddr, fpgaAddr, fpgaRegAddr, &fpgaRegValue);
    if (0 != retVal)
	{
        return retVal;
    }
#endif

    *pRegValue = fpgaRegValue & 0xFFFF;

    return 0;
}

uint32_t ATE305WriteReg(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t channel, uint32_t regAddr, uint32_t regValue)
{
    uint32_t fpgaRegAddr, fpgaRegValue;
    uint32_t retVal;

    fpgaRegAddr = 0x850 + channel;
    fpgaRegValue = regValue + (regAddr << 16) + (1 << 21);
    retVal = DcmWriteReg(slotAddr, fpgaAddr, fpgaRegAddr, fpgaRegValue);
    if (0 != retVal)
	{
        return retVal;
    }

#if 0
    uint32_t timeVal;

    for (timeVal = 0; timeVal < 0x100; timeVal++) {
        retVal = DcmReadReg(slotAddr, fpgaAddr, fpgaRegAddr, &fpgaRegValue);
        if (0 != retVal) {
            return retVal;
        }
        if (0 != (fpgaRegValue & (1 << 22))) {
            break;
        }
    }
    if (0 == (fpgaRegValue & (1 << 22))) {
        return ERR_ATE305_RD_TIMEOUT;
    }
#else
    DelayUs(1.5);
#endif

    return 0;
}

uint32_t ATE305MultReadReg(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t channel, uint32_t regAddr, uint32_t *pRegBuff)
{
    uint32_t fpgaRegAddr, fpgaRegValue, timeVal;
    uint32_t retVal, index;

    fpgaRegAddr = 0x830;
    fpgaRegValue = (channel << 16) + (regAddr << 3) + (1 << 0);
    retVal = DcmWriteReg(slotAddr, fpgaAddr, fpgaRegAddr, fpgaRegValue);
    if (0 != retVal)
	{
        return retVal;
    }

    for (timeVal = 0; timeVal < 0x100; timeVal++)
	{
        retVal = DcmReadReg(slotAddr, fpgaAddr, 0x831, &fpgaRegValue);
        if (0 != retVal)
		{
            return retVal;
        }
        if (0 == (fpgaRegValue & 0xFFFF0000))
		{
            break;
        }
    }
    if (0 != (fpgaRegValue & 0xFFFF0000))
	{
        return ERR_ATE305_RD_TIMEOUT;
    }

    for (index = 0; index < 16; index += 2)
	{
        if (0 == (channel & (3 << index)))
		{
            continue;
        }

        fpgaRegAddr = 0x848 + index / 2;
        retVal = DcmReadReg(slotAddr, fpgaAddr, fpgaRegAddr, &fpgaRegValue);
        if (0 != retVal)
		{
            return retVal;
        }

        pRegBuff[index] = fpgaRegValue & 0xFFFF;
        pRegBuff[index + 1] = fpgaRegValue >> 16;
    }

    return 0;
}

uint32_t ATE305MultWriteReg(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t channel, uint32_t regAddr, uint32_t *pRegBuff, uint32_t isBurst)
{
    uint32_t fpgaRegAddr, fpgaRegValue, timeVal;
    uint32_t retVal, index;

    if (isBurst)
	{
        fpgaRegAddr = 0x840;
        fpgaRegValue = pRegBuff[0];
        retVal = DcmWriteReg(slotAddr, fpgaAddr, fpgaRegAddr, fpgaRegValue);
        if (0 != retVal)
		{
            return retVal;
        }
    }
    else {
        for (index = 0; index < 16; index += 2)
		{
            if (0 == (channel & (3 << index))) 
			{
                continue;
            }

            fpgaRegAddr = 0x840 + index / 2;
            fpgaRegValue = pRegBuff[index] + (pRegBuff[index + 1] << 16);
            retVal = DcmWriteReg(slotAddr, fpgaAddr, fpgaRegAddr, fpgaRegValue);
            if (0 != retVal) 
			{
                return retVal;
            }
        }
    }

    fpgaRegAddr = 0x830;
    fpgaRegValue = (channel << 16) + (regAddr << 3) + (1 << 1) + (1 << 0);
    if (!isBurst) 
	{
        fpgaRegValue |= (1 << 2);
    }
    retVal = DcmWriteReg(slotAddr, fpgaAddr, fpgaRegAddr, fpgaRegValue);
    if (0 != retVal) 
	{
        return retVal;
    }

    for (timeVal = 0; timeVal < 0x100; timeVal++) {
        retVal = DcmReadReg(slotAddr, fpgaAddr, 0x831, &fpgaRegValue);
        if (0 != retVal) 
		{
            return retVal;
        }
        if (0 == (fpgaRegValue & 0xFFFF)) 
		{
            break;
        }
    }
    if (0 != (fpgaRegValue & 0xFFFF))
	{
        return ERR_ATE305_WR_TIMEOUT;
    }

    return 0;
}

uint32_t AD7606Start(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t sampMode, uint32_t sampDepth)
{
    uint32_t fpgaRegValue, retVal;

    fpgaRegValue = (sampMode << 2) + (1 << 0);
    if (AD7606_MODE_MULTIPLE == sampMode) 
	{
        sampDepth -= 1;
        fpgaRegValue |= (sampDepth & 0xFF) << 8;
    }

    retVal = DcmWriteReg(slotAddr, fpgaAddr, 0x822, fpgaRegValue);
    if (0 != retVal) 
	{
        return retVal;
    }

    return 0;
}

uint32_t AD7606Read(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t sampMode, uint32_t channel, double *pAdcVal, uint32_t *pDepth)
{
    uint32_t fpgaRegAddr, fpgaRegValue, timeVal, retVal, index;
    uint32_t bramAddr, bramData[512], lastOfst;

    if (AD7606_MODE_CONTINOUS != sampMode) 
	{
        for (timeVal = 0; timeVal < 0x1000; timeVal++) 
		{
            retVal = DcmReadReg(slotAddr, fpgaAddr, 0x823, &fpgaRegValue);
            if (0 != retVal) 
			{
                return retVal;
            }
            if (0 != (fpgaRegValue & 0x02)) 
			{
                break;
            }
        }
        if (0 == (fpgaRegValue & 0x02)) 
		{
            return ERR_AD7606_SAMP_TIMEOUT;
        }
    }
    
    if (AD7606_MODE_SINGLE == sampMode) 
	{
        fpgaRegAddr = 0x824 + channel / 2;
        retVal = DcmReadReg(slotAddr, fpgaAddr, fpgaRegAddr, &fpgaRegValue);
        if (0 != retVal) 
		{
            return retVal;
        }

        if (0 == (channel % 2)) 
		{
            fpgaRegValue = fpgaRegValue & 0xFFFF;
        }
        else 
		{
            fpgaRegValue = fpgaRegValue >> 16;
        }

        *pAdcVal = CalcAd7606Voltage(fpgaRegValue & 0xFFFF);
    }
    else if(AD7606_MODE_MULTIPLE == sampMode) 
	{
        fpgaRegAddr = 0x828 + channel;
        retVal = DcmReadReg(slotAddr, fpgaAddr, fpgaRegAddr, &fpgaRegValue);
        if (0 != retVal) 
		{
            return retVal;
        }

        fpgaRegValue /= *pDepth;
        *pAdcVal = CalcAd7606Voltage(fpgaRegValue & 0xFFFF);
    }
    else
    {
        retVal = DcmReadReg(slotAddr, fpgaAddr, 0x0870, &fpgaRegValue);
        if (0 != retVal) 
		{
            return retVal;
        }
        *pDepth = fpgaRegValue;

        fpgaRegValue = (fpgaRegValue > 512) ? 512 : fpgaRegValue;
        bramAddr = 0x00400000 + 0x200 * channel;
        retVal = DcmBramRead(slotAddr, fpgaAddr, bramAddr, (uint8_t *)bramData, fpgaRegValue * 4);
        if (0 != retVal) 
		{
            return retVal;
        }

        for (lastOfst = 0; lastOfst < 512; lastOfst++)
		{
            if (*pDepth - 1 == (bramData[lastOfst] >> 16)) 
			{
                break;
            }
        }

        if (*pDepth > 512) 
		{
            for (index = lastOfst + 1; index < 512; index++) 
			{
                *pAdcVal = CalcAd7606Voltage(bramData[index] & 0xFFFF);
                pAdcVal++;
            }
        }
        for (index = 0; index < lastOfst + 1; index++) 
		{
            *pAdcVal = CalcAd7606Voltage(bramData[index] & 0xFFFF);
            pAdcVal++;
        }
    }

    return 0;
}

uint32_t AD7606Stop(uint32_t slotAddr, uint32_t fpgaAddr)
{
    uint32_t retVal;

    retVal = DcmWriteReg(slotAddr, fpgaAddr, 0x822, 0x00000002);
    if (0 != retVal) 
	{
        return retVal;
    }

    return 0;
}

double CalcAd7606Voltage(uint16_t adcValue)
{
    double fLsb, dbResult;

    fLsb = 10.0 / 32768;
    if (adcValue >= 0x8000) 
	{
        dbResult = (adcValue - 0x8000) * fLsb - 10.0 + fLsb / 2;
    }
    else 
	{
        dbResult = adcValue * fLsb - fLsb / 2;
    }

    return dbResult;
}

uint32_t DcmRelayCtrl(BYTE slotAddr, DWORD ch0_ch31, DWORD ch32_ch63, BYTE hvch, BYTE relay_status)
{
	DWORD tlc_wr_reg_addr[3] = { 0x00 };
	DWORD tlc_updata_reg_addr = 0x00;
	uint32_t regAddr, index;

	int tlc_addr_index = 0;
	int ch_index = 0;

	for (ch_index = 0; ch_index < BITS_PER_DWORD; ch_index++)
	{
		//CH0-CH31
		if ((ch0_ch31 >> ch_index) & 0x01)
		{
			if (g_ch_kfunc[ch_index] < 32)
			{
				if (DCM_RELAY_ON == relay_status)
				{
					g_tlc_func_reg_data[0] |= 1 << g_ch_kfunc[ch_index];
				}
				else if (DCM_RELAY_OFF == relay_status)
				{
					g_tlc_func_reg_data[0] &= ~(1 << g_ch_kfunc[ch_index]);
				}
				else
				{
					;
				}
			}
			else if ((g_ch_kfunc[ch_index] >= 32) && (g_ch_kfunc[ch_index] < 64))
			{
				if (DCM_RELAY_ON == relay_status)
				{
					g_tlc_func_reg_data[1] |= 1 << (g_ch_kfunc[ch_index] - 32);
				}
				else if (DCM_RELAY_OFF == relay_status)
				{
					g_tlc_func_reg_data[1] &= ~(1 << (g_ch_kfunc[ch_index] - 32));
				}
				else
				{
					;
				}
			}
			else if ((g_ch_kfunc[ch_index] >= 64) && (g_ch_kfunc[ch_index] < 72))
			{
				if (DCM_RELAY_ON == relay_status)
				{
					g_tlc_func_reg_data[2]
						|= 1 << (g_ch_kfunc[ch_index] - 64);
				}
				else if (DCM_RELAY_OFF == relay_status)
				{
					g_tlc_func_reg_data[2]
						&= ~(1 << (g_ch_kfunc[ch_index] - 64));
				}
				else
				{
					;
				}
			}
			else
			{
				;
			}
		}
		else
		{
			;
		}

		//CH32-CH63
		if ((ch32_ch63 >> ch_index) & 0x01)
		{
			if (g_ch_kfunc[ch_index + 32] < 32)
			{
				if (DCM_RELAY_ON == relay_status)
				{
					g_tlc_func_reg_data[0] |= 1 << g_ch_kfunc[ch_index + 32];
				}
				else if (DCM_RELAY_OFF == relay_status)
				{
					g_tlc_func_reg_data[0] &= ~(1 << g_ch_kfunc[ch_index + 32]);
				}
				else
				{
					;
				}
			}
			else if ((g_ch_kfunc[ch_index + 32] >= 32) && (g_ch_kfunc[ch_index + 32] < 64))
			{
				if (DCM_RELAY_ON == relay_status)
				{
					g_tlc_func_reg_data[1] |= 1 << (g_ch_kfunc[ch_index + 32] - 32);
				}
				else if (DCM_RELAY_OFF == relay_status)
				{
					g_tlc_func_reg_data[1] &= ~(1 << (g_ch_kfunc[ch_index + 32] - 32));
				}
				else
				{
					;
				}
			}
			else if ((g_ch_kfunc[ch_index + 32] >= 64) && (g_ch_kfunc[ch_index + 32] < 72))
			{
				if (DCM_RELAY_ON == relay_status)
				{
					g_tlc_func_reg_data[2]
						|= 1 << (g_ch_kfunc[ch_index + 32] - 64);
				}
				else if (DCM_RELAY_OFF == relay_status)
				{
					g_tlc_func_reg_data[2]
						&= ~(1 << (g_ch_kfunc[ch_index + 32] - 64));
				}
				else
				{
					;
				}
			}
			else
			{
				;
			}
		}
		else
		{
			;
		}

		//HVCH0-HVCH7
		if ((ch_index < MAX_HVCH_PER_BOARD) && ((hvch >> ch_index) & 0x01))
		{
			if (g_ch_kfunc[ch_index + 64] < 32)
			{
				if (DCM_RELAY_ON == relay_status)
				{
					g_tlc_func_reg_data[0] |= 1 << g_ch_kfunc[ch_index + 64];
				}
				else if (DCM_RELAY_OFF == relay_status)
				{
					g_tlc_func_reg_data[0] &= ~(1 << g_ch_kfunc[ch_index + 64]);
				}
				else
				{
					;
				}
			}
			else if ((g_ch_kfunc[ch_index + 64] >= 32) && (g_ch_kfunc[ch_index + 64] < 64))
			{
				if (DCM_RELAY_ON == relay_status)
				{
					g_tlc_func_reg_data[1] |= 1 << (g_ch_kfunc[ch_index + 64] - 32);
				}
				else if (DCM_RELAY_OFF == relay_status)
				{
					g_tlc_func_reg_data[1] &= ~(1 << (g_ch_kfunc[ch_index + 64] - 32));
				}
				else
				{
					;
				}
			}
			else if ((g_ch_kfunc[ch_index + 64] >= 64) && (g_ch_kfunc[ch_index + 64] < 72))
			{
				if (DCM_RELAY_ON == relay_status)
				{
					g_tlc_func_reg_data[2] |= 1 << (g_ch_kfunc[ch_index + 64] - 64);
				}
				else if (DCM_RELAY_OFF == relay_status)
				{
					g_tlc_func_reg_data[2] &= ~(1 << (g_ch_kfunc[ch_index + 64] - 64));
				}
				else
				{
					;
				}
			}
			else
			{
				;
			}
		}
		else
		{
			;
		}
	}

	for (index = 0; index < 3; index++)
	{
		regAddr = 0x20 + index;
		regAddr = (slotAddr << 18) | (5 << 15) | (regAddr << 2);

		write_dw(regAddr, g_tlc_func_reg_data[index]);

		PrintLog(LOG_LEVEL_DEBUG, "Write PCI register 0x%08X value 0x%08X\n", regAddr, g_tlc_func_reg_data[index]);
	}

	regAddr = (slotAddr << 18) | (5 << 15) | (0x23 << 2);
	write_dw(regAddr, 1);
	PrintLog(LOG_LEVEL_DEBUG, "Write PCI register 0x%08X value 0x%08X\n", regAddr, 1);
	write_dw(regAddr, 0);
	PrintLog(LOG_LEVEL_DEBUG, "Write PCI register 0x%08X value 0x%08X\n", regAddr, 0);

	return 0;
}