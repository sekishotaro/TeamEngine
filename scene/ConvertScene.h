#pragma once
#include "Sprite.h"
#include <DirectXMath.h>

class ConvertScene
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
	/// �������J��
	/// </summary>
	static void InitializeOut();

	/// <summary>
	/// ����������
	/// </summary>
	static void InitializeIn();

	/// <summary>
	/// ���
	/// </summary>
	static void Finalize();

	/// <summary>
	/// ���J��
	/// </summary>
	static void besideOut(bool &flag);

	/// <summary>
	/// ���ɕ���
	/// </summary>
	static void besideIn(bool &flag);

	/// <summary>
	/// �`��
	/// </summary>
	static void Draw();
private:
	static Sprite* switchIn; //�V�[���؂�ւ��p1
	static Sprite* switchOut; //�V�[���؂�ւ��p2

};
