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
#include "Mapchip.h"

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
		NORMAL, TWICE, JUMP, FIN
	};

	struct EnemyData
	{
		XMFLOAT3 e_pos; //���W
		XMFLOAT3 old_e_pos; //1�t���[���O�̍��W

		float e_x_radius; //���S�_����x�̍ő�l�܂�
		float e_y_radius; //���S�_����y�̍ő�l�܂�

		bool is_normal; //�p�j���
		bool is_bounce; //���˂���
		bool is_catch; //�ߔ����
		bool is_alive; //����
		bool is_grand; //�n�ʂɂ��Ă��邩
		bool is_turn; //�U�����󂯂���

		float e_speed; //�ړ���
		float e_down; //���~�x

		float e_acc; //�����x

		int angle; //�~�^���̊p�x

		int enemy_type; //�G�l�~�[�̎��
		bool can_catch; //�ߔ��\��

		bool is_add; //���Z���邩

		float turn_move; //���Ԃ�Ƃ��̓���

		int escape_time; //������܂ł̎���

		float max_rope; //���[�v�̍ő�
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
	static const int EnemySpawnMax = 50;
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
	Model* modelPlayer = nullptr;
	Model* enemy_model_2 = nullptr;
	Model* block = nullptr;
	Model* rope = nullptr;

	//�I�u�W�F�N�g
	std::vector<std::vector<int>> map; //�}�b�v�`�b�v
	std::unique_ptr<Object3d> objBlock[map_max_y][map_max_x]; //�X�e�[�W�u���b�N
	std::unique_ptr<Object3d> player = nullptr; //�v���C���[
	std::unique_ptr<Object3d> enemy[EnemySpawnMax]; //�G�l�~�[
	std::unique_ptr<Object3d> Rope[EnemySpawnMax]; //���[�v

	//FBX����
	FbxModel* fbxModel1 = nullptr;
	FbxObject3d* fbxObject1 = nullptr;

	//�G�t�F�N�g
	int maxLocus = 0;
	std::vector<std::unique_ptr<Object3d>>locus;
	std::unique_ptr<Object3d> shock = nullptr;
	//locus = new std::unique_ptr<Object3d>[MaxLocus];
	//std::unique_ptr<Object3d> locus[MaxLocus]; //�O�՗p�|��
	Model* locusModel = nullptr; //�O��
	Model* shockWaveModel = nullptr; //�g��
	bool shockFlag = false; //�g�����J�n�����

	//�V�[���ڍs�����m�F�t���O
	bool ConvertFlag = false;
	//�Q�[���J�n�J�E���g�����m�F�t���O
	bool countFinishFlag = false;
	//�Q�[���I����G���h�V�[���Ɉڍs�m�F�p�t���O
	bool finishFinishFlag = false;
	//�G���h�V�[���ɃR���o�[�g����
	bool endConvertflag = false;
	//�Q�[���v���C�\�t���O
	bool PlayPossibleflag = false;

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
	Sprite* spriteLevelUp = nullptr;
	Sprite* minimap = nullptr; //�X�e�[�W(�~�j�}�b�v)
	Sprite* finish = nullptr; //�I���

	//UI�E�X�R�Aetc
	int score; //�X�R�A
	int scoreTick[EnemySpawnMax]; //�X�R�A�v�Z�p
	bool is_shake; //�V�F�C�N�t���O
	int shake_power; //�V�F�C�N�̋���
	int shake_time; //�V�F�C�N����
	float shake_x; //x���̕�
	float shake_y; //y���̕�
	float lastTime; //�c�莞��
	int level;
	int oldLevel;
	int enemySpawn; //�G�̐�
	int catch_count; //�������Ă邩
	float levelTime;


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

	bool is_damage; //�U�����󂯂���
	int damage_time; //�_���[�W���A�N�V�����̎���
	float damage_move; //�_���[�W�̈ړ�

	//���G����
	bool is_invincible; //���G���
	float invincible_time; //���G����

	//�G�l�~�[
	EnemyData enemy_data[EnemySpawnMax]; //�G�l�~�[�̃f�[�^

	XMFLOAT3 c_pos; //�J�����̎�_���W
	bool camera_chase; //�J�����������o���܂ł̎���

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
	bool MapCollide(XMFLOAT3& pos, float radiusX, float radiusY, float &add, int mapNumber,const XMFLOAT3 old_pos, bool is_jump = false);

	/// <summary>
	/// ���[�v�̊p�x�ύX
	/// <summary>
	void RopeMove(const int num);

	//�I�u�W�F�N�g���m�̓����蔻��
	bool CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b);

	//���������߂�
	float GetObjectLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b);

	/// <summary>
	/// ������̒��ɂ��邩
	/// </summary>
	/// <param name="playerPosition">���@</param>
	/// <param name="negativePoint">�Ώۂ̕��̒��_</param>
	/// <param name="positivePoint">�Ώۂ̐��̒��_</param>
	/// <param name="width">����</param>
	/// <param name="height">�c��</param>
	/// <returns>����</returns>
	bool inFrustum(XMFLOAT3 playerPosition, XMFLOAT3 negativePoint, XMFLOAT3 positivePoint, const float width, const float height);
};