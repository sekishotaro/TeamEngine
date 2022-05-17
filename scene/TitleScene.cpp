#include "TitleScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"
#include "DirectXCommon.h"

using namespace DirectX;

void TitleScene::Initialize()
{

	// カメラ生成
	camera = new Camera(WinApp::window_width, WinApp::window_height);

	// カメラセット
	Object3d::SetCamera(camera);

	// テクスチャ読み込み
	Sprite::LoadTexture(1, L"Resources/haikei.png");
	Sprite::LoadTexture(2, L"Resources/titletext.png");

	// オブジェクト生成
	model = Model::LoadFromOBJ("weakEnemy");
	modelPlayer = Model::LoadFromOBJ("cowgirl");
	enemy_model_2 = Model::LoadFromOBJ("oneHitEnemy");
	block = Model::LoadFromOBJ("block");
	rope = Model::LoadFromOBJ("rope");
	locusModel = Model::LoadFromOBJ("locus");

	block = Model::LoadFromOBJ("block");

	//マップチップ用のCSV読み込み
	//(map, "Resource/scv/なんたら.csv")で追加可能
	Mapchip::CsvToVector(map, "Resources/csv/tutorial.csv");//mapNum=0

	//マップチップ用のオブジェクトの初期化
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			objBlock[y][x] = Object3d::Create();
			objBlock[y][x]->SetModel(block);
			objBlock[y][x]->SetScale({ 1.0f,1.0f,1.0f });
			objBlock[y][x]->SetPosition({ 0.0f,1000.0f,0.0f });
		}
	}

	level = 1;
	enemySpawn = 1;
	gravity = 0.125f;

	for (int i = 0; i < EnemySpawnMax; i++)
	{
		enemy[i] = Object3d::Create();
		Rope[i] = Object3d::Create();
	}
	player = Object3d::Create();

	//エフェクト
	shock = Object3d::Create();
	shockWaveModel = Model::LoadFromOBJ("shock");
	shock->SetModel(shockWaveModel);
	shock->SetRotation({ 270.0f, 0.0f, 0.0f });

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
		enemy[i]->SetModel(model);
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
	player->SetModel(modelPlayer);
	player->SetScale({ 3, 3, 3 });
	p_pos = { 0, 10, 0 };
	old_p_pos = { 0, 0, 0 };
	p_x_radius = 0.4f * player->GetScale().x;
	p_y_radius = 0.8f * player->GetScale().y;
	is_jump = false;
	p_add = 0;
	p_down = 0;
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


	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });
	spriteBG->SetSize({ WinApp::window_width * 1.2, WinApp::window_height * 1.2 });
	titleText = Sprite::Create(2, { 300.0f,150.0f });
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	// ゲームシーンの毎フレーム処理
	Input* input = Input::GetInstance();

	//実験用エフェクト置き場
	if (input->PushKey(DIK_M))  //衝撃波開始
	{
		shockFlag = true;
		Effect::DeletLocus(locus, camera, p_pos);
	}


	Effect::DeletLocus(locus, camera, p_pos);

	Effect::ShockWaveUpdate(shock, p_pos, &shockFlag);

	MapCreate(0);
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			objBlock[y][x]->Update();
		}
	}


	//プレイヤー処理
	{
		//座標更新
		p_pos = player->GetPosition();
		old_p_pos = p_pos;

		//移動
		if (is_damage == false)
		{
			if (input->LeftStickAngle().x)
			{
				p_pos.x += input->LeftStickAngle().x / 2 + 0.08 * (level - 1);

				//進行方向に向きを変える
				if (input->LeftStickAngle().x >= 0)
				{
					player->SetRotation(XMFLOAT3(0, 90, 0));
				}
				else
				{
					player->SetRotation(XMFLOAT3(0, 270, 0));
				}
			}
			//キーボード用
			if (input->PushKey(DIK_D))
			{
				p_pos.x += 0.5f + 0.08 * (level - 1);
				player->SetRotation(XMFLOAT3(0, 90, 0));
			}
			if (input->PushKey(DIK_A))
			{
				p_pos.x -= 0.5f + 0.08 * (level - 1);
				player->SetRotation(XMFLOAT3(0, 270, 0));
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

			if (damage_time > 20)
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
			invincible_time++;

			if (invincible_time > 60)
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
			}
			//敵の処理
			else
			{
				//座標取得
				enemy_data[i].e_pos = enemy[i]->GetPosition();
				enemy_data[i].old_e_pos = enemy_data[i].e_pos;

				//プレイヤーとエネミーが接触したら
				if (is_damage == false && is_invincible == false)
				{
					if (CollisionObject(player, enemy[i]) == true && enemy_data[i].can_catch == true)
					{
						enemy_data[i].is_catch = true;
						enemy_data[i].is_bounce = false;
					}
					else if (CollisionObject(player, enemy[i]) == true && enemy_data[i].can_catch == false)
					{
						is_damage = true;
						//左
						if (p_pos.x < enemy_data[i].e_pos.x)
						{
							damage_move = -0.4f;
						}
						//右
						else
						{
							damage_move = 0.4f;
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
							if (MapCollide(enemy_data[i].e_pos, enemy_data[i].e_x_radius, enemy_data[i].e_y_radius, 0, enemy_data[i].old_e_pos))
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
							Effect::CreateLocus(locus, *locusModel, enemy_data[i].e_pos);
						}
						//左向きなら
						else if (player->GetRotation().y == 270)
						{
							CircularMotion(enemy_data[i].e_pos, p_pos, GetObjectLength(p_pos, enemy_data[i].e_pos), enemy_data[i].angle, 15);
							Effect::CreateLocus(locus, *locusModel, enemy_data[i].e_pos);
						}
						//マップの当たり判定
						if (MapCollide(enemy_data[i].e_pos, enemy_data[i].e_x_radius, enemy_data[i].e_y_radius, 0, enemy_data[i].old_e_pos))
						{
							//エフェクト
							shockFlag = true;
							shock->SetPosition(enemy_data[i].e_pos);


							is_attack = false;
							is_shake = true;
							tenScore = (int)GetObjectLength(p_pos, enemy_data[i].e_pos);
							score += tenScore;
							enemy_data[i].is_alive = false;
							enemy_data[i].is_catch = false;
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
										enemy_data[j].can_catch = true;
										enemy_data[j].is_turn = true;
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
						if (MapCollide(enemy_data[i].e_pos, enemy_data[i].e_x_radius, enemy_data[i].e_y_radius, 0, enemy_data[i].old_e_pos))
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
					else if (enemy_data[i].enemy_type == JUMP && enemy_data[i].is_bounce == true)
					{
						if (MapCollide(enemy_data[i].e_pos, enemy_data[i].e_x_radius, enemy_data[i].e_y_radius, 0, enemy_data[i].old_e_pos, true))
						{
							enemy_data[i].e_down = 0;
							enemy_data[i].is_grand = true;
							enemy_data[i].e_acc = 2.25f;
						}
					}
				}
				//ロープの更新
				if (enemy_data[i].is_catch == true)
				{
					RopeMove(enemy_data[i].e_pos, i);
				}
			}
		}
	}

	if (input->TriggerKey(DIK_RETURN) || input->TriggerButton(Start))
	{
		//シーン切り替え
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

	DebugText::GetInstance()->Print(50, 20, 3, "fafa");

	//エネミー更新
	for (int i = 0; i < enemySpawn; i++)
	{
		enemy[i]->SetPosition(enemy_data[i].e_pos);
		//落下の最大値を超えたら
		if (enemy_data[i].e_pos.y < -500.0f)
		{
			enemy[i]->SetPosition({ 20, 10, 0 });
			enemy_data[i].e_down = 0;
			enemy_data[i].e_acc = 0;
			enemy_data[i].is_bounce = false;
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
		shake_x = static_cast<float>(rand() % 16) / 10;
		shake_y = static_cast<float>(rand() % 16) / 10;
		shake_time++;
		if (shake_time > 20)
		{
			shake_x = 0.0f;
			shake_y = 0.0f;
			shake_time = 0;
			is_shake = false;
		}
	}
	camera->SetTarget({ p_pos.x + shake_x, p_pos.y + shake_y, p_pos.z });
	camera->SetEye({ p_pos.x + shake_x, p_pos.y + shake_y, p_pos.z - 55.0f - (5 * level) });
	camera->Update();


	//エフェクト
	for (int i = 0; i < locus.size(); i++)
	{
		locus[i]->Update();
	}
	shock->Update();


	//プレイヤーの座標（X：Y)
	/*DebugText::GetInstance()->Print(50, 35 * 3, 2, "player_x:%f", p_pos.x);
	DebugText::GetInstance()->Print(50, 35 * 4, 2, "player_y:%f", p_pos.y);
	DebugText::GetInstance()->Print(50, 35 * 5, 2, "rope_len:%f", max_rope);
	DebugText::GetInstance()->Print(50, 35 * 6, 2, "enemySpawn:%d", enemySpawn);
	DebugText::GetInstance()->Print(50, 35 * 7, 2, "min:%f~max:%f", p_pos.x - 123 / 2, p_pos.x + 123 / 2);
	DebugText::GetInstance()->Print(50, 35 * 8, 2, "min:%f~max:%f", p_pos.y - 70 / 2, p_pos.y + 70 / 2);*/
}

void TitleScene::Draw()
{
	// ゲームシーンの描画

// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCmdList();

	// 背景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// 背景スプライト描画
	spriteBG->Draw();
	titleText->Draw();

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	DirectXCommon::GetInstance()->ClearDepthBuffer();

	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(cmdList);

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

	if (shockFlag == true)
	{
		shock->Draw();
	}

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	// デバッグテキストの描画
	//DebugText::GetInstance()->DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
}

void TitleScene::MapCreate(int mapNumber)
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

void TitleScene::SpawnEnemy(int mapNumber, int enemyNumber)
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
		enemy[enemyNumber + 1]->SetPosition({ (spawnX + 1) * LAND_SCALE,  -spawnY * LAND_SCALE, 0 });//位置をセット
		enemy[enemyNumber + 1]->SetRotation({ 0, 0, 0 });
		enemy_data[enemyNumber + 1].e_pos = { (spawnX + 1) * LAND_SCALE,  -spawnY * LAND_SCALE, 0 };
		enemy_data[enemyNumber + 1].is_alive = true;//スポーン
		enemy_data[enemyNumber + 1].enemy_type = NORMAL;
		int num = rand() % 101;
		if (level * 10 > num)
		{
			enemy_data[enemyNumber].enemy_type = rand() % JUMP + 1;
			enemy_data[enemyNumber + 1].enemy_type = rand() % JUMP + 1;
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
			enemy_data[enemyNumber + 1].enemy_type = NORMAL;
		}
		for (int i = 0; i < 2; i++)
		{
			if (enemy_data[enemyNumber + i].enemy_type == TWICE)
			{
				enemy_data[enemyNumber + i].can_catch = false;
				enemy[enemyNumber + i]->SetModel(enemy_model_2);
				enemy[enemyNumber + i]->SetRotation({ 0, 0, 0 });
			}
			else
			{
				enemy_data[enemyNumber + i].can_catch = true;
				enemy[enemyNumber + i]->SetModel(model);
				enemy[enemyNumber + i]->SetRotation({ 0, 0, 0 });
			}
			enemy_data[enemyNumber + i].is_turn = false;
		}
	}
}

void TitleScene::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 center_pos, const float r, int& angle, const int add)
{
	angle += add;

	pos.x = (cosf(3.14 / 180.0f * angle) * r) + center_pos.x;//円運動の処理
	pos.y = (sinf(3.14 / 180.0f * angle) * r) + center_pos.y;//円運動の処理
}

