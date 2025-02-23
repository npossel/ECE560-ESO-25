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
	{0x00000611, 0x000006ba, "Calc_Bearing"}, // 27
	{0x000006c9, 0x000006f8, "Control_RGB_LEDs"}, // 28
	{0x000006fd, 0x00000712, "Delay"}, // 29
	{0x00000715, 0x0000071c, "Disable_Profiling"}, // 30
	{0x00000721, 0x00000728, "Enable_Profiling"}, // 31
	{0x0000072d, 0x00000918, "Find_Nearest_Waypoint"}, // 32
	{0x0000093d, 0x00000990, "Init_Profiling"}, // 33
	{0x000009a1, 0x000009f6, "Init_RGB_LEDs"}, // 34
	{0x00000a09, 0x00000a64, "PIT0_IRQHandler"}, // 35
	{0x00000a6d, 0x00000ab8, "PIT0_Init"}, // 36
	{0x00000ac9, 0x00000ad8, "PIT0_Start"}, // 37
	{0x00000add, 0x00000b46, "Process_Profile_Sample"}, // 38
	{0x00000b59, 0x00000be6, "Sort_Profile_Regions"}, // 39
	{0x00000bf5, 0x00000cae, "SystemInit"}, // 40
	{0x00000ccd, 0x00000cde, "__ARM_common_memcpy4_10"}, // 41
	{0x00000cdf, 0x00000d04, "__ARM_fpclassifyf"}, // 42
	{0x00000d05, 0x00000dee, "__hardfp_acosf"}, // 43
	{0x00000e1d, 0x00001012, "__hardfp_atan2f"}, // 44
	{0x0000106d, 0x00001184, "__hardfp_cosf"}, // 45
	{0x000011bd, 0x00001314, "__hardfp_sinf"}, // 46
	{0x0000134d, 0x00001352, "__mathlib_flt_infnan"}, // 47
	{0x00001353, 0x00001358, "__mathlib_flt_infnan2"}, // 48
	{0x00001359, 0x00001362, "__mathlib_flt_invalid"}, // 49
	{0x00001369, 0x00001372, "__mathlib_flt_underflow"}, // 50
	{0x00001379, 0x000014b4, "__mathlib_rredf2"}, // 51
	{0x000014cd, 0x00001526, "main"}, // 52
	{0x00001531, 0x0000156e, "sqrtf"}, // 53
	{0x0000156f, 0x00001578, "_fp_init"}, // 54
}; 
const unsigned NumProfileRegions=55;
volatile unsigned RegionCount[55];
unsigned SortedRegions[55];
