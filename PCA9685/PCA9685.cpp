#include "PCA9685.h"
#include <cmath>
/**
每个通道有4个寄存器（共12位分辨率，范围0-4095）：
| 寄存器类型       | 地址公式                | 作用描述                     |
|------------------|-------------------------|------------------------------|
| `LEDx_ON_L`      | `0x06 + 4 × channel`    | 通道x的ON时间低8位           |
| `LEDx_ON_H`      | `0x07 + 4 × channel`    | 通道x的ON时间高4位（低4位保留） |
| `LEDx_OFF_L`     | `0x08 + 4 × channel`    | 通道x的OFF时间低8位          |
| `LEDx_OFF_H`     | `0x09 + 4 × channel`    | 通道x的OFF时间高4位（低4位保留） |


u16DevAddr 1+A5+A4+A3+A2+A1+A0+w/r

MODE1寄存器配置
| 位   | 名称      | 值 | 功能说明                                                                 |
|------|-----------|----|--------------------------------------------------------------------------|
| **7**| RESTART   | 1  | 置位时重启 PWM 输出（需先退出 SLEEP 模式）                         |
| **6**| EXTCLK    | 0  | 使用内部时钟（默认）                                                     |
| **5**| **AI**    | 1  | **自动递增使能**：连续写入时寄存器地址自动+1，简化多通道配置        |
| **4**| SLEEP     | 0  | **退出休眠模式**：必须为 0 才能激活 PWM 输出                      |
| **3**| SUB1     | 0  | 子地址 1 禁用（默认）                                                    |
| **2**| SUB2     | 0  | 子地址 2 禁用                                                            |
| **1**| SUB3     | 0  | 子地址 3 禁用                                                            |
| **0**| **ALLCALL**| 1| **响应广播地址**（0x70），允许多设备同步控制                    |

PCA9685 objPCA9685(0x80, 200);
objPCA9685.SetPWM(0, 0, 2048);

  I2C Speed Mode: Standard Mode
  I2C Clock Speed(Hz): 100k

  Clock No Stretch Mode: Disabled
  Primary Address Length selection: 7bit
  Dual Address Acknowledged: Disabled
  Primary slave address: 0
  General Call address detection Disabled
 */

#define REG_MODE1_ADDR 0x00
#define REG_PRE_SCALE 0xfe

uint8_t MODE1_SLEEP=0x10;
uint8_t MODE1_REBOOT=0x80;
uint8_t MODE1_AI=0x20;

extern "C"{
extern void Delayus(uint16_t nus);
extern void Delayms(uint32_t nms);
}

#include "stm32f1xx_hal.h"
extern I2C_HandleTypeDef hi2c1;
static void HW_I2C_WRITE(uint16_t u16DevAddr, uint8_t u8Addr, uint8_t* pu8Data, uint16_t u16DataLen)
{
	HAL_StatusTypeDef eStat = HAL_I2C_Mem_Write(&hi2c1, u16DevAddr, u8Addr, I2C_MEMADD_SIZE_8BIT, pu8Data, u16DataLen, 0xffff);
	if (HAL_OK != eStat)
	{

	}
}

static uint8_t GetChannelRegStartAddr(uint8_t u8Channel)
{
	if (u8Channel >=0 && u8Channel < 16)
	{
		return 0x06 + 4 * u8Channel;
	}
	return 0x06;
}

PCA9685::PCA9685(uint16_t u16DevAddr, float fFreq) : m_u16DevAddr(u16DevAddr), m_fFreq(fFreq)
{
	Reset();
}

PCA9685::PCA9685(uint16_t u16DevAddr) : PCA9685(u16DevAddr, 50)
{

}

PCA9685::~PCA9685()
{

}

void PCA9685::Reset()
{
	SetFreq(m_fFreq);
}

void PCA9685::SetFreq(float fFreq)
{
	//freq = 25,000,000 / (4096 * (prescale + 1))，其中25MHz是内部时钟频率，4096是计数器分辨率
	float prescaleval = 25000000.0 / 4096.0 / fFreq;
	uint8_t prescale = std::floor(prescaleval + 0.5) - 1;

	uint8_t u8Mode = MODE1_SLEEP | MODE1_AI;
	Write(REG_MODE1_ADDR, &u8Mode, 1);

	Write(REG_PRE_SCALE, &prescale, 1);

	u8Mode = MODE1_AI;
	Write(REG_MODE1_ADDR, &u8Mode, 1);

	// 重要！等待振荡器稳定（至少500ms）
	Delayms(500);

	u8Mode = MODE1_REBOOT | MODE1_AI;
	Write(REG_MODE1_ADDR, &u8Mode, 1);

	Delayms(50);
}

void PCA9685::SetPWM(uint8_t u8Channel, uint16_t u16ON, uint16_t u16OFF)
{
    uint8_t data[4];
    uint8_t u8RegBase = GetChannelRegStartAddr(u8Channel);  // 通道寄存器基地址

    // 拆分ON/OFF值
    data[0] = u16ON & 0xFF;        // ON_L
    data[1] = (u16ON >> 8) & 0x0F; // ON_H（仅高4位有效）
    data[2] = u16OFF & 0xFF;       // OFF_L
    data[3] = (u16OFF >> 8) & 0x0F; // OFF_H

    Write(u8RegBase, data, 4);
}

void PCA9685::Write(uint8_t u8Addr, uint8_t* pu8Data, uint16_t u16DataLen)
{
	HW_I2C_WRITE(m_u16DevAddr, u8Addr, pu8Data, u16DataLen);
}
