//
// Created by Lumos on 2024/8/22.
//

#include "main.h"
#include "filter.h"

/************ һ�׿������˲�*************/

/**
  * @name   kalmanCreate
  * @brief  ����һ���������˲���
  * @param  p:  �˲���
  *         T_Q:ϵͳ����Э����
  *         T_R:��������Э����
  *
  * @retval none
  * @attention R�̶�,QԽ�����Խ���β���ֵ��Q�������ֻ�ò���ֵ
  *		       Q�̶�,RԽ�����Խ����Ԥ��ֵ��R�������ֻ��Ԥ��ֵ
  */
void first_Kalman_Create(first_kalman_filter_t *p, float T_Q, float T_R)
{
    p->X_last = (float)0;
    p->P_last = 0;
    p->Q = T_Q;
    p->R = T_R;
    p->A = 1;
    p->B = 0;
    p->H = 1;
    p->X_mid = p->X_last;
}


/**
  * @name   KalmanFilter
  * @brief  �������˲���
  * @param  p:  �˲���
  *         dat:���˲�����
  * @retval �˲��������
  * @attention Z(k)��ϵͳ����,������ֵ   X(k|k)�ǿ������˲����ֵ,���������
  *            A=1 B=0 H=1 I=1  W(K)  V(k)�Ǹ�˹������,�����ڲ���ֵ����,���Բ��ù�
  *            �����ǿ�������5�����Ĺ�ʽ
  *            һ��H'��Ϊ������,����Ϊת�þ���
  */

float first_Kalman_Filter(first_kalman_filter_t* p, float dat)
{
    p->X_mid =p->A*p->X_last;                     //�ٶȶ�Ӧ��ʽ(1)    x(k|k-1) = A*X(k-1|k-1)+B*U(k)+W(K)     ״̬����
    p->P_mid = p->A*p->P_last+p->Q;               //�ٶȶ�Ӧ��ʽ(2)    p(k|k-1) = A*p(k-1|k-1)*A'+Q            �۲ⷽ��
    p->kg = p->P_mid/(p->P_mid+p->R);             //�ٶȶ�Ӧ��ʽ(4)    kg(k) = p(k|k-1)*H'/(H*p(k|k-1)*H'+R)   ���¿���������
    p->X_now = p->X_mid + p->kg*(dat-p->X_mid);   //�ٶȶ�Ӧ��ʽ(3)    x(k|k) = X(k|k-1)+kg(k)*(Z(k)-H*X(k|k-1))  ��������ֵ
    p->P_now = (1-p->kg)*p->P_mid;                //�ٶȶ�Ӧ��ʽ(5)    p(k|k) = (I-kg(k)*H)*P(k|k-1)           ���º������Э����
    p->P_last = p->P_now;                         //״̬����
    p->X_last = p->X_now;
    return p->X_now;							  //���Ԥ����x(k|k)
}

/************ һ�׿������˲�*************/


//�ο�����RPս�ӵ��˲��㷨

/************ ������ֵ�˲� *************/

/**
  * @brief    average_init
  * @note    �����˲�����ʼ�������ó���
  * @param  None
  * @retval None
  */
void average_init(moving_Average_Filter *Aver, uint8_t length)
{
    uint16_t i;

    for(i = 0; i<MAF_MaxSize; i++)
        Aver->num[i] = 0;

    if(length >MAF_MaxSize)
    {
        length = MAF_MaxSize;
    }

    Aver->length = length;
    Aver->pot = 0;
    Aver->aver_num = 0;
    Aver->total = 0;
}

/**
  * @brief    average_clear
  * @note    �����˲������
  * @param  None
  * @retval None
  */
void average_clear(moving_Average_Filter *Aver)
{
    uint16_t i;

    for(i = 0; i<MAF_MaxSize; i++)
        Aver->num[i] = 0;

    Aver->pot = 0;
    Aver->aver_num = 0;
    Aver->total = 0;
}

/**
  * @brief    average_fill
  * @note    �����˲������ĳ��ֵ
  * @param  None
  * @retval None
  * @author  RobotPilots
  */
void average_fill(moving_Average_Filter *Aver, float temp)
{
    uint16_t i;

    for(i = 0; i<(Aver->length); i++)
        Aver->num[i] = temp;

    Aver->pot = 0;
    Aver->aver_num = temp;
    Aver->total = temp*(Aver->length);
}

/**
  * @brief    average_add
  * @note    ����ƽ���˲���������У��Ƚ��ȳ�
  * @param  None
  * @retval None
  * @author  RobotPilots
  */
void average_add(moving_Average_Filter *Aver, float add_data)
{

    Aver->total -=  Aver->num[Aver->pot];
    Aver->total += add_data;

    Aver->num[Aver->pot] = add_data;

    Aver->aver_num = (Aver->total)/(Aver->length);
    Aver->pot++;

    if(Aver->pot == Aver->length)
    {
        Aver->pot = 0;
    }

}

/**
  * @brief    average_get
  * @note    ��ȡ��ǰpre�ε����ݣ�����������鳤����ȡ��¼�����������
  * @param  None
  * @retval None
  * @author  RobotPilots
  */
float average_get(moving_Average_Filter *Aver, uint16_t pre)
{
    float member;
    uint8_t temp;

    if(Aver->pot != 0)
    {
        temp = Aver->pot-1;
    }
    else
    {
        temp = Aver->length-1;
    }

    if(pre>Aver->length)
        pre = pre % Aver->length;

    if(pre>temp)
    {
        pre = Aver->length+temp-pre;
    }
    else
    {
        pre = temp-pre;
    }

    member = Aver->num[pre];

    return member;
}
