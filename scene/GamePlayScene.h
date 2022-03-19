#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <memory>
#include "Camera.h"
#include "Angle.h"
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
	Sprite* spriteBG = nullptr;
	Sprite* miniplayer = nullptr;
	Sprite* minienemy = nullptr;
	Sprite* minimap = nullptr;
	Model *model = nullptr;
	Camera *camera = nullptr;
	Model* block = nullptr;
	Model* rope = nullptr;
	Angle* rope_angle = new Angle();
	std::vector<std::vector<int>> map;
	std::vector<Object3d*> box;
	std::unique_ptr<Object3d> objBlock[12][52];
	enum MapNumber //�}�b�v�`�b�v�̔ԍ�
	{
		None, Ground
	};

	//�ėp�ϐ�
	float gravity = 0.15f; //�d�͉����x
	int angle = 0; //�~�^���̊p�x

	//�v���C���[
	std::unique_ptr<Object3d> player = nullptr; //�I�u�W�F�N�g�N���X
	XMFLOAT3 p_pos = {0, 5, 0}; //���W
	XMFLOAT3 old_p_pos;
	float p_max_speed = 0.5f; //�ō����x
	bool is_jump = false; // �W�����v�t���O
	float p_add = 0; //�㏸�x
	float p_down = 0; //���~�x
	bool is_attack = false; //�U���t���O
	bool is_air = false;//�󒆃t���O

	//�G�l�~�[
	std::unique_ptr<Object3d> enemy; //�I�u�W�F�N�g�N���X
	XMFLOAT3 e_pos = {10, 5, 0}; //���W
	XMFLOAT3 old_e_pos;
	std::unique_ptr<Object3d> mini_enemy; //�I�u�W�F�N�g�N���X
	XMFLOAT3 mini_e_pos = { 0, 0, 0 }; //���W
	bool is_normal = false; //�ʏ���
	bool is_chase = false; //�ǐՏ��
	bool is_catch = true; //�ߔ����
	float e_add = 0.25f; //�ړ���
	float e_down = 0; //���~�x

	//���[�v
	std::unique_ptr<Object3d> Rope = nullptr; //�I�u�W�F�N�g�N���X
	float max_rope = 15.0f;

	/// <summary>
	/// �G�l�~�[����
	/// </summary>
	void SpawnEnemy();

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
	bool MapCollide(const std::unique_ptr<Object3d>& object, int mapNumber, const XMFLOAT3 old_pos, bool is_jump = false);

	/// <summary>
	/// ���[�v�̊p�x�ύX
	/// <summary>
	void RopeMove();

	//�I�u�W�F�N�g���m�̓����蔻��
	bool CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b);

	//���������߂�
	float GetLengthObject(XMFLOAT3 pos_a, XMFLOAT3 pos_b);
};