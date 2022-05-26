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
#include "ConvertScene.h"
#include "Count.h"
#include <fstream> 

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
	Sprite::LoadTexture(11, L"Resources/haikei.png");
	Sprite::LoadTexture(12, L"Resources/minimap.png");
	Sprite::LoadTexture(13, L"Resources/miniplayer.png");
	Sprite::LoadTexture(14, L"Resources/minienemy.png");
	Sprite::LoadTexture(15, L"Resources/score.png");
	Sprite::LoadTexture(16, L"Resources/level.png");
	Sprite::LoadTexture(17, L"Resources/koron.png");
	Sprite::LoadTexture(18, L"Resources/timer.png");
	Sprite::LoadTexture(19, L"Resources/timeLimit.png");
	//Sprite::LoadTexture(20, L"Resources/timer.png");
	Sprite::LoadTexture(21, L"Resources/switch_In.png");
	Sprite::LoadTexture(22, L"Resources/switch_Out.png");
	Sprite::LoadTexture(23, L"Resources/Finish.png");
	Sprite::LoadTexture(24, L"Resources/Count1.png");
	Sprite::LoadTexture(25, L"Resources/Count2.png");
	Sprite::LoadTexture(26, L"Resources/Count3.png");
	Sprite::LoadTexture(27, L"Resources/CountStart.png");
	Sprite::LoadTexture(28, L"Resources/LevelUp.png");
	// 背景スプライト生成
	spriteBG = Sprite::Create(11, { 0.0f,0.0f });
	spriteBG->SetSize({ WinApp::window_width * 1.2, WinApp::window_height * 1.2 });
	texScore = Sprite::Create(15, { WinApp::window_width - 532, 0 });
	texLevel = Sprite::Create(16, { WinApp::window_width - 100, WinApp::window_height - 64 });

	//スプライト生成
	minimap = Sprite::Create(12, { 0.0f,50.0f });
	miniplayer = Sprite::Create(13, { 20.0f,20.0f });
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		minienemy[i] = Sprite::Create(14, { 40.0f,20.0f });
	}
	spriteTimer = Sprite::Create(18, { 0,0 });
	spriteTime[0] = Sprite::Create(0, { 64,0 });
	spriteCoron = Sprite::Create(17, { 96,0 });
	spriteTime[1] = Sprite::Create(0, { 128,0 });
	spriteTime[2] = Sprite::Create(0, { 160,0 });
	spriteScore[1] = Sprite::Create(0, { WinApp::window_width - 32,0 });
	spriteScore[2] = Sprite::Create(0, { WinApp::window_width - 64,0 });
	spriteScore[3] = Sprite::Create(0, { WinApp::window_width - 96,0 });
	spriteScore[4] = Sprite::Create(0, { WinApp::window_width - 128,0 });
	spriteScore[5] = Sprite::Create(0, { WinApp::window_width - 160,0 });
	spriteScore[6] = Sprite::Create(0, { WinApp::window_width - 192,0 });
	spriteScore[7] = Sprite::Create(0, { WinApp::window_width - 224,0 });
	spriteScore[8] = Sprite::Create(0, { WinApp::window_width - 256,0 });
	spriteLevel[1] = Sprite::Create(0, { WinApp::window_width - 32 ,WinApp::window_height - 64 });
	spriteLevelUp = Sprite::Create(28, { WinApp::window_width / 2 - 80 ,WinApp::window_height / 2 - 80 });
	finish = Sprite::Create(23, { 0.0f ,0.0f });



	// オブジェクト生成
	model = Model::LoadFromOBJ("weakEnemy");
	modelPlayer = Model::LoadFromOBJ("cowgirl");
	enemy_model_2 = Model::LoadFromOBJ("oneHitEnemy");
	block = Model::LoadFromOBJ("block");
	rope = Model::LoadFromOBJ("rope");
	locusModel = Model::LoadFromOBJ("locus");

	for (int i = 0; i < EnemySpawnMax; i++)
	{
		enemy[i] = Object3d::Create();
		Rope[i] = Object3d::Create();
	}
	player = Object3d::Create();	

	
	//シーン切り替え
	ConvertScene::InitializeOut();
	Count::Initilize();
	//エフェクト
	Effect::Initialize();

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
	lastTime = 160.0f;
	level = 1;
	levelTime = 0;
	enemySpawn = 1;
	gravity = 0.125f;
	catch_count = 0;
	shake_power = 0;

	//オブジェクトにモデルをひも付ける
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		//エネミー
		enemy_data[i].e_pos = { 0, 0, 0 };
		enemy[i]->SetScale({ 3, 3, 3 });
		enemy[i]->Update();
		enemy_data[i].e_x_radius = 0.6f * player->GetScale().x;
		enemy_data[i].e_y_radius = 0.8f * player->GetScale().y;
		enemy_data[i].is_normal = true;
		enemy_data[i].is_bounce = false;
		enemy_data[i].is_catch = false;
		enemy_data[i].is_alive = false;
		enemy_data[i].is_grand = false;
		enemy_data[i].is_turn = false;
		enemy_data[i].e_speed = 0.25f;
		enemy_data[i].e_down = 0.0f;
		enemy_data[i].angle = 0;
		enemy_data[i].turn_move = 1;
		enemy_data[i].enemy_type = NORMAL;
		enemy_data[i].e_acc = 0;
		enemy_data[i].can_catch = false;
		enemy_data[i].is_add = true;
		enemy_data[i].escape_time = 0;
		enemy_data[i].max_rope = 15;
		enemy[i]->SetModel(model);
		enemy[i]->SetPosition(enemy_data[i].e_pos);
		enemy[i]->Update();

		//ロープ
		Rope[i]->SetModel(rope);
		Rope[i]->SetScale({ 0.3, 5, 0.3 });
		Rope[i]->Update();

		scoreTick[i] = 0;
	}

	//プレイヤー
	player->SetModel(modelPlayer);
	player->SetScale({ 3, 3, 3 });
	p_pos = { 20, 10, 0 };
	old_p_pos = { 0, 0, 0 };
	p_x_radius = 0.4f * player->GetScale().x;
	p_y_radius = 0.8f * player->GetScale().y;
	is_jump = false;
	p_add = 0;
	p_down = 0;
	oldLevel = 0;
	is_attack = false;
	is_air = false;
	is_damage = false;
	damage_time = 0;
	damage_move = 0;
	invincible_time = 0;
	is_invincible = false;
	player->SetPosition(p_pos);
	player->SetRotation({ 0, 180, 0 });
	player->Update();

	c_pos = p_pos;

	camera->SetTarget(c_pos);
	camera->SetEye({ c_pos.x, c_pos.y, c_pos.z - 55.0f - (2.5f * level) });
	camera->Update();

	camera_chase = false;

	//モデル名を指定してファイル読み込み
	//fbxModel1 = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	//3Dオブジェクト生成とモデルのセット
	//fbxObject1 = new FbxObject3d;
	//fbxObject1->Initialize();
	//fbxObject1->SetModel(fbxModel1);
	
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

	ConvertScene::Finalize();
	Count::Finalize();
	ConvertFlag = false;
	countFinishFlag = false;
	finishFinishFlag = false;
	endConvertflag = false;
	PlayPossibleflag = false;
}

