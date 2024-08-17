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
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
		//delay_us(15);
		Delay_us(15);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET);
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET)
		{
			time++;
			Delay_us(1);
		}
		time_end = time;
		if(time_end < 3800)
		{
			distance = (time_end * 346) / 2000;
			if(distance < 80)
				detected_obstacle = 1;
		}
    vTaskDelay(1);
	}
}

void Delay_1us(uint32_t us) 
{
		__IO uint32_t Delay = us * 180 / 7;

    do
		{
        __NOP();
    }while(Delay--);
}

void Delay_us(uint32_t us)
{
	/*保存sysTick的状态*/
	uint32_t val_temp=SysTick->VAL;
	uint32_t load_temp=SysTick->LOAD;
	uint16_t ctrl_temp=SysTick->CTRL;

	/*SysTick状态重新设置，用于延时（系统暂停）*/
	SysTick->LOAD = 9 * us;		// 设置定时器重装值,72MHz 8分频后9HMz，即一微秒记9个数
	SysTick->VAL = 0x00;		// 清空当前计数值,清空后会自动装入重载值
	SysTick->CTRL = 0x00000001; // 位2设置为0，8分频，启动定时器
	while (!(SysTick->CTRL & 0x00010000));// 等待计数到0
	
	/*延时结束后恢复之前的状态，系统继续运行*/
	SysTick->LOAD = load_temp;
	SysTick->VAL = val_temp;
	SysTick->CTRL = ctrl_temp;
	//SysTick->CTRL = 0x00000000; // 关闭定时器
}

