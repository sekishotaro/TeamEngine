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
	minienemy = Sprite::Create(4, { 40.0f,20.0f });

	// �I�u�W�F�N�g����
	model = Model::LoadFromOBJ("sphere");
	block = Model::LoadFromOBJ("block");
	rope = Model::LoadFromOBJ("rope");

	enemy = Object3d::Create();
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
	enemy->SetModel(model);
	player->SetModel(model);

	Rope->SetModel(rope);
	Rope->SetScale({ 0.3, 5, 0.3 });
}

void GamePlayScene::Finalize()
{
	delete model;
}

void GamePlayScene::Update()
{
	// �Q�[���V�[���̖��t���[������
	Input *input = Input::GetInstance();

	//M�L�[�Ń}�b�v�`�b�v�ݒu
	if (input->TriggerKey(DIK_M) || true)
	{
		MapCreate(0);
	}

	//�v���C���[����
	{
		//���W�X�V
		p_pos = player->GetPosition();

		//�v���C���[�̈ړ�
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
		//�v���C���[�̍U��
		if ((input->TriggerKey(DIK_SPACE) || input->PushButton(Button_B)) && is_attack == false)
		{
			is_attack = true;

			//�v���C���[�̌����œ����������ς���
			if (player->GetRotation().y == 0)
			{
				angle = 180;
			}
			else if (player->GetRotation().y == 180)
			{
				angle = 0;
			}
		}
		//�v���C���[�̃W�����v
		if ((input->TriggerKey(DIK_W) || input->TriggerButton(Button_A)) && is_air == false && is_jump == false)
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

			if (MapCollide(player, 0, is_jump))
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

			if (MapCollide(player, 0))
			{
				//������
				p_down = 0;
				is_air = false;
			}
		}

		//�~�j�}�b�v�p���W�ϊ�
		miniplayer->SetPosition({ p_pos.x , -p_pos.y + 28 });

		camera->SetTarget(player->GetPosition());
		camera->SetEye({ player->GetPosition().x, player->GetPosition().y, player->GetPosition().z - 60.0f });
	}

	//�G�l�~�[����
	{
		//�ʏ���
		if (is_normal && is_attack == false)
		{
			//�ړ�
			e_pos.x += e_add;

			//�[�܂ōs������
			if (e_pos.x >= 50.0f || e_pos.x <= -50.0f)
			{
				e_add = -e_add;

				//������ς���
				XMFLOAT3 e_rot = enemy->GetRotation();
				e_rot.y += 180.0f;
				if (e_rot.y >= 360)
				{
					e_rot.y = 0;
				}
				enemy->SetRotation(e_rot);
			}
		}
		//�v���C���[��ǔ�
		else if (is_chase && is_attack == false)
		{
			//�v���C���[�ƃG�l�~�[�̋���
			XMFLOAT2 pe_len = { p_pos.x - e_pos.x, p_pos.y - e_pos.y };

			//���̒l�Ȃ�
			if (pe_len.x > 0)
			{
				e_pos.x += e_add;
				if (p_pos.x < e_pos.x)
				{
					e_pos.x = p_pos.x;
				}
			}
			//���̒l�Ȃ�
			else if (pe_len.x < 0)
			{
				e_pos.x -= e_add;
				if (p_pos.x > e_pos.x)
				{
					e_pos.x = p_pos.x;
				}
			}
		}
		//�U�����
		if (is_attack)
		{
			//�E�����Ȃ�
			if (player->GetRotation().y == 0)
			{
				CircularMotion(e_pos, p_pos, 10, angle, -20);
				enemy->SetPosition(e_pos);
			} 
			//�������Ȃ�
			else if (player->GetRotation().y == 180)
			{
				CircularMotion(e_pos, p_pos, 10, angle, 20);
				enemy->SetPosition(e_pos);
			}
			enemy->Update();

			//�}�b�v�̓����蔻��
			if (MapCollide(enemy, 0))
			{
				e_down = 0;
				is_attack = false;
			}
		}
		else
		{
			//�v���C���[�̎��R����
			//���~�x���}�C�i�X
			e_pos = enemy->GetPosition();
			e_down -= gravity;
			e_pos.y += e_down;
			enemy->SetPosition(e_pos);
			enemy->Update();

			if (MapCollide(enemy, 0))
			{
				//������
				e_down = 0;
			}
		}
		enemy->Update();

		//�~�j�}�b�v�p���W�ϊ�
		minienemy->SetPosition({ e_pos.x , -e_pos.y + 28 });

	}

	//���݂̍��W���擾
	XMFLOAT3 playerPosition = player->GetPosition();
	XMFLOAT3 ropePosition = Rope->GetPosition();
	XMFLOAT3 enemyPosition = enemy->GetPosition();

	//���݂̃X�P�[�����擾
	XMFLOAT3 ropeScale = Rope->GetScale();

	//���[�v�̍��W�l
	Rope->SetPosition({ (enemyPosition.x + playerPosition.x) / 2,(enemyPosition.y + playerPosition.y) / 2,(enemyPosition.z + playerPosition.z) / 2 });
	//���[�v�̑傫��
	Rope->SetScale({ ropeScale.x, ropeScale.y, ropeScale.z });
	//���[�v�X�V
	Rope->Update();
	//�v���C���[�ƃG�l�~�[���Ȃ�
	ropeMove();

	//�����̍ő�l�𒴂�����
	float limit_y = player->GetPosition().y;
	if (limit_y < -500.0f)
	{
		player->SetPosition({ 0, 10, 0 });
		p_down = 0;
	}
	limit_y = enemy->GetPosition().y;
	if (limit_y < -500.0f)
	{
		enemy->SetPosition({ 0, 10, 0 });
		e_down = 0;
	}
	MapCollide(enemy, 0);

	//�v���C���[�̍��W�iX�FY�j
	DebugText::GetInstance()->Print(50, 30 * 1, 2, "%f", objBlock[8][0]->GetPosition().x);
	DebugText::GetInstance()->Print(50, 30 * 2, 2, "%f", objBlock[8][0]->GetPosition().y);
	DebugText::GetInstance()->Print(50, 30 * 3, 2, "%f", enemy->GetPosition().x);
	DebugText::GetInstance()->Print(50, 30 * 4, 2, "%f", enemy->GetPosition().y);
	DebugText::GetInstance()->Print(50, 30 * 5, 2, "%f", e_add);
	DebugText::GetInstance()->Print(50, 30 * 6, 2, "%f", e_down);

	if (input->TriggerKey(DIK_RETURN))
	{
		//BGM�~�߂�
		//Audio::GetInstance()->SoundStop("zaza.wav");
		
		//�V�[���؂�ւ�
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//�I�u�W�F�N�g���̍X�V
	camera->Update();
	enemy->Update();
	player->Update();
	Rope->Update();
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			objBlock[y][x]->Update();
		}
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
	enemy->Draw();
	player->Draw();
	Rope->Draw();

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
	minienemy->Draw();
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

