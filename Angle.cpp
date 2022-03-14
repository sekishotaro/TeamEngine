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

////����
//if (keyg[37]) {	//��
//	angle += 5;
//	//cos,sin�͔z����g�p���Ă���̂Ŋp�x��0�`359�ɂȂ�悤�ɒ�������K�v������
//	if (angle >= 360)
//		angle -= 360;
//	//angle%=360;�Ə�����
//}
//if (keyg[39]) {	//��
//	angle -= 5;
//
//	if (angle < 0)
//		angle += 360;
//	//angle=(angle+360)%360;�Ə�����(���Ԃ�
//}
//
////�����Ă���p�x�֐i��
//if (keyg[38]) {	//��
//	endPosX += fcos[angle] * mv;
//	endPosY -= fsin[angle] * mv;
//}