bool TitleScene::MapCollide(XMFLOAT3& pos, float radiusX, float radiusY, int mapNumber, const XMFLOAT3 old_pos, bool is_jump)
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

void TitleScene::RopeMove(XMFLOAT3& pos, const int num)
{
	Rope[num]->SetPosition({ (p_pos.x + enemy_data[num].e_pos.x) / 2, (p_pos.y + enemy_data[num].e_pos.y) / 2, 0 });
	Rope[num]->Update();

	//ロープの位置を取得
	XMFLOAT3 r_pos = Rope[num]->GetPosition();

	//プレイヤーとエネミーの距離
	XMFLOAT2 length = { p_pos.x - enemy_data[num].e_pos.x, p_pos.y - enemy_data[num].e_pos.y };
	float len = GetObjectLength(p_pos, enemy_data[num].e_pos);
	//最大値より大きいなら
	if (len > max_rope)
	{
		float wq = len / max_rope;
		len = max_rope;
		enemy_data[num].e_pos = { p_pos.x - length.x / wq, p_pos.y - length.y / wq, 0 };
		pos = enemy_data[num].e_pos;
		MapCollide(pos, enemy_data[num].e_x_radius, enemy_data[num].e_y_radius, 0, enemy_data[num].e_pos);
	}

	//ロープの長さ
	float angleX = rope_angle->PosForAngle(p_pos.x, r_pos.y, r_pos.x, p_pos.y);
	Rope[num]->SetPosition({ (p_pos.x + enemy_data[num].e_pos.x) / 2, (p_pos.y + enemy_data[num].e_pos.y) / 2, 0 });
	Rope[num]->SetScale({ 0.3f, len / 2, 0.3f });
	Rope[num]->SetRotation({ 0, 0 ,XMConvertToDegrees(angleX) });
}

bool TitleScene::CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b)
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

float TitleScene::GetObjectLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b)
{
	XMFLOAT3 len = { pos_a.x - pos_b.x, pos_a.y - pos_b.y, pos_a.z - pos_b.z };

	return sqrtf(len.x * len.x + len.y * len.y + len.z * len.z);
}

bool TitleScene::inFrustum(XMFLOAT3 playerPosition, XMFLOAT3 negativePoint, XMFLOAT3 positivePoint)
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
