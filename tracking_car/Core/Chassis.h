//
// Created by Lumos on 2024/8/14.
//

#include "main.h"

typedef struct
{
	float vx;
	float vy;
	float vw;
	
	int32_t wheel_pwm[4];
	
	uint8_t chassis_track_relax;
	uint8_t chassis_last_track_relax;
	uint8_t chassis_pc_relax;
}chassis_t;

enum
{
	LF = 0,
	LB = 1,
	RB = 2,
	RF = 3
};

#define CHASSIS_TASK_INIT_TIME 150
void chassis_task(void const * argument);
void tracking_update(void);
void LQ_StepAhead(int speed);
void LQ_StepBack(int speed);
void LH_StepAhead(int speed);
void LH_StepBack(int speed);
void RQ_StepAhead(int speed);
void RQ_StepBack(int speed);
void RH_StepAhead(int speed);
void RH_StepBack(int speed);
void chassis_moveon(void);
void chassis_goStraight(void);
void chassis_stepBack(void);
void chassis_standstill(void);
void chassis_turnLeft(int level);
void chassis_turnRight(int level);
void chassis_speed_update(void);
void chassis_moveon_pc(void);
