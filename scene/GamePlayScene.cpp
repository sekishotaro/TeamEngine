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
	Sprite::LoadTexture(0, L"Resources/Number0.png");
	Sprite::LoadTexture(1, L"Resources/Number1.png");
	Sprite::LoadTexture(2, L"Resources/Number2.png");
	Sprite::LoadTexture(3, L"Resources/Number3.png");
	Sprite::LoadTexture(4, L"Resources/Number4.png");
	Sprite::LoadTexture(5, L"Resources/Number5.png");
	Sprite::LoadTexture(6, L"Resources/Number6.png");
	Sprite::LoadTexture(7, L"Resources/Number7.png");
	Sprite::LoadTexture(8, L"Resources/Number8.png");
	Sprite::LoadTexture(9, L"Resources/Number9.png");
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
	spriteTimer[1] = Sprite::Create(1, { 0,0 });
	spriteTimer[2] = Sprite::Create(1, { 32,0 });
	spriteScore[1] = Sprite::Create(0, { WinApp::window_width - 32,0 });
	spriteScore[2] = Sprite::Create(0, { WinApp::window_width - 64,0 });

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

	//etc
	score = 0;
	is_shake = false;
	shake_time = 0;
	shake_x = 0;
	shake_y = 0;
	lastTime = 60.0f;
	level = 1;
	enemySpawn = 1;
	gravity = 0.15f;

	//�I�u�W�F�N�g�Ƀ��f�����Ђ��t����
	//�G�l�~�[
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		e_pos[i] = { 0, 0, 0 };
		is_normal[i] = true;
		is_catch[i] = false;
		is_alive[i] = false;
		is_grand[i] = false;
		e_speed[i] = 0.25f;
		e_down[i] = 0.0f;
		enemy[i]->SetModel(model);
		enemy[i]->SetPosition(e_pos[i]);
		enemy[i]->Update();
		angle[i] = 0;
		Rope[i]->SetModel(rope);
		Rope[i]->SetScale({ 0.3, 5, 0.3 });
		Rope[i]->Update();
	}

	//�v���C���[
	player->SetModel(model);
	p_pos = { 110, 10, 0 };
	old_p_pos = { 0, 0, 0 };
	p_x_radius = 1.0f * player->GetScale().x;
	p_y_radius = 1.0f * player->GetScale().y;
	is_jump = false;
	p_add = 0;
	p_down = 0;
	is_attack = false;
	is_air = false;
	player->SetPosition(p_pos);
	player->Update();

	//���[�v
	max_rope = 15;

	//����
	srand(time(NULL));
}

void GamePlayScene::Finalize()
{
	safe_delete(camera);
	safe_delete(rope_angle);
	safe_delete(model);
	safe_delete(block);
	safe_delete(rope);
	safe_delete(spriteBG);
	safe_delete(miniplayer);
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		safe_delete(minienemy[i]);
	}
	for (int i = 0; i < 10; i++)
	{
		safe_delete(spriteTimer[i]);
	}
	for (int i = 0; i < 10; i++)
	{
		safe_delete(spriteScore[i]);
	}
	safe_delete(minimap);
}

