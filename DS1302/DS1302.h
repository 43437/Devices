/**
 *
 *
 DS1302::SDateTime stuDateTime{0x25, 0x05, 0x30, 0x21, 0x58, 0x00};
 DS1302& rDS = DS1302::Instance();
 rDS.SetDateTime(stuDateTime);

 rDS.GetDateTime(stuDateTime);
 std::string strNow = stuDateTime.ReadableString();
 */
#ifndef __DS1302_2105232229_H__
#define __DS1302_2105232229_H__

#include "stm32f1xx_hal.h"
#include <string>

class DS1302{
public:
	struct SDateTime{
		//hex格式表示，如：uint8_t time_buf[8] = {0x25,0x06,0x01,0x21,0x45,0x00}表示25-06-01 21:45:00
		uint8_t m_u8Year;
		uint8_t m_u8Month;
		uint8_t m_u8Day;
		uint8_t m_u8Hour;
		uint8_t m_u8Min;
		uint8_t m_u8Sec;

		std::string ReadableString();
	};

	static DS1302& Instance();
	~DS1302();
	void SetDateTime(const SDateTime& stuDateTime);
	void GetDateTime(SDateTime& stuDateTime);

private:
	DS1302();
};

#endif
