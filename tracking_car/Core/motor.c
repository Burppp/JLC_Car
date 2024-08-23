//
// Created by Lumos on 2024/8/20.
//

#include "motor.h"

uint32_t current_speed = 10000;

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim2;

void LQ_StepAhead(int speed)
{
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, current_speed * speed / 100);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 0);
}

void LQ_StepBack(int speed)
{
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 0);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, current_speed * speed / 100);
}

void LH_StepAhead(int speed)
{
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, current_speed * speed / 100);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 0);
}

void LH_StepBack(int speed)
{
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 0);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, current_speed * speed / 100);
}

void RQ_StepAhead(int speed)
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, current_speed * speed / 100);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 0);
}

void RQ_StepBack(int speed)
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 0);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, current_speed * speed / 100);
}

void RH_StepAhead(int speed)
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 0);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, current_speed * speed / 100);
}

void RH_StepBack(int speed)
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, current_speed * speed / 100);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 0);
}

void RF_MotorRun(int32_t pwm)
{
	if(pwm >= 0)
	{
		RQ_StepAhead(0.8 * pwm);
	}
	else
	{
		RQ_StepBack(-0.6 * pwm);
	}
}

void RB_MotorRun(int32_t pwm)
{
	if(pwm >= 0)
	{
		RH_StepAhead(0.8 * pwm);
	}
	else
	{
		RH_StepBack(-0.6 * pwm);
	}
}

void LF_MotorRun(int32_t pwm)
{
	if(pwm >= 0)
	{
		LQ_StepAhead(0.8 * pwm);
	}
	else
	{
		LQ_StepBack(-0.6 * pwm);
	}
}

void LB_MotorRun(int32_t pwm)
{
	if(pwm >= 0)
	{
		LH_StepAhead(0.8 * pwm);
	}
	else
	{
		LH_StepBack(-0.6 * pwm);
	}
}
