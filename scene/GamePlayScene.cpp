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

	Sprite::LoadTexture(1, L"Resources/Number1.png");
	Sprite::LoadTexture(2, L"Resources/Number2.png");
	Sprite::LoadTexture(3, L"Resources/Number3.png");
	Sprite::LoadTexture(4, L"Resources/Number4.png");
	Sprite::LoadTexture(5, L"Resources/Number5.png");
	Sprite::LoadTexture(6, L"Resources/Number6.png");
	Sprite::LoadTexture(7, L"Resources/Number7.png");
	Sprite::LoadTexture(8, L"Resources/Number8.png");
	Sprite::LoadTexture(9, L"Resources/Number9.png");
	Sprite::LoadTexture(10, L"Resources/Number0.png");
	Sprite::LoadTexture(11, L"Resources/background.png");
	Sprite::LoadTexture(12, L"Resources/minimap.png");
	Sprite::LoadTexture(13, L"Resources/miniplayer.png");
	Sprite::LoadTexture(14, L"Resources/minienemy.png");
	// �w�i�X�v���C�g����
	spriteBG = Sprite::Create(11, { 0.0f,0.0f });

	//�X�v���C�g����
	minimap = Sprite::Create(12, { 0.0f,0.0f });
	miniplayer = Sprite::Create(13, { 20.0f,20.0f });
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		minienemy[i] = Sprite::Create(14, { 40.0f,20.0f });
	}
	for (int i = 0; i < 10; i++)
	{
		spriteNumber[i] = Sprite::Create(i, { 0,0 });
	}

	// �I�u�W�F�N�g����
	model = Model::LoadFromOBJ("sphere");
	block = Model::LoadFromOBJ("block");
	rope = Model::LoadFromOBJ("rope");

	for (int i = 0; i < EnemySpawnMax; i++)
	{
		enemy[i] = Object3d::Create();
		Rope[i] = Object3d::Create();
	}
	player = Object3d::Create();

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

		max_rope[i] = 15.0f;
		Rope[i]->SetModel(rope);
		Rope[i]->SetScale({ 0.3, 5, 0.3 });
		Rope[i]->Update();
	}
	player->SetModel(model);
	player->SetPosition(p_pos);
	player->Update();

	for (int i = 0; i < EnemySpawnMax; i++)
	{
		angle[i] = 0;
	}
}

void GamePlayScene::Finalize()
{
	safe_delete(model);
	safe_delete(spriteBG);
	safe_delete(miniplayer);
	for (int i = 0; i < 10; i++)
	{
		safe_delete(minienemy[i]);
	}
	safe_delete(minimap);
	safe_delete(camera);
	safe_delete(block);
	safe_delete(rope);
	safe_delete(rope_angle);
}

