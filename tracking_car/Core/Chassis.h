//
// Created by Lumos on 2024/8/14.
//

#include "main.h"

#define CHASSIS_TASK_INIT_TIME 150
void chassis_task(void const * argument);
void tracking_update(void);
void LQ_StepAhead(void);
void LQ_StepBack(void);
void LH_StepAhead(void);
void LH_StepBack(void);
void RQ_StepAhead(void);
void RQ_StepBack(void);
void RH_StepAhead(void);
void RH_StepBack(void);
void chassis_moveon(void);
void chassis_turnLeft(void);
void chassis_turnRight(void);
void chassis_goStraight(void);
void chassis_stepBack(void);
void chassis_standStill_TurnLeft(void);
void chassis_standStill_TurnRight(void);
void chassis_standstill(void);
