#ifndef __AT24C256_H__
#define __AT24C256_H__

#include <cstdint>

class AT24C256
{
public:
	AT24C256(uint16_t u16DevAddr);
	bool Seek(uint16_t u16MemAddr);
	uint16_t Read(char* buf, uint16_t u16Len);
	uint16_t Write(char* buf, uint16_t u16Len);

private:
	uint16_t m_u16DevAddr;
	uint16_t m_u16MemAddr;
};

#endif
