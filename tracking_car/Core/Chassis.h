//
// Created by Lumos on 2024/8/14.
//

#include "main.h"
#include "PID.h"

#define CHASSIS_PERIOD 1

typedef struct
{
	float vx;
	float vy;
	float vw;
	
	int32_t wheel_pwm[4];
	
	uint8_t chassis_track_relax;
	uint8_t chassis_last_track_relax;
	uint8_t chassis_pc_relax;
	
	pid_t chassis_turn_pid;
	float angle_feedforward;
	float k_angle_feedforward;
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
void chassis_moveon(void);
void chassis_goStraight(void);
void chassis_stepBack(void);
void chassis_standstill(void);
void chassis_turnLeft(int level);
void chassis_turnRight(int level);
void chassis_speed_update(void);
void chassis_moveon_pc(void);
int int_abs(int num);
float float_abs(float num);
void straightLine_obstacleAvoidence(void);
void linearLine_obstacleAvoidence(void);
void chassis_leftTurn_withTime(void);
void chassis_rightTurn_withTime(void);
