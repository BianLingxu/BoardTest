#ifndef __FE2D6EBE_FEE3_4866_ABDE_86C0ABDC79EF_DCMLIBRARY_H__
#define __FE2D6EBE_FEE3_4866_ABDE_86C0ABDC79EF_DCMLIBRARY_H__

#include <stdint.h>

/********************************************************************************
error number define
********************************************************************************/
#define ERR_DCM_FUNC_PARM_ILLEGAL (0x0001)
#define ERR_DCM_DLL_INIT_FAIL     (0x0002)
#define ERR_DCM_DRAM_WR_TIMEOUT   (0x0003)
#define ERR_DCM_DRAM_RD_TIMEOUT   (0x0004)
#define ERR_ATE305_WR_TIMEOUT     (0x0005)
#define ERR_ATE305_RD_TIMEOUT     (0x0006)
#define ERR_AD7606_SAMP_TIMEOUT   (0x0007)

/********************************************************************************
ATE305 channel define
********************************************************************************/
#define ATE305_CHAN_00   (1<<0)
#define ATE305_CHAN_01   (1<<1)
#define ATE305_CHAN_02   (1<<2)
#define ATE305_CHAN_03   (1<<3)
#define ATE305_CHAN_04   (1<<4)
#define ATE305_CHAN_05   (1<<5)
#define ATE305_CHAN_06   (1<<6)
#define ATE305_CHAN_07   (1<<7)
#define ATE305_CHAN_08   (1<<8)
#define ATE305_CHAN_09   (1<<9)
#define ATE305_CHAN_10   (1<<10)
#define ATE305_CHAN_11   (1<<11)
#define ATE305_CHAN_12   (1<<12)
#define ATE305_CHAN_13   (1<<13)
#define ATE305_CHAN_14   (1<<14)
#define ATE305_CHAN_15   (1<<15)

/********************************************************************************
AD7606 sample mode define
********************************************************************************/
#define AD7606_MODE_SINGLE    (0)
#define AD7606_MODE_MULTIPLE  (1)
#define AD7606_MODE_CONTINOUS (2)

/********************************************************************************
function declaration
********************************************************************************/
uint32_t DcmTotalStart();
uint32_t DcmReadReg(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t regAddr, uint32_t *pRegValue);
uint32_t DcmWriteReg(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t regAddr, uint32_t regValue);
uint32_t DcmBramWrite(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t bramAddr, uint8_t *pBuff, uint32_t ramSize);
uint32_t DcmBramRead(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t bramAddr, uint8_t *pBuff, uint32_t ramSize);
uint32_t DcmDramWrite(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t dramAddr, uint8_t *pBuff, uint32_t ramSize);
uint32_t DcmDramRead(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t dramAddr, uint8_t *pBuff, uint32_t ramSize);
uint32_t ATE305ReadReg(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t channel, uint32_t regAddr, uint32_t *pRegValue);
uint32_t ATE305WriteReg(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t channel, uint32_t regAddr, uint32_t regValue);
uint32_t ATE305MultReadReg(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t channel, uint32_t regAddr, uint32_t *pRegBuff);
uint32_t ATE305MultWriteReg(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t channel, uint32_t regAddr, uint32_t *pRegBuff, uint32_t isBurst);
uint32_t AD7606Start(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t sampMode, uint32_t sampDepth);
uint32_t AD7606Read(uint32_t slotAddr, uint32_t fpgaAddr, uint32_t sampMode, uint32_t channel, double *pAdcVal, uint32_t *pDepth);
uint32_t AD7606Stop(uint32_t slotAddr, uint32_t fpgaAddr);
double CalcAd7606Voltage(uint16_t adcValue);
uint32_t DcmRelayCtrl(BYTE slotAddr, DWORD ch0_ch31, DWORD ch32_ch63, BYTE hvch, BYTE relay_status);
#endif /* __FE2D6EBE_FEE3_4866_ABDE_86C0ABDC79EF_DCMLIBRARY_H__ */