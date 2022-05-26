#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Vector3 :public XMFLOAT3 {
public:
	//コンストラクタ
	Vector3();
	Vector3(float x, float y, float z);

	//メンバ関数
	float length();
	XMFLOAT3 normalize();
	float dot(const Vector3& v);
	Vector3 cross(const Vector3& v);

	//単項演算子オーバーロード
	Vector3 operator+();
	Vector3 operator-();

	//代入演算子オーバーロード
	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator/=(float s);
};

//2項演算子オーバーロード
//※いろんな引数(引数の型と順序)のパターンに対応するため、以下のように準備している
const XMFLOAT3 operator+(const XMFLOAT3& v1, const XMFLOAT3& v2);
const XMFLOAT3 operator+(const XMFLOAT3& v, float s);
const XMFLOAT3 operator-(const XMFLOAT3& v1, const XMFLOAT3& v2);
const XMFLOAT3 operator-(const XMFLOAT3& v, float s);
const XMFLOAT3 operator*(const XMFLOAT3& v, float s);
const XMFLOAT3 operator*(float s, const XMFLOAT3& v);
const XMFLOAT3 operator/(const XMFLOAT3& v, float s);

//補間関数
//線形補間(1次関数補間)
const XMFLOAT3 lerp(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
const XMFLOAT3 easeInQuad(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
const XMFLOAT3 easeOutQuad(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
const XMFLOAT3 easeInOutQuad(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
const XMFLOAT3 easeInQuint(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
const XMFLOAT3 easeOutQuint(const XMFLOAT3& start, const XMFLOAT3& end, const float t);
const XMFLOAT3 easeInOutQuint(const XMFLOAT3& start, const XMFLOAT3& end, const float t);