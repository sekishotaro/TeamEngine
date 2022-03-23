#include "GamePlayScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"
#include "DirectXCommon.h"
#include "Mapchip.h"
#include <safe_delete.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>

using namespace DirectX;

void GamePlayScene::Initialize()
{
	Audio::GetInstance()->LoadWave("futta-dream.wav");
	Audio::GetInstance()->LoadWave("zaza.wav");

	//Audio::GetInstance()->PlayWave("zaza.wav", true);

	// �J��������
	camera = new Camera(WinApp::window_width, WinApp::window_height);

	// �J�����Z�b�g
	Object3d::SetCamera(camera);

	// �e�N�X�`���ǂݍ���

	Sprite::LoadTexture(1, L"Resources/background.png");
	Sprite::LoadTexture(2, L"Resources/minimap.png");
	Sprite::LoadTexture(3, L"Resources/miniplayer.png");
	Sprite::LoadTexture(4, L"Resources/minienemy.png");
	// �w�i�X�v���C�g����
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });

	//�X�v���C�g����
	minimap = Sprite::Create(2, { 0.0f,0.0f });
	miniplayer = Sprite::Create(3, { 20.0f,20.0f });
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		minienemy[i] = Sprite::Create(4, { 40.0f,20.0f });
	}

	// �I�u�W�F�N�g����
	model = Model::LoadFromOBJ("sphere");
	block = Model::LoadFromOBJ("block");
	rope = Model::LoadFromOBJ("rope");

	for (int i = 0; i < EnemySpawnMax; i++)
	{
		enemy[i] = Object3d::Create();
	}
	player = Object3d::Create();

	Rope = Object3d::Create();

	//�}�b�v�`�b�v�p��CSV�ǂݍ���
	//(map, "Resource/scv/�Ȃ񂽂�.csv")�Œǉ��\
	Mapchip::CsvToVector(map, "Resources/csv/demo.csv");//mapNum=0


	//�}�b�v�`�b�v�p�̃I�u�W�F�N�g�̏�����
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			objBlock[y][x] = Object3d::Create();
			objBlock[y][x]->SetModel(block);
			objBlock[y][x]->SetScale({ 1.0f,1.0f,1.0f });
			objBlock[y][x]->SetPosition({ 1000.0f,1000.0f,0.0f });
		}
	}

	//�I�u�W�F�N�g�Ƀ��f�����Ђ��t����
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		e_pos[i] = { 20,5,0 }; //���W
		is_normal[i] = false; //�ʏ���
		is_chase[i] = false; //�ǐՏ��
		is_catch[i] = false; //�ߔ����
		is_alive[i] = false; //���S���
		is_alive[0] = true;//�����`��̂���1�C�����o��
		e_speed[i] = 0.25f; //�ړ���
		e_down[i] = 0.0f; //���~�x
		enemy[i]->SetModel(model);
		enemy[i]->SetPosition(e_pos[i]);
		//enemy[i]->Update();
	}
	player->SetModel(model);
	player->SetPosition(p_pos);
	player->Update();

	Rope->SetModel(rope);
	Rope->SetScale({ 0.3, 5, 0.3 });
	Rope->Update();
}

void GamePlayScene::Finalize()
{
	delete model;
}

