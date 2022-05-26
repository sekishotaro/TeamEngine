#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Vector3 :public XMFLOAT3 {
public:
	//�R���X�g���N�^
	Vector3();
	Vector3(float x, float y, float z);

	//�����o�֐�
	float length();
	XMFLOAT3 normalize();
	float dot(const Vector3& v);
	Vector3 cross(const Vector3& v);

	//�P�����Z�q�I�[�o�[���[�h
	Vector3 operator+();
	Vector3 operator-();

	//������Z�q�I�[�o�[���[�h
	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);
};

//2�����Z�q�I�[�o�[���[�h
//�������Ȉ���(�����̌^�Ə���)�̃p�^�[���ɑΉ����邽�߁A�ȉ��̂悤�ɏ������Ă���
const XMFLOAT3 operator+(const XMFLOAT3& v1, const XMFLOAT3& v2);
const XMFLOAT3 operator+(const XMFLOAT3& v, float s);
const XMFLOAT3 operator-(const XMFLOAT3& v1, const XMFLOAT3& v2);
const XMFLOAT3 operator-(const XMFLOAT3& v, float s);
const XMFLOAT3 operator*(const XMFLOAT3& v, float s);
const XMFLOAT3 operator*(float s, const XMFLOAT3& v);
const XMFLOAT3 operator/(const XMFLOAT3& v, float s);

//��Ԋ֐�
//���`���(1���֐����)
const XMFLOAT3 lerp(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
const XMFLOAT3 easeInQuad(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
const XMFLOAT3 easeOutQuad(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
const XMFLOAT3 easeInOutQuad(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
const XMFLOAT3 easeInQuint(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
const XMFLOAT3 easeOutQuint(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
const XMFLOAT3 easeInOutQuint(const XMFLOAT3& start, const XMFLOAT3& end, const float t);