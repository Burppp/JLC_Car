//
// Created by Lumos on 2024/8/14.
//

#include "Chassis.h"
#include "cmsis_os.h"
#include "main.h"

uint8_t XJ_states[5] = {0};
int result = 0;
uint8_t chassis_relax = 1;
uint32_t current_speed = 10000;
uint32_t level_3[2] = {0};
uint32_t last_level_3[2] = {0};
uint32_t last_tick[2] = {0};
int goStraightSpeed = 80;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

void chassis_task(void const * argument)
{
  vTaskDelay(CHASSIS_TASK_INIT_TIME);
	while(1)
	{
		//vTaskSuspendAll();
		
		tracking_update();
		
		if(chassis_relax != 1)
		{
			chassis_moveon();
		}
		
		//xTaskResumeAll();
		
    vTaskDelay(1);
	}
}

void tracking_update(void)
{
	XJ_states[0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);
	XJ_states[1] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10);
	XJ_states[2] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12);
	XJ_states[3] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13);
	XJ_states[4] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);
}

void chassis_moveon()
{
	while(XJ_states[0] == 0 && XJ_states[1] == 0 && XJ_states[2] == 0 && XJ_states[3] == 0 && XJ_states[4] == 0)
	{
		if(result >= 0)
		{
			LQ_StepAhead(100);
			LH_StepAhead(100);
			RQ_StepBack(100);
			RQ_StepBack(100);
		}
		else if(result < 0)
		{
			LQ_StepBack(100);
			LH_StepBack(100);
			RQ_StepAhead(100);
			RQ_StepAhead(100);
		}
		
		tracking_update();
		result = XJ_states[0] * 2 + XJ_states[1] - XJ_states[3] - XJ_states[4] * 2;
	}
	
	result = XJ_states[0] * 2 + XJ_states[1] - XJ_states[3] - XJ_states[4] * 2;

	if(last_tick[0] == 0)
	{
		last_tick[0] = HAL_GetTick();
		last_level_3[0] = level_3[0];
	}
	if(last_tick[1] == 0)
	{
		last_tick[1] = HAL_GetTick();
		last_level_3[1] = level_3[1];
	}
	
	switch(result)
	{
		case 3:
			chassis_turnLeft(3);
			break;
		case 2:
			chassis_turnLeft(2);
			break;
		case 1:
			chassis_turnLeft(1);
			break;
		case 0:
			chassis_goStraight();
			break;
		case -1:
			chassis_turnRight(1);
			break;
		case -2:
			chassis_turnRight(2);
			break;
		case -3:
			chassis_turnRight(3);
			break;
		default:
			chassis_standstill();
			break;
	}
}

void chassis_standstill()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
}

void chassis_turnLeft(int level)
{
	switch(level)
	{
		case 1:
			LQ_StepAhead(40);
			LH_StepAhead(40);
			RQ_StepAhead(87);
			RH_StepAhead(87);
			break;
		case 2:
			LQ_StepBack(40);
			LH_StepBack(40);
			RQ_StepAhead(80);
			RH_StepAhead(80);
			break;
		case 3:
			LQ_StepBack(10);
			LH_StepBack(10);
			RQ_StepAhead(40);
			RH_StepAhead(40);
			break;
		default:
			break;
	}
	
}

void chassis_turnRight(int level)
{
	switch(level)
	{
		case 1:
			LQ_StepAhead(87);
			LH_StepAhead(87);
			RQ_StepAhead(40);
			RH_StepAhead(40);
			break;
		case 2:
			LQ_StepAhead(80);
			LH_StepAhead(80);
			RQ_StepBack(40);
			RH_StepBack(40);
			break;
		case 3:
			LQ_StepAhead(40);
			LH_StepAhead(40);
			RQ_StepBack(10);
			RH_StepBack(10);
			break;
		default:
			break;
	}
	
}

void chassis_stepBack()
{
	LQ_StepBack(goStraightSpeed);
	LH_StepBack(goStraightSpeed);
	RQ_StepBack(goStraightSpeed);
	RH_StepBack(goStraightSpeed);
	
}

void chassis_goStraight()
{
	LQ_StepAhead(goStraightSpeed);
	LH_StepAhead(goStraightSpeed);
	RQ_StepAhead(goStraightSpeed);
	RH_StepAhead(goStraightSpeed);
	
}

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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_5)
	{
		if(chassis_relax)
		{
			chassis_relax = 0;
		}
	}
}
