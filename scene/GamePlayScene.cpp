#include "GamePlayScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"
#include "DirectXCommon.h"
#include "Mapchip.h"
#include "Effect.h"
#include <safe_delete.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include "FbxLoader.h"
#include "FbxObject3d.h"

using namespace DirectX;

void GamePlayScene::Initialize()
{
	Audio::GetInstance()->LoadWave("futta-dream.wav");
	Audio::GetInstance()->LoadWave("zaza.wav");

	//Audio::GetInstance()->PlayWave("zaza.wav", true);
	
	//デバイスのセット
	FbxObject3d::SetDevice(DirectXCommon::GetInstance()->GetDev());
	// カメラ生成
	camera = new Camera(WinApp::window_width, WinApp::window_height);

	// カメラセット
	Object3d::SetCamera(camera);

	FbxObject3d::SetCamera(camera);
	//グラフィックスパイプライン生成
	FbxObject3d::CreateGraphicsPipeline();

	// テクスチャ読み込み
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
	Sprite::LoadTexture(15, L"Resources/score.png");
	Sprite::LoadTexture(16, L"Resources/level.png");
	Sprite::LoadTexture(17, L"Resources/koron.png");
	Sprite::LoadTexture(18, L"Resources/timer.png");
	Sprite::LoadTexture(20, L"Resources/shock.png");

	// 背景スプライト生成
	spriteBG = Sprite::Create(11, { 0.0f,0.0f });
	texScore = Sprite::Create(15, { WinApp::window_width - 372, 0 });
	texLevel = Sprite::Create(16, { WinApp::window_width - 100, WinApp::window_height - 64 });

	//スプライト生成
	minimap = Sprite::Create(12, { 0.0f,50.0f });
	miniplayer = Sprite::Create(13, { 20.0f,20.0f });
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		minienemy[i] = Sprite::Create(14, { 40.0f,20.0f });
	}
	spriteTime[0] = Sprite::Create(0, { 64,0 });
	spriteCoron = Sprite::Create(17, { 96,0 });
	spriteTimer = Sprite::Create(18, { 0,0 });
	spriteTime[1] = Sprite::Create(0, { 128,0 });
	spriteTime[2] = Sprite::Create(0, { 160,0 });
	spriteScore[1] = Sprite::Create(0, { WinApp::window_width - 32,0 });
	spriteScore[2] = Sprite::Create(0, { WinApp::window_width - 64,0 });
	spriteScore[3] = Sprite::Create(0, { WinApp::window_width - 96,0 });
	spriteLevel[1] = Sprite::Create(0, { WinApp::window_width - 32 ,WinApp::window_height - 64 });

	//エフェクト
	shockWave = Sprite::Create(20, { 700, 200 });
	shockWave->SetAnchorPoint({ 0.5f, 0.5f });
	// オブジェクト生成
	model = Model::LoadFromOBJ("sphere");
	enemy_model_2 = Model::LoadFromOBJ("sphere2");
	block = Model::LoadFromOBJ("block");
	rope = Model::LoadFromOBJ("rope");
	locusModel = Model::LoadFromOBJ("locus");

	for (int i = 0; i < EnemySpawnMax; i++)
	{
		enemy[i] = Object3d::Create();
		Rope[i] = Object3d::Create();
	}
	player = Object3d::Create();

	

	

	//マップチップ用のCSV読み込み
	//(map, "Resource/scv/なんたら.csv")で追加可能
	Mapchip::CsvToVector(map, "Resources/csv/demo.csv");//mapNum=0


	//マップチップ用のオブジェクトの初期化
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

	//オブジェクトにモデルをひも付ける
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		//エネミー
		enemy_data[i].e_pos = { 0, 0, 0 };
		enemy_data[i].e_x_radius = 1.0f * player->GetScale().x;
		enemy_data[i].e_y_radius = 1.0f * player->GetScale().y;
		enemy_data[i].is_normal = true;
		enemy_data[i].is_catch = false;
		enemy_data[i].is_alive = false;
		enemy_data[i].is_grand = false;
		enemy_data[i].e_speed = 0.25f;
		enemy_data[i].e_down = 0.0f;
		enemy_data[i].angle = 0;
		enemy_data[i].enemy_type = TWICE;
		enemy_data[0].enemy_type = NORMAL;
		if (enemy_data[i].enemy_type == TWICE)
		{
			enemy_data[i].can_catch = false;
		}
		else
		{
			enemy_data[i].can_catch = true;
		}
		enemy_data[i].is_add = true;
		if (enemy_data[i].enemy_type == TWICE)
		{
			enemy[i]->SetModel(enemy_model_2);
		}
		else
		{
			enemy[i]->SetModel(model);
		}
		enemy[i]->SetPosition(enemy_data[i].e_pos);
		enemy[i]->Update();

		//ロープ
		Rope[i]->SetModel(rope);
		Rope[i]->SetScale({ 0.3, 5, 0.3 });
		Rope[i]->Update();
	}

	//ロープ
	max_rope = 15;

	//プレイヤー
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


	//モデル名を指定してファイル読み込み
	fbxModel1 = FbxLoader::GetInstance()->LoadModelFromFile("prin");
	//3Dオブジェクト生成とモデルのセット
	fbxObject1 = new FbxObject3d;
	fbxObject1->Initialize();
	fbxObject1->SetModel(fbxModel1);

	//エフェクト
	/*for (int i = 0; i < maxLocus; i++)
	{
		locus.emplace_back(Object3d::Create());
	}
	for (int i = 0; i < sizeof(locus); i++)
	{
		locus[i]->SetModel(locusModel);
		locus[i]->SetPosition({ 110.0f, -35.0f + (float) i, 0.0f});
		locus[i]->SetRotation({ 0.0f, 90.0f, 0.0f });
		locus[i]->SetScale({ 0.5f, 0.5f, 0.5f });
	}*/
	
	//乱数
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
	delete fbxObject1;
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		safe_delete(minienemy[i]);
	}
	for (int i = 0; i < 10; i++)
	{
		safe_delete(spriteTime[i]);
	}
	for (int i = 0; i < 10; i++)
	{
		safe_delete(spriteScore[i]);
	}
	safe_delete(minimap);
}

