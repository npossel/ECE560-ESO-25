// Automatically generated file. Do not edit if you plan to regenerate it.
#include "region.h"
const REGION_T RegionTable[] = {
	{0x000000d5, 0x000000e0, "Reset_Handler"}, // 0
	{0x000000e1, 0x000000e2, "NMI_Handler"}, // 1
	{0x000000e3, 0x000000e4, "HardFault_Handler"}, // 2
	{0x000000e5, 0x000000e6, "SVC_Handler"}, // 3
	{0x000000e7, 0x000000e8, "PendSV_Handler"}, // 4
	{0x000000e9, 0x000000ea, "SysTick_Handler"}, // 5
	{0x000000fd, 0x0000013a, "__aeabi_uidivmod"}, // 6
	{0x0000013d, 0x00000162, "__scatterload"}, // 7
	{0x0000016d, 0x000001b0, "Delay"}, // 8
	{0x000001b5, 0x00000208, "Init_RGB_LEDs"}, // 9
	{0x00000219, 0x00000224, "LCD_Erase"}, // 10
	{0x00000229, 0x000003ec, "LCD_Fill_Buffer"}, // 11
	{0x000003ed, 0x000003ee, "__scatterload_null"}, // 12
	{0x00000411, 0x00000534, "LCD_GPIO_Init"}, // 13
	{0x00000549, 0x00000608, "LCD_Init"}, // 14
	{0x00000629, 0x0000072c, "LCD_Start_Rectangle"}, // 15
	{0x00000731, 0x00000748, "LCD_TS_Init"}, // 16
	{0x00000751, 0x000007ac, "LCD_Text_Init"}, // 17
	{0x000007bd, 0x00000aac, "LCD_Text_PrintChar"}, // 18
	{0x00000aad, 0x00000afc, "LCD_Text_PrintStr"}, // 19
	{0x00000b01, 0x00000bcc, "LCD_Write_Rectangle_Pixe"}, // 20
	{0x00000bd1, 0x00000c0c, "PIT_IRQHandler"}, // 21
	{0x00000c15, 0x00000c84, "PWM_Init"}, // 22
	{0x00000c91, 0x00000ce8, "Process_Profile_Sample"}, // 23
	{0x00000cf9, 0x00000e04, "SystemInit"}, // 24
	{0x00000e31, 0x00000e3c, "TPM0_IRQHandler"}, // 25
	{0x00000e41, 0x00000fe8, "main"}, // 26
	{0x00000fe9, 0x00000ff6, "__scatterload_copy"}, // 27
	{0x00000ff7, 0x00001004, "__scatterload_zeroinit"}, // 28
}; 
const unsigned NumProfileRegions=29;
volatile unsigned RegionCount[29];
unsigned SortedRegions[29];
