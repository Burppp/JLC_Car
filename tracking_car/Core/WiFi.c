//
// Created by Lumos on 2024/8/14.
//

#include "WiFi.h"
#include "cmsis_os.h"

void WiFi_task(void const * argument)
{
	while(1)
	{
		vTaskSuspendAll();
		
		
		
		
		xTaskResumeAll();
		
    vTaskDelay(1);
	}
}