void GamePlayScene::SpawnEnemy()
{
	
}

void GamePlayScene::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 center_pos, const float r, int& angle, const int add)
{
	angle += add;

	pos.x = (cosf(3.14 / 180.0f * angle) * r) + center_pos.x;//�~�^���̏���
	pos.y = (sinf(3.14 / 180.0f * angle) * r) + center_pos.y;//�~�^���̏���
}

bool GamePlayScene::MapCollide(const std::unique_ptr<Object3d>& object, int mapNumber, bool is_jump)
{
	float a = object->GetPosition().x;
	float b = object->GetPosition().y;
	float r = 1.0f * object->GetScale().x;

	float x = 0;
	float y = 0;
	float w = 0;
	float h = 0;

	bool is_hit = false;

	for (int b_y = map_max_y - 1; b_y >= 0; b_y--) //y��12
	{
		for (int b_x = map_max_x - 1; b_x >= 0; b_x--) //x��52
		{
			if (Mapchip::GetChipNum(b_x, b_y, map[mapNumber]) == Ground)
			{
				x = objBlock[b_y][b_x]->GetPosition().x;
				y = objBlock[b_y][b_x]->GetPosition().y;
				w = 2.5f * objBlock[b_y][b_x]->GetScale().x;
				h = 2.5f * objBlock[b_y][b_x]->GetScale().y;

				if (powf(y - b, 2) < powf(h + r, 2) && (x - w <= a && a <= x + w))
				{
					XMFLOAT3 pos = object->GetPosition();
					//��
					if (y - b < 0)
					{
						pos.y = y + h + r;
						is_hit = true;
						/*if (is_jump)
						{
							this->is_jump = false;
						}*/
					}
					//��
					else if (y - b > 0)
					{
						pos.y = y - h - r;
						if (!is_jump)
						{
							is_hit = true;
						}
						else
						{
							p_add = 0;
						}
					}
					object->SetPosition(pos);
					object->Update();
				}
				else if (powf(x - a, 2) < powf(w + r, 2) && (y - h <= b  && b <= y + h))
				{
					XMFLOAT3 pos = object->GetPosition();
					//�E
					if (x - a < 0)
					{
						pos.x = x + w + r;
						if (!is_jump)
						{
							is_hit = true;
						}
					}
					//��
					else if (x - a > 0)
					{
						pos.x = x - w - r;
						if (!is_jump)
						{
							is_hit = true;
						}
					}
					object->SetPosition(pos);
					object->Update();
				}
			}
		}
	}

	return is_hit;
}

void GamePlayScene::ropeMove()
{
	if (catched)
	{
		//���[�v�̈ʒu���擾
		XMFLOAT3 ropePosition = Rope->GetPosition();
		//�v���C���[�̈ʒu���擾
		XMFLOAT3 playerPosition = player->GetPosition();
		//�G�l�~�[�̈ʒu
		XMFLOAT3 enemyPosition = enemy->GetPosition();

		//�v���C���[�ƃG�l�~�[�̋���
		XMFLOAT2 length = { playerPosition.x - enemyPosition.x, playerPosition.y - enemyPosition.y };
		float len = sqrtf(length.x * length.x + length.y * length.y);
		if (len > max_rope)
		{
			float wq = len / max_rope;
			len = max_rope;
			enemy->SetPosition({ playerPosition.x - length.x / wq, playerPosition.y - length.y / wq, 0 });
		}

		Rope->SetScale({ 0.3f, len / 2, 0.3f });

		float angleX = rope_angle->PosForAngle(playerPosition.x, ropePosition.y, ropePosition.x, playerPosition.y);

		Rope->SetRotation({ 0, 0 ,XMConvertToDegrees(angleX) });
	}
}