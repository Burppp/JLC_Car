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
