//
// Created by Lumos on 2024/8/14.
//

#include "Chassis.h"
#include "cmsis_os.h"
#include "motor.h"
#include "main.h"
#include "Lora.h"
#include "tools.h"
#include "PID.h"

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
	.wheel_pwm[3] = 0,
	.angle_feedforward = 0,
	.k_angle_feedforward = 1
};

uint8_t XJ_states[5] = {0};
int result = 0;
int last_result = 0;
int goStraightSpeed = 80;

int result_arr[30] = {0};
uint32_t result_index = 0;
uint32_t last = 0;
uint8_t flag = 0;
int sum = 0;

//pid_t chassis_turn_pid;
uint32_t pid_param[5] = {20000, 8000, 3000, 10 ,0};
float pid_output = 0;
uint32_t start = 0, now = 0, duration = 0;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart7;
extern uint8_t bRxBufferUart1[1]; //接收数据
extern int8_t wasdLR[7];
extern uint8_t detected_obstacle;
float deltaSpeed = 5;
extern uint8_t finished_turn;

void chassis_task(void const * argument)
{
  vTaskDelay(CHASSIS_TASK_INIT_TIME);
	
	//LoRa_T_V_Attach(1, 1);
	
	pid_init(&chassis.chassis_turn_pid, pid_param[0], pid_param[1], pid_param[2], pid_param[3], pid_param[4]);
	
	while(1)
	{
		//vTaskSuspendAll();
		
		HAL_UART_Receive_IT(&huart7, bRxBufferUart1, 1);
		
		tracking_update();
		
		chassis_speed_update();
				
		chassis.chassis_last_track_relax = chassis.chassis_track_relax;
		
		if(chassis.chassis_track_relax != 1)
		{
			if(start == 0)
				start = HAL_GetTick();
			
			now = HAL_GetTick();
			duration = now - start;
			
			if(detected_obstacle == 0)
			{
				chassis_moveon();
				HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);
			}
			else if(detected_obstacle == 1)
			{
				if(duration < 2800)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);//L
					HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);//R
			
					chassis_rightTurn_withTime();
				}
				else if(duration < 9000)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);//L
					HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);//R
					
					chassis_leftTurn_withTime();
				}
				else if(duration < 13000)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);//L
					HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);//R
					
					chassis_rightTurn_withTime();
				}
				else if(duration < 19000)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);//L
					HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);//R
					
					chassis_leftTurn_withTime();
				}
				else if(duration < 22000)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);//L
					HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);//R
					
					chassis_rightTurn_withTime();
				}
				else
				{
					straightLine_obstacleAvoidence();
				}
				finished_turn = 0;
				detected_obstacle = 0;
				flag = 0;
				//linearLine_obstacleAvoidence();
			}
		}
//		else if(chassis.chassis_pc_relax != 1)
//		{
//			chassis_moveon_pc();
//		}
		
		//xTaskResumeAll();
		
    vTaskDelay(1);
	}
}

/*                     																	   CHASSIS OBSTACLE AVIODENCE     																							*/
void straightLine_obstacleAvoidence()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);
	
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
	osDelay(1000);
	chassis_goStraight();
	osDelay(1700);
	tracking_update();
	while(XJ_states[0] == 0 && XJ_states[1] == 0 && XJ_states[2] == 0 && XJ_states[3] == 0 && XJ_states[4] == 0)
	{
		chassis_goStraight();
		osDelay(100);
		tracking_update();
	}
	chassis_turnRight(3);
	osDelay(1000);
	flag++;
	detected_obstacle = 0;
	finished_turn = 0;
}

void linearLine_obstacleAvoidence()
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);
	if(sum < -5)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);
		
		chassis_leftTurn_withTime();
	}
	else if(sum > 5)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);
		
		chassis_rightTurn_withTime();
	}
	finished_turn = 0;
	detected_obstacle = 0;
	flag = 0;
}

/*																																			INFO UPDATE																															*/
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

void tracking_update(void)
{
	XJ_states[0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);
	XJ_states[1] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10);
	XJ_states[2] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12);
	XJ_states[3] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13);
	XJ_states[4] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);
}

/*																														CHASSIS MOVE APPLICATION																												*/
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