void GamePlayScene::Update()
{
	// ゲームシーンの毎フレーム処理
	Input* input = Input::GetInstance();

	//実験用エフェクト置き場
	if (input->PushKey(DIK_M))  //衝撃波開始
	{
		const int mono = 10;
		Effect::DestroyEffectCreate( mono ,p_pos);
	}

	oldLevel = level;
	
	//シーン切り替え
	if (ConvertFlag == false)
	{
		ConvertScene::besideOut(ConvertFlag);
	}
	//開始カウントダウン
	if (ConvertFlag == true)
	{
		Count::CountDown3(countFinishFlag);
		if (countFinishFlag == true)
		{
			PlayPossibleflag = true;
			LoadText();
		}
	}

	//プレイタイムカウントダウン
	if (countFinishFlag == true)
	{
		if (lastTime > 0)
		{
			lastTime -= 0.0166;
		}
	}

	//終了処理
	if (lastTime < 0)
	{
		PlayPossibleflag = false;
		Count::Fnish(finishFinishFlag, lastTime);
	}
	//エンド画面移行処理
	if (finishFinishFlag == true)
	{
		ConvertScene::besideIn(endConvertflag);

		if (endConvertflag == true)
		{


			//シーン切り替え
			SceneManager::GetInstance()->ChangeScene("END");
		}
	}
	spriteTime[2]->ChangeTex((int)lastTime % 10);
	spriteTime[1]->ChangeTex((int)lastTime / 10);
	spriteTime[0]->ChangeTex((int)lastTime / 60);

	int second;
	second = (int)lastTime % 60;
	spriteTime[2]->ChangeTex(second % 10);
	spriteTime[1]->ChangeTex(second / 10);

	//Mキーでマップチップ設置
	MapCreate(0);
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			objBlock[y][x]->Update();
		}
	}

	//プレイヤー処理
	if (PlayPossibleflag == true)
	{
		//座標更新
		p_pos = player->GetPosition();
		old_p_pos = p_pos;

		//移動
		if (is_damage == false)
		{
			if (input->LeftStickAngle().x)
			{
				if (input->LeftStickAngle().x > 0)
				{
					p_pos.x += input->LeftStickAngle().x / 2 + 0.08 * (level - 1);
				}
				else
				{
					p_pos.x -= -input->LeftStickAngle().x / 2 + 0.08 * (level - 1);
				}

				//進行方向に向きを変える
				if (is_attack == false)
				{
					if (input->LeftStickAngle().x >= 0)
					{
						player->SetRotation(XMFLOAT3(0, 90, 0));
					}
					else
					{
						player->SetRotation(XMFLOAT3(0, 270, 0));
					}
				}
			}
			//キーボード用
			if (input->PushKey(DIK_D))
			{
				p_pos.x += 0.5f + 0.08 * (level - 1);
				//進行方向に向きを変える
				if (is_attack == false)
				{
					player->SetRotation(XMFLOAT3(0, 90, 0));
				}
			}
			if (input->PushKey(DIK_A))
			{
				p_pos.x -= 0.5f + 0.08 * (level - 1);
				//進行方向に向きを変える
				if (is_attack == false)
				{
					player->SetRotation(XMFLOAT3(0, 270, 0));
				}
			}
		}

		//攻撃
		if ((input->TriggerKey(DIK_SPACE) || input->PushButton(Button_B)) && is_attack == false && is_damage == false)
		{
			for (int i = 0; i < enemySpawn; i++)
			{
				//プレイヤーの攻撃
				if (enemy_data[i].is_catch == true)
				{
					//プレイヤーの向きで投げる方向を変える
					if (player->GetRotation().y == 90 && p_pos.x > enemy[i]->GetPosition().x)
					{
						enemy_data[i].angle = static_cast<int>(XMConvertToDegrees(rope_angle->PosForAngle(p_pos.x, enemy[i]->GetPosition().y, enemy[i]->GetPosition().x, p_pos.y))) - 90;
						is_attack = true;
					} 
					else if (player->GetRotation().y == 270 && p_pos.x < enemy[i]->GetPosition().x)
					{
						enemy_data[i].angle = static_cast<int>(XMConvertToDegrees(rope_angle->PosForAngle(p_pos.x, enemy[i]->GetPosition().y, enemy[i]->GetPosition().x, p_pos.y))) - 90;
						is_attack = true;
					}
				}
			}
		}

		//ダメージリアクション
		if (is_damage == true)
		{
			p_pos.x += damage_move;
			damage_time++;

			if (damage_time > 10)
			{
				is_damage = false;
				damage_move = 0;
				damage_time = 0;
				is_invincible = true;
			}
		}

		//無敵時間
		if (is_invincible == true)
		{
			invincible_time += 0.166;

			if (invincible_time > 6)
			{
				invincible_time = 0;
				is_invincible = false;
			}
		}

		//ジャンプ
		if ((input->TriggerKey(DIK_W) || input->TriggerButton(Button_A)) && is_air == false && is_jump == false && is_damage == false)
		{
			is_jump = true;

			//上昇率の初期化
			p_add = 2.25f + (level - 1) * 0.125f;
		}

		//ジャンプ処理
		if (is_jump == true)
		{
			//座標の上昇
			p_add -= gravity;
			p_pos.y += p_add;

			if (MapCollide(p_pos, p_x_radius, p_y_radius, p_add, 0, old_p_pos, true))
			{
				//初期化
				is_jump = false;
			}
		}
		//重力
		else
		{
			is_air = true;
			//下降度をマイナス
			p_down -= gravity;
			p_pos.y += p_down;

			if (MapCollide(p_pos, p_x_radius, p_y_radius, p_down, 0, old_p_pos, true))
			{
				//初期化
				p_down = 0;
				is_air = false;
				camera_chase = true;
			}
		}

		//ミニマップ用座標変換
		miniplayer->SetPosition({ p_pos.x , -p_pos.y + 78 });
	}

	//エネミー処理
	if (PlayPossibleflag == true)
	{
		for (int i = 0; i < enemySpawn; i++)
		{
			//敵のスポーン
			if (enemy_data[i].is_alive == false)
			{
				SpawnEnemy(0, i);
			}
			//敵の処理
			else
			{
				//座標取得
				enemy_data[i].e_pos = enemy[i]->GetPosition();
				enemy_data[i].old_e_pos = enemy_data[i].e_pos;

				//プレイヤーとエネミーが接触したら
				if (is_damage == false && is_invincible == false && enemy_data[i].is_catch == false)
				{
					if (CollisionObject(player, enemy[i]) == true && enemy_data[i].can_catch == true)
					{
						enemy_data[i].is_catch = true;
						enemy_data[i].is_bounce = false;
						catch_count++;
						enemy_data[i].max_rope += static_cast<float>(rand() % 50) / 10 - 2.5f;
					} 
					else if (CollisionObject(player, enemy[i]) == true && enemy_data[i].can_catch == false)
					{
						is_damage = true;
						//左
						if (p_pos.x < enemy_data[i].e_pos.x)
						{
							damage_move = -0.6f;
						}
						//右
						else
						{
							damage_move = 0.6f;
						}
					}
				}
				//更新処理
				if (enemy_data[i].is_catch == false)
				{
					//通常状態
					if (enemy_data[i].is_turn == true && enemy_data[i].enemy_type == TWICE)
					{
						enemy_data[i].e_pos.y += enemy_data[i].turn_move;
						XMFLOAT3 e_rot = enemy[i]->GetRotation();
						e_rot.z += 10.0f;
						enemy[i]->SetRotation(e_rot);

						if (e_rot.z >= 90 && enemy_data[i].turn_move > 0.0f)
						{
							enemy_data[i].turn_move = -enemy_data[i].turn_move;
						}
						if (e_rot.z >= 180 && enemy_data[i].turn_move < 0.0f)
						{
							e_rot.z = 180.0f;
							enemy[i]->SetRotation(e_rot);
							enemy_data[i].turn_move = -enemy_data[i].turn_move;
							enemy_data[i].is_turn = false;
						}
					}
					else if (enemy_data[i].is_normal == true && enemy[i]->GetRotation().z != 180)
					{
						//移動
						enemy_data[i].e_pos.x += enemy_data[i].e_speed;

						 if (enemy_data[i].is_grand == true)
						{
							//端まで行ったら
							if (MapCollide(enemy_data[i].e_pos, enemy_data[i].e_x_radius, enemy_data[i].e_y_radius, enemy_data[i].e_down, 0, enemy_data[i].old_e_pos))
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
						if (player->GetRotation().y == 90)
						{
							CircularMotion(enemy_data[i].e_pos, p_pos, GetObjectLength(p_pos, enemy_data[i].e_pos), enemy_data[i].angle, -15);
							Effect::CreateLocus( locus, *locusModel, enemy_data[i].e_pos);
							scoreTick[i]++;
						}
						//左向きなら
						else if (player->GetRotation().y == 270)
						{
							CircularMotion(enemy_data[i].e_pos, p_pos, GetObjectLength(p_pos, enemy_data[i].e_pos), enemy_data[i].angle, 15);
							Effect::CreateLocus(locus, *locusModel, enemy_data[i].e_pos);
							scoreTick[i]++;
						}
						//マップの当たり判定
						if (MapCollide(enemy_data[i].e_pos, enemy_data[i].e_x_radius, enemy_data[i].e_y_radius, enemy_data[i].e_down, 0, enemy_data[i].old_e_pos))
						{
							//エフェクト
							shockFlag = true;
							enemy_data[i].is_alive = false;
							enemy_data[i].is_catch = false;
							is_shake = true;
							is_attack = false;
							if (shake_power == 0)
							{
								shake_power = catch_count;
							}
							if (catch_count > 0)
							{
								catch_count--;
							}
							for (int j = 0; j < enemySpawn; j++)
							{
								if (i != j && enemy_data[j].is_alive == true && enemy_data[j].is_catch == true)
								{
									is_attack = true;
									break;
								}
							}
							if (is_attack == false)
							{
								catch_count = 0;
							}
							score++;
							score += scoreTick[i];
							int hundredScore = 0;
							int hundredScore2 = 0;
							int hundredScore3 = 0;
							int hundredScore4 = 0;
							int hundredScore5 = 0;
							int hundredScore6 = 0;
							int hundredScore7 = 0;
							hundredScore = score / 10;
							hundredScore2 = score / 100;
							hundredScore3 = score / 1000;
							hundredScore4 = score / 10000;
							hundredScore5 = score / 100000;
							hundredScore6 = score / 1000000;
							hundredScore7 = score / 10000000;
							spriteScore[1]->ChangeTex((int)score % 10);
							spriteScore[2]->ChangeTex((int)hundredScore % 10);
							spriteScore[3]->ChangeTex((int)hundredScore2 % 10);
							spriteScore[4]->ChangeTex((int)hundredScore3 % 10);
							spriteScore[5]->ChangeTex((int)hundredScore4 % 10);
							spriteScore[6]->ChangeTex((int)hundredScore5 % 10);
							spriteScore[7]->ChangeTex((int)hundredScore6 % 10);
							spriteScore[8]->ChangeTex((int)hundredScore7 % 10);
							scoreTick[i] = 0;
							if (enemy_data[i].is_add == true)
							{
								enemy_data[i].max_rope += 0.5f;

								if (enemy_data[i].max_rope > 25.0f)
								{
									enemy_data[i].max_rope = 25.0f;
								}
							}
							for (int j = 0; j < enemySpawn; j++)
							{
								if (i != j && enemy_data[j].is_alive == true && enemy_data[j].enemy_type == TWICE && enemy_data[j].can_catch == false)
								{
									XMFLOAT3 positivePos = { enemy_data[j].e_pos.x + enemy_data[j].e_x_radius, enemy_data[j].e_pos.y + enemy_data[j].e_y_radius, 0 };
									XMFLOAT3 negativePos = { enemy_data[j].e_pos.x - enemy_data[j].e_x_radius, enemy_data[j].e_pos.y - enemy_data[j].e_y_radius, 0 };
									float w_width = 210;
									float w_height = 120;
									if (catch_count >= 6)
									{
										w_width *= 6;
										w_height *= 6;
									}
									else if (catch_count < 6)
									{
										w_width *= catch_count;
										w_height *= catch_count;
									}
									if (inFrustum(enemy_data[i].e_pos, negativePos, positivePos, w_width, w_height) == true)
									{
										enemy[j]->SetModel(model);
										enemy_data[j].can_catch = true;
										enemy_data[j].is_turn = true;
									}
								}
							}
						}

					} 
					else
					{
						if (static_cast<float>(enemy_data[i].escape_time / 60) > 10)
						{
							enemy_data[i].escape_time = 0;
							enemy_data[i].is_catch = false;
							if (enemy_data[i].enemy_type == TWICE)
							{
								enemy[i]->SetModel(model);
								enemy_data[i].can_catch = true;
							}
						}
						else
						{
							enemy_data[i].escape_time++;
						}
					}
				}
				//重力
				if (is_attack == false || enemy_data[i].is_bounce == true)
				{
					//下降度をマイナス
					if (enemy_data[i].is_bounce == false)
					{
						enemy_data[i].e_down -= gravity;
						enemy_data[i].e_pos.y += enemy_data[i].e_down;
					}
					else
					{
						enemy_data[i].e_acc -= gravity;
						enemy_data[i].e_pos.y += enemy_data[i].e_acc;
					}
					enemy_data[i].is_grand = false;

					//マップの当たり判定
					if (enemy_data[i].is_bounce == false)
					{
						if (MapCollide(enemy_data[i].e_pos, enemy_data[i].e_x_radius, enemy_data[i].e_y_radius, enemy_data[i].e_down , 0, enemy_data[i].old_e_pos, true))
						{
							enemy_data[i].e_down = 0;
							enemy_data[i].is_grand = true;
							if (enemy_data[i].enemy_type == JUMP && enemy_data[i].is_catch == false)
							{
								enemy_data[i].is_bounce = true;
								enemy_data[i].e_acc = 2.25f;
							}
						}
					}
					else
					{
						if (MapCollide(enemy_data[i].e_pos, enemy_data[i].e_x_radius, enemy_data[i].e_y_radius, enemy_data[i].e_acc, 0, enemy_data[i].old_e_pos, true))
						{
							enemy_data[i].is_grand = true;
							enemy_data[i].e_acc = 2.25f;
						}
					}
				}
				//ロープの更新
				if (enemy_data[i].is_catch == true)
				{
					RopeMove(i);
					if (enemy_data[i].e_pos.x == enemy_data[i].old_e_pos.x && enemy_data[i].e_pos.y == enemy_data[i].old_e_pos.y)
					{
						enemy_data[i].e_down = 0;
					}
				}

				//ミニマップ用座標変換
				minienemy[i]->SetPosition({ enemy_data[i].e_pos.x , -enemy_data[i].e_pos.y + 78 });
			}
		}
	}

	if (is_attack == false)
	{
		Effect::DeletLocus(locus, camera, p_pos); //エフェクト削除処理
	}

	//エネミー更新
	for (int i = 0; i < enemySpawn; i++)
	{
		//落下の最大値を超えたら
		if (enemy_data[i].e_pos.y < -300.0f)
		{
			enemy_data[i].e_pos.y = 10;
			enemy_data[i].e_down = 0;
			enemy_data[i].e_acc = 0;
		}
		else if (enemy_data[i].is_catch == false && (enemy_data[i].e_pos.x < -3.7 || enemy_data[i].e_pos.x > map_max_x * LAND_SCALE - 2))
		{
			if (enemy_data[i].e_pos.x < -3.7)
			{
				enemy_data[i].e_pos.x = (map_max_x - 1) * LAND_SCALE;
				enemy_data[i].e_pos.y = -10;
			} 
			else
			{
				enemy_data[i].e_pos.x = 0;
				enemy_data[i].e_pos.y = -35;
			}
		}
		//オブジェクト情報の更新
		enemy[i]->SetPosition(enemy_data[i].e_pos);
		enemy[i]->Update();
	}
	
	//プレイヤー更新
	if (p_pos.y < -300.0f)
	{
		p_pos.y = 10;
		p_down = 0;
		p_add = 0;
	}
	else if (p_pos.x < -3.7 || p_pos.x > map_max_x * LAND_SCALE - 2)
	{
		if (p_pos.x < -3.7)
		{
			p_pos.x = (map_max_x - 1) * LAND_SCALE;
			p_pos.y = -10;
		}
		else
		{
			p_pos.x = 0;
			p_pos.y = -35;
		}
	}
	player->SetPosition(p_pos);
	player->Update();

	//カメラ更新
	if (is_shake == true)
	{
		if (shake_power > 5)
		{
			shake_power = 5;
		}
		int power = shake_power * 10 + 1;
		shake_x = static_cast<float>(rand() % power) / 10;
		shake_y = static_cast<float>(rand() % power) / 10;
		shake_time++;
		if (shake_time > 20)
		{
			shake_x = 0.0f;
			shake_y = 0.0f;
			shake_time = 0;
			shake_power = 0;
			is_shake = false;
		}
	}
	if (camera_chase == false)
	{
		XMFLOAT3 camera_pos = p_pos;
		camera->SetTarget(camera_pos);
		camera_pos.z -= 55.0f + (1.0 * level);
		camera->SetEye(camera_pos);
		camera->Update();
		c_pos = camera->GetTarget();
	}
	else
	{
		XMFLOAT3 camera_pos = c_pos;
		if (powf(c_pos.x - p_pos.x, 2) > powf(10, 2))
		{
			camera_pos.x += p_pos.x - old_p_pos.x;
		}
		if (powf(c_pos.y - p_pos.y, 2) > powf(5, 2))
		{
			camera_pos.y += p_pos.y - old_p_pos.y;
		}
		camera->SetTarget(camera_pos);
		camera_pos.z -= 55.0f + (1.0 * level);
		camera->SetEye(camera_pos);
		camera->Update();
		c_pos = camera->GetTarget();
	}
	if (is_shake == true)
	{
		XMFLOAT3 camera_pos = c_pos;
		camera_pos.x += shake_x;
		camera_pos.y += shake_y;
		camera->SetTarget(camera_pos);
		camera_pos.z -= 55.0f + (2.5 * level);
		camera->SetEye(camera_pos);
		camera->Update();
	}

	if (score > 1000)
	{
		level = 7;
	} 
	else if (score > 750)
	{
		level = 6;
	} 
	else if (score > 500)
	{
		level = 5;
	} 
	else if (score > 200)
	{
		level = 4;
	}
	else if (score > 70)
	{
		level = 3;
	} 
	else if (score > 30)
	{
		level = 2;
	}

	if (level > oldLevel)
	{
		levelTime = 1;
	}

	if (level > oldLevel)
	{
		if (level == 2)
		{
			Mapchip::SetChipNum(11, 7, map[0]);
			Mapchip::SetChipNum(12, 7, map[0]);
			Mapchip::SetChipNum(13, 7, map[0]);
			Mapchip::SetChipNum(11, 8, map[0]);
			Mapchip::SetChipNum(12, 8, map[0]);
			Mapchip::SetChipNum(13, 8, map[0]);
			Mapchip::SetChipNum(23, 3, map[0]);
			Mapchip::SetChipNum(24, 3, map[0]);
			Mapchip::SetChipNum(25, 3, map[0]);
			Mapchip::SetChipNum(26, 3, map[0]);
			Mapchip::SetChipNum(27, 3, map[0]);
			Mapchip::SetChipNum(23, 4, map[0]);
			Mapchip::SetChipNum(24, 4, map[0]);
			Mapchip::SetChipNum(25, 4, map[0]);
			Mapchip::SetChipNum(26, 4, map[0]);
			Mapchip::SetChipNum(27, 4, map[0]);
			Mapchip::SetChipNum(29, 6, map[0]);
			Mapchip::SetChipNum(30, 6, map[0]);
			Mapchip::SetChipNum(31, 6, map[0]);
			Mapchip::SetChipNum(32, 6, map[0]);
			Mapchip::SetChipNum(33, 6, map[0]);
			Mapchip::SetChipNum(29, 7, map[0]);
			Mapchip::SetChipNum(30, 7, map[0]);
			Mapchip::SetChipNum(31, 7, map[0]);
			Mapchip::SetChipNum(32, 7, map[0]);
			Mapchip::SetChipNum(33, 7, map[0]);
			Mapchip::SetChipNum(88, 2, map[0]);
			Mapchip::SetChipNum(89, 2, map[0]);
			Mapchip::SetChipNum(90, 2, map[0]);
			Mapchip::SetChipNum(91, 2, map[0]);
			Mapchip::SetChipNum(92, 2, map[0]);
			Mapchip::SetChipNum(88, 3, map[0]);
			Mapchip::SetChipNum(89, 3, map[0]);
			Mapchip::SetChipNum(90, 3, map[0]);
			Mapchip::SetChipNum(91, 3, map[0]);
			Mapchip::SetChipNum(92, 3, map[0]);
		}

		if (level == 4)
		{
			Mapchip::SetChipNum(11, 6, map[0]);
			Mapchip::SetChipNum(12, 6, map[0]);
			Mapchip::SetChipNum(13, 6, map[0]);
			Mapchip::SetChipNum(11, 7, map[0]);
			Mapchip::SetChipNum(12, 7, map[0]);
			Mapchip::SetChipNum(13, 7, map[0]);
			Mapchip::SetChipNum(23, 2, map[0]);
			Mapchip::SetChipNum(24, 2, map[0]);
			Mapchip::SetChipNum(25, 2, map[0]);
			Mapchip::SetChipNum(26, 2, map[0]);
			Mapchip::SetChipNum(27, 2, map[0]);
			Mapchip::SetChipNum(23, 3, map[0]);
			Mapchip::SetChipNum(24, 3, map[0]);
			Mapchip::SetChipNum(25, 3, map[0]);
			Mapchip::SetChipNum(26, 3, map[0]);
			Mapchip::SetChipNum(27, 3, map[0]);
			Mapchip::SetChipNum(29, 5, map[0]);
			Mapchip::SetChipNum(30, 5, map[0]);
			Mapchip::SetChipNum(31, 5, map[0]);
			Mapchip::SetChipNum(32, 5, map[0]);
			Mapchip::SetChipNum(33, 5, map[0]);
			Mapchip::SetChipNum(29, 6, map[0]);
			Mapchip::SetChipNum(30, 6, map[0]);
			Mapchip::SetChipNum(31, 6, map[0]);
			Mapchip::SetChipNum(32, 6, map[0]);
			Mapchip::SetChipNum(33, 6, map[0]);
			Mapchip::SetChipNum(88, 1, map[0]);
			Mapchip::SetChipNum(89, 1, map[0]);
			Mapchip::SetChipNum(90, 1, map[0]);
			Mapchip::SetChipNum(91, 1, map[0]);
			Mapchip::SetChipNum(92, 1, map[0]);
			Mapchip::SetChipNum(88, 2, map[0]);
			Mapchip::SetChipNum(89, 2, map[0]);
			Mapchip::SetChipNum(90, 2, map[0]);
			Mapchip::SetChipNum(91, 2, map[0]);
			Mapchip::SetChipNum(92, 2, map[0]);
		}

		if (level == 6)
		{
			Mapchip::SetChipNum(11, 5, map[0]);
			Mapchip::SetChipNum(12, 5, map[0]);
			Mapchip::SetChipNum(13, 5, map[0]);
			Mapchip::SetChipNum(11, 6, map[0]);
			Mapchip::SetChipNum(12, 6, map[0]);
			Mapchip::SetChipNum(13, 6, map[0]);
			Mapchip::SetChipNum(23, 1, map[0]);
			Mapchip::SetChipNum(24, 1, map[0]);
			Mapchip::SetChipNum(25, 1, map[0]);
			Mapchip::SetChipNum(26, 1, map[0]);
			Mapchip::SetChipNum(27, 1, map[0]);
			Mapchip::SetChipNum(23, 2, map[0]);
			Mapchip::SetChipNum(24, 2, map[0]);
			Mapchip::SetChipNum(25, 2, map[0]);
			Mapchip::SetChipNum(26, 2, map[0]);
			Mapchip::SetChipNum(27, 2, map[0]);
			Mapchip::SetChipNum(29, 4, map[0]);
			Mapchip::SetChipNum(30, 4, map[0]);
			Mapchip::SetChipNum(31, 4, map[0]);
			Mapchip::SetChipNum(32, 4, map[0]);
			Mapchip::SetChipNum(33, 4, map[0]);
			Mapchip::SetChipNum(29, 5, map[0]);
			Mapchip::SetChipNum(30, 5, map[0]);
			Mapchip::SetChipNum(31, 5, map[0]);
			Mapchip::SetChipNum(32, 5, map[0]);
			Mapchip::SetChipNum(33, 5, map[0]);
			Mapchip::SetChipNum(88, 1, map[0]);
			Mapchip::SetChipNum(89, 1, map[0]);
			Mapchip::SetChipNum(90, 1, map[0]);
			Mapchip::SetChipNum(91, 1, map[0]);
			Mapchip::SetChipNum(92, 1, map[0]);
			Mapchip::SetChipNum(88, 0, map[0]);
			Mapchip::SetChipNum(89, 0, map[0]);
			Mapchip::SetChipNum(90, 0, map[0]);
			Mapchip::SetChipNum(91, 0, map[0]);
			Mapchip::SetChipNum(92, 0, map[0]);
		}
		for (int y = 0; y < map_max_y; y++)
		{
			for (int x = 0; x < map_max_x; x++)
			{
				objBlock[y][x]->Update();
			}
		}
	}

	if (levelTime >= 0)
	{
		levelTime -= 0.01;
	}
	enemySpawn = level * 6;
	spriteLevel[1]->ChangeTex((int)level % 10);


	//エフェクト
	for (int i = 0; i < locus.size(); i++)
	{
		locus[i]->Update();
	}
	Effect::DestroyEffectUpdate(camera, p_pos);

	//fbxObject1->AnimationFlag = true;
	//fbxObject1->AnimationNum = 0;
	//アップデート
	//fbxObject1->Update();
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

	if (is_damage == true)
	{
		Effect::flashingEffectDraw(player, damage_time);
	}
	else if (is_invincible == true)
	{
		Effect::flashingEffectDraw(player, invincible_time);
	}
	else
	{
		player->Draw();
	}

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
	
	Effect::DestroyEffectDraw();
	
	//FBX3Dオブジェクトの描画
	//fbxObject1->Draw(cmdList);
	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	Effect::TimeLimitEffectDraw(lastTime);
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
	spriteScore[4]->Draw();
	spriteScore[5]->Draw();
	spriteScore[6]->Draw();
	spriteScore[7]->Draw();
	spriteScore[8]->Draw();

	if (levelTime > 0 && 0.35 > levelTime || levelTime > 0.70 && 1 > levelTime)
	{
		spriteLevelUp->Draw();
	}

	//シーン移行
	ConvertScene::Draw();
	//ゲーム開始カウントダウン
	if (ConvertFlag == true)
	{
		Count::Draw();
	}
	
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
			else
			{
				objBlock[y][x]->SetPosition({ 1000, 1000, 0 });
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
		enemy[enemyNumber]->SetRotation({ 0, 0, 0 });
		enemy_data[enemyNumber].e_pos = { spawnX * LAND_SCALE,  -spawnY * LAND_SCALE, 0 };
		enemy_data[enemyNumber].is_alive = true;//スポーン
		enemy_data[enemyNumber].enemy_type = NORMAL;
		int num = rand() % 101;
		if (level * 10 > num)
		{
			enemy_data[enemyNumber].enemy_type = rand() % JUMP + 1;
		}
		int j_num = 0;
		for (int j = 0; j < enemySpawn; j++)
		{
			if (enemy_data[j].is_alive == true && enemy_data[j].enemy_type == NORMAL)
			{
				j_num++;
				break;
			}
		}
		if (j_num == 0)
		{
			enemy_data[enemyNumber].enemy_type = NORMAL;
		}
		if (enemy_data[enemyNumber].enemy_type == TWICE)
		{
			enemy_data[enemyNumber].can_catch = false;
			enemy[enemyNumber]->SetModel(enemy_model_2);
			enemy[enemyNumber]->SetRotation({ 0, 0, 0 });
		}
		else
		{
			enemy_data[enemyNumber].can_catch = true;
			enemy[enemyNumber]->SetModel(model);
			enemy[enemyNumber]->SetRotation({ 0, 0, 0 });
		}
		if (enemy_data[enemyNumber].enemy_type == JUMP)
		{
			enemy_data[enemyNumber].is_bounce = true;
		}
		else
		{
			enemy_data[enemyNumber].is_bounce = false;
		}
		enemy_data[enemyNumber].is_turn = false;
	}
}

