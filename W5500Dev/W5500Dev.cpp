/*
 * common.c
 *
 *  Created on: 2025年6月14日
 *      Author: geey17u
 *
 *      W5500Dev obj;
 *      int8_t iRet = socket(0, Sn_MR_TCP, 0, 0);
		  if (0 != iRet)
		  {
			  printf("get socket failed, ret[%d]\r\n", iRet);
		  }
			uint8_t ipServer[4] = {0};
		  ipServer[0] = 192;
		  ipServer[1] = 168;
		  ipServer[2] = 2;
		  ipServer[3] = 1;

		  iRet = connect(0, ipServer, 8888);
		  if (SOCK_OK != iRet)
		  {
			  printf("connect failed. ret[%d]\r\n", iRet);
		  }
		  send(0, (uint8_t*)buf, strlen(buf));
		  recv(0, rbuf, 20);
 */
#include "W5500Dev.h"
extern "C"{
#include <stdio.h>
#include "stm32f1xx_hal.h"
//一定要在stm32f1xx_hal.h后面，因为有宏定义冲突
#include "Ethernet/wizchip_conf.h"
#include <cstring>
}
//进入临界区
static void SPI_CrisEnter(void)
{
    __set_PRIMASK(1);
}
//退出临界区
static void SPI_CrisExit(void)
{
    __set_PRIMASK(0);
}

//hardware api begin

//CPOL 0
//CPHA 0
//MSB
//8bit
//18MB

//    PA5(SPI1_SCK)		------>SCLK
//    PA6(SPI1_MISO)	------>MISO
//    PA7(SPI1_MOSI)	------>MOSI
//	  PA4				------>CS
//	  PB10				------>IT
//	  PB1				------>RST

extern "C"{
extern SPI_HandleTypeDef hspi1;
}

static void HW_SPI_CS_Select(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);	//W5500_NSS_L;
}

static void HW_SPI_CS_Deselect(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);		//W5500_NSS_H;
}

//SPI写一字节数据
static void HW_SPI_WriteByte(uint8_t Tdata)
{
	HAL_SPI_Transmit(&hspi1, &Tdata, 1, 0xffff);
}

//SPI读取一字节数据
static uint8_t HW_SPI_ReadByte(void)
{
    uint8_t data=0;
	HAL_SPI_Receive(&hspi1, &data, 1, 0xffff);
    return data;
}

static void HW_Reset(const uint8_t u8Stat)
{
	auto ePinStat = u8Stat == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, ePinStat);
}
//hardware api end

static void RegisterFunction(void)
{
	reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);    //注册临界区回调函数

//	#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
	  reg_wizchip_cs_cbfunc(HW_SPI_CS_Select, HW_SPI_CS_Deselect);//注册片选回调函数
//	#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
//	  reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);
//	#else
//	   #if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
//	      #error "Unknown _WIZCHIP_IO_MODE_"
//	   #else
//	      reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
//	   #endif
//	#endif
	  reg_wizchip_spi_cbfunc(HW_SPI_ReadByte, HW_SPI_WriteByte);    //SPI读写字节回调函数
}

//初始化芯片参数
static void ChipParametersConfiguration(void)
{
  uint8_t tmp;
  uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
        {
            printf("WIZCHIP Initialized fail.\r\n");
            while(1);
        }

    do{
        if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
            {
            printf("Unknown PHY Link stauts.\r\n");
            }
      }while(tmp == PHY_LINK_OFF);
}

//初始化网络通讯参数,mac,ip等
static void NetworkParameterConfiguration(const wiz_NetInfo& gWIZNETINFO)  //Intialize the network information to be used in WIZCHIP

{

  uint8_t tmpstr[6];

  ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);

  ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);

  ctlwizchip(CW_GET_ID,(void*)tmpstr);
    //串口打印出参数
    printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
    printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
            gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
    printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
    printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
    printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
    printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
    printf("======================\r\n");

}

W5500Dev::W5500Dev() : W5500Dev( { .mac = {0x78, 0x83, 0x68, 0x88, 0x56, 0x38},
									.ip =  {192, 168, 2,2},
									.sn =  {255,255,255,0},
									.gw =  {192, 168,2,1},
									.dns = {8,8,8,8},
									.dhcp = eDHCPMode_Static})
{
}

W5500Dev::W5500Dev(const SNetInfo& stuNetInfo) : m_stuNetInfo(stuNetInfo)
{
	Init();
}

void W5500Dev::Enable()
{
	reg_wizchip_cs_cbfunc(HW_SPI_CS_Select, HW_SPI_CS_Deselect);//注册片选回调函数
}

void W5500Dev::Disable()
{
	HW_SPI_CS_Deselect();

	reg_wizchip_cs_cbfunc(nullptr, nullptr);
}

bool W5500Dev::IsEnable()
{
	return (HW_SPI_CS_Select == WIZCHIP.CS._select
			&& HW_SPI_CS_Deselect == WIZCHIP.CS._deselect);
}

//W5500芯片复位
void W5500Dev::Reset(void)
{
	HW_Reset(0);//RESET置低复位
    HAL_Delay(50);
    HW_Reset(1);//RESET置高
    HAL_Delay(10);
}

static wiz_NetInfo ConvertNetInfo(const W5500Dev::SNetInfo& stuNetInfo){

	wiz_NetInfo stuWizNetInfo;

	memcpy(stuWizNetInfo.mac, stuNetInfo.mac, 6);
	memcpy(stuWizNetInfo.ip, stuNetInfo.ip, 4);
	memcpy(stuWizNetInfo.sn, stuNetInfo.sn, 4);
	memcpy(stuWizNetInfo.gw, stuNetInfo.gw, 4);
	memcpy(stuWizNetInfo.dns, stuNetInfo.dns, 4);
	stuWizNetInfo.dhcp = stuNetInfo.dhcp == W5500Dev::eDHCPMode_DHCP ? NETINFO_DHCP : NETINFO_STATIC;

	return stuWizNetInfo;
}

void W5500Dev::Init()
{
	Reset();
	RegisterFunction();
	ChipParametersConfiguration();
	NetworkParameterConfiguration(ConvertNetInfo(m_stuNetInfo));
}
