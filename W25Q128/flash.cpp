#include "flash.h"
#include "W25XDataDef.h"


static SPI_FLASH_CS s_pFuncCS = nullptr;
SPI_FLASH_CS SPI_FLASH_GetCS()
{
	return s_pFuncCS;
}
void SPI_FLASH_SetCS(SPI_FLASH_CS pFuncCS)
{
	s_pFuncCS = pFuncCS;
}

void CS(const int8_t iCS)
{
	if (nullptr != s_pFuncCS)
	{
		s_pFuncCS(iCS);
	}
}

#include "stm32f1xx_hal.h"
extern SPI_HandleTypeDef hspi1;
static bool HW_SPI_SEND(uint8_t* pData, uint16_t u16Len)
{
	return HAL_OK == HAL_SPI_Transmit(&hspi1, pData, u16Len, 0xffff);
}

static bool HW_SPI_RECV(uint8_t* pData, uint16_t u16Len)
{
	return HAL_OK == HAL_SPI_Receive(&hspi1, pData, u16Len, 0xffff);
}


void SPI_FLASH_WriteEnable(void)
{
  uint8_t byte = 0;
  /* Select the FLASH: Chip Select low */
  CS(0);

  /* Send "Write Enable" instruction */
  byte = W25X_WriteEnable;
  HW_SPI_SEND(&byte, 1);

  /* Deselect the FLASH: Chip Select high */
  CS(1);
}

void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t FLASH_Status = 0;
  uint8_t byte = Dummy_Byte;

  /* Select the FLASH: Chip Select low */
  CS(0);

  /* Send "Read Status Register" instruction */
  byte = W25X_ReadStatusReg;
  HW_SPI_SEND(&byte, 1);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
//	  HAL_SPI_TransmitReceive(&hspi1, &byte, &FLASH_Status, 1, 0xffff );
	  HW_SPI_RECV(&FLASH_Status, 1);
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  CS(1);
}

void SPI_FLASH_BulkErase(void)
{
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable();

  /* Bulk Erase */
  /* Select the FLASH: Chip Select low */
  CS(0);

  /* Send Bulk Erase instruction  */
  uint8_t byte = W25X_ChipErase;
  HAL_SPI_Transmit(&hspi1, &byte, 1, 0xffff);

  /* Deselect the FLASH: Chip Select high */
  CS(1);

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
  uint8_t byte = 0;
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  /* Sector Erase */
  /* Select the FLASH: Chip Select low */
  CS(0);

  /* Send Sector Erase instruction */
  byte = W25X_SectorErase;
  HW_SPI_SEND(&byte, 1);

  /* Send SectorAddr high nibble address byte */
  byte = (SectorAddr & 0xFF0000) >> 16;
  HW_SPI_SEND(&byte, 1);

  /* Send Secto rAddr medium nibble address byte */
  byte = (SectorAddr & 0xFF00) >> 8;
  HW_SPI_SEND(&byte, 1);

  /* Send SectorAddr low nibble address byte */
  byte = SectorAddr & 0xFF;
  HW_SPI_SEND(&byte, 1);

  /* Deselect the FLASH: Chip Select high */
  CS(1);

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t byte = 0;
  /* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable();

  /* Select the FLASH: Chip Select low */
  CS(0);
  /* Send "Write to Memory " instruction */
  byte = W25X_PageProgram;
  HW_SPI_SEND(&byte, 1);

  /* Send WriteAddr high nibble address byte to write to */
  byte = (WriteAddr & 0xFF0000) >> 16;
  HW_SPI_SEND(&byte, 1);

  /* Send WriteAddr medium nibble address byte to write to */
  byte = (WriteAddr & 0xFF00) >> 8;
  HW_SPI_SEND(&byte, 1);

  /* Send WriteAddr low nibble address byte to write to */
  byte = WriteAddr & 0xFF;
  HW_SPI_SEND(&byte, 1);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     //printf("\n\r Err: SPI_FLASH_PageWrite too large!");
  }

  /* while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /* Send the current byte */
	  HW_SPI_SEND(pBuffer, 1);
    /* Point on the next byte to be written */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  CS(1);

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}

void FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{

}

void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  if (Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  uint8_t byte = 0;
  /* Select the FLASH: Chip Select low */
	CS(0);

  /* Send "Read from Memory " instruction */
  byte = W25X_ReadData;
  HW_SPI_SEND(&byte, 1);

  /* Send ReadAddr high nibble address byte to read from */
  byte = (ReadAddr & 0xFF0000) >> 16;
  HW_SPI_SEND(&byte, 1);

  /* Send ReadAddr medium nibble address byte to read from */
  byte = (ReadAddr& 0xFF00) >> 8;
  HW_SPI_SEND(&byte, 1);

  /* Send ReadAddr low nibble address byte to read from */
  byte = ReadAddr & 0xFF;
  HW_SPI_SEND(&byte, 1);

  while (NumByteToRead--) /* while there is data to be read */
  {
    /* Read a byte from the FLASH */
//	byte = Dummy_Byte;
//	HAL_SPI_TransmitReceive(&hspi1, &byte, pBuffer, 1, 0xffff );
	  HW_SPI_RECV(pBuffer, 1);
    /* Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  CS(1);
}