void GamePlayScene::Update()
{
	// ゲームシーンの毎フレーム処理
	Input* input = Input::GetInstance();

	//実験用置き場
	if (input->PushKey(DIK_M))
	{
		XMFLOAT2 size = shockWave->GetSize();
		shockWave->SetSize({ size.x + 20.0f, size.y + 20.0f });
	}
	Effect::DeletLocus(locus, camera, p_pos);
	

	if (lastTime > 0)
	{
		lastTime -= 0.02;
	}
	spriteTime[2]->ChangeTex((int)lastTime % 10);
	spriteTime[1]->ChangeTex((int)lastTime / 10);

	//Mキーでマップチップ設置
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

	//プレイヤー処理
	{
		//座標更新
		p_pos = player->GetPosition();
		old_p_pos = p_pos;

		//移動
		if (input->LeftStickAngle().x)
		{
			p_pos.x += input->LeftStickAngle().x / 2;

			//進行方向に向きを変える
			if (input->LeftStickAngle().x >= 0)
			{
				player->SetRotation(XMFLOAT3(0, 0, 0));
			} 
			else
			{
				player->SetRotation(XMFLOAT3(0, 180, 0));
			}
		}
		//キーボード用
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

		//攻撃
		for (int i = 0; i < enemySpawn; i++)
		{
			if (enemy_data[i].is_alive == true)
			{
				//プレイヤーの攻撃
				if ((input->TriggerKey(DIK_SPACE) || input->PushButton(Button_B)) && is_attack == false && enemy_data[i].is_catch == true)
				{
					//プレイヤーの向きで投げる方向を変える
					if (player->GetRotation().y == 0 && p_pos.x > enemy[i]->GetPosition().x)
					{
						enemy_data[i].angle = static_cast<int>(XMConvertToDegrees(rope_angle->PosForAngle(p_pos.x, enemy[i]->GetPosition().y, enemy[i]->GetPosition().x, p_pos.y))) - 90;
						is_attack = true;
					} 
					else if (player->GetRotation().y == 180 && p_pos.x < enemy[i]->GetPosition().x)
					{
						enemy_data[i].angle = static_cast<int>(XMConvertToDegrees(rope_angle->PosForAngle(p_pos.x, enemy[i]->GetPosition().y, enemy[i]->GetPosition().x, p_pos.y))) - 90;
						is_attack = true;
					}
				}
			}
		}

		//ジャンプ
		if ((input->TriggerKey(DIK_W) || input->TriggerButton(Button_A)) && is_air == false && is_jump == false)
		{
			is_jump = true;

			//上昇率の初期化
			p_add = 2.5f;
		}

		//ジャンプ処理
		if (is_jump == true)
		{
			//座標の上昇
			p_add -= gravity;
			p_pos.y += p_add;

			if (MapCollide(p_pos, p_x_radius, p_y_radius, 0, old_p_pos, is_jump))
			{
				//初期化
				is_jump = false;
				p_add = 0;
			}
		}
		//重力
		else
		{
			is_air = true;
			//下降度をマイナス
			p_down -= gravity;
			p_pos.y += p_down;

			if (MapCollide(p_pos, p_x_radius, p_y_radius, 0, old_p_pos, is_air))
			{
				//初期化
				p_down = 0;
				is_air = false;
			}
		}

		//ミニマップ用座標変換
		miniplayer->SetPosition({ p_pos.x , -p_pos.y + 78 });
	}

	//エネミー処理
	{
		for (int i = 0; i < enemySpawn; i++)
		{
			//敵のスポーン
			if (enemy_data[i].is_alive == false)
			{
				if (is_attack == false)
				{
					if (i % 2 == 0)
					{
						enemy_data[i + 1].is_alive = false;
						enemy_data[i + 1].is_catch = false;
					}
					if (i % 2 == 1)
					{
						enemy_data[i - 1].is_alive = false;
						enemy_data[i - 1].is_catch = false;
					}
				}
				if (i % 2 == 0)
				{
					SpawnEnemy(0, i);
				}
				if (enemy_data[i].enemy_type == TWICE)
				{
					enemy_data[i].can_catch = false;
					enemy[i]->SetModel(enemy_model_2);
					enemy[i]->SetRotation({ 0, 0, 0 });
				}
				else
				{
					enemy_data[i].can_catch = true;
					enemy[i]->SetModel(model);
					enemy[i]->SetRotation({ 0, 0, 0 });
				}
			}
			//敵の処理
			else
			{
				//座標取得
				enemy_data[i].e_pos = enemy[i]->GetPosition();
				enemy_data[i].old_e_pos = enemy_data[i].e_pos;
				//プレイヤーとエネミーが接触したら
				if (CollisionObject(player, enemy[i]) == true && enemy_data[i].can_catch == true)
				{
					if (enemy_data[i].enemy_type == NORMAL)
					{
						enemy_data[i].is_catch = true;
					}
					else if (enemy_data[i].enemy_type == TWICE && enemy_data[i].can_catch == true)
					{
						enemy_data[i].is_catch = true;
					}
				}
				else if (CollisionObject(player, enemy[i]) == true && enemy_data[i].can_catch == false)
				{
					if (p_pos.x - old_p_pos.x > 0)
					{
						p_pos.x -= 5.0f;
					}
					else
					{
						p_pos.x += 5.0f;
					}
				}
				//更新処理
				if (enemy_data[i].is_catch == false)
				{
					//通常状態
					if (enemy_data[i].is_normal == true)
					{
						//移動
						enemy_data[i].e_pos.x += enemy_data[i].e_speed;

						if (enemy_data[i].is_grand == true)
						{
							//端まで行ったら
							if (MapCollide(enemy_data[i].e_pos, p_x_radius, p_y_radius, 0, enemy_data[i].old_e_pos))
							{
								enemy_data[i].e_speed = -enemy_data[i].e_speed;

								//向きを変える
								XMFLOAT3 e_rot;
								e_rot = enemy[i]->GetRotation();
								e_rot.y += 180.0f;
								if (e_rot.y >= 360)
								{
									e_rot.y = 0;
								}
								enemy[i]->SetRotation(e_rot);
							} 
							else if (Mapchip::GetChipNum(static_cast<int>((enemy_data[i].e_pos.x + p_x_radius + LAND_SCALE / 2) / LAND_SCALE), -static_cast<int>((enemy_data[i].e_pos.y - p_y_radius + LAND_SCALE / 2) / LAND_SCALE - 1), map[0]) == None && enemy_data[i].e_speed > 0)
							{
								enemy_data[i].e_speed = -enemy_data[i].e_speed;

								//向きを変える
								XMFLOAT3 e_rot;
								e_rot = enemy[i]->GetRotation();
								e_rot.y += 180.0f;
								if (e_rot.y >= 360)
								{
									e_rot.y = 0;
								}
								enemy[i]->SetRotation(e_rot);
							} 
							else if (Mapchip::GetChipNum(static_cast<int>((enemy_data[i].e_pos.x - p_x_radius + LAND_SCALE / 2) / LAND_SCALE), -static_cast<int>((enemy_data[i].e_pos.y - p_y_radius + LAND_SCALE / 2) / LAND_SCALE - 1), map[0]) == None && enemy_data[i].e_speed < 0)
							{
								enemy_data[i].e_speed = -enemy_data[i].e_speed;

								//向きを変える
								XMFLOAT3 e_rot;
								e_rot = enemy[i]->GetRotation();
								e_rot.y += 180.0f;
								if (e_rot.y >= 360)
								{
									e_rot.y = 0;
								}
								enemy[i]->SetRotation(e_rot);
							} 
							else if ((enemy_data[i].e_pos.x - p_x_radius + LAND_SCALE / 2) / LAND_SCALE < 0)
							{
								if (Mapchip::GetChipNum(-1, -static_cast<int>((enemy_data[i].e_pos.y - p_y_radius + LAND_SCALE / 2) / LAND_SCALE - 1), map[0]) == None && enemy_data[i].e_speed < 0)
								{
									enemy_data[i].e_speed = -enemy_data[i].e_speed;

									//向きを変える
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
				//攻撃状態
				else if (enemy_data[i].is_catch == true)
				{
					if (is_attack == true)
					{
						//右向きなら
						if (player->GetRotation().y == 0)
						{
							CircularMotion(enemy_data[i].e_pos, p_pos, GetLengthObject(p_pos, enemy_data[i].e_pos), enemy_data[i].angle, -15);
							Effect::CreateLocus( locus, *locusModel, enemy_data[i].e_pos);
						}
						//左向きなら
						else if (player->GetRotation().y == 180)
						{
							CircularMotion(enemy_data[i].e_pos, p_pos, GetLengthObject(p_pos, enemy_data[i].e_pos), enemy_data[i].angle, 15);
							Effect::CreateLocus(locus, *locusModel, enemy_data[i].e_pos);
						}
						//マップの当たり判定
						if (MapCollide(enemy_data[i].e_pos, p_x_radius, p_y_radius, 0, enemy_data[i].old_e_pos))
						{
							enemy_data[i].is_alive = false;
							enemy_data[i].is_catch = false;
							is_attack = false;
							is_shake = true;
							score++;
							int hundredScore = 0;
							hundredScore = score / 10;
							spriteScore[1]->ChangeTex((int)score % 10);
							spriteScore[2]->ChangeTex((int)hundredScore % 10);
							spriteScore[3]->ChangeTex((int)score / 100);
							if (enemy_data[i].is_add == true)
							{
								max_rope += 0.5f;

								if (max_rope > 25.0f)
								{
									max_rope = 25.0f;
								}
							}
							else
							{
								max_rope -= 0.5f;

								if (max_rope < 15.0f)
								{
									max_rope = 15.0f;
								}
							}
							if (enemy_data[i].enemy_type == TWICE)
							{
								enemy_data[i].can_catch = false;
							}
							for (int j = 0; j < enemySpawn; j++)
							{
								if (enemy_data[j].is_catch == true)
								{
									is_attack = true;
									break;
								}
							}
							for (int j = 0; j < enemySpawn; j++)
							{
								if (i != j)
								{
									XMFLOAT3 positivePos = { enemy_data[j].e_pos.x + enemy_data[j].e_x_radius, enemy_data[j].e_pos.y + enemy_data[j].e_y_radius, 0 };
									XMFLOAT3 negativePos = { enemy_data[j].e_pos.x - enemy_data[j].e_x_radius, enemy_data[j].e_pos.y - enemy_data[j].e_y_radius, 0 };
									if (inFrustum(p_pos, negativePos, positivePos) == true)
									{
										enemy[j]->SetModel(model);
										XMFLOAT3 rot = enemy[j]->GetRotation();
										rot.z += 180;
										enemy[j]->SetRotation(rot);
										enemy_data[j].can_catch = true;
									}
								}
							}
						}
					} 
				}
				//重力
				if (is_attack == false)
				{
					//下降度をマイナス
					enemy_data[i].e_down -= gravity;
					enemy_data[i].e_pos.y += enemy_data[i].e_down;
					enemy_data[i].is_grand = false;

					//マップの当たり判定
					if (MapCollide(enemy_data[i].e_pos, p_x_radius, p_y_radius, 0, enemy_data[i].old_e_pos))
					{
						enemy_data[i].e_down = 0;
						enemy_data[i].is_grand = true;
					}
				}
				//ロープの更新
				if (enemy_data[i].is_catch == true)
				{
					RopeMove(enemy_data[i].e_pos, i);
				}

				//ミニマップ用座標変換
				minienemy[i]->SetPosition({ enemy_data[i].e_pos.x , -enemy_data[i].e_pos.y + 78 });
			}
		}
	}

	if (score > 50)
	{
		level = 7;
	}
	else if (score > 30)
	{
		level = 6;
	}
	else if (score > 20)
	{
		level = 5;
	}
	else if (score > 10)
	{
		level = 4;
	}
	else if (score > 5)
	{
		level = 3;
	}
	else if (score > 2)
	{
		level = 2;
	}
	enemySpawn = level * 4;
	spriteLevel[1]->ChangeTex((int)level % 10);

	//エネミー更新
	for (int i = 0; i < enemySpawn; i++)
	{
		enemy[i]->SetPosition(enemy_data[i].e_pos);
		//落下の最大値を超えたら
		if (enemy_data[i].e_pos.y < -500.0f)
		{
			enemy[i]->SetPosition({ 20, 10, 0 });
			enemy_data[i].e_down = 0;
		}
		//オブジェクト情報の更新
		enemy[i]->Update();
		Rope[i]->Update();
	}
	
	//プレイヤー更新
	player->SetPosition(p_pos);
	if (p_pos.y < -500.0f)
	{
		player->SetPosition({ 0, 10, 0 });
		p_down = 0;
		p_add = 0;
	}
	player->Update();

	//カメラ更新
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
	camera->SetEye({ p_pos.x + shake_x, p_pos.y + shake_y, p_pos.z - 55.0f - (5 * level)});
	camera->Update();

	if (input->TriggerKey(DIK_RETURN))
	{
		//BGM止める
		//Audio::GetInstance()->SoundStop("zaza.wav");

		//シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//エフェクト
	for (int i = 0; i < locus.size(); i++)
	{
		locus[i]->Update();
	}

	//プレイヤーの座標（X：Y)
	DebugText::GetInstance()->Print(50, 35 * 3, 2, "player_x:%f", p_pos.x);
	DebugText::GetInstance()->Print(50, 35 * 4, 2, "player_y:%f", p_pos.y);
	DebugText::GetInstance()->Print(50, 35 * 5, 2, "rope_len:%f", max_rope);
	DebugText::GetInstance()->Print(50, 35 * 6, 2, "enemySpawn:%d", enemySpawn);
	DebugText::GetInstance()->Print(50, 35 * 7, 2, "min:%f~max:%f", p_pos.x - 123 / 2, p_pos.x + 123 / 2);
	DebugText::GetInstance()->Print(50, 35 * 8, 2, "min:%f~max:%f", p_pos.y - 70 / 2, p_pos.y + 70 / 2);

	fbxObject1->AnimationFlag = true;
	fbxObject1->AnimationNum = 1;
	//アップデート
	fbxObject1->Update();
}

void GamePlayScene::Draw()
{
	// ゲームシーンの描画
	// コマンドリストの取得
	ID3D12GraphicsCommandList *cmdList = DirectXCommon::GetInstance()->GetCmdList();

	// 背景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// 背景スプライト描画
	spriteBG->Draw();

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	DirectXCommon::GetInstance()->ClearDepthBuffer();

	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(cmdList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブクジェクトの描画
	for (int i = 0; i < enemySpawn; i++)
	{
		if (enemy_data[i].is_alive == true)
		{
			enemy[i]->Draw();
		}
		if (enemy_data[i].is_catch == true)
		{
			Rope[i]->Draw();
		}
	}
	player->Draw();

	//マップチップの描画
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			objBlock[y][x]->Draw();
		}
	}

	//エフェクト
	for (int i = 0; i < locus.size(); i++)
	{
		locus[i]->Draw();
	}

	//FBX3Dオブジェクトの描画
	//fbxObject1->Draw(cmdList);
	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	//ミニマップの描画
	minimap->Draw();
	for (int i = 0; i < enemySpawn; i++)
	{
		if (enemy_data[i].is_alive == true)
		{
			minienemy[i]->Draw();
		}
	}
	miniplayer->Draw();
	spriteTime[0]->Draw();
	spriteCoron->Draw();
	spriteTime[1]->Draw();
	spriteTime[2]->Draw();
	spriteTimer->Draw();
	texLevel->Draw();
	spriteLevel[1]->Draw();
	texScore->Draw();
	spriteScore[1]->Draw();
	spriteScore[2]->Draw();
	spriteScore[3]->Draw();

	shockWave->Draw();

	// デバッグテキストの描画
	DebugText::GetInstance()->DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
}

void GamePlayScene::MapCreate(int mapNumber)
{
	for (int y = 0; y < map_max_y; y++) {//(yが12)
		for (int x = 0; x < map_max_x; x++) {//(xが52)

			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == Ground)
			{
				//位置と大きさの変更(今は大きさは変更しないで)
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

	if (Mapchip::GetChipNum(spawnX, spawnY, map[mapNumber]) == None && Mapchip::GetChipNum(spawnX + 1, spawnY, map[mapNumber]) == None)
	{
		enemy[enemyNumber]->SetPosition({ spawnX * LAND_SCALE,  -spawnY * LAND_SCALE, 0 });//位置をセット
		enemy_data[enemyNumber].e_pos = { spawnX * LAND_SCALE,  -spawnY * LAND_SCALE, 0 };
		enemy_data[enemyNumber].is_alive = true;//スポーン
		enemy[enemyNumber + 1]->SetPosition({ (spawnX + 1) * LAND_SCALE,  -spawnY * LAND_SCALE, 0 });//位置をセット
		enemy_data[enemyNumber + 1].e_pos = { (spawnX + 1) * LAND_SCALE,  -spawnY * LAND_SCALE, 0 };
		enemy_data[enemyNumber + 1].is_alive = true;//スポーン
	}
}

void GamePlayScene::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 center_pos, const float r, int& angle, const int add)
{
	angle += add;

	pos.x = (cosf(3.14 / 180.0f * angle) * r) + center_pos.x;//円運動の処理
	pos.y = (sinf(3.14 / 180.0f * angle) * r) + center_pos.y;//円運動の処理
}

bool GamePlayScene::MapCollide(XMFLOAT3& pos, float radiusX, float radiusY, int mapNumber, const XMFLOAT3 old_pos, bool is_jump)
{
	//マップチップ
	//X, Y
	float x = 0;
	float y = 0;
	//Radius
	float r_x = 0;
	float r_y = 0;

	//フラグ
	bool is_hit = false;

	//判定
	int max_x = static_cast<int>((pos.x + radiusX + LAND_SCALE / 2) / LAND_SCALE);
	int min_x = static_cast<int>((pos.x - radiusX + LAND_SCALE / 2) / LAND_SCALE);
	int max_y = -static_cast<int>((pos.y - radiusY + LAND_SCALE / 2) / LAND_SCALE - 1);
	int min_y = -static_cast<int>((pos.y + radiusY + LAND_SCALE / 2) / LAND_SCALE - 1);

	for (int h = min_y; h <= max_y; h++)
	{
		if (h < 0)
		{
			continue;
		}
		for (int w = min_x; w <= max_x; w++)
		{
			if (w < 0)
			{
				continue;
			}
			if (Mapchip::GetChipNum(w, h, map[mapNumber]) == Ground)
			{	
				x = objBlock[h][w]->GetPosition().x;
				y = objBlock[h][w]->GetPosition().y;
				r_x = 2.5f * objBlock[h][w]->GetScale().x;
				r_y = 2.5f * objBlock[h][w]->GetScale().y;

				if (pos.x <= x + r_x && x - r_x <= pos.x)
				{
					if (y + r_y + radiusY > pos.y && y < old_pos.y)
					{
						pos.y = y + r_y + radiusY;
						is_hit = true;
					} 
					else if (y - r_y - radiusY < pos.y && y > old_pos.y)
					{
						pos.y = y - r_y - radiusY;
						if (is_jump == false)
						{
							is_hit = true;
						} 
						else
						{
							p_add = 0;
						}
					}
				}
				if (pos.y <= y + r_y && y - r_y <= pos.y)
				{
					if (x + r_x + radiusX > pos.x && x < old_pos.x)
					{
						pos.x = x + r_x + radiusX;
						if (is_jump == false)
						{
							is_hit = true;
						}
					} 
					else if (x - r_x - radiusX < pos.x && x > old_pos.x)
					{
						pos.x = x - r_x - radiusX;
						if (is_jump == false)
						{
							is_hit = true;
						}
					}
				}
			}
		}
	}

	return is_hit;
}

void GamePlayScene::RopeMove(XMFLOAT3& pos, const int num)
{
	Rope[num]->SetPosition({ (p_pos.x + enemy_data[num].e_pos.x) / 2, (p_pos.y + enemy_data[num].e_pos.y) / 2, 0 });
	Rope[num]->Update();

	//ロープの位置を取得
	XMFLOAT3 r_pos = Rope[num]->GetPosition();

	//プレイヤーとエネミーの距離
	XMFLOAT2 length = { p_pos.x - enemy_data[num].e_pos.x, p_pos.y - enemy_data[num].e_pos.y };
	float len = GetLengthObject(p_pos, enemy_data[num].e_pos);
	//最大値より大きいなら
	if (len > max_rope)
	{
		float wq = len / max_rope;
		len = max_rope;
		enemy_data[num].e_pos = { p_pos.x - length.x / wq, p_pos.y - length.y / wq, 0 };
		pos = enemy_data[num].e_pos;
		MapCollide(pos, p_x_radius, p_y_radius, 0, enemy_data[num].e_pos);
	}

	//ロープの長さ
	float angleX = rope_angle->PosForAngle(p_pos.x, r_pos.y, r_pos.x, p_pos.y);
	Rope[num]->SetPosition({ (p_pos.x + enemy_data[num].e_pos.x) / 2, (p_pos.y + enemy_data[num].e_pos.y) / 2, 0 });
	Rope[num]->SetScale({ 0.3f, len / 2, 0.3f });
	Rope[num]->SetRotation({ 0, 0 ,XMConvertToDegrees(angleX) });
}

bool GamePlayScene::CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b)
{
	//座標
	XMFLOAT3 A = object_a->GetPosition();
	XMFLOAT3 B = object_b->GetPosition();

	//半径
	float a_r = 1.0f * object_a->GetScale().x;
	float b_r = 1.0f * object_a->GetScale().x;

	//長さ
	float l_x = sqrtf(powf(A.x - B.x, 2));
	float l_y = sqrtf(powf(A.y - B.y, 2));

	//半径の合計より短ければ
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

bool GamePlayScene::inFrustum(XMFLOAT3 playerPosition, XMFLOAT3 negativePoint, XMFLOAT3 positivePoint)
{
	//長さの単位
	float IdentityLen = 720 / 2 * sqrtf(3);

	//ターゲットの横と縦の長さ
	float eyeLen = camera->GetTarget().z - camera->GetEye().z;
	float targetWidth = 1280 * eyeLen / IdentityLen;
	float targetHeight = 720 * eyeLen / IdentityLen;

	//横
	if (positivePoint.x <= targetWidth / 2 + playerPosition.x && -targetWidth / 2 + playerPosition.x <= positivePoint.x)
	{
		//縦
		if (positivePoint.y <= targetHeight / 2 + playerPosition.y && -targetHeight / 2 + playerPosition.y <= positivePoint.y)
		{
			return true;
		} 
		else if (negativePoint.y <= targetHeight / 2 + playerPosition.y && -targetHeight / 2 + playerPosition.y <= negativePoint.y)
		{
			return true;
		}
	}
	//横
	else if (negativePoint.x <= targetWidth / 2 + playerPosition.x && -targetWidth / 2 + playerPosition.x <= negativePoint.x)
	{
		//縦
		if (positivePoint.y <= targetHeight / 2 + playerPosition.y && -targetHeight / 2 + playerPosition.y <= positivePoint.y)
		{
			return true;
		} 
		else if (negativePoint.y <= targetHeight / 2 + playerPosition.y && -targetHeight / 2 + playerPosition.y <= negativePoint.y)
		{
			return true;
		}
	}

	return false;
}