void GamePlayScene::Update()
{
	// �Q�[���V�[���̖��t���[������
	Input* input = Input::GetInstance();

	//M�L�[�Ń}�b�v�`�b�v�ݒu
	if (input->TriggerKey(DIK_M) || true)
	{
		MapCreate(0);
	}

	old_p_pos = player->GetPosition();
	for (int i = 0; i < EnemySpawnMax; i++) {
		old_e_pos[i] = enemy[i]->GetPosition();
	}

	//�v���C���[����
	{
		//���W�X�V
		p_pos = player->GetPosition();

		//�v���C���[�̈ړ�
		if (is_attack == false)
		{
			if (input->LeftStickAngle().x)
			{
				p_pos.x += input->LeftStickAngle().x / (1.0f / p_max_speed) * 1.0f; // * 1.0f = ���{����

				//�i�s�����Ɍ�����ς���
				if (input->LeftStickAngle().x >= 0)
				{
					player->SetRotation(XMFLOAT3(0, 0, 0));
				}
				else
				{
					player->SetRotation(XMFLOAT3(0, 180, 0));
				}
			}
			//�L�[�{�[�h�p
			if (input->PushKey(DIK_D))
			{
				p_pos.x += 0.5f;
				player->SetRotation(XMFLOAT3(0, 0, 0));
			}
			if (input->PushKey(DIK_A))
			{
				p_pos.x -= 0.5f;
				player->SetRotation(XMFLOAT3(0, 180, 0));
			}
		}
		for (int i = 0; i < EnemySpawnMax; i++) {
			//�v���C���[�̍U��
			if ((input->TriggerKey(DIK_SPACE) || input->PushButton(Button_B)) && is_attack == false && is_catch[i])
			{

				//�v���C���[�̌����œ����������ς���
				if (player->GetRotation().y == 0 && p_pos.x >= enemy[i]->GetPosition().x)
				{
					angle = static_cast<int>(XMConvertToDegrees(rope_angle->PosForAngle(p_pos.x, enemy[i]->GetPosition().y, enemy[i]->GetPosition().x, p_pos.y))) - 90;
					is_attack = true;
				}
				else if (player->GetRotation().y == 180 && p_pos.x < enemy[i]->GetPosition().x)
				{
					angle = static_cast<int>(XMConvertToDegrees(rope_angle->PosForAngle(p_pos.x, enemy[i]->GetPosition().y, enemy[i]->GetPosition().x, p_pos.y))) - 90;
					is_attack = true;
				}
			}
		}
		//�v���C���[�̃W�����v
		if ((input->TriggerKey(DIK_W) || input->TriggerButton(Button_A)) && is_air == false && is_jump == false && is_attack == false)
		{
			is_jump = true;

			//�㏸���̏�����
			p_add = 2.5f;
		}
		//�W�����v����
		if (is_jump)
		{
			//���W�̏㏸
			p_add -= gravity;
			p_pos.y += p_add;

			player->SetPosition(p_pos);
			player->Update();

			if (MapCollide(player, 0, old_p_pos, is_jump))
			{
				//������
				is_jump = false;
				p_add = 0;
			}
		}
		//�v���C���[�̎��R����
		if (is_jump == false)
		{
			is_air = true;
			//���~�x���}�C�i�X
			p_down -= gravity;
			p_pos.y += p_down;

			player->SetPosition(p_pos);
			player->Update();

			if (MapCollide(player, 0, old_p_pos))
			{
				//������
				p_down = 0;
				is_air = false;
			}
		}
		//�v���C���[�ƃG�l�~�[���ڐG������
		//for (int i = 0; i < EnemySpawnMax; i++)
		//{
		//	if (is_alive[i])
		//	{

		//	}
		//}
		//�~�j�}�b�v�p���W�ϊ�
		p_pos = player->GetPosition();
		miniplayer->SetPosition({ p_pos.x , -p_pos.y + 28 });

		camera->SetTarget(p_pos);
		camera->SetEye({ p_pos.x, p_pos.y, p_pos.z - 60.0f });
	}



	//�G�l�~�[����
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		//�G�̃X�|�[��
		if (is_alive[i] == false)
		{
			SpawnEnemy(0, i);
		}

		if (is_alive[i])
		{
			e_pos[i] = enemy[i]->GetPosition();
			//�v���C���[�ƃG�l�~�[���ڐG������
			if (CollisionObject(player, enemy[i]))
			{
				is_catch[i] = true;
			}
			//�ʏ���
			if (is_normal[i] && is_attack == false)
			{
				//�ړ�
				e_pos[i].x += e_speed[i];

				//�[�܂ōs������
				if (e_pos[i].x >= 50.0f || e_pos[i].x <= -50.0f)
				{
					e_speed[i] = -e_speed[i];

					//������ς���
					XMFLOAT3 e_rot;
					e_rot = enemy[i]->GetRotation();
					e_rot.y += 180.0f;
					if (e_rot.y >= 360)
					{
						e_rot.y = 0;
					}
					enemy[i]->SetRotation(e_rot);
				}
			}
			//�v���C���[��ǔ�
			else if (is_chase[i] && is_attack == false)
			{
				//�v���C���[�ƃG�l�~�[�̋���
				XMFLOAT2 pe_len = { p_pos.x - e_pos[i].x, p_pos.y - e_pos[i].y };

				//���̒l�Ȃ�
				if (pe_len.x > 0)
				{
					e_pos[i].x += e_speed[i];
					if (p_pos.x < e_pos[i].x)
					{
						e_pos[i].x = p_pos.x;
					}
				}
				//���̒l�Ȃ�
				else if (pe_len.x < 0)
				{
					e_pos[i].x -= e_speed[i];
					if (p_pos.x > e_pos[i].x)
					{
						e_pos[i].x = p_pos.x;
					}
				}
			}

			//�U�����
			if (is_catch[i] && is_attack)
			{
				//�E�����Ȃ�
				if (player->GetRotation().y == 0)
				{
					CircularMotion(e_pos[i], p_pos, GetLengthObject(p_pos, e_pos[i]), angle, -15);
					enemy[i]->SetPosition(e_pos[i]);
				}
				//�������Ȃ�
				else if (player->GetRotation().y == 180)
				{
					CircularMotion(e_pos[i], p_pos, GetLengthObject(p_pos, e_pos[i]), angle, 15);
					enemy[i]->SetPosition(e_pos[i]);
				}
				//enemy[i]->Update();

				//�}�b�v�̓����蔻��
				if (MapCollide(enemy[i], 0, old_e_pos[i]))
				{
					e_down[i] = 0;
					for (int i = 0; i < EnemySpawnMax; i++) {
						is_catch[i] = false;
					}
					is_attack = false;
				}
			}
			else
			{
				//�v���C���[�̎��R����
				//���~�x���}�C�i�X
				e_pos[i] = enemy[i]->GetPosition();
				e_down[i] -= gravity;
				e_pos[i].y += e_down[i];
				enemy[i]->SetPosition(e_pos[i]);
				//enemy[i]->Update();
				//�}�b�v�̓����蔻��
				if (MapCollide(enemy[i], 0, old_e_pos[i]))
				{
					e_down[i] = 0;
				}
			}
			//enemy[i]->Update();
		}

		if (is_catch[i]) {

			Rope->SetPosition({ (player->GetPosition().x + enemy[i]->GetPosition().x) / 2, (player->GetPosition().y + enemy[i]->GetPosition().y) / 2, (player->GetPosition().z + enemy[i]->GetPosition().z) / 2 });

		}
		//���[�v�̍X�V
		RopeMove();
		if (is_catch[i]) {

			Rope->SetPosition({ (player->GetPosition().x + enemy[i]->GetPosition().x) / 2, (player->GetPosition().y + enemy[i]->GetPosition().y) / 2, (player->GetPosition().z + enemy[i]->GetPosition().z) / 2 });

		}
		//�~�j�}�b�v�p���W�ϊ�
		minienemy[i]->SetPosition({ e_pos[i].x , -e_pos[i].y + 28 });

		//�����̍ő�l�𒴂�����
		float limit_y = player->GetPosition().y;
		if (limit_y < -500.0f)
		{
			player->SetPosition({ 0, 10, 0 });
			p_down = 0;
		}

		limit_y = enemy[i]->GetPosition().y;

		if (limit_y < -500.0f)
		{
			enemy[i]->SetPosition({ 0, 10, 0 });
			e_down[i] = 0;
		}

		MapCollide(enemy[i], 0, old_e_pos[i]);

		if (input->TriggerKey(DIK_RETURN))
		{
			//BGM�~�߂�
			//Audio::GetInstance()->SoundStop("zaza.wav");

			//�V�[���؂�ւ�
			SceneManager::GetInstance()->ChangeScene("TITLE");
		}

		//�I�u�W�F�N�g���̍X�V
		camera->Update();
		enemy[i]->Update();
		player->Update();
		Rope->Update();
		for (int y = 0; y < map_max_y; y++)
		{
			for (int x = 0; x < map_max_x; x++)
			{
				objBlock[y][x]->Update();
			}
		}

		//�v���C���[�̍��W�iX�FY�j
		DebugText::GetInstance()->Print(50, 30 * 1, 2, "%f", objBlock[8][0]->GetPosition().x);
		DebugText::GetInstance()->Print(50, 30 * 2, 2, "%f", objBlock[8][0]->GetPosition().y);
		DebugText::GetInstance()->Print(50, 30 * 3, 2, "rope_X:%f", Rope->GetPosition().x);
		DebugText::GetInstance()->Print(50, 30 * 4, 2, "rope_Y:%f", Rope->GetPosition().y);
		DebugText::GetInstance()->Print(50, 30 * 5, 2, "player_X:%f", player->GetPosition().y);
		DebugText::GetInstance()->Print(50, 30 * 6, 2, "enemy_Y:%f", enemy[i]->GetPosition().y);
	}
}

