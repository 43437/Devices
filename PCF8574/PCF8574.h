/*
 * PCF8574.h
 *
 *  Created on: Aug 25, 2023
 *      Author: geey17u

 	PCF8574 objPCF8574(0x40);
	objPCF8574.Set(PCF8574::ePin_1, PCF8574::eStat_HIGH);


| A2 | A1 | A0 | 7位地址（十六进制） | 完整写地址 | 完整读地址 |
|----|----|----|-------------------|------------|------------|
| 0  | 0  | 0  | 0x20              | 0x40       | 0x41       |
| 0  | 0  | 1  | 0x21              | 0x42       | 0x43       |
| 0  | 1  | 0  | 0x22              | 0x44       | 0x45       |
| 0  | 1  | 1  | 0x23              | 0x46       | 0x47       |
| 1  | 0  | 0  | 0x24              | 0x48       | 0x49       |
| 1  | 0  | 1  | 0x25              | 0x4A       | 0x4B       |
| 1  | 1  | 0  | 0x26              | 0x4C       | 0x4D       |
| 1  | 1  | 1  | 0x27              | 0x4E       | 0x4F       |

 */

#ifndef SRC_DRIVER_PCF8574_H_
#define SRC_DRIVER_PCF8574_H_

#include <stdint.h>

class PCF8574 {
public:
	enum EPin{
		ePin_From,
		ePin_0 = ePin_From,
		ePin_1,
		ePin_2,
		ePin_3,
		ePin_4,
		ePin_5,
		ePin_6,
		ePin_7,
		ePin_To,
	};

	enum EStat{
		eStat_From = 0,
		eStat_LOW = eStat_From,
		eStat_HIGH,
		eStat_To,
	};
public:
	PCF8574(const uint8_t address);
	virtual ~PCF8574();
	void Set(const EPin ePin, const EStat eStat);

private:
	void Init();

private:
	uint8_t m_Stat;
	uint8_t m_address;	//设备地址
};

#endif /* SRC_DRIVER_PCF8574_H_ */
