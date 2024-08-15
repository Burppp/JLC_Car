//
// Created by Lumos on 2024/8/14.
//

#include "Chassis.h"
#include "cmsis_os.h"
#include "main.h"

uint8_t XJ_states[5] = {0};
int result = 0;

void chassis_task(void const * argument)
{
  vTaskDelay(CHASSIS_TASK_INIT_TIME);
	while(1)
	{
		vTaskSuspendAll();
		
		tracking_update();
		
		chassis_moveon();
		
		xTaskResumeAll();
		
    vTaskDelay(60);
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
	result = XJ_states[0] * 2 + XJ_states[1] - XJ_states[3] - XJ_states[4] * 2;

	switch(result)
	{
		case 3:
		case 2:
			chassis_standStill_TurnRight();
			break;
		case 1:
			chassis_turnRight();
			break;
		case 0:
			chassis_goStraight();
			break;
		case -1:
			chassis_turnLeft();
			break;
		case -3:
		case -2:
			chassis_standStill_TurnLeft();
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

void chassis_standStill_TurnLeft()
{
	LQ_StepBack();
	LH_StepBack();
	RQ_StepAhead();
	RH_StepAhead();
}

void chassis_standStill_TurnRight()
{
	LQ_StepAhead();
	LH_StepAhead();
	RQ_StepBack();
	RH_StepBack();
}

void chassis_stepBack()
{
	LQ_StepBack();
	LH_StepBack();
	RQ_StepBack();
	RH_StepBack();
}

void chassis_goStraight()
{
	LQ_StepAhead();
	LH_StepAhead();
	RQ_StepAhead();
	RH_StepAhead();
}

void chassis_turnRight()
{
	LQ_StepAhead();
	LH_StepAhead();
	RQ_StepBack();
	RH_StepAhead();
}

void chassis_turnLeft()
{
	LQ_StepBack();
	LH_StepAhead();
	RQ_StepAhead();
	RH_StepAhead();
}

void LQ_StepAhead()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}

void LQ_StepBack()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
}

void LH_StepAhead()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
}

void LH_StepBack()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}

void RQ_StepAhead()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
}

void RQ_StepBack()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
}

void RH_StepAhead()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
}

void RH_StepBack()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}