void GamePlayScene::Draw()
{
	// �Q�[���V�[���̕`��
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList *cmdList = DirectXCommon::GetInstance()->GetCmdList();

	// �w�i�X�v���C�g�`��O����
	Sprite::PreDraw(cmdList);
	// �w�i�X�v���C�g�`��
	spriteBG->Draw();

	/// <summary>
	/// �����ɔw�i�X�v���C�g�̕`�揈����ǉ��ł���
	/// </summary>

	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
	// �[�x�o�b�t�@�N���A
	DirectXCommon::GetInstance()->ClearDepthBuffer();

	// 3D�I�u�W�F�N�g�`��O����
	Object3d::PreDraw(cmdList);

	// 3D�I�u�N�W�F�N�g�̕`��
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		if (is_alive[i])
		{
			enemy[i]->Draw();
		}


		if (is_catch[i])
		{
			Rope->Draw();
		}
	}
	player->Draw();

	//�}�b�v�`�b�v�̕`��
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			objBlock[y][x]->Draw();
		}
	}
	/// <summary>
	/// ������3D�I�u�W�F�N�g�̕`�揈����ǉ��ł���
	/// </summary>

	// 3D�I�u�W�F�N�g�`��㏈��
	Object3d::PostDraw();

	// �O�i�X�v���C�g�`��O����
	Sprite::PreDraw(cmdList);

	//�~�j�}�b�v�̕`��
	minimap->Draw();
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		if (is_alive[i])
		{
			minienemy[i]->Draw();
		}
	}
	miniplayer->Draw();

	// �f�o�b�O�e�L�X�g�̕`��
	DebugText::GetInstance()->DrawAll(cmdList);

	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
}