void GamePlayScene::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 center_pos, const float r, int& angle, const int add)
{
	angle += add;

	pos.x = (cosf(3.14 / 180.0f * angle) * r) + center_pos.x;//円運動の処理
	pos.y = (sinf(3.14 / 180.0f * angle) * r) + center_pos.y;//円運動の処理
}

bool GamePlayScene::MapCollide(XMFLOAT3& pos, float radiusX, float radiusY, float& add, int mapNumber, const XMFLOAT3 old_pos, bool is_jump)
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
							add = 0;
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

void GamePlayScene::RopeMove(const int num)
{
	//プレイヤーとエネミーの距離
	XMFLOAT2 length = { p_pos.x - enemy_data[num].e_pos.x, p_pos.y - enemy_data[num].e_pos.y };
	float len = GetObjectLength(p_pos, enemy_data[num].e_pos);
	//最大値より大きいなら
	if (len > enemy_data[num].max_rope)
	{
		float tmp = len / (enemy_data[num].max_rope);
		len = enemy_data[num].max_rope;
		enemy_data[num].e_pos = { p_pos.x - length.x / tmp, p_pos.y - length.y / tmp, 0 };
		MapCollide(enemy_data[num].e_pos, enemy_data[num].e_x_radius, enemy_data[num].e_y_radius, p_add, 0, enemy_data[num].e_pos);
	}

	//ロープの長さ
	XMFLOAT3 r_pos = { (p_pos.x + enemy_data[num].e_pos.x) / 2, (p_pos.y + enemy_data[num].e_pos.y) / 2, 0 };
	float angleX = rope_angle->PosForAngle(p_pos.x, r_pos.y, r_pos.x, p_pos.y);
	Rope[num]->SetPosition(r_pos);
	Rope[num]->SetScale({ 0.3f, len / 2, 0.3f });
	Rope[num]->SetRotation({ 0, 0 ,XMConvertToDegrees(angleX) });
	Rope[num]->Update();
}

