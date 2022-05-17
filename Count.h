#pragma once
#include "Sprite.h"
#include <DirectXMath.h>

class Count
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
	/// ������
	/// </summary>
	static void Initilize();

	/// <summary>
	/// ���
	/// </summary>
	static void Finalize();

	/// <summary>
	/// 3�J�E���g�_�E��
	/// </summary>
	/// <param name="finishFlag">�C���m�F</param>
	static void CountDown3(bool &finishFlag);

	/// <summary>
	/// 3�J�E���g�_�E���̕`��
	/// </summary>
	static void CountDown3Draw();

	/// <summary>
	/// �I���`
	/// </summary>
	/// <param name="finishFlag">�C���m�F</param>
	static void Fnish(bool& finishFlag, const float &x);

	/// <summary>
	/// �I���`��
	/// </summary>
	static void FnishDraw();

	/// <summary>
	/// ���ׂƂ̕`��
	/// </summary>
	static void Draw();

private:
	//static Sprite* count0;
	static Sprite* count1;
	static Sprite* count2;
	static Sprite* count3;
	//static Sprite* count4;
	//static Sprite* count5;
	static Sprite*  countStart;
	static Sprite*  countFinish;


	static float count3Time;
	static bool count3finishFlag;	//3�J�E���g�X�^�[�g�C���m�F�t���O

	static float countFinishTime;
	static bool fnishFinishFlag;	//Finish�C���m�F�t���O
};

