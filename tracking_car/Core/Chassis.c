//
// Created by Lumos on 2024/8/14.
//

#include "Chassis.h"
#include "cmsis_os.h"
#include "main.h"
#include "Lora.h"

chassis_t chassis = {
	.chassis_track_relax = 1,
	.chassis_last_track_relax = 1,
	.chassis_pc_relax = 1,
	.vx = 0,
	.vy = 0,
	.vy = 0,
	.wheel_pwm[0] = 0,
	.wheel_pwm[1] = 0,
	.wheel_pwm[2] = 0,
	.wheel_pwm[3] = 0
};

uint8_t XJ_states[5] = {0};
int result = 0;
uint32_t current_speed = 10000;
int goStraightSpeed = 80;

int result_arr[50] = {0};
uint32_t result_index = 0;
uint32_t last = 0;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart7;
extern uint8_t bRxBufferUart1[1]; //接收数据
extern int8_t wasdLR[7];
extern uint8_t detected_obstacle;
float deltaSpeed = 5;

void chassis_task(void const * argument)
{
  vTaskDelay(CHASSIS_TASK_INIT_TIME);
	
	//LoRa_T_V_Attach(1, 1);
	
	while(1)
	{
		//vTaskSuspendAll();
		
		HAL_UART_Receive_IT(&huart7, bRxBufferUart1, 1);
		
		tracking_update();
		
		chassis_speed_update();
		
		if(chassis.chassis_last_track_relax == 0 && chassis.chassis_track_relax == 1)
			chassis.chassis_track_relax = 0;
		
		chassis.chassis_last_track_relax = chassis.chassis_track_relax;
		
		if(chassis.chassis_track_relax != 1)
		{
			if(detected_obstacle == 0)
			{
				chassis_moveon();
				HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);
				chassis_turnRight(3);
				osDelay(900);
				chassis_goStraight();
				osDelay(2300);
				chassis_turnLeft(3);
				osDelay(800);
				chassis_goStraight();
				osDelay(1600);
				chassis_turnLeft(3);
				osDelay(700);
				chassis_goStraight();
				osDelay(2200);
				chassis_turnRight(3);
				osDelay(800);
				detected_obstacle = 0;
			}
		}
		//else if(chassis.chassis_pc_relax != 1)
//		{
//			chassis_moveon_pc();
//		}
		
		//xTaskResumeAll();
		
    vTaskDelay(1);
	}
}

void chassis_speed_update()
{
	  if(wasdLR[0])
        chassis.vx = 10000;
    if(wasdLR[2])
        chassis.vx = -10000;
    if(!wasdLR[0] && !wasdLR[2])
        chassis.vx = 0;

    if(wasdLR[1] || wasdLR[3])
        chassis.vw = (wasdLR[1] - wasdLR[3]) * 8000;
    if(!wasdLR[1] && !wasdLR[3])
        chassis.vw = 0;
		
		if(wasdLR[4])
			chassis.chassis_pc_relax = 0;
		else
			chassis.chassis_pc_relax = 1;
}

void chassis_moveon_pc()
{
	chassis.wheel_pwm[LF] = chassis.vx;
	chassis.wheel_pwm[LB] = chassis.vx;
	chassis.wheel_pwm[RF] = chassis.vx;
	chassis.wheel_pwm[RB] = chassis.vx;
	if(chassis.vw > 0)
	{
		chassis.wheel_pwm[RF] = chassis.vw;
		chassis.wheel_pwm[RB] = chassis.vw;
		chassis.wheel_pwm[LF] = -chassis.vw;
		chassis.wheel_pwm[LB] = -chassis.vw;
	}
	if(chassis.vw < 0)
	{
		chassis.wheel_pwm[RF] = chassis.vw;
		chassis.wheel_pwm[RB] = chassis.vw;
		chassis.wheel_pwm[LF] = -chassis.vw;
		chassis.wheel_pwm[LB] = -chassis.vw;
	}
	for(int i = 0;i < 4;i++)
	{
		if(chassis.wheel_pwm[i] > 10000)
			chassis.wheel_pwm[i] = 10000;
		if(chassis.wheel_pwm[i] < -10000)
			chassis.wheel_pwm[i] = -10000;
	}
	if(chassis.chassis_pc_relax == 1)
	{
		chassis.wheel_pwm[LF] = chassis.wheel_pwm[LB] = chassis.wheel_pwm[RF] = chassis.wheel_pwm[RB] = 0;
	}
	
	if(chassis.wheel_pwm[LF] >= 0)
	{
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, chassis.wheel_pwm[LF]);
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 0);
	}
	if(chassis.wheel_pwm[LF] < 0)
	{
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 0);
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, -chassis.wheel_pwm[LF]);
	}
	
	if(chassis.wheel_pwm[LB] >= 0)
	{
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, chassis.wheel_pwm[LB]);
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 0);
	}
	if(chassis.wheel_pwm[LB] < 0)
	{
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 0);
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, -chassis.wheel_pwm[LB]);
	}
	
	if(chassis.wheel_pwm[RF] >= 0)
	{
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 0);
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, chassis.wheel_pwm[RF]);
	}
	if(chassis.wheel_pwm[RF] < 0)
	{
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, -chassis.wheel_pwm[RF]);
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 0);
	}
	
	if(chassis.wheel_pwm[RB] >= 0)
	{
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, chassis.wheel_pwm[RB]);
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 0);
	}
	if(chassis.wheel_pwm[RB] < 0)
	{
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 0);
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, -chassis.wheel_pwm[RB]);
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

