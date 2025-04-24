#ifndef MMA8451_H
#define MMA8451_H
#include <stdint.h>

// Select one of these orientations

#define UP_AXIS_Z 1 // FRDM board laying flat and face-up
// Roll depends on Y vs. Z. Pitch depends on X vs. Y and Z.

// #define UP_AXIS_X 1 // FDRM board standing on end, USB connectors are up
// Roll depends on Y vs. X. Pitch depends on Z vs. X and Y.

#define MMA_ADDR 0x3A

#define REG_XHI 0x01
#define REG_XLO 0x02
#define REG_YHI 0x03
#define REG_YLO 0x04
#define REG_ZHI	0x05
#define REG_ZLO 0x06

#define REG_WHOAMI 0x0D
#define REG_CTRL1  0x2A
#define REG_CTRL4  0x2D

#define WHOAMI 0x1A

#define COUNTS_PER_G  (16384.0f)
#define M_PI 					(3.14159265)
#define M_PI_2 				(M_PI/2.0f)
#define M_PI_4 				(M_PI/4.0f)
#define M_180_OVER_PI (180.0/M_PI)

#define MU 	(0.01f)
#define SIGN(x) (x>=0? 1:-1)

int init_mma(void);
void read_full_xyz(void);
void read_xyz(void);
void convert_xyz_to_roll_pitch(void);

extern float roll, pitch;
extern int16_t acc_X, acc_Y, acc_Z;

#endif
