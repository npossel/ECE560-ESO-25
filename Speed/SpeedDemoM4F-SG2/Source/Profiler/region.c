// Automatically generated file. Do not edit if you plan to regenerate it.
#include "region.h"
const REGION_T RegionTable[] = {
	{0x00000411, 0x00000418, "__main"}, // 0
	{0x00000419, 0x00000444, "__scatterload_rt2"}, // 1
	{0x0000044d, 0x00000466, "__scatterload_copy"}, // 2
	{0x00000469, 0x00000484, "__scatterload_zeroinit"}, // 3
	{0x000004b1, 0x000004b2, "init_data_bss"}, // 4
	{0x000004b3, 0x000004be, "Reset_Handler"}, // 5
	{0x000004bf, 0x000004c0, "NMI_Handler"}, // 6
	{0x000004c1, 0x000004c2, "HardFault_Handler"}, // 7
	{0x000004c3, 0x000004c4, "MemManage_Handler"}, // 8
	{0x000004c5, 0x000004c6, "BusFault_Handler"}, // 9
	{0x000004c7, 0x000004c8, "UsageFault_Handler"}, // 10
	{0x000004c9, 0x000004ca, "SVC_Handler"}, // 11
	{0x000004cb, 0x000004cc, "DebugMon_Handler"}, // 12
	{0x000004cd, 0x000004ce, "PendSV_Handler"}, // 13
	{0x000004cf, 0x000004d0, "SysTick_Handler"}, // 14
	{0x000004f9, 0x00000578, "strcmp"}, // 15
	{0x00000579, 0x0000057a, "__use_two_region_memory"}, // 16
	{0x0000057b, 0x0000057c, "__rt_heap_escrow$2region"}, // 17
	{0x0000057d, 0x0000057e, "__rt_heap_expand$2region"}, // 18
	{0x0000057f, 0x00000588, "__read_errno"}, // 19
	{0x00000589, 0x00000594, "__set_errno"}, // 20
	{0x00000595, 0x0000059c, "__aeabi_errno_addr"}, // 21
	{0x0000059d, 0x000005a4, "__user_libspace"}, // 22
	{0x000005a5, 0x000005ee, "__user_setup_stackheap"}, // 23
	{0x000005ef, 0x00000600, "exit"}, // 24
	{0x00000601, 0x00000608, "_sys_exit"}, // 25
	{0x0000060d, 0x0000060e, "__use_no_semihosting_swi"}, // 26
	{0x00000611, 0x00000640, "Control_RGB_LEDs"}, // 27
	{0x00000645, 0x0000065a, "Delay"}, // 28
	{0x0000065d, 0x00000664, "Disable_Profiling"}, // 29
	{0x00000669, 0x00000670, "Enable_Profiling"}, // 30
	{0x00000675, 0x0000084a, "Find_Nearest_Waypoint"}, // 31
	{0x00000879, 0x000008cc, "Init_Profiling"}, // 32
	{0x000008dd, 0x00000932, "Init_RGB_LEDs"}, // 33
	{0x00000945, 0x000009a0, "PIT0_IRQHandler"}, // 34
	{0x000009a9, 0x000009f4, "PIT0_Init"}, // 35
	{0x00000a05, 0x00000a14, "PIT0_Start"}, // 36
	{0x00000a19, 0x00000a82, "Process_Profile_Sample"}, // 37
	{0x00000a95, 0x00000b22, "Sort_Profile_Regions"}, // 38
	{0x00000b31, 0x00000bea, "SystemInit"}, // 39
	{0x00000c09, 0x00000c1a, "__ARM_common_memcpy4_10"}, // 40
	{0x00000c1b, 0x00000c40, "__ARM_fpclassifyf"}, // 41
	{0x00000c41, 0x00000d2a, "__hardfp_acosf"}, // 42
	{0x00000d59, 0x00000f4e, "__hardfp_atan2f"}, // 43
	{0x00000fa9, 0x000010c0, "__hardfp_cosf"}, // 44
	{0x000010f9, 0x00001250, "__hardfp_sinf"}, // 45
	{0x00001289, 0x0000128e, "__mathlib_flt_infnan"}, // 46
	{0x0000128f, 0x00001294, "__mathlib_flt_infnan2"}, // 47
	{0x00001295, 0x0000129e, "__mathlib_flt_invalid"}, // 48
	{0x000012a5, 0x000012ae, "__mathlib_flt_underflow"}, // 49
	{0x000012b5, 0x000013f0, "__mathlib_rredf2"}, // 50
	{0x00001409, 0x00001464, "main"}, // 51
	{0x0000146d, 0x000014aa, "sqrtf"}, // 52
	{0x000014ab, 0x000014b4, "_fp_init"}, // 53
}; 
const unsigned NumProfileRegions=54;
volatile unsigned RegionCount[54];
unsigned SortedRegions[54];
