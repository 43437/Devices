/*
 * C12864ST7567.h
 *
 *  Created on: 2023年4月16日
 *      Author: geey17u
 *
  rLCD.Init();
  rLCD.SetReverseMode(true);
  rLCD.ClearScreen();
  rLCD.Display("Hello", 6, 1);			//display at 6, 1
  rLCD.Display("World", 2, 2);			//display at 2, 2
  rLCD.Display("Great Of All", 3);		//display at 3, 0
  rLCD.Display("New Content", 4, 15);	//display at 4, 15 and wrap to next row
  rLCD.Display("1", 0, 0);				//display at 0, 0
  rLCD.Display("2", 0, 19);				//display at 0, 19
  rLCD.Display("3", 7, 0);				//display at 7, 0
  rLCD.Display("4", 7, 19);				//display at 7, 19

  HAL_Delay(1500);

  rLCD.ClearLine(3, 3);					//clear row 3 from col 3

  HAL_Delay(1500);

  rLCD.ClearScreen();

  rLCD.ClearOne(3, 1);
  rLCD.Display(":", 3, 1);
  rLCD.Display("abcdefGhijklmnopqrstuvwxyz");

  HAL_Delay(1500);

  rLCD.Display("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60");  // 在(0,8)位置显示字符串
 */

#ifndef SRC_USERS_C12864ST7567_H_
#define SRC_USERS_C12864ST7567_H_

#include <stdint.h>

class C12864ST7567 {

public:
	static C12864ST7567& Instance();
	virtual ~C12864ST7567();

	void Init();

	void SetReverseMode(bool isReverse);

	//display string at row, col pos, support auto wrap to the next line
	void Display(const char* str, uint8_t row = 0, uint8_t col = 0);

	//clear one char at row, col pos
	void ClearOne(uint8_t row, uint8_t col);

	//clear line at row from col to the end
	void ClearLine(uint8_t row, uint8_t col = 0);

	//clear whole screen
	void ClearScreen();

private:
	void DisplayChar(uint8_t x, uint8_t y, char c);                    // 显示单个ASCII字符
	void DisplayString(uint8_t x, uint8_t y, const char* str);         // 显示ASCII字符串

	void Write(uint8_t Dat, uint8_t Flag);

	void WriteCommand(uint8_t Dat);
	void WriteData(uint8_t Dat);

	void LCDInitial(void);

	void PageSet(uint8_t Y);
	void ColumnSet(uint8_t L);

	C12864ST7567();
};

#endif /* SRC_USERS_C12864ST7567_H_ */
