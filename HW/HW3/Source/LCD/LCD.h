#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include "config.h"
#include "misc.h"
#include "colors.h"

#ifdef USING_RTOS
  #include "cmsis_os2.h"
#endif 

typedef struct {
	int32_t X, Y;
} PT_T;


extern int LCD_on;

#ifdef USING_RTOS
	extern osMutexId_t LCD_mutex;
	extern const osMutexAttr_t LCD_mutex_attr;
#endif

// Ported functions for common LCD API

/** Initialize the LCD
*/
 void LCD_Init(void);

/** Initialize the LCD RTOS objects
*/
 void LCD_RTOS_Init(void);

/** Set pixel color
*/
 void LCD_Plot_Pixel(PT_T * pos, COLOR_T * color);

/** Refresh LCD from local MCU frame buffer
*/
 void LCD_Refresh(void);

/** Fill entire LCD with given color
*/
 void LCD_Fill_Buffer(COLOR_T * color);
/** Fill specified rectangle with given color
*/
 void LCD_Fill_Rectangle(PT_T * p1, PT_T * p2, COLOR_T * color); // Not ported to T6963.c yet

/** Plot given byte of packed data at given position
*/
 void LCD_Plot_Packed_Pixels(uint8_t fill_byte, PT_T * pos);

/** Plot given byte of packed data at given position and next byte based on right-shift amount
*/
void LCD_Plot_Packed_Pixels_Unaligned(uint8_t fill_byte, uint8_t r_shift, PT_T * pos);

void LCD_24S_Write_Command(uint8_t command);
void LCD_24S_Write_Data(uint8_t data);

 uint32_t LCD_Start_Rectangle(PT_T * p1, PT_T * p2);						// Not ported to T6963.c yet

 void LCD_Write_Rectangle_Pixel(COLOR_T * color, unsigned int count);// Not ported to T6963.c yet
 
 void LCD_Set_BL(uint8_t on);
 void LCD_Set_Backlight_Brightness(uint32_t brightness_percent);
 void LCD_Set_Controller_Power_Mode(int normal, int idle, int sleep, int on);

 void LCD_Text_Set_Colors(COLOR_T * foreground, COLOR_T * background);

 void LCD_Erase(void);
 
 void LCD_Text_Test(void);
 int LCD_Text_Init(uint8_t font_num);
 void LCD_Text_PrintChar(PT_T * pos, char ch);
 void LCD_Text_PrintStr(PT_T * pos, char * str);
 void LCD_Text_PrintStr_RC( uint8_t  row, uint8_t col, char *  str);

 void Graphics_Test(void);
 void LCD_Draw_Line(PT_T * p1, PT_T * p2, COLOR_T * color);
 void LCD_Draw_Lines(PT_T * pts, int num, int connected, COLOR_T * c);
 void LCD_Draw_Circle(PT_T * p1, int radius, COLOR_T * color, int filled);
 void Rotate_Points(PT_T * ipt, int num_points, PT_T * center, float roll_rad, PT_T * opt);
 void Rotate_Points_3_dev(PT_T * ipt, int num_points, PT_T * center, float angle_rad, float pitch_rad, PT_T * opt);


 void LCD_Create_OS_Objects(void);
 void LCD_TS_Init(void);
 uint32_t LCD_TS_Read(PT_T * position);
 void LCD_TS_Blocking_Read(PT_T * position);
 void LCD_TS_Test(void);
 void LCD_TS_Calibrate(void);
 extern uint32_t g_LCD_TS_Settling_Delay;

 extern uint8_t G_LCD_char_width, G_LCD_char_height; // Screen dimensions in characters

#endif
