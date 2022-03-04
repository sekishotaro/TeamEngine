#pragma once
class Ease
{
public:
	static float lerp(const float& start, const float& end, const float time);

	static float easeIn(const float& start, const float& end, const float time);

	static float easeOut(const float& start, const float& end, const float time);

	static float easeInOut(const float& start, const float& end, const float time);
};