#ifndef GAME_H
#define GAME_H

// Game Configuration
#define GAME_PSEUDORAND (0)		// pseudo-randomize bounce angle using accelerometer data


// Game Constants
#define PADDLE_WIDTH (40)
#define PADDLE_HEIGHT (9)
#define PADDLE_Y_POS (LCD_HEIGHT-4-PADDLE_HEIGHT)
#define MAX_BASE_VEL (30)
#define BALL_RADIUS (14)
#define AUTO_PADDLE (1)
#define BASE_VEL_START (14)
#define FILLED_BALL (0)
#define POST_BOUNCE_DURATION (10) // Duration of ball color change after bounce
#define DRAW_TRAIL (0)

extern void Init_Ball(void);
extern void Update_Ball(void);
extern void Update_Paddle(void);
extern int returns, misses, num_players, stress;

#endif // GAME_H