bool GamePlayScene::CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b)
{
	//座標
	XMFLOAT3 A = object_a->GetPosition();
	XMFLOAT3 B = object_b->GetPosition();

	//半径
	float a_x = 0.4f * object_a->GetScale().x;
	float a_y = 0.8f * object_a->GetScale().x;
	float b_x = 0.6f * object_a->GetScale().x;
	float b_y = 0.6f * object_a->GetScale().x;

	//長さ
	float l_x = sqrtf(powf(A.x - B.x, 2));
	float l_y = sqrtf(powf(A.y - B.y, 2));

	//半径の合計より短ければ
	if (l_x < a_x + b_x && l_y < a_y + b_y)
	{
		return true;
	}

	return false;
}

float GamePlayScene::GetObjectLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b)
{
	XMFLOAT3 len = { pos_a.x - pos_b.x, pos_a.y - pos_b.y, pos_a.z - pos_b.z };

	return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
}

bool GamePlayScene::inFrustum(XMFLOAT3 playerPosition, XMFLOAT3 negativePoint, XMFLOAT3 positivePoint, const float width, const float height)
{
	//長さの単位
	float IdentityLen = 720 / 2 * sqrtf(3);

	//ターゲットの横と縦の長さ
	float eyeLen = camera->GetTarget().z - camera->GetEye().z;
	float targetWidth = width * eyeLen / IdentityLen;
	float targetHeight = height * eyeLen / IdentityLen;

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

void GamePlayScene::writeText()
{
	for (int i = 0; i < 3; i++)
	{
		if (score >= score_list[i])
		{
			if (i + 1 < 3)
			{
				score_list[i + 1] = score_list[i];
			}
			if (i + 2 < 3)
			{
				score_list[i + 2] = score_list[i];
			}
			score_list[i] = score;
			break;
		}
	}

	std::string first = std::to_string(score_list[0]);
	std::string second = std::to_string(score_list[1]);
	std::string third = std::to_string(score_list[2]);

	std::ofstream ofs("Resources/ScoreList.txt");
	if (!ofs)
	{
		assert(0);
	}
	else
	{
		ofs << " " << first << " " << second << " " << third;
	}
	ofs.close();
}

void GamePlayScene::LoadText()
{
	std::ifstream file;
	const std::string fileName = "Resources/ScoreList.txt";
	file.open(fileName);
	if (file.fail())
	{
		assert(0);
	}

	//1行ずつ読み込む
	std::string line;
	while (getline(file, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		//半角スペースゥ切りで行の先頭文字列を取得
		std::string key;
		getline(line_stream, key, ' ');

		line_stream >> score_list[0];
		line_stream >> score_list[1];
		line_stream >> score_list[2];
	}
	//ファイルを閉じる
	file.close();
}
