/*
 * common.h
 *
 *  Created on: 2025年6月14日
 *      Author: geey17u
 */

#ifndef INC_W5500DEV_H_
#define INC_W5500DEV_H_

extern "C"{
#include <cstdint>
}

class W5500Dev{
public:
	enum EDHCPMode{
		eDHCPMode_From = 0,
		eDHCPMode_Static = eDHCPMode_From,
		eDHCPMode_DHCP,
		eDHCPMode_To,
	};
	struct SNetInfo
	{
	   uint8_t mac[6];  ///< Source Mac Address
	   uint8_t ip[4];   ///< Source IP Address
	   uint8_t sn[4];   ///< Subnet Mask
	   uint8_t gw[4];   ///< Gateway IP Address
	   uint8_t dns[4];  ///< DNS server IP Address
	   EDHCPMode dhcp;  ///
	};
public:
	W5500Dev();
	W5500Dev(const SNetInfo& stuNetInfo);
	void Enable();
	void Disable();
	bool IsEnable();

private:
	void Reset();
	void Init();

private:
	SNetInfo m_stuNetInfo;
};

#endif /* INC_W5500DEV_H_ */
