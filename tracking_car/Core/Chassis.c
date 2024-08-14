//
// Created by Lumos on 2024/8/14.
//

#include "Chassis.h"
#include "cmsis_os.h"

void chassis_task(void const * argument)
{
  vTaskDelay(CHASSIS_TASK_INIT_TIME);
	while(1)
	{
		vTaskSuspendAll();
		
		
		
		
		xTaskResumeAll();
		
    vTaskDelay(1);
	}
}
