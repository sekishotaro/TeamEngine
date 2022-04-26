#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <memory>
#include "Camera.h"
#include "Angle.h"
#include <vector>
#include "FbxObject3d.h"

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

public: //�T�u�N���X
	enum EnemyType
	{
		NORMAL, TWICE
	};

	struct EnemyData
	{
		XMFLOAT3 e_pos; //���W
		XMFLOAT3 old_e_pos; //1�t���[���O�̍��W

		float e_x_radius; //���S�_����x�̍ő�l�܂�
		float e_y_radius; //���S�_����y�̍ő�l�܂�

		bool is_normal; //�p�j���
		bool is_catch; //�ߔ����
		bool is_alive; //����
		bool is_grand; //�n�ʂɂ��Ă��邩

		float e_speed; //�ړ���
		float e_down; //���~�x

		int angle; //�~�^���̊p�x

		int enemy_type; //�G�l�~�[�̎��
		bool can_catch; //�ߔ��\��

		bool is_add; //���Z���邩
	};

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
	
	//�萔
	static const int EnemySpawnMax = 40;
	//�}�b�v�`�b�v1�̑傫��
	const float LAND_SCALE = 5.0f;

	//�}�b�v�`�b�v�̔ԍ�
	enum MapNumber
	{
		None, Ground
	};

	//�N���X
	Angle* rope_angle = new Angle(); //Angle
	Camera* camera = nullptr; //�J����

	//���f��
	Model* model = nullptr;
	Model* enemy_model_2 = nullptr;
	Model* block = nullptr;
	Model* rope = nullptr;

	//�I�u�W�F�N�g
	std::vector<std::vector<int>> map; //�}�b�v�`�b�v
	std::unique_ptr<Object3d> objBlock[12][52]; //�X�e�[�W�u���b�N
	std::unique_ptr<Object3d> player = nullptr; //�v���C���[
	std::unique_ptr<Object3d> enemy[EnemySpawnMax]; //�G�l�~�[
	std::unique_ptr<Object3d> Rope[EnemySpawnMax]; //���[�v

	//FBX����
	FbxModel* fbxModel1 = nullptr;
	FbxObject3d* fbxObject1 = nullptr;

	//�G�t�F�N�g
	int maxLocus = 0;
	std::vector<std::unique_ptr<Object3d>>locus;
	//locus = new std::unique_ptr<Object3d>[MaxLocus];
	//std::unique_ptr<Object3d> locus[MaxLocus]; //�O�՗p�|��
	Model* locusModel = nullptr; //�O��
	

	//�X�v���C�g
	Sprite* spriteBG = nullptr; //�w�i
	Sprite* miniplayer = nullptr; //�v���C���[(�~�j�}�b�v)
	Sprite* minienemy[EnemySpawnMax]; //�G�l�~�[(�~�j�}�b�v)
	Sprite* spriteTime[10]; //�X�R�A
	Sprite* spriteScore[10]; //�X�R�A
	Sprite* spriteLevel[10]; //�X�R�A
	Sprite* spriteCoron = nullptr;
	Sprite* texScore = nullptr;
	Sprite* texLevel = nullptr;
	Sprite* spriteTimer = nullptr;
	Sprite* minimap = nullptr; //�X�e�[�W(�~�j�}�b�v)

	//UI�E�X�R�Aetc
	int score; //�X�R�A
	bool is_shake; //�V�F�C�N�t���O
	int shake_time; //�V�F�C�N����
	float shake_x; //x���̕�
	float shake_y; //y���̕�
	float lastTime; //�c�莞��
	int level;
	int enemySpawn; //�G�̐�


	//�ėp�ϐ�
	float gravity; //�d�͉����x

	//�v���C���[
	XMFLOAT3 p_pos; //���W
	XMFLOAT3 old_p_pos; //1�t���[���O�̍��W

	float p_x_radius; //���S�_����x�̍ő�l�܂�
	float p_y_radius; //���S�_����y�̍ő�l�܂�

	bool is_jump; // �W�����v�t���O

	float p_add; //�㏸�x
	float p_down; //���~�x

	bool is_attack; //�U���t���O
	bool is_air;//�󒆃t���O

	//�G�l�~�[
	EnemyData enemy_data[EnemySpawnMax]; //�G�l�~�[�̃f�[�^

	//���[�v
	float max_rope; //���[�v�̍ő�

	/// <summary>
	/// �G�l�~�[����
	/// </summary>
	void SpawnEnemy(int mapNumber, int enemyNumber);

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
	bool MapCollide(XMFLOAT3& pos, float radiusX, float radiusY, int mapNumber,const XMFLOAT3 old_pos, bool is_jump = false);

	/// <summary>
	/// ���[�v�̊p�x�ύX
	/// <summary>
	void RopeMove(XMFLOAT3& pos, const int num);

	//�I�u�W�F�N�g���m�̓����蔻��
	bool CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b);

	//���������߂�
	float GetLengthObject(XMFLOAT3 pos_a, XMFLOAT3 pos_b);

	/// <summary>
	/// ������̒��ɂ��邩
	/// </summary>
	/// <param name="playerPosition">���@</param>
	/// <param name="negativePoint">�Ώۂ̕��̒��_</param>
	/// <param name="positivePoint">�Ώۂ̐��̒��_</param>
	/// <returns>����</returns>
	bool inFrustum(XMFLOAT3 playerPosition, XMFLOAT3 negativePoint, XMFLOAT3 positivePoint);
};