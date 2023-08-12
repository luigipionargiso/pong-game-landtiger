#include "game.h"
#include "..\GLCD\GLCD.h"
#include "../timer/timer.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define  MAX_X  240
#define  MAX_Y  320
#define PI 3.14

typedef struct {
	short x, y;
	short oldX, oldY;
	float vx, vy;
	short speed;
	short size;
} ball_t;

typedef struct {
	short x, y;
	short oldX, oldY;
	short thickness, height, width;
} paddle_t;

static ball_t ball;
static paddle_t paddle, cpuPaddle;

static short wallThickness = 5;
static unsigned char score1, score2;
static int lowPitch = 1263, highPitch = 1062;

static GAME_STATUS gameStatus;

static void drawBall()
{
	unsigned char i, j;
	
	for(i=0; i<5; i++)																		// delete previous ball
		for(j=0; j<5; j++)
			LCD_SetPoint(ball.oldX+i, ball.oldY+j, Black);
		
	for(i=0; i<5; i++)																		// draw new ball
		for(j=0; j<5; j++)
			LCD_SetPoint(ball.x+i, ball.y+j, Green);
	
	return;
}

static void drawPaddle( paddle_t* p )
{
	short i, j, shift;
	
	shift = p->x - p->oldX;
	
	if(shift > 0) {																											// paddle moved to the right
		for(i=0; i<shift; i++)
			for(j=0; j<(p->thickness); j++)
				LCD_SetPoint( p->oldX + i, p->oldY + j, Black);
	
	for(i=0; i<shift; i++)
		for(j=0; j<(p->thickness); j++)
			LCD_SetPoint( p->oldX + p->width + i, p->y + j, Green );
	}
	
	else if (shift < 0) {																								// paddle moved to the left
		shift*=-1;
		for(i=0; i<shift; i++)
			for(j=0; j<(p->thickness); j++)
				LCD_SetPoint( p->x + p->width + i, p->oldY + j, Black);
	
		for(i=0; i<shift; i++)
			for(j=0; j<p->thickness; j++)
				LCD_SetPoint( p->x + i, p->y + j, Green);
	}
	
	return;
}

static void drawWalls(int thickness, int color)
{
	int i;
	for(i=0; i<thickness; i++) {	
		LCD_DrawLine(i, cpuPaddle.thickness+cpuPaddle.height, i, MAX_Y-paddle.thickness-paddle.height, color);
		LCD_DrawLine(MAX_X-i, cpuPaddle.thickness+cpuPaddle.height, MAX_X-i, MAX_Y-paddle.thickness-paddle.height, color);
	}
	
	return;
}

