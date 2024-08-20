//
// Created by Lumos on 2024/8/20.
//

#include "main.h"

#ifndef __pid_H__
#define __pid_H__

enum
{
    LAST  = 0,
    NOW   = 1,
};

/**
  * @brief     PID �ṹ��
  */
typedef struct
{
    /* p��i��d���� */
    float p;
    float i;
    float d;

    /* Ŀ��ֵ������ֵ�����ֵ */
    float set;
    float get;
    float err[2];

    /* p��i��d������������� */
    float pout;
    float iout;
    float dout;

    /* pid��ʽ������������ */
    float out;

    /* pid����������  */
    uint32_t max_output;

    /* pid����������޷� */
    uint32_t integral_limit;

} pid_t;

/**
  * @brief     PID ��ʼ������
  * @param[in] pid: PID �ṹ��
  * @param[in] max_out: ������
  * @param[in] intergral_limit: �����޷�
  * @param[in] kp/ki/kd: ���� PID ����
  */
extern void pid_init(pid_t *pid, uint32_t max_out, uint32_t intergral_limit, \
              float kp, float ki, float kd);

/**
  * @brief     PID ������λ����
  * @param[in] pid: PID �ṹ��
  * @param[in] kp/ki/kd: ���� PID ����
  */
extern void pid_reset(pid_t *pid, float kp, float ki, float kd);

/**
  * @brief     PID ���㺯����ʹ��λ��ʽ PID ����
  * @param[in] pid: PID �ṹ��
  * @param[in] get: ��������
  * @param[in] set: Ŀ������
  * @retval    PID �������
  */
extern float pid_calc(pid_t *pid, float get, float set);
extern float pid_calc_balance(pid_t *pid, float get, float set,float gyro_y);
extern float pid_calc_KI_Separation(pid_t* pid,float get,float set,float err_threshold);

extern float pid_loop_calc(pid_t *pid,float get,float set,float max_value,float min_value);

#endif
