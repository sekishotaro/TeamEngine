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
	// �w�i�X�v���C�g����
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });

	// �I�u�W�F�N�g����
	model = Model::LoadFromOBJ("sphere");
	block = Model::LoadFromOBJ("block");
	rope = Model::LoadFromOBJ("rope");

	mini_enemy = Object3d::Create();
	enemy = Object3d::Create();

	mini_player = Object3d::Create();
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
			objMiniBlock[y][x] = Object3d::Create();
			objMiniBlock[y][x]->SetModel(block);
			objMiniBlock[y][x]->SetScale({ 0.2f,0.2f,0.2f });
			objMiniBlock[y][x]->SetPosition({ 1000.0f,1000.0f,0.0f });
			objBlock[y][x] = Object3d::Create();
			objBlock[y][x]->SetModel(block);
			objBlock[y][x]->SetScale({ 1.0f,1.0f,1.0f });
			objBlock[y][x]->SetPosition({ 1000.0f,1000.0f,0.0f });
		}
	}

	//�I�u�W�F�N�g�Ƀ��f�����Ђ��t����
	mini_enemy->SetModel(model);
	mini_enemy->SetScale({ 0.2f,0.2f,0.2f });
	enemy->SetModel(model);

	mini_player->SetModel(model);
	mini_player->SetScale({ 0.2f,0.2f,0.2f });
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
		MiniMapCreate(0);
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
		if ((input->TriggerKey(DIK_RETURN) || input->PushButton(Button_B)) && is_attack == false)
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
		if ((input->PushKey(DIK_W) || input->TriggerButton(Button_A)) && is_jump == false)
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
			//���~�x���}�C�i�X
			p_down -= gravity;
			p_pos.y += p_down;

			player->SetPosition(p_pos);
			player->Update();

			if (MapCollide(player, 0))
			{
				//������
				p_down = 0;
			}
		}

		//�~�j�}�b�v�p���W�ϊ�
		mini_p_pos.x = (p_pos.x / 5) - 52.5f;
		mini_p_pos.y = (p_pos.y / 5) + 27.5f;
		mini_p_pos.z = p_pos.z / 5;
		mini_player->SetPosition(mini_p_pos);

		camera->SetTarget(player->GetPosition());
		camera->SetEye({ player->GetPosition().x, player->GetPosition().y, player->GetPosition().z - 60.0f });
	}

	//�G�l�~�[����
	{
		//�ʏ���
		if (is_normal)
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
		else if (is_chase)
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
		else if (is_attack)
		{
			//�E�����Ȃ�
			if (player->GetRotation().y == 0)
			{
				CircularMotion(e_pos, p_pos, 10, angle, -20);
				enemy->SetPosition(e_pos);
				enemy->Update();
			} 
			//�������Ȃ�
			else if (player->GetRotation().y == 180)
			{
				CircularMotion(e_pos, p_pos, 10, angle, 20);
				enemy->SetPosition(e_pos);
				enemy->Update();
			}
			//�}�b�v�̓����蔻��
			if (MapCollide(enemy, 0))
			{
				is_attack = false;
			}
		}
		enemy->Update();

		//�~�j�}�b�v�p���W�ϊ�
		mini_e_pos.x = (e_pos.x / 5) - 52.5f;
		mini_e_pos.y = (e_pos.y / 5) + 27.5f;
		mini_e_pos.z = e_pos.z / 5;
		mini_enemy->SetPosition(mini_e_pos);
	}

	ropeRotation();
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

	//�v���C���[�̍��W�iX�FY�j
	DebugText::GetInstance()->Print(50, 30 * 1, 2, "%f", objBlock[8][0]->GetPosition().x);
	DebugText::GetInstance()->Print(50, 30 * 2, 2, "%f", objBlock[8][0]->GetPosition().y);
	DebugText::GetInstance()->Print(50, 30 * 3, 2, "%f", player->GetPosition().x);
	DebugText::GetInstance()->Print(50, 30 * 4, 2, "%f", player->GetPosition().y);
	DebugText::GetInstance()->Print(50, 30 * 5, 2, "%f", p_add);
	DebugText::GetInstance()->Print(50, 30 * 6, 2, "%f", p_down);
	DebugText::GetInstance()->Print(50, 30 * 7, 2, "%f", MapCollide(player, 0));

	if (input->TriggerKey(DIK_SPACE))
	{
		//BGM�~�߂�
		//Audio::GetInstance()->SoundStop("zaza.wav");
		
		//�V�[���؂�ւ�
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//�I�u�W�F�N�g���̍X�V
	camera->Update();
	enemy->Update();
	mini_enemy->Update();
	player->Update();
	mini_player->Update();
	Rope->Update();
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			objBlock[y][x]->Update();
			objMiniBlock[y][x]->Update();
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
	mini_enemy->Draw();
	player->Draw();
	mini_player->Draw();
	Rope->Draw();

	//�}�b�v�`�b�v�̕`��
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			objBlock[y][x]->Draw();
			objMiniBlock[y][x]->Draw();
		}
	}
	/// <summary>
	/// ������3D�I�u�W�F�N�g�̕`�揈����ǉ��ł���
	/// </summary>

	// 3D�I�u�W�F�N�g�`��㏈��
	Object3d::PostDraw();

	// �O�i�X�v���C�g�`��O����
	Sprite::PreDraw(cmdList);

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
	pos.x = (cosf(3.14 / 180.0f * angle) * r) + center_pos.x;//�~�^���̏���
	pos.y = (sinf(3.14 / 180.0f * angle) * r) + center_pos.y;//�~�^���̏���

	angle += add;
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

	for (int b_y = 0; b_y < map_max_y; b_y++) //y��12
	{
		for (int b_x = 0; b_x < map_max_x; b_x++) //x��52
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
					}
					//��
					else if (y - b > 0)
					{
						pos.y = y - h - r;
						if (!is_jump)
						{
							is_hit = true;
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

void GamePlayScene::MiniMapCreate(int mapNumber)
{
	//�}�b�v�`�b�v1�̑傫��(player��5�Ȃ̂�5�̔{���Ŏw�肷�邱��)
	const float LAND_SCALE = 1.0f;
	for (int y = 0; y < map_max_y; y++) {//(y��12)
		for (int x = 0; x < map_max_x; x++) {//(x��52)

			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == Ground)
			{
				//�ʒu�Ƒ傫���̕ύX
				objMiniBlock[y][x]->SetScale({ LAND_SCALE / 5, LAND_SCALE / 5, LAND_SCALE / 5 });
				objMiniBlock[y][x]->SetPosition({ x * LAND_SCALE - 60,  y * -LAND_SCALE + 35, 0 });
			}
		}
	}
}

void GamePlayScene::ropeRotation() {
	//�G�̈ʒu���擾
	XMFLOAT3 ropePosition = Rope->GetPosition();
	//�v���C���[�̈ʒu���擾
	XMFLOAT3 playerPosition = player->GetPosition();

	XMFLOAT3 length = { playerPosition.x - ropePosition.x, playerPosition.y - ropePosition.y, playerPosition.z - ropePosition.z };
	float len = sqrtf(length.x * length.x + length.y * length.y + length.z * length.z);

	Rope->SetScale({ 0.3f, len, 0.3f });

	float angleX = ANGLE->PosForAngle(playerPosition.x, ropePosition.y, ropePosition.x, playerPosition.y);

	Rope->SetRotation({ 0, 0 , DirectX::XMConvertToDegrees(angleX) });
}