void GamePlayScene::Update()
{
	// �Q�[���V�[���̖��t���[������
	Input* input = Input::GetInstance();

	//M�L�[�Ń}�b�v�`�b�v�ݒu
	if (input->TriggerKey(DIK_M) || true)
	{
		MapCreate(0);
		for (int y = 0; y < map_max_y; y++)
		{
			for (int x = 0; x < map_max_x; x++)
			{
				objBlock[y][x]->Update();
			}
		}
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
		for (int i = 0; i < EnemySpawnMax; i++) 
		{
			//�v���C���[�̍U��
			if ((input->TriggerKey(DIK_SPACE) || input->PushButton(Button_B)) && is_attack == false && is_catch[i])
			{
				//�v���C���[�̌����œ����������ς���
				if (player->GetRotation().y == 0 && p_pos.x >= enemy[i]->GetPosition().x)
				{
					angle[i] = static_cast<int>(XMConvertToDegrees(rope_angle->PosForAngle(p_pos.x, enemy[i]->GetPosition().y, enemy[i]->GetPosition().x, p_pos.y))) - 90;
					is_attack = true;
				}
				else if (player->GetRotation().y == 180 && p_pos.x < enemy[i]->GetPosition().x)
				{
					angle[i] = static_cast<int>(XMConvertToDegrees(rope_angle->PosForAngle(p_pos.x, enemy[i]->GetPosition().y, enemy[i]->GetPosition().x, p_pos.y))) - 90;
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

			if (MapCollide(player, 0, old_p_pos, is_air))
			{
				//������
				p_down = 0;
				is_air = false;
			}
		}

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
		if (!is_alive[i])
		{
			SpawnEnemy(0, i);
		} 
		else if (is_alive[i])
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
					CircularMotion(e_pos[i], p_pos, GetLengthObject(p_pos, e_pos[i]), angle[i], -15);
					enemy[i]->SetPosition(e_pos[i]);
				}
				//�������Ȃ�
				else if (player->GetRotation().y == 180)
				{
					CircularMotion(e_pos[i], p_pos, GetLengthObject(p_pos, e_pos[i]), angle[i], 15);
					enemy[i]->SetPosition(e_pos[i]);
				}
				enemy[i]->Update();
				//�}�b�v�̓����蔻��
				if (MapCollide(enemy[i], 0, old_e_pos[i]))
				{
					is_alive[i] = false;
					is_catch[i] = false;
					is_attack = false;
					for (int i = 0; i < EnemySpawnMax; i++)
					{
						if (is_catch[i])
						{
							is_attack = true;
							break;
						}
					}
					enemy[i]->Update();
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
				enemy[i]->Update();
				//�}�b�v�̓����蔻��
				if (MapCollide(enemy[i], 0, old_e_pos[i]))
				{
					e_down[i] = 0;
				}
			}
		}

		if (is_catch[i])
		{
			Rope[i]->SetPosition({ (player->GetPosition().x + enemy[i]->GetPosition().x) / 2, (player->GetPosition().y + enemy[i]->GetPosition().y) / 2, (player->GetPosition().z + enemy[i]->GetPosition().z) / 2 });
		}

		//���[�v�̍X�V
		RopeMove();

		if (is_catch[i])
		{
			Rope[i]->SetPosition({ (player->GetPosition().x + enemy[i]->GetPosition().x) / 2, (player->GetPosition().y + enemy[i]->GetPosition().y) / 2, (player->GetPosition().z + enemy[i]->GetPosition().z) / 2 });
		}

		//�~�j�}�b�v�p���W�ϊ�
		minienemy[i]->SetPosition({ e_pos[i].x , -e_pos[i].y + 28 });

		//�����̍ő�l�𒴂�����
		float limit_y = enemy[i]->GetPosition().y;
		if (limit_y < -500.0f)
		{
			enemy[i]->SetPosition({ 20, 10, 0 });
			e_down[i] = 0;
		}

		MapCollide(enemy[i], 0, old_e_pos[i]);

		//�I�u�W�F�N�g���̍X�V
		enemy[i]->Update();
		Rope[i]->Update();
	}

	float limit_y = player->GetPosition().y;
	if (limit_y < -500.0f)
	{
		player->SetPosition({ 0, 10, 0 });
		p_down = 0;
	}
	player->Update();

	camera->Update();

	if (input->TriggerKey(DIK_RETURN))
	{
		//BGM�~�߂�
		//Audio::GetInstance()->SoundStop("zaza.wav");

		//�V�[���؂�ւ�
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//�v���C���[�̍��W�iX�FY�j
	DebugText::GetInstance()->Print(50, 35 * 1, 2, "%f", objBlock[8][0]->GetPosition().x);
	DebugText::GetInstance()->Print(50, 35 * 2, 2, "%f", objBlock[8][0]->GetPosition().y);
	DebugText::GetInstance()->Print(50, 35 * 3, 2, "rope_X:%f", Rope[0]->GetPosition().x);
	DebugText::GetInstance()->Print(50, 35 * 4, 2, "rope_Y:%f", Rope[0]->GetPosition().y);
	DebugText::GetInstance()->Print(50, 35 * 5, 2, "player_Y:%f", player->GetPosition().y);
	DebugText::GetInstance()->Print(50, 35 * 6, 2, "enemy_Y:%f", enemy[0]->GetPosition().y);
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
			Rope[i]->Draw();
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
	spriteNumber[score % 10]->Draw();

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

	for (int b_x = 0; b_x < map_max_x; b_x++) //y��12
	{
		for (int b_y = 0; b_y < map_max_y; b_y++) //x��52
		{
			if (Mapchip::GetChipNum(b_x, b_y, map[mapNumber]) == Ground)
			{
				x = objBlock[b_y][b_x]->GetPosition().x;
				y = objBlock[b_y][b_x]->GetPosition().y;
				r_x = 2.5f * objBlock[b_y][b_x]->GetScale().x;
				r_y = 2.5f * objBlock[b_y][b_x]->GetScale().y;
				//��
				if (b - r < y + r_y && y < old_pos.y - r && (x - r_x <= a && a <= x + r_x))
				{
					XMFLOAT3 pos = object->GetPosition();
					pos.y = y + r_y + r;
					b = pos.y;
					object->SetPosition(pos);
					object->Update();
					y_hit = true;
					break;
				} 
				//��
				else if (b + r > y - r_y && old_pos.y + r < y && (x - r_x <= a && a <= x + r_x))
				{
					XMFLOAT3 pos = object->GetPosition();
					pos.y = y - r_y - r;
					b = pos.y;
					object->SetPosition(pos);
					object->Update();
					if (!is_jump)
					{
						y_hit = true;
					}
					else
					{
						p_add = 0;
					}
					break;
				}
			}
		}
		if (y_hit)
		{
			break;
		}
	}
	for (int b_y = 0; b_y < map_max_y; b_y++) //x��52
	{
		for (int b_x = 0; b_x < map_max_x; b_x++) //y��12
		{
			if (Mapchip::GetChipNum(b_x, b_y, map[mapNumber]) == Ground)
			{
				x = objBlock[b_y][b_x]->GetPosition().x;
				y = objBlock[b_y][b_x]->GetPosition().y;
				r_x = 2.5f * objBlock[b_y][b_x]->GetScale().x;
				r_y = 2.5f * objBlock[b_y][b_x]->GetScale().y;
				//��
				if (a - r < x + r_x && x < old_pos.x - r && (y - r_y <= b && b <= y + r_y))
				{
					XMFLOAT3 pos = object->GetPosition();
					pos.x = x + r_x + r;
					a = pos.x;
					object->SetPosition(pos);
					object->Update();
					if (!is_jump)
					{
						x_hit = true;
					}
					break;
				}
				//�E
				else if (a + r > x - r_x && old_pos.x + r < x && (y - r_y <= b && b <= y + r_y))
				{
					XMFLOAT3 pos = object->GetPosition();
					pos.x = x - r_x - r;
					a = pos.x;
					object->SetPosition(pos);
					object->Update();
					if (!is_jump)
					{
						x_hit = true;
					}
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
			Rope[i]->Update();

			//�v���C���[�̈ʒu���擾
			XMFLOAT3 playerPosition = player->GetPosition();
			//�G�l�~�[�̈ʒu
			XMFLOAT3 enemyPosition = enemy[i]->GetPosition();
			//���[�v�̈ʒu���擾
			XMFLOAT3 ropePosition = Rope[i]->GetPosition();

			//�v���C���[�ƃG�l�~�[�̋���
			XMFLOAT2 length = { playerPosition.x - enemyPosition.x, playerPosition.y - enemyPosition.y };
			float len = GetLengthObject(playerPosition, enemyPosition);
			//�ő�l���傫���Ȃ�
			if (len > max_rope[i])
			{
				float wq = len / max_rope[i];
				len = max_rope[i];
				enemy[i]->SetPosition({ playerPosition.x - length.x / wq, playerPosition.y - length.y / wq, 0 });
				enemy[i]->Update();
			}

			//���[�v�̒���
			Rope[i]->SetScale({ 0.3f, len / 2, 0.3f });

			float angleX = rope_angle->PosForAngle(playerPosition.x, ropePosition.y, ropePosition.x, playerPosition.y);

			Rope[i]->SetRotation({ 0, 0 ,XMConvertToDegrees(angleX) });
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
