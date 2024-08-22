//
// Created by Lumos on 2024/8/22.
//

#include "main.h"

#define MAF_MaxSize 100

typedef struct {
    float X_last; 
    float X_mid;  
    float X_now;  
    float P_mid;  
    float P_now;  
    float P_last; 
    float kg;     
    float A;      
    float B;
    float Q;
    float R;
    float H;
}first_kalman_filter_t;

typedef struct moving_Average_Filter
{
    float num[MAF_MaxSize];
    uint8_t length;
    uint8_t pot;
    float total;
    float aver_num;

}moving_Average_Filter;

typedef struct {

    float           _cutoff_freq1;
    float           _a11;
    float           _a21;
    float           _b01;
    float           _b11;
    float           _b21;
    float           _delay_element_11;       
    float           _delay_element_21;       
}second_lowPass_filter;

void first_Kalman_Create(first_kalman_filter_t *p, float T_Q, float T_R);
float first_Kalman_Filter(first_kalman_filter_t* p, float dat);
