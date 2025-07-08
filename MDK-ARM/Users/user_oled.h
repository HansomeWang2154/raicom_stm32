#ifndef _user_oled_h
#define _user_oled_h

#include "main.h"

#define _OLED_ADDR 0x78

#define _OLED_MODE 			0
#define _OLED_SIZE 			8
#define _OLED_XLevelL		0x00
#define _OLED_XLevelH		0x10
#define _OLED_MAX_COLUMN	128
#define _OLED_MAX_ROW			32
#define	_OLED_BRIGHT			0xFF 
#define _OLED_X_WIDTH 		128
#define _OLED_Y_WIDTH 		32	  

typedef enum
{
	OLED_cmd,
	OLED_data
}OLED_WR_MODE;

typedef enum 
{
	OLED_on,
	OLED_off,
	OLED_clear,
	OLED_test
}OLED_DISP_MODE;


typedef struct
{
	uint8_t lev;

}LIGHT_TYPEDEF;
extern LIGHT_TYPEDEF LIGHT;



void User_OledInit(void);
void User_OledDisplayStatus(OLED_DISP_MODE mode);
void User_OledShowChinese(uint8_t x,uint8_t y,uint8_t no);
void User_OledShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
void User_OledShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size);
void User_OledDrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);

void User_OledShow(void);
#endif



