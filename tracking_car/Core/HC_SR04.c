//
// Created by Lumos on 2024/8/14.
//

#include "HC_SR04.h"
#include "cmsis_os.h"

uint64_t time = 0;
uint64_t time_end = 0;
uint32_t distance = 0;
uint8_t detected_obstacle = 0;
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
		Delay_1us(15);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET);
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET)
		{
			time++;
			Delay_1us(1);
		}
		time_end = time;
		if(time_end < 3800)
		{
			distance = (time_end * 346) / 2000;
			if(distance < 80)
				detected_obstacle = 1;
		}
		
		//距离过近时超声波不反馈上升沿
		//xTaskResumeAll();
		
    vTaskDelay(1);
	}
}

void Delay_1us(uint32_t us) 
{
    uint32_t i;

    for(i = 0; i < us * 45; i++) 
		{
        __NOP();
    }
}

static uint8_t fac_us = 0;
void delay_us(uint16_t nus)
{
    uint32_t ticks = 0;
    uint32_t told = 0;
    uint32_t tnow = 0;
    uint32_t tcnt = 0;
    uint32_t reload = 0;
    reload = SysTick->LOAD;
    ticks = nus * fac_us;
    told = SysTick->VAL;
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}
