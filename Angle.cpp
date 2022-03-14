#include "Angle.h"

void Angle::Initialize() {
}

void Angle::Update() {
	angleX = endPosX - startPosX;
	angleY = endPosY - startPosY;

	resultAngle = atan2(angleX, angleY);
}

float Angle::GetAngle() {
	return resultAngle;
}

float Angle::PosForAngle(float startPosX, float startPosY, float endPosX, float endPosY) {
	angleX = endPosX - startPosX;
	angleY = endPosY - startPosY;

	resultAngle = atan2(angleX, angleY);

	return resultAngle;
}

float Angle::lR(float startPosX, float startPosY, float endPosX, float endPosY) {
	float px = endPosX - startPosX;
	float py = endPosY - startPosY;

	float l = sqrt(px * px + py * py);
	return l;
}

////旋回
//if (keyg[37]) {	//←
//	angle += 5;
//	//cos,sinは配列を使用しているので角度が0～359になるように調整する必要がある
//	if (angle >= 360)
//		angle -= 360;
//	//angle%=360;と書ける
//}
//if (keyg[39]) {	//→
//	angle -= 5;
//
//	if (angle < 0)
//		angle += 360;
//	//angle=(angle+360)%360;と書ける(たぶん
//}
//
////向いている角度へ進む
//if (keyg[38]) {	//↑
//	endPosX += fcos[angle] * mv;
//	endPosY -= fsin[angle] * mv;
//}