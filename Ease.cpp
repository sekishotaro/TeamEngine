#include "Ease.h"
#include<cmath>

float Ease::lerp(const float& start, const float& end, const float time)
{
	return start * (1.0f - time) + end * time;
}

float Ease::easeIn(const float& start, const float& end, const float time)
{
	float t = time * time;
	return start * (1.0f - t) + end * t;
}

float Ease::easeOut(const float& start, const float& end, const float time)
{
	float y = time * (2 - time);
	return start * (1.0f - y) + end * y;
}

float Ease::easeInOut(const float& start, const float& end, const float time)
{
	float y = time * time * (3 - 2 * time);
	return start * (1.0f - y) + end * y;
}