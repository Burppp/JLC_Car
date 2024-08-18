//
// Created by Lumos on 2024/8/14.
//

#include "HC_SR04.h"
#include "cmsis_os.h"

uint64_t time = 0;
uint64_t time_end = 0;
uint32_t distance = 0;
uint32_t last_distance = 0;
uint8_t is_closing = 0;
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
		delay_us(15);
		//Delay_us(15);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET);
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET)
		{
			time++;
			delay_us(1);
		}
		time_end = time;
		if(time_end < 3800)
		{
			last_distance = distance;
			distance = (time_end * 346) / 2000;
			if(distance < 80)
			{
				if(distance > last_distance)
				{
					is_closing++;
				}
				if(is_closing >= 3)
				{
					detected_obstacle = 1;
					is_closing = 0;
				}
			}
			else
			{
				detected_obstacle = 0;
			}
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
	/*����sysTick��״̬*/
	uint32_t val_temp=SysTick->VAL;
	uint32_t load_temp=SysTick->LOAD;
	uint16_t ctrl_temp=SysTick->CTRL;

	/*SysTick״̬�������ã�������ʱ��ϵͳ��ͣ��*/
	SysTick->LOAD = 9 * us;		// ���ö�ʱ����װֵ,72MHz 8��Ƶ��9HMz����һ΢���9����
	SysTick->VAL = 0x00;		// ��յ�ǰ����ֵ,��պ���Զ�װ������ֵ
	SysTick->CTRL = 0x00000001; // λ2����Ϊ0��8��Ƶ��������ʱ��
	while (!(SysTick->CTRL & 0x00010000));// �ȴ�������0
	
	/*��ʱ������ָ�֮ǰ��״̬��ϵͳ��������*/
	SysTick->LOAD = load_temp;
	SysTick->VAL = val_temp;
	SysTick->CTRL = ctrl_temp;
	//SysTick->CTRL = 0x00000000; // �رն�ʱ��
}

//ʹ�øú���ǰ���ȳ�ʼ��sysytick��ʱ��
//��������ԭ�����̽����޸�
void delay_us(uint32_t nus)
{
       uint32_t ticks;
       uint32_t told,tnow,reload,tcnt=0;
 
       reload = SystemCoreClock;                     //��ȡ��װ�ؼĴ���ֵ
       ticks = nus * (SystemCoreClock / 1000000);  //����ʱ��ֵ
       told=SysTick->VAL;                          //��ȡ��ǰ��ֵ�Ĵ���ֵ����ʼʱ��ֵ��
 
       while(1)
       {
              tnow=SysTick->VAL;          //��ȡ��ǰ��ֵ�Ĵ���ֵ
              if(tnow!=told)              //��ǰֵ�����ڿ�ʼֵ˵�����ڼ���
              {         
 
                     if(tnow<told)             //��ǰֵС�ڿ�ʼ��ֵ��˵��δ�Ƶ�0
                          tcnt+=told-tnow;     //����ֵ=��ʼֵ-��ǰֵ
 
                     else                  //��ǰֵ���ڿ�ʼ��ֵ��˵���ѼƵ�0�����¼���
                            tcnt+=reload-tnow+told;   //����ֵ=��װ��ֵ-��ǰֵ+��ʼֵ  ����
                                                      //�ӿ�ʼֵ�Ƶ�0�� 
 
                     told=tnow;                //���¿�ʼֵ
                     if(tcnt>=ticks)break;     //ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
              } 
       }     
}
 
//SystemCoreClockΪϵͳʱ��(system_stmf4xx.c��)��ͨ��ѡ���ʱ����Ϊ
//systick��ʱ��ʱ�ӣ����ݾ����������
