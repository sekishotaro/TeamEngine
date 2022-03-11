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

	enemy = Object3d::Create();

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
	enemy->SetModel(model);

	player->SetModel(model);
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
		//���~�x�̏�����
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

			//�n�ʂɓ���������
			if (p_pos.y < 0)
			{
				p_pos.y = 0;
				p_add = 0;
				is_jump = false;
			}
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
		//�v���C���[�̗���
		if (is_jump == false && !MapCollide(player, 0))
		{
			//���~�x���}�C�i�X
			p_down += gravity;
			p_pos.y -= p_down;
		}
		//�u���b�N�ɓ���������
		if (MapCollide(player, 0))
		{
			//�����߂�
			p_pos.y = 0;
			p_down = 0;
		}

		player->SetPosition(p_pos);
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
			} 
			//�������Ȃ�
			else if (player->GetRotation().y == 180)
			{
				CircularMotion(e_pos, p_pos, 10, angle, 20);
				enemy->SetPosition(e_pos);
			}
			//�}�b�v�̓����蔻��
			if (MapCollide(enemy, 0))
			{
				is_attack = false;
			}
		}

		enemy->SetPosition(e_pos);
	}

	//�v���C���[�̍��W�iX�FY�j
	DebugText::GetInstance()->Print(50, 30 * 1, 2, "%f", objBlock[8][0]->GetPosition().x);
	DebugText::GetInstance()->Print(50, 30 * 2, 2, "%f", objBlock[8][0]->GetPosition().y);
	DebugText::GetInstance()->Print(50, 30 * 3, 2, "%f", player->GetPosition().x);
	DebugText::GetInstance()->Print(50, 30 * 4, 2, "%f", player->GetPosition().y);

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
	player->Update();
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

	// �f�o�b�O�e�L�X�g�̕`��
	DebugText::GetInstance()->DrawAll(cmdList);

	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
}

void GamePlayScene::MapCreate(int mapNumber)
{
	//�}�b�v�`�b�v1�̑傫��(player��5�Ȃ̂�5�̔{���Ŏw�肷�邱��)
	const float LAND_SCALE = 5.0f;
	for (int y = 0; y < map_max_y; y++) {//(y��12)
		for (int x = 0; x < map_max_x; x++) {//(x��52)

			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == Ground)
			{
				//�ʒu�Ƒ傫���̕ύX(���͑傫���͕ύX���Ȃ���)
				//objBlock[y][x]->SetScale({ LAND_SCALE, LAND_SCALE, LAND_SCALE });
				objBlock[y][x]->SetPosition({ x * LAND_SCALE - 40,  y * -LAND_SCALE + 35, 0 });
			}
		}
	}
}

void GamePlayScene::SpawnEnemy(bool& active, int& spawn_num)
{
	if (active)
	{
		srand(time(NULL));
		int num = rand() % 100;

		if (num < 40)
		{


			spawn_num -= 1;
			if (max_spawn == 0)
			{
				active = false;
			}
		}
	}
}

void GamePlayScene::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 center_pos, const float r, int& angle, const int add)
{
	pos.x = (cosf(3.14 / 180.0f * angle) * r) + center_pos.x;//�~�^���̏���
	pos.y = (sinf(3.14 / 180.0f * angle) * r) + center_pos.y;//�~�^���̏���

	angle += add;
}

bool GamePlayScene::MapCollide(const std::unique_ptr<Object3d>& object, int mapNumber)
{
	for (int y = 0; y < map_max_y; y++) //y��12
	{
		for (int x = 0; x < map_max_x; x++) //x��52
		{
			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == Ground)
			{
				if ((object->GetPosition().x <= objBlock[y][x]->GetPosition().x + 2.5f * objBlock[0][0]->GetScale().x)
					&& (object->GetPosition().x >= objBlock[y][x]->GetPosition().x - 2.5f * objBlock[0][0]->GetScale().x)
					&& (object->GetPosition().y <= objBlock[y][x]->GetPosition().y + 2.5f * objBlock[0][0]->GetScale().x)
					&& (object->GetPosition().y >= objBlock[y][x]->GetPosition().y - 2.5f * objBlock[0][0]->GetScale().x))
				{
					return true;
				}
			}
		}
	}

	return false;
}

/*
//���a
				float block_r = 2.5f * objBlock[y][x]->GetScale().x;
				float obj_r = 1.0f * object->GetScale().x;
				//X��Y�̒���
				float len_x = objBlock[y][x]->GetPosition().x - object->GetPosition().x;
				float len_y = objBlock[y][x]->GetPosition().y - object->GetPosition().y;
				//���ꂼ�ꂪ���a�ȉ��ł���
				if (block_r + obj_r >= sqrtf(len_x * len_x) && block_r + obj_r >= sqrtf(len_y * len_y))
				{
					return true;
				}
				//�l�p�̋�����~�̒��S�܂ł̃x�N�g��
				XMFLOAT2 vec = {0, 0};
				//����
				float len_r = 0;
				//�l�̋��𒲂ׂ�
				for (int i = 0; i < 4; i++)
				{
					if (i == 0)
					{
						vec = { objBlock[y][x]->GetPosition().x - block_r + object->GetPosition().x, objBlock[y][x]->GetPosition().y - block_r + object->GetPosition().y };
					}
					else if (i == 1)
					{
						vec = { objBlock[y][x]->GetPosition().x - block_r + object->GetPosition().x, objBlock[y][x]->GetPosition().y + block_r + object->GetPosition().y };
					}
					else if (i == 2)
					{
						vec = { objBlock[y][x]->GetPosition().x + block_r + object->GetPosition().x, objBlock[y][x]->GetPosition().y - block_r + object->GetPosition().y };
					}
					else if (i == 3)
					{
						vec = { objBlock[y][x]->GetPosition().x + block_r + object->GetPosition().x, objBlock[y][x]->GetPosition().y + block_r + object->GetPosition().y };
					}
					//���������߂�
					len_r = sqrtf(vec.x * vec.x + vec.y * vec.y);
					//�~�̔��a�����������Z��
					if (len_r <= obj_r)
					{
						return true;
					}
				}
*/