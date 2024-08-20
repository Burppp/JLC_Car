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
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 0);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, current_speed * speed / 100);
}

void RQ_StepBack(int speed)
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, current_speed * speed / 100);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 0);
}

void RH_StepAhead(int speed)
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, current_speed * speed / 100);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 0);
}

void RH_StepBack(int speed)
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 0);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, current_speed * speed / 100);
}
