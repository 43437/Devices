#include "Driver/DS1302.h"
#include <cstring>
#include <cstdio>
#include <string>

//DS1302地址定义
#define ds1302_sec_addr			0x80		//秒数据地址
#define ds1302_min_addr			0x82		//分数据地址
#define ds1302_hr_addr			0x84		//时数据地址
#define ds1302_date_addr			0x86		//日数据地址
#define ds1302_month_addr		0x88		//月数据地址
#define ds1302_day_addr			0x8a		//星期数据地址
#define ds1302_year_addr			0x8c		//年数据地址
#define ds1302_control_addr		0x8e		//控制数据地址
#define ds1302_charger_addr		0x90
#define ds1302_clkburst_addr	0xbe

#define PIN_SCK		GPIO_PIN_4
#define PIN_IO		GPIO_PIN_5
#define PIN_RST		GPIO_PIN_6

extern "C"{
extern void Delayus(uint16_t nus);
extern void Delayms(uint32_t nms);
}

enum EData{
	eDataL,
	eDataH,
};

static void HW_DAT_IN_MODE()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void HW_DAT_OUT_MODE()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void HW_SCK(EData high_low)
{
	GPIO_PinState eStat = (eDataL == high_low) ? GPIO_PIN_RESET : GPIO_PIN_SET;
	HAL_GPIO_WritePin(GPIOA, PIN_SCK, eStat);
}

static void HW_DAT_SET(EData stat)
{
	GPIO_PinState eStat = (eDataL == stat) ? GPIO_PIN_RESET : GPIO_PIN_SET;
	HAL_GPIO_WritePin(GPIOA, PIN_IO, eStat);
}

static EData HW_DAT_GET()
{
	return GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOA, PIN_IO) ? eDataH: eDataL;
}



static void Reset_H()
{
	HAL_GPIO_WritePin(GPIOA, PIN_RST, GPIO_PIN_SET);
}

static void Reset_L()
{
	HAL_GPIO_WritePin(GPIOA, PIN_RST, GPIO_PIN_RESET);
}

static void SCK_H()
{
	HW_SCK(eDataH);
}

static void SCK_L()
{
	HW_SCK(eDataL);
}

static void SetDataIn()
{
	HW_DAT_IN_MODE();
}

static void SetDataOut()
{
	HW_DAT_OUT_MODE();
}

static void SetData(EData stat)
{
	HW_DAT_SET(stat);
}

static EData GetData()
{
	return HW_DAT_GET();
}



static void Write(uint8_t addr, uint8_t d)
{
	uint8_t i;
	Reset_H();
//	Delayus(2);
	addr = addr & 0xFE;
	SetDataOut();
	for (i = 0; i < 8; i ++)
	{
		if (addr & 0x01)
		{
			SetData(eDataH);
		}
		else
		{
			SetData(eDataL);
		}
		SCK_H();
//		Delayus(2);
		SCK_L();
//		Delayus(2);
		addr = addr >> 1;
		}
	for (i = 0; i < 8; i ++)
	{
		if (d & 0x01)
		{
			SetData(eDataH);
		}
		else
		{
			SetData(eDataL);
		}
		SCK_H();
//		Delayus(2);
		SCK_L();
//		Delayus(2);
		d = d >> 1;
	}
	Reset_L();
}

static uint8_t Read(uint8_t addr) {

	uint8_t i, temp;
	Reset_H();
//	Delayus(2);
	addr = addr | 0x01;
	SetDataOut();
	for (i = 0; i < 8; i ++)
	{
		if (addr & 0x01)
		{
			SetData(eDataH);
		}
		else
		{
			SetData(eDataL);
		}
		SCK_H();
//		Delayus(2);
		SCK_L();;
//		Delayus(2);
		addr = addr >> 1;
	}

	SetDataIn();
	for (i = 0; i < 8; i ++)
	{
		temp = temp >> 1;
		if (eDataH == GetData())
		{
			temp |= 0x80;
		}
		else
		{
			temp &= 0x7F;
		}
		SCK_H();
//		Delayus(2);
		SCK_L();
//		Delayus(2);
	}
	Reset_L();
	return temp;
}

std::string DS1302::SDateTime::ReadableString()
{
    static char tm_buf[] = "25-06-01 10:00:00";

	sprintf(tm_buf, "%02X-%02X-%02X %02X:%02X:%02X", m_u8Year, m_u8Month, m_u8Day, m_u8Hour, m_u8Min, m_u8Sec);
	return std::string(tm_buf);
}

DS1302& DS1302::Instance()
{
	static DS1302 __instance;
	return __instance;
}
DS1302::~DS1302()
{
}

DS1302::DS1302()
{
}

void DS1302::SetDateTime(const SDateTime& stuDateTime)
{
	Write(ds1302_control_addr,0x00);			//关闭写保护
	Write(ds1302_sec_addr,0x80);				//暂停时钟
	//Write(ds1302_charger_addr,0xa9);	    //涓流充点

	Write(ds1302_year_addr, 	stuDateTime.m_u8Year);		//年
	Write(ds1302_month_addr, stuDateTime.m_u8Month);		//月
	Write(ds1302_date_addr, 	stuDateTime.m_u8Day);		//日
	Write(ds1302_hr_addr, 	stuDateTime.m_u8Hour);		//时
	Write(ds1302_min_addr, 	stuDateTime.m_u8Min);		//分
	Write(ds1302_sec_addr, 	stuDateTime.m_u8Sec);		//秒
//	Write(ds1302_day_addr, time_buf[7]);		//周

	Write(ds1302_control_addr, 0x80);			//打开写保护
}

void DS1302::GetDateTime(SDateTime& stuDateTime)
{
	stuDateTime.m_u8Year	=Read(ds1302_year_addr);		//年
	stuDateTime.m_u8Month=Read(ds1302_month_addr);		//月
	stuDateTime.m_u8Day	=Read(ds1302_date_addr);			//日
	stuDateTime.m_u8Hour=Read(ds1302_hr_addr);			//时
	stuDateTime.m_u8Min	=Read(ds1302_min_addr);			//分
	stuDateTime.m_u8Sec	=(Read(ds1302_sec_addr))&0x7f;//秒，屏蔽秒的第7位，避免超出59
}
