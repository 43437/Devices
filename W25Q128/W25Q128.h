/**

  W25Q128 objW25Q128;
  objW25Q128.Activate();

  objW25Q128.Seek(0x1FFD);

  uint8_t buf[] = "Hello World";
  objW25Q128.Write(buf, 11);

  uint8_t rbuf[20];
  objW25Q128.Read(rbuf, 20);

  objW25Q128.Seek(0x1FFF);
  objW25Q128.Write((uint8_t*)"111", 3);

  std::memset(rbuf, 0, 20);
  objW25Q128.Seek(0x1FFD);
  objW25Q128.Read(rbuf, 20);

 */
#ifndef __W25Q128_H__
#define __W25Q128_H__

#include <cstdint>

class W25Q128
{
public:
	W25Q128();
	uint32_t Read(uint8_t buf[], uint32_t u32Len);
	uint32_t Write(uint8_t buf[], uint32_t u32Len);
	bool Seek(uint32_t u32MemAddr);

	void Activate();
	void DeActivate();
	bool IsActivate();

private:
	uint32_t m_u32MemAddr;
};

#endif
