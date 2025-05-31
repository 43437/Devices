#ifndef __PCA9685_H__
#define __PCA9685_H__

#include <stdint.h>

class PCA9685
{
public:
	PCA9685(uint16_t u16DevAddr, float fFreq);
	PCA9685(uint16_t u16DevAddr);
	~PCA9685();

	void Reset();
	void SetFreq(float freq);

	void SetPWM(uint8_t channel, uint16_t on, uint16_t off);

private:
	void Write(uint8_t u8Addr, uint8_t* pu8Data, uint16_t u16DataLen);

private:
	uint16_t m_u16DevAddr;
	float m_fFreq;
};

#endif