void GamePlayScene::MapCreate(int mapNumber)
{
	//�}�b�v�`�b�v1�̑傫��
	const float LAND_SCALE = 5.0f;
	for (int y = 0; y < map_max_y; y++) {//(y��12)
		for (int x = 0; x < map_max_x; x++) {//(x��52)

			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == Ground)
			{
				//�ʒu�Ƒ傫���̕ύX(���͑傫���͕ύX���Ȃ���)
				//objBlock[y][x]->SetScale({ LAND_SCALE, LAND_SCALE, LAND_SCALE });
				objBlock[y][x]->SetPosition({ x * LAND_SCALE,  -y * LAND_SCALE, 0 });
			}
		}
	}
}

void GamePlayScene::SpawnEnemy(int mapNumber, int enemyNumber)
{
	int spawnY = 0;
	int spawnX = 0;
	spawnY = rand() % map_max_y;
	spawnX = rand() % map_max_x;
	const float LAND_SCALE = 5.0f;
	if (Mapchip::GetChipNum(spawnX, spawnY, map[mapNumber]) == None)
	{
		enemy[enemyNumber]->SetPosition({ spawnX * LAND_SCALE,  -spawnY * LAND_SCALE, 0 });//�ʒu���Z�b�g
		is_alive[enemyNumber] = true;//�X�|�[��
	}
}

void GamePlayScene::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 center_pos, const float r, int& angle, const int add)
{
	angle += add;

	pos.x = (cosf(3.14 / 180.0f * angle) * r) + center_pos.x;//�~�^���̏���
	pos.y = (sinf(3.14 / 180.0f * angle) * r) + center_pos.y;//�~�^���̏���
}

