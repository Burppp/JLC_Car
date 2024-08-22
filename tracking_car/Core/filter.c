//
// Created by Lumos on 2024/8/22.
//

#include "main.h"
#include "filter.h"

/************ 一阶卡尔曼滤波*************/

/**
  * @name   kalmanCreate
  * @brief  创建一个卡尔曼滤波器
  * @param  p:  滤波器
  *         T_Q:系统噪声协方差
  *         T_R:测量噪声协方差
  *
  * @retval none
  * @attention R固定,Q越大代表越信任测量值，Q无穷代表只用测量值
  *		       Q固定,R越大代表越信任预测值，R无穷代表只用预测值
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
  * @brief  卡尔曼滤波器
  * @param  p:  滤波器
  *         dat:待滤波数据
  * @retval 滤波后的数据
  * @attention Z(k)是系统输入,即测量值   X(k|k)是卡尔曼滤波后的值,即最终输出
  *            A=1 B=0 H=1 I=1  W(K)  V(k)是高斯白噪声,叠加在测量值上了,可以不用管
  *            以下是卡尔曼的5个核心公式
  *            一阶H'即为它本身,否则为转置矩阵
  */

float first_Kalman_Filter(first_kalman_filter_t* p, float dat)
{
    p->X_mid =p->A*p->X_last;                     //百度对应公式(1)    x(k|k-1) = A*X(k-1|k-1)+B*U(k)+W(K)     状态方程
    p->P_mid = p->A*p->P_last+p->Q;               //百度对应公式(2)    p(k|k-1) = A*p(k-1|k-1)*A'+Q            观测方程
    p->kg = p->P_mid/(p->P_mid+p->R);             //百度对应公式(4)    kg(k) = p(k|k-1)*H'/(H*p(k|k-1)*H'+R)   更新卡尔曼增益
    p->X_now = p->X_mid + p->kg*(dat-p->X_mid);   //百度对应公式(3)    x(k|k) = X(k|k-1)+kg(k)*(Z(k)-H*X(k|k-1))  修正估计值
    p->P_now = (1-p->kg)*p->P_mid;                //百度对应公式(5)    p(k|k) = (I-kg(k)*H)*P(k|k-1)           更新后验估计协方差
    p->P_last = p->P_now;                         //状态更新
    p->X_last = p->X_now;
    return p->X_now;							  //输出预测结果x(k|k)
}

/************ 一阶卡尔曼滤波*************/


//参考深圳RP战队的滤波算法

/************ 滑动均值滤波 *************/

/**
  * @brief    average_init
  * @note    滑动滤波器初始化，设置长度
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
  * @note    滑动滤波器清空
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
  * @note    滑动滤波器填充某个值
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
  * @note    滑动平均滤波器进入队列，先进先出
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
  * @note    获取第前pre次的数据，如果超出数组长度则取记录的最早的数据
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
