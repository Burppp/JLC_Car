//
// Created by Lumos on 2024/8/14.
//

#include "LED.h"
#include "cmsis_os.h"

void led_task(void const * argument)
{
	while(1)
	{
		vTaskSuspendAll();
		
		
		
		
		xTaskResumeAll();
		
    vTaskDelay(1);
	}
}
