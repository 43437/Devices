/*
 * PCF8574.cpp
 *
 *  Created on: Aug 25, 2023
 *      Author: geey17u
 	PCF8574 objPCF8574(0x40);
	PCF8574 objPCF8574_1(0x42);

	I2C Speed Mode: Standard Mode
  	I2C Clock Speed(Hz): 100k
	Clock No Stretch Mode: Disabled
	Primary Address Length selection: 7bit
	Dual Address Acknowledged: Disabled
	Primary slave address: 0
	General Call address detection Disabled
 */

#include "PCF8574.h"
#include "i2c.h"

static void HW_I2C_WRITE(uint8_t address, uint8_t dat)
{
	auto ret = HAL_I2C_Master_Transmit(&hi2c1, address, &dat, 1, 0xffff);
	if (HAL_OK != ret){

	}
}

PCF8574::PCF8574(const uint8_t address) : m_address(address){
	// TODO Auto-generated constructor stub
	Init();
}

PCF8574::~PCF8574() {
	// TODO Auto-generated destructor stub
}

void PCF8574::Set(const EPin ePin,  const EStat eStat){


	switch (eStat){
	case eStat_HIGH:
		m_Stat |= 1 << ePin;
		break;
	case eStat_LOW:
		m_Stat &= ~(1 << ePin);
		break;
	default:
		break;
	}

	HW_I2C_WRITE(m_address, m_Stat);
}

void PCF8574::Init(){

	m_Stat = 0x00;
}
