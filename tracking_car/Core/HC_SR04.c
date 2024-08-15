//
// Created by Lumos on 2024/8/14.
//

#include "HC_SR04.h"
#include "cmsis_os.h"

uint64_t time = 0;
uint64_t time_end = 0;
uint32_t distance = 0;
extern TIM_HandleTypeDef htim3;

void SR04_task(void const * argument)
{
	vTaskDelay(CHASSIS_TASK_INIT_TIME);
	HAL_TIM_Base_Start_IT(&htim3);
	while(1)
	{
		//vTaskSuspendAll();
		
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
		osDelay(1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET);
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET);
		time_end = time;
		if(time_end / 100 < 38)
		{
			distance = (time_end * 346) / 2000;
		}
		
		//距离过近时超声波不反馈上升沿
		//xTaskResumeAll();
		
    vTaskDelay(1);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM3)
  {
    time++;
  }
}
