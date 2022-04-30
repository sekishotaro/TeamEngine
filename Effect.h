#pragma once

#include "Object3d.h"
#include <DirectXMath.h>
#include <vector>
#include "Camera.h"
#include "Sprite.h"

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

	/// <summary>
	/// �O�Ր���
	/// </summary>
	/// <param name="object">�O��</param>
	/// <param name="model">�O�Ճ��f��</param>
	/// <param name="pos">�O�Ք����n�_</param>
	static void CreateLocus(std::vector<std::unique_ptr<Object3d>> &object, Model &model, XMFLOAT3& pos);

	/// <summary>
	/// �O�Ս폜
	/// </summary>
	/// <param name="object">�O��</param>
	/// <param name="camera">�J����</param>
	/// <param name="player">���@(��ʒ��S��)</param>
	static void DeletLocus(std::vector<std::unique_ptr<Object3d>>& object, Camera *camera, XMFLOAT3& player);

	/// <summary>
	/// �g��
	/// </summary>
	/// <param name="object">�g��</param>
	/// <param name="generationPoint">�����n�_</param>
	/// <param name="flag">�t���O</param>
	static void ShockWaveUpdate(std::unique_ptr<Object3d>& object, XMFLOAT3& generationPoint, bool *flag);
};

