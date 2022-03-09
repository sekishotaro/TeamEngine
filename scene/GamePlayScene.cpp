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

	enemy = Object3d::Create();

	player = Object3d::Create();

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

	//�v���C���[����
	{
		//�v���C���[�̈ړ�
		if (input->LeftStickAngle().x/* || input->LeftStickAngle().y*/)
		{
			p_pos.x += input->LeftStickAngle().x / (1 / p_max_speed) * 1;
			/*if (input->LeftStickAngle().y > 0)
			{
				p_pos.y += input->LeftStickAngle().y / 2;
			}*/
		}
		//�W�����v�t���O
		if (input->TriggerButton(Button_A) && is_jump == false)
		{
			is_jump = true;
			p_add = 4.0f;
			p_gravity = 0.3f;
		}
		//�W�����v����
		if (is_jump == true)
		{
			p_add -= p_gravity;
			p_pos.y += p_add;
			if (p_pos.y < 0)
			{
				p_pos.y = 0;
				p_add = 0;
				p_gravity = 0;
				is_jump = false;
			}
		}

		player->SetPosition(p_pos);
	}

	//�G�l�~�[����
	{
		//�ʏ���
		if (normal == true)
		{
			e_pos.x += e_add;

			if (e_pos.x >= 50.0f || e_pos.x <= -50.0f)
			{
				e_add = -e_add;
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
		else if (chase == true)
		{
			//�v���C���[�ƃG�l�~�[�̋���
			XMFLOAT2 pe_len = { p_pos.x - e_pos.x, p_pos.y - e_pos.y };
			if (pe_len.x > 0)
			{
				e_pos.x += 0.2f;
				if (p_pos.x < e_pos.x)
				{
					e_pos.x = p_pos.x;
				}
			}
			else if (pe_len.x < 0)
			{
				e_pos.x -= 0.2f;
				if (p_pos.x > e_pos.x)
				{
					e_pos.x = p_pos.x;
				}
			}
		}

		enemy->SetPosition(e_pos);
	}

	DebugText::GetInstance()->Print(50, 30 * 1, 2, "%f", player->GetPosition().x);
	DebugText::GetInstance()->Print(50, 30 * 2, 2, "%f", player->GetPosition().y);

	if (input->TriggerKey(DIK_SPACE))
	{
		//BGM�~�߂�
		//Audio::GetInstance()->SoundStop("zaza.wav");
		
		//�V�[���؂�ւ�
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//�A�b�v�f�[�g
	camera->Update();
	enemy->Update();
	player->Update();
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
	for (auto i : box)
	{
		safe_delete(i);
	}
	//�O�ɓ����Ă����v�f�폜
	box.clear();

	const float LAND_SCALE = 1.0f;
	for (int y = 0; y < map_max_y; y++) {
		for (int x = 0; x < map_max_x; x++) {

			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == 1)
			{
				goal->SetScale({ LAND_SCALE, LAND_SCALE, LAND_SCALE });
				goal->SetPosition({ x * LAND_SCALE,  y * -LAND_SCALE , 0 });
				//object3d->SetRotation({ 0,90,0 });
				//box.push_back(object3d);
			}
		}
	}
}

void GamePlayScene::SpawnEnemy(bool& active, int& spawn_num)
{
	if (active == true)
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