void GamePlayScene::Update()
{

	// �Q�[���V�[���̖��t���[������
	Input* input = Input::GetInstance();

	if (lastTime > 0)
	{
		lastTime -= 0.02;
	}
	spriteTimer[2]->ChangeTex((int)lastTime % 10);
	spriteTimer[1]->ChangeTex((int)lastTime / 10);

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

	for (int i = 0; i < enemySpawn; i++) 
	{
		old_e_pos[i] = enemy[i]->GetPosition();
	}

	//�v���C���[����
	{
		//���W�X�V
		p_pos = player->GetPosition();
		old_p_pos = p_pos;

		//�ړ�
		if (is_attack == false)
		{
			if (input->LeftStickAngle().x)
			{
				p_pos.x += input->LeftStickAngle().x / 2;

				//�i�s�����Ɍ�����ς���
				if (input->LeftStickAngle().x >= 0)
				{
					player->SetRotation(XMFLOAT3(0, 0, 0));
				} else
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

		//�U��
		for (int i = 0; i < enemySpawn; i++)
		{
			if (is_alive[i] == true)
			{
				//�v���C���[�̍U��
				if ((input->TriggerKey(DIK_SPACE) || input->PushButton(Button_B)) && is_attack == false && is_catch[i])
				{
					//�v���C���[�̌����œ����������ς���
					if (player->GetRotation().y == 0 && p_pos.x >= enemy[i]->GetPosition().x)
					{
						angle[i] = static_cast<int>(XMConvertToDegrees(rope_angle->PosForAngle(p_pos.x, enemy[i]->GetPosition().y, enemy[i]->GetPosition().x, p_pos.y))) - 90;
						is_attack = true;
					} else if (player->GetRotation().y == 180 && p_pos.x < enemy[i]->GetPosition().x)
					{
						angle[i] = static_cast<int>(XMConvertToDegrees(rope_angle->PosForAngle(p_pos.x, enemy[i]->GetPosition().y, enemy[i]->GetPosition().x, p_pos.y))) - 90;
						is_attack = true;
					}
				}
			}
		}

		//�W�����v
		if ((input->TriggerKey(DIK_W) || input->TriggerButton(Button_A)) && is_air == false && is_jump == false && is_attack == false)
		{
			is_jump = true;

			//�㏸���̏�����
			p_add = 2.5f;
		}

		//�W�����v����
		if (is_jump == true)
		{
			//���W�̏㏸
			p_add -= gravity;
			p_pos.y += p_add;

			if (MapCollide(p_pos, p_x_radius, p_y_radius, 0, old_p_pos, is_jump))
			{
				//������
				is_jump = false;
				p_add = 0;
			}
		}
		//�d��
		else
		{
			is_air = true;
			//���~�x���}�C�i�X
			p_down -= gravity;
			p_pos.y += p_down;

			if (MapCollide(p_pos, p_x_radius, p_y_radius, 0, old_p_pos, is_air))
			{
				//������
				p_down = 0;
				is_air = false;
			}
		}

		//�~�j�}�b�v�p���W�ϊ�
		miniplayer->SetPosition({ p_pos.x , -p_pos.y + 28 });
	}



	//�G�l�~�[����
	{
		for (int i = 0; i < enemySpawn; i++)
		{
			//�G�̃X�|�[��
			if (is_alive[i] == false)
			{
				SpawnEnemy(0, i);
			}
			//�G�̏���
			else
			{
				e_pos[i] = enemy[i]->GetPosition();
				old_e_pos[i] = e_pos[i];
				//�v���C���[�ƃG�l�~�[���ڐG������
				if (CollisionObject(player, enemy[i]))
				{
					is_catch[i] = true;
				}
				if (is_catch[i] == false && is_attack == false)
				{
					//�ʏ���
					if (is_normal[i] == true)
					{
						//�ړ�
						e_pos[i].x += e_speed[i];

						if (is_grand[i] == true)
						{
							//�[�܂ōs������
							if (MapCollide(e_pos[i], p_x_radius, p_y_radius, 0, old_e_pos[i]))
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
							else if (Mapchip::GetChipNum(static_cast<int>((e_pos[i].x + p_x_radius + LAND_SCALE / 2) / LAND_SCALE), -static_cast<int>((e_pos[i].y - p_y_radius + LAND_SCALE / 2) / LAND_SCALE - 1), map[0]) == None && e_speed[i] > 0)
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
							else if (Mapchip::GetChipNum(static_cast<int>((e_pos[i].x - p_x_radius + LAND_SCALE / 2) / LAND_SCALE), -static_cast<int>((e_pos[i].y - p_y_radius + LAND_SCALE / 2) / LAND_SCALE - 1), map[0]) == None && e_speed[i] < 0)
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
							else if ((e_pos[i].x - p_x_radius + LAND_SCALE / 2) / LAND_SCALE < 0)
							{
								if (Mapchip::GetChipNum(-1, -static_cast<int>((e_pos[i].y - p_y_radius + LAND_SCALE / 2) / LAND_SCALE - 1), map[0]) == None && e_speed[i] < 0)
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
						}
					}
				}
				//�U�����
				else if (is_catch[i] == true)
				{
					if (is_attack == true)
					{
						//�E�����Ȃ�
						if (player->GetRotation().y == 0)
						{
							CircularMotion(e_pos[i], p_pos, GetLengthObject(p_pos, e_pos[i]), angle[i], -15);
						}
						//�������Ȃ�
						else if (player->GetRotation().y == 180)
						{
							CircularMotion(e_pos[i], p_pos, GetLengthObject(p_pos, e_pos[i]), angle[i], 15);
						}
						//�}�b�v�̓����蔻��
						if (MapCollide(e_pos[i], p_x_radius, p_y_radius, 0, old_e_pos[i]))
						{
							is_alive[i] = false;
							is_catch[i] = false;
							is_attack = false;
							is_shake = true;
							score++;
							spriteScore[1]->ChangeTex((int)score % 10);
							spriteScore[2]->ChangeTex((int)score / 10);
							for (int i = 0; i < enemySpawn; i++)
							{
								if (is_catch[i] == true)
								{
									is_attack = true;
									break;
								}
							}
						}
					} 
				}
				//�d��
				if (is_attack == false)
				{
					//���~�x���}�C�i�X
					e_down[i] -= gravity;
					e_pos[i].y += e_down[i];
					is_grand[i] = false;

					//�}�b�v�̓����蔻��
					if (MapCollide(e_pos[i], p_x_radius, p_y_radius, 0, old_e_pos[i]))
					{
						e_down[i] = 0;
						is_grand[i] = true;
					}
				}
				//���[�v�̍X�V
				if (is_catch[i] == true)
				{
					RopeMove(e_pos[i], i);
				}

				//�~�j�}�b�v�p���W�ϊ�
				minienemy[i]->SetPosition({ e_pos[i].x , -e_pos[i].y + 28 });
			}
		}
	}

	if (score > 2)
	{
		level = 2;
	}
	else if (score > 5)
	{
		level = 3;
	}
	else if (score > 10)
	{
		level = 4;
	}
	else if (score > 20)
	{
		level = 5;
	}
	else if (score > 30)
	{
		level = 6;
	}
	enemySpawn = level;

	//�G�l�~�[�X�V
	for (int i = 0; i < enemySpawn; i++)
	{
		enemy[i]->SetPosition(e_pos[i]);
		//�����̍ő�l�𒴂�����
		if (e_pos[i].y < -500.0f)
		{
			enemy[i]->SetPosition({ 20, 10, 0 });
			e_down[i] = 0;
		}
		//�I�u�W�F�N�g���̍X�V
		enemy[i]->Update();
		Rope[i]->Update();
	}
	
	//�v���C���[�X�V
	player->SetPosition(p_pos);
	if (p_pos.y < -500.0f)
	{
		player->SetPosition({ 0, 10, 0 });
		p_down = 0;
		p_add = 0;
	}
	player->Update();

	//�J�����X�V
	if (is_shake == true)
	{
		shake_x = rand() % 11 / 10.0f;
		shake_y = rand() % 11 / 10.0f;
		shake_time++;
		if (shake_time > 10)
		{
			shake_x = 0.0f;
			shake_y = 0.0f;
			shake_time = 0;
			is_shake = false;
		}
	}
	camera->SetTarget({ p_pos.x + shake_x, p_pos.y + shake_y, p_pos.z });
	camera->SetEye({ p_pos.x + shake_x, p_pos.y + shake_y, p_pos.z - 60.0f });  
	camera->Update();

	if (input->TriggerKey(DIK_RETURN))
	{
		//BGM�~�߂�
		//Audio::GetInstance()->SoundStop("zaza.wav");

		//�V�[���؂�ւ�
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//�v���C���[�̍��W�iX�FY
	//DebugText::GetInstance()->Print(50, 35 * 1, 2, "%f", objBlock[8][0]->GetPosition().x);
	//DebugText::GetInstance()->Print(50, 35 * 2, 2, "%f", objBlock[8][0]->GetPosition().y);
	//DebugText::GetInstance()->Print(50, 35 * 3, 2, "rope_X:%f", Rope[0]->GetPosition().x);
	//DebugText::GetInstance()->Print(50, 35 * 4, 2, "rope_Y:%f", Rope[0]->GetPosition().y);
	DebugText::GetInstance()->Print(50, 35 * 5, 2, "player_x:%f", p_pos.x);
	DebugText::GetInstance()->Print(50, 35 * 6, 2, "player_y:%f", p_pos.y);
	DebugText::GetInstance()->Print(50, 35 * 7, 2, "mapchip_x:%d", static_cast<int>((p_pos.x + LAND_SCALE / 2) / LAND_SCALE));
	DebugText::GetInstance()->Print(50, 35 * 8, 2, "mapchip_y:%d", -static_cast<int>((p_pos.y + LAND_SCALE / 2) / LAND_SCALE));
	DebugText::GetInstance()->Print(50, 35 * 9, 2, "map_pos:%d", Mapchip::GetChipNum((static_cast<int>((p_pos.x + LAND_SCALE / 2) / LAND_SCALE)), (-static_cast<int>((p_pos.y + LAND_SCALE / 2) / LAND_SCALE + 1)), map[0]));
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
	for (int i = 0; i < enemySpawn; i++)
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
	for (int i = 0; i < enemySpawn; i++)
	{
		if (is_alive[i])
		{
			minienemy[i]->Draw();
		}
	}
	miniplayer->Draw();
	spriteTimer[1]->Draw();
	spriteTimer[2]->Draw();
	spriteScore[1]->Draw();
	spriteScore[2]->Draw();
	// �f�o�b�O�e�L�X�g�̕`��
	DebugText::GetInstance()->DrawAll(cmdList);

	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
}

void GamePlayScene::MapCreate(int mapNumber)
{
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
		e_pos[enemyNumber] = { spawnX * LAND_SCALE,  -spawnY * LAND_SCALE, 0 };
		is_alive[enemyNumber] = true;//�X�|�[��
	}
}

void GamePlayScene::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 center_pos, const float r, int& angle, const int add)
{
	angle += add;

	pos.x = (cosf(3.14 / 180.0f * angle) * r) + center_pos.x;//�~�^���̏���
	pos.y = (sinf(3.14 / 180.0f * angle) * r) + center_pos.y;//�~�^���̏���
}

bool GamePlayScene::MapCollide(XMFLOAT3& pos, float radiusX, float radiusY, int mapNumber, const XMFLOAT3 old_pos, bool is_jump)
{
	//����Ώ�
	float a = pos.x;
	float b = pos.y;
	float r_a = radiusX;
	float r_b = radiusY;

	//�}�b�v�`�b�v
	float x = 0;
	float y = 0;
	float r_x = 0;
	float r_y = 0;

	//����
	bool x_hit = false;
	bool y_hit = false;
	bool is_skip = false;

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
				if (b - r_b < y + r_y && y < old_pos.y - r_b && (x - r_x < a + r_a && a - r_a < x + r_x))
				{
					b = y + r_y + r_b;
					pos.y = b;
					y_hit = true;
					is_skip = true;
					break;
				} 
				//��
				else if (b + r_b > y - r_y && old_pos.y + r_b < y && (x - r_x < a + r_a && a - r_a < x + r_x))
				{
					b = y - r_y - r_b;
					pos.y = b;
					is_skip = true;
					if (is_jump == false)
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
		if (is_skip == true)
		{
			break;
		}
	}

	is_skip = false;

	for (int b_y = map_max_y - 1; b_y >= 0; b_y--) //x��52
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
				if (a - r_a < x + r_x && x < old_pos.x - r_a && (y - r_y < b + r_b && b - r_b < y + r_y))
				{
					a = x + r_x + r_a;
					pos.x = a;
					is_skip = true;
					if (is_jump == false)
					{
						x_hit = true;
					}
					break;
				}
				//�E
				else if (a + r_a > x - r_x && old_pos.x + r_a < x && (y - r_y < b + r_b && b - r_b < y + r_y))
				{
					a = x - r_x - r_a;
					pos.x = a;
					is_skip = true;
					if (is_jump == false)
					{
						x_hit = true;
					}
					break;
				}
			}
		}
		if (is_skip == true)
		{
			break;
		}
	}

	if (x_hit == true || y_hit == true)
	{
		return true;
	}

	return false;
}