void chassis_moveon()
{
		
	while(XJ_states[0] == 0 && XJ_states[1] == 0 && XJ_states[2] == 0 && XJ_states[3] == 0 && XJ_states[4] == 0)
	{
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
		last_result = result;
		result = XJ_states[0] * 2 + XJ_states[1] - XJ_states[3] - XJ_states[4] * 2;
	}
	
	last_result = result;
	result = XJ_states[0] * 2 + XJ_states[1] - XJ_states[3] - XJ_states[4] * 2;
	if(HAL_GetTick() - last > 100)
	{
		last = HAL_GetTick();
		sum -= result_arr[result_index];
		sum += result;
		result_arr[result_index++] = result;
		if(result_index >= 30)
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
			osDelay(1);
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
	
//	pid_output = pid_calc(&chassis.chassis_turn_pid, result, 0);
//	chassis.wheel_pwm[LF] = 10000 + pid_output;
//	chassis.wheel_pwm[LB] = 10000 + pid_output;
//	chassis.wheel_pwm[RF] = -pid_output + 10000;
//	chassis.wheel_pwm[RB] = -pid_output + 10000;
//	
//	if(result != last_result)
//	{
//		chassis.angle_feedforward = (result - last_result) / (CHASSIS_PERIOD * 0.001) * chassis.k_angle_feedforward;
//	}
//	
//	chassis.wheel_pwm[LF] = val_limit(chassis.wheel_pwm[LF], 10000, -10000);
//	chassis.wheel_pwm[LB] = val_limit(chassis.wheel_pwm[LB], 10000, -10000);
//	chassis.wheel_pwm[RF] = val_limit(chassis.wheel_pwm[RF], 10000, -10000);
//	chassis.wheel_pwm[RB] = val_limit(chassis.wheel_pwm[RB], 10000, -10000);
//	
//	RF_MotorRun(chassis.wheel_pwm[RF]);
//	RB_MotorRun(chassis.wheel_pwm[RB]);
//	LF_MotorRun(chassis.wheel_pwm[LF]);
//	LB_MotorRun(chassis.wheel_pwm[LB]);
}

/*																																				CHASSIS MOVE BSP																																	*/
void chassis_leftTurn_withTime()
{
	uint32_t start_turning;
	start_turning = HAL_GetTick();
	while(finished_turn == 0)
	{
		chassis_turnLeft(3);
		osDelay(1);
	}
	uint32_t finished_turning;
	finished_turning = HAL_GetTick();
	if(finished_turning - start_turning < 1000)
	{
		//HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);
		osDelay(300);
	}
	
	chassis_goStraight();
	osDelay(1200);
	chassis_turnRight(3);
	osDelay(finished_turning - start_turning);
	if(finished_turning - start_turning < 2000)
	{
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);
		osDelay(100);
	}
	chassis_goStraight();
	osDelay(500);
	tracking_update();
	while(XJ_states[0] == 0 && XJ_states[1] == 0 && XJ_states[2] == 0 && XJ_states[3] == 0 && XJ_states[4] == 0)
	{
		chassis_goStraight();
		osDelay(100);
		tracking_update();
	}
}

void chassis_rightTurn_withTime()
{
	uint32_t start_turning;
	start_turning = HAL_GetTick();
	while(finished_turn == 0)
	{
		chassis_turnRight(3);
		osDelay(1);
	}
	uint32_t finished_turning;
	finished_turning = HAL_GetTick();
	if(finished_turning - start_turning < 1000)
	{
		//HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);
		osDelay(300);
	}
	
	chassis_goStraight();
	osDelay(1200);
	chassis_turnLeft(3);
	osDelay(finished_turning - start_turning);
	if(finished_turning - start_turning < 2000)
	{
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);
		osDelay(100);
	}
	chassis_goStraight();
	osDelay(600);
	tracking_update();
	while(XJ_states[0] == 0 && XJ_states[1] == 0 && XJ_states[2] == 0 && XJ_states[3] == 0 && XJ_states[4] == 0)
	{
		chassis_goStraight();
		osDelay(100);
		tracking_update();
	}
	chassis_turnRight(3);
	osDelay(500);
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
			LQ_StepAhead(10);
			LH_StepAhead(10);
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
			LQ_StepBack(40);
			LH_StepBack(40);
			RQ_StepAhead(80);
			RH_StepAhead(80);
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
			RQ_StepBack(20);
			RH_StepBack(20);
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
	LQ_StepAhead(100);
	LH_StepAhead(100);
	RQ_StepAhead(100);
	RH_StepAhead(100);
	
}

//KEY SCAN
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
