#include "W25Q128.h"
#include "flash.h"
#include <cstring>

#include "stm32f1xx_hal.h"
static void CS(const int8_t iCS)
{
	switch(iCS)
	{
	case 0:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		break;
	default:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		break;
	}
}
const uint32_t MAX_SIZE=0x1000000;
const uint32_t SECTOR_SIZE=4096;

W25Q128::W25Q128() : m_u32MemAddr(0)
{

}

uint32_t W25Q128::Read(uint8_t buf[], uint32_t u32Len)
{
	if (MAX_SIZE < u32Len + m_u32MemAddr)
	{
		u32Len = MAX_SIZE - m_u32MemAddr;
	}

	SPI_FLASH_BufferRead(buf, m_u32MemAddr, u32Len);
	return u32Len;
}

uint32_t W25Q128::Write(uint8_t buf[], uint32_t u32Len)
{
	if (!IsActivate())
	{
		return 0;
	}

	if (MAX_SIZE < u32Len + m_u32MemAddr)
	{
		u32Len = MAX_SIZE - m_u32MemAddr;
	}


	uint32_t u32CurMemAddr = m_u32MemAddr;
	uint8_t u8SectorBuf[SECTOR_SIZE] = {0};


	uint32_t u32SectorMemAddrStart = m_u32MemAddr & 0xFFFFF000;
	uint32_t u32SectorMemAddrEnd = u32SectorMemAddrStart + 0xFFF;



	uint32_t u32SectorDiff = m_u32MemAddr - u32SectorMemAddrStart;
	uint32_t u32DataLen = u32Len;


	uint32_t u32ModifyLen = 0;

	uint8_t* pSrcBuf = buf;
	while (u32DataLen > 0)
	{
		if (SECTOR_SIZE-u32SectorDiff <u32DataLen)
			u32ModifyLen = SECTOR_SIZE-u32SectorDiff;
		else
			u32ModifyLen = u32DataLen;

		//读
		Seek(u32SectorMemAddrStart);
		Read(u8SectorBuf, SECTOR_SIZE);

		//修改
		std::memcpy(u8SectorBuf + u32SectorDiff, pSrcBuf, u32ModifyLen);

		SPI_FLASH_SectorErase(u32SectorMemAddrStart);
		// 写入, 使用SPI_FLASH_BufferWrite写入数据
		SPI_FLASH_BufferWrite(u8SectorBuf, u32SectorMemAddrStart, SECTOR_SIZE);

		u32SectorMemAddrStart += SECTOR_SIZE;
		u32SectorMemAddrEnd += SECTOR_SIZE;
		pSrcBuf += u32ModifyLen;
		u32DataLen -= u32ModifyLen;
		u32SectorDiff = 0;
	}
	Seek(u32CurMemAddr);

	return u32Len;
}

bool W25Q128::Seek(uint32_t u32MemAddr)
{
	if (u32MemAddr >= 0 && u32MemAddr < MAX_SIZE)
	{
		m_u32MemAddr = u32MemAddr;
		return true;
	}
	return false;
}

void W25Q128::Activate()
{
	SPI_FLASH_SetCS(CS);
}

void W25Q128::DeActivate()
{
	SPI_FLASH_SetCS(nullptr);
}

bool W25Q128::IsActivate()
{
	return SPI_FLASH_GetCS() == CS;
}


