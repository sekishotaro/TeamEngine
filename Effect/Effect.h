#pragma once
#include "Object3d.h"
#include <DirectXMath.h>
#include <vector>
#include "Camera.h"
#include "Sprite.h"
#include "Object3d.h"

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
	static void ShockWaveUpdate(std::unique_ptr<Object3d>& object, Camera *camera, float &radius, bool *flag);

	/// <summary>
	/// ������
	/// </summary>
	static void Initialize();

	/// <summary>
	/// ���
	/// </summary>
	static void Finalize();

	/// <summary>
	/// �^�C�����~�b�g
	/// </summary>
	/// <param name="time">����</param>
	static void TimeLimitEffectDraw(float& time);

	/// <summary>
	/// ���j���G�t�F�N�g����
	/// </summary>
	/// <param name="enemyNum"></param>
	/// <param name="pos"></param>
	static void DestroyEffectCreate(int enemyNum, const XMFLOAT3 &pos);

	/// <summary>
	/// ���j���G�t�F�N�g�X�V
	/// </summary>
	/// <param name="camera">�J����</param>
	/// <param name="player">���@�̈ʒu</param>
	static void DestroyEffectUpdate(Camera* camera, XMFLOAT3& player);

	/// <summary>
	/// ���j���G�t�F�N�g�̕`��
	/// </summary>
	static void DestroyEffectDraw();

	/// <summary>
	/// �_�ŃI�u�W�F�N�g
	/// </summary>
	/// <param name="object"></param>
	static void flashingEffectDraw(const std::unique_ptr<Object3d>& object, float flashingTime);


public:
	static const float maxTime;					//�ő�l
	static std::vector<float> nowTime;			//��
	static std::vector<float> timeRate;			//�O�Ղ̉�%���Ԃ��i�񂾂�(��)
	static std::vector<XMFLOAT3> generationPos; //���������ꏊ
	static std::vector<float> delayTime;		//�x������
	static std::vector <bool> processStartFlag;
	static Sprite* timeLimitSprite;

	static Model* destroyEffectModel;
	static std::vector<std::unique_ptr<Object3d>> destroyEffect;
	static std::vector<float> nowTimeD;
	static std::vector<float> timeRateD;
	static std::vector<float> delayTimeD;
	static std::vector <bool> processStartFlagD;
	static std::vector<XMFLOAT3> generationPosD;
};

 