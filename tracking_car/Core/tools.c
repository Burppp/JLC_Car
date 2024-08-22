//
// Created by Lumos on 2024/8/20.
//

#include "tools.h"

float float_abs(float num)
{
	if(num < 0)
	{
		return -1 * num;
	}
	return num;
}

int int_abs(int num)
{
	if(num < 0)
	{
		return -1 * num;
	}
	return num;
}

int32_t val_limit(int32_t val, int32_t max_output, int32_t min_output)
{
	if(max_output < val)
	{
		return max_output;
	}
	if(min_output > val)
	{
		return min_output;
	}
	return val;
}
