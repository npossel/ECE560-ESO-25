#ifndef COLORS_H
#define COLORS_H
#include <stdint.h>
#include "config.h"

#define DEV_PACKED_PIXEL_COLOR_T 			(0)

#if DEV_PACKED_PIXEL_COLOR_T
typedef 
	union __attribute__((packed)) {
		struct __attribute__((packed)) {
			uint8_t G:6, R:5, B:5; // note: using 5-6-5 color mode for LCD. 
		};
		uint8_t Byte[2];
} COLOR_T;

#define GET_B1(n) (n->Byte[0])
#define GET_B2(n) (n->Byte[1])
#define CVT_R(x) (x/8)
#define CVT_G(x) (x/4)
#define CVT_B(x) (x/8)

#else

typedef struct {
	uint8_t R, G, B; // note: using 5-6-5 color mode for LCD. 
									 // Values are left aligned here
} COLOR_T;

#define GET_B1(n) (((n->R&0xf8) | ((n->G&0xe0)>>5)))
#define GET_B2(n) (((n->G&0x1c)<<3) | ((n->B&0xf8)>>3))
#define CVT_R(x) (x)
#define CVT_G(x) (x)
#define CVT_B(x) (x)

#endif



extern COLOR_T black, 
	white,
	red,
	green,
	blue,
	yellow,
	cyan,
	magenta,
	dark_red,
	dark_green,
	dark_blue,
	dark_yellow,
	dark_cyan,
	dark_magenta,
	orange,
	light_gray, 
	dark_gray,
	light_blue,
	light_red,
	light_green;

#endif // COLORS_H