bool GamePlayScene::MapCollide(const std::unique_ptr<Object3d>& object, int mapNumber,const XMFLOAT3 old_pos, bool is_jump)
{
	//����Ώ�
	float a = object->GetPosition().x;
	float b = object->GetPosition().y;
	float r = 1.0f * object->GetScale().x;

	//�}�b�v�`�b�v
	float x = 0;
	float y = 0;
	float r_x = 0;
	float r_y = 0;

	//����
	bool x_hit = false;
	bool y_hit = false;

	for (int b_x = map_max_x - 1; b_x >= 0; b_x--) //y��12
	{
		for (int b_y = map_max_y - 1; b_y >= 0; b_y--) //x��52
		{
			if (Mapchip::GetChipNum(b_x, b_y, map[mapNumber]) == Ground)
			{
				x = objBlock[b_y][b_x]->GetPosition().x;
				y = objBlock[b_y][b_x]->GetPosition().y;
				r_x = 2.5f * objBlock[b_y][b_x]->GetScale().x;
				r_y = 2.5f * objBlock[b_y][b_x]->GetScale().y;

				if (powf(y - b, 2) <= powf(r_y + r, 2) && (x - r_x <= a && a <= x + r_x))
				{
					XMFLOAT3 pos = object->GetPosition();
					//��
					if (y < old_pos.y)
					{
						pos.y = y + r_y + r;
						y_hit = true;
					}
					//��
					else
					{
						pos.y = y - r_y - r;
						if (!is_jump)
						{
							y_hit = true;
						}
						else
						{
							p_add = 0;
						}
					}
					object->SetPosition(pos);
					object->Update();
					b = pos.y;
				}
				if (y_hit)
				{
					break;
				}
			}
		}
		if (y_hit)
		{
			break;
		}
	}
	for (int b_y = map_max_y - 1; b_y >= 0; b_y--) //x��52
	{
		for (int b_x = map_max_x - 1; b_x >= 0; b_x--) //y��12
		{
			if (Mapchip::GetChipNum(b_x, b_y, map[mapNumber]) == Ground)
			{
				x = objBlock[b_y][b_x]->GetPosition().x;
				y = objBlock[b_y][b_x]->GetPosition().y;
				r_x = 2.5f * objBlock[b_y][b_x]->GetScale().x;
				r_y = 2.5f * objBlock[b_y][b_x]->GetScale().y;

				if (powf(x - a, 2) <= powf(r_x + r, 2) && (y - r_y <= b && b <= y + r_y))
				{
					XMFLOAT3 pos = object->GetPosition();
					//�E
					if (x < old_pos.x)
					{
						pos.x = x + r_x + r;
						if (!is_jump)
						{
							x_hit = true;
						}
					}
					//��
					else
					{
						pos.x = x - r_x - r;
						if (!is_jump)
						{
							x_hit = true;
						}
					}
					object->SetPosition(pos);
					object->Update();
				}
				if (x_hit)
				{
					break;
				}
			}
		}
		if (x_hit)
		{
			break;
		}
	}

	if (x_hit || y_hit)
	{
		return true;
	}

	return false;
}

void GamePlayScene::RopeMove()
{
	for (int i = 0; i < EnemySpawnMax; i++) {
		if (is_catch[i])
		{
			Rope->Update();

			//�v���C���[�̈ʒu���擾
			XMFLOAT3 playerPosition = player->GetPosition();
			//�G�l�~�[�̈ʒu
			XMFLOAT3 enemyPosition = enemy[i]->GetPosition();
			//���[�v�̈ʒu���擾
			XMFLOAT3 ropePosition = Rope->GetPosition();

			//�v���C���[�ƃG�l�~�[�̋���
			XMFLOAT2 length = { playerPosition.x - enemyPosition.x, playerPosition.y - enemyPosition.y };
			float len = GetLengthObject(playerPosition, enemyPosition);
			//�ő�l���傫���Ȃ�
			if (len > max_rope)
			{
				float wq = len / max_rope;
				len = max_rope;
				enemy[i]->SetPosition({ playerPosition.x - length.x / wq, playerPosition.y - length.y / wq, 0 });
				enemy[i]->Update();
			}

			//���[�v�̒���
			Rope->SetScale({ 0.3f, len / 2, 0.3f });

			float angleX = rope_angle->PosForAngle(playerPosition.x, ropePosition.y, ropePosition.x, playerPosition.y);

			Rope->SetRotation({ 0, 0 ,XMConvertToDegrees(angleX) });
		}
	}
}

bool GamePlayScene::CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b)
{
	//���W
	XMFLOAT3 A = object_a->GetPosition();
	XMFLOAT3 B = object_b->GetPosition();

	//���a
	float a_r = 1.0f * object_a->GetScale().x;
	float b_r = 1.0f * object_a->GetScale().x;

	//����
	float l_x = sqrtf(powf(A.x - B.x, 2));
	float l_y = sqrtf(powf(A.y - B.y, 2));

	//���a�̍��v���Z�����
	if (l_x < a_r + b_r && l_y < a_r + b_r)
	{
		return true;
	}

	return false;
}

float GamePlayScene::GetLengthObject(XMFLOAT3 pos_a, XMFLOAT3 pos_b)
{
	XMFLOAT3 len = { pos_a.x - pos_b.x, pos_a.y - pos_b.y, pos_a.z - pos_b.z };

	return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
}