int sum = 0;
void chassis_moveon()
{
		
	while(XJ_states[0] == 0 && XJ_states[1] == 0 && XJ_states[2] == 0 && XJ_states[3] == 0 && XJ_states[4] == 0)
	{
//		while(sum == 0)
//		{
//			tracking_update();
//			result = XJ_states[0] * 2 + XJ_states[1] - XJ_states[3] - XJ_states[4] * 2;
//			sum += result;
//		}
		
		if(sum > 0)
		{
			LQ_StepBack(50);
			LH_StepBack(50);
			RQ_StepAhead(50);
			RQ_StepAhead(50);
		}
		if(sum < 0)
		{
			LQ_StepAhead(87);
			LH_StepAhead(87);
			RQ_StepBack(87);
			RQ_StepBack(87);
		}
		
		tracking_update();
		result = XJ_states[0] * 2 + XJ_states[1] - XJ_states[3] - XJ_states[4] * 2;
	}
	
	result = XJ_states[0] * 2 + XJ_states[1] - XJ_states[3] - XJ_states[4] * 2;
	if(HAL_GetTick() - last > 100)
	{
		last = HAL_GetTick();
		sum -= result_arr[result_index];
		sum += result;
		result_arr[result_index++] = result;
		if(result_index >= 50)
			result_index = 0;
	}
	
	switch(result)
	{
		case 3:
			chassis_turnLeft(3);
			break;
		case 2:
			chassis_turnLeft(2);
			break;
		case 1:
			chassis_turnLeft(1);
			break;
		case 0:
			chassis_goStraight();
			osDelay(10);
			break;
		case -1:
			chassis_turnRight(1);
			break;
		case -2:
			chassis_turnRight(2);
			break;
		case -3:
			chassis_turnRight(3);
			break;
		default:
			//chassis_standstill();
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

void chassis_turnLeft(int level)
{
	switch(level)
	{
		case 1:
			LQ_StepBack(40);
			LH_StepBack(40);
			RQ_StepAhead(100);
			RH_StepAhead(100);
			break;
		case 2:
			LQ_StepBack(40);
			LH_StepBack(40);
			RQ_StepAhead(80);
			RH_StepAhead(80);
			break;
		case 3:
			LQ_StepBack(45);
			LH_StepBack(45);
			RQ_StepAhead(87);
			RH_StepAhead(87);
			break;
		default:
			break;
	}
	
}

void chassis_turnRight(int level)
{
	switch(level)
	{
		case 1:
			LQ_StepAhead(100);
			LH_StepAhead(100);
			RQ_StepBack(50);
			RH_StepBack(50);
			break;
		case 2:
			LQ_StepAhead(100);
			LH_StepAhead(100);
			RQ_StepBack(35);
			RH_StepBack(35);
			break;
		case 3:
			LQ_StepAhead(100);
			LH_StepAhead(100);
			RQ_StepBack(45);
			RH_StepBack(45);
			break;
		default:
			break;
	}
	
}

void chassis_stepBack()
{
	LQ_StepBack(goStraightSpeed);
	LH_StepBack(goStraightSpeed);
	RQ_StepBack(goStraightSpeed);
	RH_StepBack(goStraightSpeed);
	
}

void chassis_goStraight()
{
	LQ_StepAhead(goStraightSpeed);
	LH_StepAhead(goStraightSpeed);
	RQ_StepAhead(goStraightSpeed);
	RH_StepAhead(goStraightSpeed);
	
}

void LQ_StepAhead(int speed)
{
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, current_speed * speed / 100);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 0);
	
}

void LQ_StepBack(int speed)
{
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 0);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, current_speed * speed / 100);
	
}

void LH_StepAhead(int speed)
{
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, current_speed * speed / 100);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 0);
	
}

void LH_StepBack(int speed)
{
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 0);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, current_speed * speed / 100);
	
}

void RQ_StepAhead(int speed)
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 0);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, current_speed * speed / 100);
}

void RQ_StepBack(int speed)
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, current_speed * speed / 100);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 0);
	
}

void RH_StepAhead(int speed)
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, current_speed * speed / 100);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 0);
	
}

void RH_StepBack(int speed)
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 0);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, current_speed * speed / 100);
	
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_5)
	{
		if(chassis.chassis_track_relax)
		{
			chassis.chassis_track_relax = 0;
		}
	}
}
