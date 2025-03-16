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
	{0x00000625, 0x00000728, "LCD_Start_Rectangle"}, // 15
	{0x00000729, 0x00000740, "LCD_TS_Init"}, // 16
	{0x00000749, 0x000007a4, "LCD_Text_Init"}, // 17
	{0x000007b9, 0x00000884, "LCD_Write_Rectangle_Pixe"}, // 18
	{0x00000889, 0x000008c4, "PIT_IRQHandler"}, // 19
	{0x000008cd, 0x0000093c, "PWM_Init"}, // 20
	{0x00000951, 0x000009a8, "Process_Profile_Sample"}, // 21
	{0x000009b9, 0x00000e58, "Sim_Enhance_PrintChar"}, // 22
	{0x00000e5d, 0x00000f68, "SystemInit"}, // 23
	{0x00000f95, 0x00000fa0, "TPM0_IRQHandler"}, // 24
	{0x00000fa5, 0x00000fc6, "main"}, // 25
	{0x00000fc7, 0x00000fd4, "__scatterload_copy"}, // 26
	{0x00000fd5, 0x00000fe2, "__scatterload_zeroinit"}, // 27
}; 
const unsigned NumProfileRegions=28;
volatile unsigned RegionCount[28];
unsigned SortedRegions[28];