static void checkCollision()
{
	short relIntersect;
	float angle;
	char str[6];
	
	if(ball.x <= wallThickness) {																					// ball touched the left wall
		ball.vx *= -1; 																											// ball.angle = 180-ball.angle
		ball.x += 2*(wallThickness - ball.x);
		init_timer(0, 0, 0, 3, highPitch);
		enable_timer(0);
	}
	
	else if(ball.x >= MAX_X - wallThickness - ball.size) {								// ball touched the right wall
		ball.vx *= -1; 																											// ball.angle = 180-ball.angle
		ball.x -= 2*(wallThickness - (MAX_X - ball.x) + ball.size);
		init_timer(0, 0, 0, 3, highPitch);
		enable_timer(0);
	}
	
	else if(ball.y <= cpuPaddle.height + cpuPaddle.thickness) {																			// ball reached the top part
		if (ball.x >= (cpuPaddle.x-ball.size) && ball.x <= (cpuPaddle.width+cpuPaddle.x)) {						// ball touched the cpuPaddle
			
			ball.vy *= -1;																																							// ball.angle = 360-ball.angle
			ball.y += 2*(cpuPaddle.y + cpuPaddle.thickness - ball.y);
			
			relIntersect = cpuPaddle.x + cpuPaddle.width/2 - ball.x;																		// ball distance from the paddle center
			if (relIntersect < 0 ) relIntersect*=-1;
			
			if ( relIntersect < cpuPaddle.width/6 ) {																										// if distance < 10
				angle = atan2f(ball.vy, ball.vx);																													//    calculate the ball speed angle
				angle += (angle < PI/2.0) ? PI/8.0 : -PI/8.0;																							//  (the y axis is directed downwards)
				ball.vx = cosf(angle);
				ball.vy = sinf(angle);
			}
			else if (relIntersect >= cpuPaddle.width/3) {																								// if distance >= 20
				angle = atan2f(ball.vy, ball.vx);
				angle += (angle < PI/2.0) ? -PI/8.0 : +PI/8.0;
				if(angle >= PI/4.0 && angle <= 3.0*PI/4.0) {																							// if the new angle is too close to the paddle don't change the speed
					ball.vx = cosf(angle);
					ball.vy = sinf(angle);
				}
			}
			
			init_timer(0, 0, 0, 3, lowPitch);
			enable_timer(0);
		}
		else {																												// ball didn't touch the cpuPaddle
			sprintf(str, "%d", ++score1);
  		GUI_Text(15, 160, (unsigned char*)str, White, Black);
			
			ball.x = MAX_X-wallThickness-ball.size;
			ball.y = 160-ball.size/2;
			ball.vx = cosf(135.0*PI/180.0);
			ball.vy = sinf(135.0*PI/180.0);
			drawBall();
			
		}
	}
	
	else if(ball.y >= MAX_Y - paddle.height - paddle.thickness - ball.size) {							// ball reached the bottom part
		if (ball.x >= (paddle.x-ball.size) && ball.x <= (paddle.width+paddle.x)) {					// ball touched the paddle
			
			ball.vy *= -1;
			ball.y += 2*(MAX_Y - paddle.height - paddle.thickness - ball.y - ball.size);
			
			relIntersect = paddle.x + paddle.width/2 - ball.x;
			if (relIntersect < 0 ) relIntersect*=-1;
			
			if ( relIntersect < paddle.width/6 ) {
				angle = atan2f(ball.vy, ball.vx) + 2.0*PI;
				angle += (angle < 3.0*PI/2.0) ? PI/8.0 : -PI/8.0;
				ball.vx = cosf(angle);
				ball.vy = sinf(angle);
			}
			else if (relIntersect >= paddle.width/3) {
				angle = atan2f(ball.vy, ball.vx) + 2.0*PI;
				angle += (angle < 3.0*PI/2.0) ? -PI/8.0 : +PI/8.0;
				if(angle >= 5.0*PI/4.0 && angle <= 7.0*PI/4.0) {
					ball.vx = cosf(angle);
					ball.vy = sinf(angle);
				}
			}
			
			init_timer(0, 0, 0, 3, lowPitch);
			enable_timer(0);
		}
		else {																												// ball didn't touch the paddle
			sprintf(str, "%d", ++score2);
			GUI_Text_R(MAX_X-15, 175, (unsigned char*)str, White, Black);
			
			ball.x = wallThickness;
			ball.y = 160-ball.size/2;
			ball.vx = cosf(7.0*PI/4.0);
			ball.vy = sinf(7.0*PI/4.0);
			drawBall();
			
		}
	}
	
	return;
}

