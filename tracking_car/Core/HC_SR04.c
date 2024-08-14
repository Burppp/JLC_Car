//
// Created by Lumos on 2024/8/14.
//

#include "HC_SR04.h"
#include "cmsis_os.h"

uint32_t times = 0;

void SR04_task(void const * argument)
{
	uint32_t distance = 0;
	while(1)
	{
		vTaskSuspendAll();
		
		distance = 0;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		for(int i = 0;i < 10;i++)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
			osDelay(1);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
			osDelay(65);
			distance += times/5.8;
		}
		distance /= 10;

		xTaskResumeAll();
		
    vTaskDelay(1);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

}
