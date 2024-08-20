//
// Created by Lumos on 2024/8/20.
//

#include "PID.h"

static void abs_limit(float *a, float ABS_MAX)
{
    if (*a > ABS_MAX)
        *a = ABS_MAX;
    if (*a < -ABS_MAX)
        *a = -ABS_MAX;
}

/**
  * @brief     PID ��ʼ������
  * @param[in] pid: PID �ṹ��
  * @param[in] max_out: ������
  * @param[in] intergral_limit: �����޷�
  * @param[in] kp/ki/kd: ���� PID ����
  */
void pid_init(pid_t *pid, uint32_t max_out, uint32_t intergral_limit, \
              float kp, float ki, float kd)
{
    pid->integral_limit = intergral_limit;
    pid->max_output     = max_out;
    pid->p = kp;
    pid->i = ki;
    pid->d = kd;
}

/**
  * @brief     PID ���㺯����ʹ��λ��ʽ PID ����
  * @param[in] pid: PID �ṹ��
  * @param[in] get: ��������
  * @param[in] set: Ŀ������
  * @retval    PID �������
  */
float pid_calc(pid_t *pid, float get, float set)
{
    pid->get = get;
    pid->set = set;
    pid->err[NOW] = set - get;

    pid->pout = pid->p * pid->err[NOW];
    pid->iout += pid->i * pid->err[NOW];
    pid->dout = pid->d * (pid->err[NOW] - pid->err[LAST]);

    abs_limit(&(pid->iout), pid->integral_limit);
    pid->out = pid->pout + pid->iout + pid->dout;
    abs_limit(&(pid->out), pid->max_output);

    pid->err[LAST]  = pid->err[NOW];

    return pid->out;
}
