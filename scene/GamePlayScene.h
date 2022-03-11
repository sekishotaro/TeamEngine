#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <memory>
#include "Camera.h"
#include <vector>

class GamePlayScene : public BaseScene
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
	void Initialize() override;

	/// <summary>
	/// ���
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// �A�b�v�f�[�g
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;

	/// <summary>
	/// �Q�[���V�[���p
	/// </summary>
	Sprite *spriteBG = nullptr;
	Model *model = nullptr;
	Camera *camera = nullptr;
	Model* block = nullptr;
	std::vector<std::vector<int>> map;
	std::vector<Object3d*> box;
	std::unique_ptr<Object3d> objBlock[12][52];

	//�v���C���[
	std::unique_ptr<Object3d> player = nullptr; //�I�u�W�F�N�g�N���X
	XMFLOAT3 p_pos = {0, 0, 0}; //���W
	float p_max_speed = 0.5f; //�ō����x
	bool is_jump = false; // �W�����v�t���O
	float p_add = 0; //�㏸�x
	float p_gravity = 0; //�d�͉����x
	bool is_attack = false;

	//�G�l�~�[
	std::unique_ptr<Object3d> enemy; //�I�u�W�F�N�g�N���X
	XMFLOAT3 e_pos = {0, 0, 0}; //���W
	bool is_normal = false;
	bool is_chase = false;
	float e_add = 0.25f;
	int max_spawn = 25;
	int angle = 0;

	/// <summary>
	/// �G�l�~�[����
	/// </summary>
	void SpawnEnemy(bool& active, int& spawn_num);

	/// <summary>
	/// �~�^��
	/// </summary>
	void CircularMotion(XMFLOAT3& pos, const XMFLOAT3 center_pos, const float r, int& angle, const int add);

	/// <summary>
	/// //�}�b�v�`�b�v����
	/// </summary>
	void MapCreate(int mapNumber);

	/// <summary>
	/// �}�b�v�`�b�v�����蔻��
	/// </summary>
	void MapCollide(int mapNumber);
};

