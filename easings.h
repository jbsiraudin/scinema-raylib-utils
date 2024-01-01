#pragma once

#include "raymath.h"

static float EaseOutSin(float x);
static float EaseOutCubic(float x);

static float EaseOutSin(float x)
{
	return sinf((x * PI) / 2.f);
}

static float EaseOutCubic(float x)
{
	return 1 - powf(1.f - x, 3.f);
}