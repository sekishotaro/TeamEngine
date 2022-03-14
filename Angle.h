#pragma once

#include <math.h>

class Angle {
public:
	void Initialize();
	void Update();
	float GetAngle();
	float PosForAngle(float startPosX, float startPosY, float endPosX, float endPosY);
	float lR(float startPosX, float startPosY, float endPosX, float endPosY);

private:
	//    cx:座標　cy:座標  　mv:移動量   
	float startPosX = 0;
	float startPosY = 0;
	float endPosX = 0;
	float endPosY = 0;
	float angleX = 0;
	float angleY = 0;
	float resultAngle = 0;
};

