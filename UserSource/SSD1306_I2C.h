#ifndef Init_SSD1306_I2C_H
#define Init_SSD1306_I2C_H

#include "stm32f0xx.h" 
#include "stm32f0xx_gpio.h"
#include "fonts.h"
#include "string.h"








/* Private SSD1306 structure */
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_t;

/* Private variable */
static SSD1306_t SSD1306;

static uint32_t ssd1306_I2C_Timeout;

typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SSD1306_COLOR_t;


 void SSD1306_Fill(SSD1306_COLOR_t color);
 void ssd1306_send(uint8_t control_byte, uint8_t data);
void 	ssd1306_Multisend(uint8_t control_byte, uint8_t* data, uint16_t count);
void SSD1306_ToggleInvert(void);
void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color) ;
void SSD1306_UpdateScreen_My(void);
void SSD1306_GotoXY(uint16_t x, uint16_t y);
char SSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color);
char SSD1306_Puts(char* str, FontDef_t* Font, SSD1306_COLOR_t color);
void Welcome_Screen(void);
void Init_SSD1306(SSD1306_COLOR_t color_init);
 







#endif

