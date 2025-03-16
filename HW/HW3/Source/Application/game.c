#include <math.h>
#include <cmsis_os2.h>

#include "misc.h"
#include "LCD.h"
#include "ST7789.h"
#include "font.h"
#include "game.h"
#include "MMA8451.h"

#include "sound.h"
#include "debug.h"

PT_T p_ball, v;
int p_paddle = LCD_CENTER_X;
int returns = 0;
int misses = 0;
float base_vel = BASE_VEL_START;
int num_players=0;
int stress=0;
int post_bounce = 0; 
COLOR_T orig_ball_color = {0x20, 0xc0, 0x00};
COLOR_T ball_color, trail_color;

void Reload_Ball(void) { // Function not used 
	for (int r=1; r <= BALL_RADIUS; r++) {
		osMutexAcquire(LCD_mutex, osWaitForever);
		LCD_Draw_Circle(&p_ball, r, &white, 1);
		osMutexRelease(LCD_mutex);
		osDelay(120);
	}
}

void Update_Colors(void){
	ball_color.R = MIN(orig_ball_color.R + 20*post_bounce, 255); 
	ball_color.G = MIN(orig_ball_color.G + 20*post_bounce, 255); 
	ball_color.B = MIN(orig_ball_color.B + 20*post_bounce, 255); 

#if DRAW_TRAIL
	trail_color.R = CLIP(0,(ball_color.R*(post_bounce-1))/POST_BOUNCE_DURATION, 255);
	trail_color.G = CLIP(0,(ball_color.G*(post_bounce-1))/POST_BOUNCE_DURATION, 255);
	trail_color.B = CLIP(0,(ball_color.B*(post_bounce-1))/POST_BOUNCE_DURATION, 255);
#else
	trail_color.R = 0;
	trail_color.G = 0; 
	trail_color.B = 0; 
#endif
}
	
void Init_Ball(void) {
	float a;

	p_ball.X = LCD_CENTER_X;
	p_ball.Y = 60;
	base_vel = BASE_VEL_START;
	a = 0.0;

#if GAME_PSEUDORAND // perturb bounces using pseudo-random accelerometer data
	v.X = base_vel*(((acc_Z & 0x3f)-32.0)/20);
#else
	v.X = base_vel/4;
#endif
	v.Y = base_vel;

	Update_Colors();
}


void Update_Ball(void) {
	static PT_T pp;
	int missed = 0;
	
	pp = p_ball;
	// Advance ball
	p_ball.X += v.X;
	p_ball.Y += v.Y;
	
	// wall bounces
	if ((p_ball.X > LCD_WIDTH-BALL_RADIUS-1) || (p_ball.X <= BALL_RADIUS)) {
		v.X *= -1;
		stress++;
		Add_Sound(1);
	}
	if (p_ball.Y < BALL_RADIUS + ROW_TO_Y(2)) {
		v.Y *= -1;
		stress++;
		Add_Sound(1);
	}
	// Paddle bounce?
	if (p_ball.Y >= PADDLE_Y_POS - BALL_RADIUS - 1 ) {
		if ((p_ball.X >= p_paddle) && (p_ball.X <= p_paddle+PADDLE_WIDTH)) {
			p_ball.Y = PADDLE_Y_POS - BALL_RADIUS - 1;
			v.Y *= -1;
			base_vel *= 1.02;
			returns++;
			stress++;
			post_bounce = POST_BOUNCE_DURATION;
		} else {
			misses++;
			returns = 0;
			base_vel *= 1.03;
			missed = 1;
			stress += 10;
		}
		Add_Sound(0);
		base_vel = MIN(base_vel, MAX_BASE_VEL);
	}
	Update_Colors();

	if (post_bounce > 0)
		post_bounce--;

	osMutexAcquire(LCD_mutex, osWaitForever);
	LCD_Draw_Circle(&pp, BALL_RADIUS, &trail_color, FILLED_BALL);
	osMutexRelease(LCD_mutex);
	
	if (missed) {
		Init_Ball();
		// Reload_Ball();
	}	else {
		osMutexAcquire(LCD_mutex, osWaitForever);
		LCD_Draw_Circle(&p_ball, BALL_RADIUS, &ball_color, FILLED_BALL);
		osMutexRelease(LCD_mutex);
	}
}

void Update_Paddle(void) {
	static PT_T p1, p2;
	static COLOR_T paddle_color = {100, 10, 100};
	int c;
	static int old_p_paddle;
	int redraw=0;
	
	redraw = (old_p_paddle != p_paddle)? 1 : 0;

	if (num_players == 0)
		p_paddle = p_ball.X - PADDLE_WIDTH/2;
	else
		p_paddle += roll;

	p_paddle = CLIP(0, p_paddle, LCD_WIDTH-1-PADDLE_WIDTH);
	
	if (redraw) {
		// Optimize later: redraw part of rectangle if old, new paddles overlap
		// Erase old paddle
		p1.X = old_p_paddle;
		p1.Y = PADDLE_Y_POS;
		p2.X = p1.X + PADDLE_WIDTH;
		p2.Y = p1.Y + PADDLE_HEIGHT;
		old_p_paddle = p_paddle; 

		osMutexAcquire(LCD_mutex, osWaitForever);
		LCD_Fill_Rectangle(&p1, &p2, &black); 		
		osMutexRelease(LCD_mutex);

		// Draw new paddle
		p1.X = p_paddle;
		p1.Y = PADDLE_Y_POS;
		p2.X = p1.X + PADDLE_WIDTH;
		p2.Y = p1.Y + PADDLE_HEIGHT;

		c = 100+roll*4;
		paddle_color.R = CLIP(0,c,255);
		c = 100-roll*4;
		paddle_color.B = CLIP(0,c,255);
		c = 100+pitch*4;
		paddle_color.G = CLIP(0,c,255);

		osMutexAcquire(LCD_mutex, osWaitForever);
		LCD_Fill_Rectangle(&p1, &p2, &paddle_color); 		
		osMutexRelease(LCD_mutex);
	}
}
