/**

容量与结构​

存储空间：​256 Kbit​（32,768 字节），分为 ​512 页，每页 ​64 字节​。
组织结构：32K × 8 位（每个地址存储 1 字节）。


​接口协议​：

支持 ​I²C 总线​（2 线串行接口），最高速率 ​1 MHz​（5V 电压下）。
设备地址固定为 ​1010​（二进制），硬件地址引脚 ​A2/A1/A0​ 可配置，支持多设备并联（最多 8 片）。

  AT24C256 objAT24C256(0xa0);
  objAT24C256.Seek(3);
  objAT24C256.Write("hello!", 6);

  char buf[10] = {0};
  objAT24C256.Read(buf, 6);

  std::memset(buf, 0, 10);
  objAT24C256.Seek(0);
  objAT24C256.Read(buf, 9);

  I2C Speed Mode: Standard Mode
  I2C Clock Speed(Hz): 100k

  Clock No Stretch Mode: Disabled
  Primary Address Length selection: 7bit
  Primary slave address: 0
  General Call address detection Disabled
 */
#include "AT24C256.h"

const static uint16_t MEM_SIZE = 32768;

#include "stm32f1xx_hal.h"
extern I2C_HandleTypeDef hi2c1;
bool HW_I2C_READ(uint16_t u16DevAddr, uint16_t u16MemAddr, uint8_t* buf, uint16_t u16Len)
{
	HAL_I2C_Mem_Read(&hi2c1, u16DevAddr, u16MemAddr, I2C_MEMADD_SIZE_16BIT, buf, u16Len, 0xffff);
}

bool HW_I2C_WRITE(uint16_t u16DevAddr, uint16_t u16MemAddr, uint8_t* buf, uint16_t u16Len)
{
	HAL_I2C_Mem_Write(&hi2c1, u16DevAddr, u16MemAddr, I2C_MEMADD_SIZE_16BIT, buf, u16Len, 0xffff);
}

AT24C256::AT24C256(uint16_t u16DevAddr) : m_u16DevAddr(u16DevAddr), m_u16MemAddr(0)
{

}

bool AT24C256::Seek(uint16_t u16MemAddr)
{
	if (u16MemAddr < MEM_SIZE){
		m_u16MemAddr = u16MemAddr;
		return true;
	}
	return false;
}

uint16_t AT24C256::Read(char* buf, uint16_t u16Len)
{
	if (m_u16MemAddr + u16Len > MEM_SIZE)
	{
		u16Len = MEM_SIZE - m_u16MemAddr;
	}

	if (HW_I2C_READ(m_u16DevAddr, m_u16MemAddr, (uint8_t*)buf, u16Len))
		return u16Len;
	else
		return 0;
}

uint16_t AT24C256::Write(char* buf, uint16_t u16Len)
{
	if (m_u16MemAddr + u16Len > MEM_SIZE)
	{
		u16Len = MEM_SIZE - m_u16MemAddr;
	}

	if (HW_I2C_WRITE(m_u16DevAddr, m_u16MemAddr, (uint8_t*)buf, u16Len))
		return u16Len;
	else
		return 0;
}