void GamePlayScene::RopeMove(XMFLOAT3& pos, const int num)
{
	Rope[num]->SetPosition({ (p_pos.x + e_pos[num].x) / 2, (p_pos.y + e_pos[num].y) / 2, 0 });
	Rope[num]->Update();

	//���[�v�̈ʒu���擾
	XMFLOAT3 r_pos = Rope[num]->GetPosition();

	//�v���C���[�ƃG�l�~�[�̋���
	XMFLOAT2 length = { p_pos.x - e_pos[num].x, p_pos.y - e_pos[num].y };
	float len = GetLengthObject(p_pos, e_pos[num]);
	//�ő�l���傫���Ȃ�
	if (len > max_rope)
	{
		float wq = len / max_rope;
		len = max_rope;
		e_pos[num] = { p_pos.x - length.x / wq, p_pos.y - length.y / wq, 0 };
		pos = e_pos[num];
		MapCollide(pos, p_x_radius, p_y_radius, 0, old_e_pos[num]);
	}

	//���[�v�̒���
	float angleX = rope_angle->PosForAngle(p_pos.x, r_pos.y, r_pos.x, p_pos.y);
	Rope[num]->SetPosition({ (p_pos.x + e_pos[num].x) / 2, (p_pos.y + e_pos[num].y) / 2, 0 });
	Rope[num]->SetScale({ 0.3f, len / 2, 0.3f });
	Rope[num]->SetRotation({ 0, 0 ,XMConvertToDegrees(angleX) });
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
