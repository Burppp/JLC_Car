//
// Created by Lumos on 2024/8/14.
//

#include "ADC.h"
#include "cmsis_os.h"

extern ADC_HandleTypeDef hadc1;
extern uint8_t chassis_relax;

uint8_t powerLow = 0;
uint16_t ADCx = 0;
float volt = 0;

void ADC_task(void const * argument)
{
	while(1)
	{
		//vTaskSuspendAll();
		
		HAL_ADC_Start(&hadc1);
		
		HAL_ADC_PollForConversion(&hadc1, 50);
		
		if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
		{
			ADCx = HAL_ADC_GetValue(&hadc1);
			volt = ADCx * 3.3 / 4096;
			
			if(volt < 1.0f)
			{
				powerLow = 1;
			}
			else
			{
				powerLow = 0;
			}
		}
		
		if(powerLow == 1 && chassis_relax == 0)
		{
			osDelay(1000);
			//HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);
			osDelay(1000);
			//HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_SET);
		}
		
		//xTaskResumeAll();
		
    vTaskDelay(1);
	}
}
