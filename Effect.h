#pragma once

#include "Object3d.h"
#include <DirectXMath.h>
#include <vector>
#include "Camera.h"

class Effect
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

public:

	//�O�Ր���
	static void CreateLocus(std::vector<std::unique_ptr<Object3d>> &object, Model &model, XMFLOAT3& pos);

	//�O�Ս폜
	static void DeletLocus(std::vector<std::unique_ptr<Object3d>>& object, Camera *camera, XMFLOAT3& player);
};

