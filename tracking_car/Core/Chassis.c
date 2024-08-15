//
// Created by Lumos on 2024/8/14.
//

#include "Chassis.h"
#include "cmsis_os.h"
#include "main.h"

uint8_t XJ_states[5] = {0};
int result = 0;
uint8_t chassis_relax = 1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

void chassis_task(void const * argument)
{
  vTaskDelay(CHASSIS_TASK_INIT_TIME);
	while(1)
	{
		vTaskSuspendAll();
		
		tracking_update();
		
		if(chassis_relax != 1)
		{
			chassis_moveon();
		}
		
		//__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 5000);//RH-
		//__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 5000);//RH+
		//__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 5000);//RF-
		//__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 5000);//RF+
		
		//__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 5000);//LF-
		//__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 5000);//LF+
		//__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 5000);//LH+
		//__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 10000);//LH-
		
		xTaskResumeAll();
		
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
	result = XJ_states[0] * 2 + XJ_states[1] - XJ_states[3] - XJ_states[4] * 2;

	switch(result)
	{
		case 3:
			chassis_standStill_TurnLeft();
			break;
		case 2:
			chassis_standStill_TurnLeft();
			break;
		case 1:
			chassis_turnLeft();
			break;
		case 0:
			chassis_goStraight();
			break;
		case -1:
			chassis_turnRight();
			break;
		case -3:
			chassis_standStill_TurnRight();
			break;
		case -2:
			chassis_standStill_TurnRight();
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
	
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);//LED_L
//	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);//LED_R
}

void chassis_standStill_TurnRight()
{
	LQ_StepAhead();
	LH_StepAhead();
	RQ_StepBack();
	RH_StepBack();
	
//	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);//LED_R
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);//LED_L
}

void chassis_stepBack()
{
	LQ_StepBack();
	LH_StepBack();
	RQ_StepBack();
	RH_StepBack();
	
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);//LED_L
//	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);//LED_R
}

void chassis_goStraight()
{
	LQ_StepAhead();
	LH_StepAhead();
	RQ_StepAhead();
	RH_StepAhead();
	
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);//LED_L
//	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);//LED_R
}

void chassis_turnRight()
{
	LQ_StepAhead();
	LH_StepAhead();
	RQ_StepBack();
	RH_StepAhead();
	
//	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);//LED_R
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);//LED_L
}

void chassis_turnLeft()
{
	LQ_StepBack();
	LH_StepAhead();
	RQ_StepAhead();
	RH_StepAhead();
	
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);//LED_L
//	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);//LED_R
}

void LQ_StepAhead()
{
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 10000);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 0);
	
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}

void LQ_StepBack()
{
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 0);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 10000);
	
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
}

void LH_StepAhead()
{
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 10000);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 0);
	
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
}

void LH_StepBack()
{
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 0);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 10000);
	
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}

void RQ_StepAhead()
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 10000);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 0);
	
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
}

void RQ_StepBack()
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 0);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 10000);
	
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
}

void RH_StepAhead()
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 10000);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 0);
	
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
}

void RH_StepBack()
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 0);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 10000);
	
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
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