void drawNextFrame( void )
{
	char str[3];
	
	// move the ball
	ball.oldX = ball.x;
	ball.oldY = ball.y;
	ball.x += ball.speed*ball.vx;
	ball.y += ball.speed*ball.vy;
	
	// move the cpuPaddle
	if( cpuPaddle.x + cpuPaddle.width/2 - ball.x > 0 && cpuPaddle.x > wallThickness ) {
		cpuPaddle.oldX = cpuPaddle.x;
		cpuPaddle.x -= 2;
	}
	if ( cpuPaddle.x + cpuPaddle.width/2 - ball.x < 0 && cpuPaddle.x + cpuPaddle.width < MAX_X - wallThickness ) {
		cpuPaddle.oldX = cpuPaddle.x;
		cpuPaddle.x += 2;
	}
	drawPaddle(&cpuPaddle);
	
	checkCollision();
	
	// if the ball is covering the score draw it again
	if(ball.x >= 5 && ball.x <= 45 && ball.y >= 150 && ball.y <= 185) {
		sprintf(str, "%d", score1);
		GUI_Text(15, 160, (unsigned char*)str, White, Black);
	}
	else if(ball.x >= MAX_X-45 && ball.x <= MAX_X-5 && ball.y >= 150 && ball.y <= 185) {
		sprintf(str, "%d", score2);
		GUI_Text_R(MAX_X-15, 176, (unsigned char*)str, White, Black);
	}
	
	drawBall();
	
	if(score1 == 5) {
		gameStatus = GAME_OVER;
		GUI_Text(92, 210, (unsigned char*)"YOU WIN", Yellow, Black);
		GUI_Text(30, 235, (unsigned char*)"Press INT0 to try again", White, Black);
		GUI_Text_R(MAX_X - 88, MAX_Y - 210, (unsigned char*)"YOU LOSE", Yellow, Black);
	}
	else if(score2 == 5) {
		gameStatus = GAME_OVER;
		GUI_Text(88, 210, (unsigned char*)"YOU LOSE", Yellow, Black);
		GUI_Text(30, 235, (unsigned char*)"Press INT0 to try again", White, Black);
		GUI_Text_R(MAX_X - 92, MAX_Y - 210, (unsigned char*)"YOU WIN", Yellow, Black);
	}

	return;
}

void gameInit( void )
{
	unsigned char i,j;
	char str[6];
	
	LCD_Clear(Black);
	
	//paddle init
	paddle.thickness = cpuPaddle.thickness = 10;
	paddle.width = cpuPaddle.width = 60;
	paddle.height = cpuPaddle.height = 32;
	
	paddle.x = (MAX_X-paddle.width)/2;
	paddle.y = MAX_Y - paddle.height - paddle.thickness;
	paddle.oldX = paddle.x;
	paddle.oldY = paddle.y;
	
	cpuPaddle.x = (MAX_X-cpuPaddle.width)/2;
	cpuPaddle.y = cpuPaddle.height;
	cpuPaddle.oldX = cpuPaddle.x;
	cpuPaddle.oldY = cpuPaddle.y;
	
	for(i=0; i<paddle.width; i++)	{																// draw the paddles for the first time
		for(j=0; j<paddle.thickness; j++) {
			LCD_SetPoint(paddle.x+i, paddle.y+j, Green);
			LCD_SetPoint(cpuPaddle.x+i, cpuPaddle.y+j, Green);
		}
	}
	
	drawWalls(wallThickness, Red);
	
	score1 = score2 = 0;
	
	// ball init
	ball.speed = 5;
	ball.size = 5;
	ball.x = MAX_X-wallThickness-ball.size;
	ball.y = 160-ball.size/2;
	ball.oldX = ball.x;
	ball.oldY = ball.y;
	ball.vx = cosf(135.0*PI/180.0);
	ball.vy = sinf(135.0*PI/180.0);
	drawBall();
				
	sprintf(str, "%d", score1);																		// draw the score for the first time
	GUI_Text(15, 160, (unsigned char*)str, White, Black);
	sprintf(str, "%d", score2);
	GUI_Text_R(MAX_X-15, 176, (unsigned char*)str, White, Black);
	
	return;
}

void titleScreen( void )
{
	setGameStatus(START);
	LCD_Clear(Black);
	GUI_Text(45, 20, (unsigned char*)"Press KEY1 to start", Yellow, Black);
	GUI_Text(85, 40, (unsigned char*)"the GAME!", White, Black);
	
	return;
}

void setGameStatus( GAME_STATUS newStatus )
{
	gameStatus = newStatus;
	return;
}

GAME_STATUS getGameStatus( void )
{
	return gameStatus;
}

void movePaddle( short position )
{
	paddle.oldX = paddle.x;
	paddle.x = position;
	
	drawPaddle(&paddle);

	return;
}

