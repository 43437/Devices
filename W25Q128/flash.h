#ifndef __FLASH_H__
#define __FLASH_H__

#include <stdint.h>

typedef void (*SPI_FLASH_CS)(const int8_t i8CS);
SPI_FLASH_CS SPI_FLASH_GetCS();
void SPI_FLASH_SetCS(SPI_FLASH_CS pFuncCS);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

#endif
