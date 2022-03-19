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

	// カメラ生成
	camera = new Camera(WinApp::window_width, WinApp::window_height);

	// カメラセット
	Object3d::SetCamera(camera);

	// テクスチャ読み込み

	Sprite::LoadTexture(1, L"Resources/background.png");
	Sprite::LoadTexture(2, L"Resources/minimap.png");
	Sprite::LoadTexture(3, L"Resources/miniplayer.png");
	Sprite::LoadTexture(4, L"Resources/minienemy.png");
	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });

	//スプライト生成
	minimap = Sprite::Create(2, { 0.0f,0.0f });
	miniplayer = Sprite::Create(3, { 20.0f,20.0f });
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		minienemy[i] = Sprite::Create(4, { 40.0f,20.0f });
	}

	// オブジェクト生成
	model = Model::LoadFromOBJ("sphere");
	block = Model::LoadFromOBJ("block");
	rope = Model::LoadFromOBJ("rope");

	for (int i = 0; i < EnemySpawnMax; i++)
	{
		enemy[i] = Object3d::Create();
	}
	player = Object3d::Create();

	Rope = Object3d::Create();

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

	//オブジェクトにモデルをひも付ける
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		e_pos[i] = { 20,5,0 }; //座標
		is_normal[i] = false; //通常状態
		is_chase[i] = false; //追跡状態
		is_catch[i] = false; //捕縛状態
		is_alive[i] = false; //死亡状態
		is_alive[0] = true;//初期描画のため1匹だけ出す
		e_speed[i] = 0.25f; //移動量
		e_down[i] = 0.0f; //下降度
		enemy[i]->SetModel(model);
		enemy[i]->SetPosition(e_pos[i]);
		enemy[i]->Update();
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
	// ゲームシーンの毎フレーム処理
	Input *input = Input::GetInstance();

	//Mキーでマップチップ設置
	if (input->TriggerKey(DIK_M) || true)
	{
		MapCreate(0);
	}

	//プレイヤー処理
	{
		//座標更新
		p_pos = player->GetPosition();

		//プレイヤーの移動
		if (is_attack == false)
		{
			if (input->LeftStickAngle().x)
			{
				p_pos.x += input->LeftStickAngle().x / (1.0f / p_max_speed) * 1.0f; // * 1.0f = 何倍速か

				//進行方向に向きを変える
				if (input->LeftStickAngle().x >= 0)
				{
					player->SetRotation(XMFLOAT3(0, 0, 0));
				} else
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
		}
		//プレイヤーの攻撃
		if ((input->TriggerKey(DIK_SPACE) || input->PushButton(Button_B)) && is_attack == false && is_catch)
		{
			//プレイヤーの向きで投げる方向を変える
			if (player->GetRotation().y == 0 && p_pos.x >= enemy->GetPosition().x)
			{
				angle = static_cast<int>(XMConvertToDegrees(rope_angle->PosForAngle(p_pos.x, enemy->GetPosition().y, enemy->GetPosition().x, p_pos.y))) - 90;
				is_attack = true;
			}
			else if (player->GetRotation().y == 180 && p_pos.x < enemy->GetPosition().x)
			{
				angle = static_cast<int>(XMConvertToDegrees(rope_angle->PosForAngle(p_pos.x, enemy->GetPosition().y, enemy->GetPosition().x, p_pos.y))) - 90;
				is_attack = true;
			}
		}
		//プレイヤーのジャンプ
		if ((input->TriggerKey(DIK_W) || input->TriggerButton(Button_A)) && is_air == false && is_jump == false && is_attack == false)
		{
			is_jump = true;

			//上昇率の初期化
			p_add = 2.5f;
		}
		//ジャンプ処理
		if (is_jump)
		{
			//座標の上昇
			p_add -= gravity;
			p_pos.y += p_add;

			player->SetPosition(p_pos);
			player->Update();

			if (MapCollide(player, 0, is_jump))
			{
				//初期化
				is_jump = false;
				p_add = 0;
			}
		}
		//プレイヤーの自由落下
		if (is_jump == false)
		{
			is_air = true;
			//下降度をマイナス
			p_down -= gravity;
			p_pos.y += p_down;

			player->SetPosition(p_pos);
			player->Update();

			if (MapCollide(player, 0))
			{
				//初期化
				p_down = 0;
				is_air = false;
			}
		}
		//プレイヤーとエネミーが接触したら
		for (int i = 0; i < EnemySpawnMax; i++)
		{
			if (is_alive[i])
			{
				if (CollisionObject(player, enemy[i]))
				{
					is_catch[i] = true;
				}
			}
		}
		//ミニマップ用座標変換
		p_pos = player->GetPosition();
		miniplayer->SetPosition({ p_pos.x , -p_pos.y + 28 });

		camera->SetTarget(p_pos);
		camera->SetEye({ p_pos.x, p_pos.y, p_pos.z - 60.0f });
	}

	//敵のスポーン
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		if (is_alive[i] == false)
		{
			SpawnEnemy(0, i);
		}
	}

	//エネミー処理
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		if (is_alive[i])
		{
			e_pos[i] = enemy[i]->GetPosition();

			//通常状態
			if (is_normal && is_attack == false)
			{
				//移動
				e_pos[i].x += e_speed[i];

				//端まで行ったら
				if (e_pos[i].x >= 50.0f || e_pos[i].x <= -50.0f)
				{
					e_speed[i] = -e_speed[i];

					//向きを変える
					XMFLOAT3 e_rot[10];
					e_rot[i] = enemy[i]->GetRotation();
					e_rot[i].y += 180.0f;
					if (e_rot[i].y >= 360)
					{
						e_rot[i].y = 0;
					}
					enemy[i]->SetRotation(e_rot[i]);
				}
			}
			//プレイヤーを追尾
			else if (is_chase && is_attack == false)
			{
				//プレイヤーとエネミーの距離
				XMFLOAT2 pe_len = { p_pos.x - e_pos[i].x, p_pos.y - e_pos[i].y };

			//正の値なら
			if (pe_len.x > 0)
			{
				e_pos.x += e_add;
				if (p_pos.x < e_pos.x)
				{
					e_pos.x = p_pos.x;
				}
			}
			//負の値なら
			else if (pe_len.x < 0)
			{
				e_pos.x -= e_add;
				if (p_pos.x > e_pos.x)
				{
					e_pos.x = p_pos.x;
				}
			}
		}
		//攻撃状態
		if (is_attack)
		{
			//右向きなら
			if (player->GetRotation().y == 0)
			{
				CircularMotion(e_pos, p_pos, GetLengthObject(p_pos, e_pos), angle, -15);
				enemy->SetPosition(e_pos);
			} 
			//左向きなら
			else if (player->GetRotation().y == 180)
			{
				CircularMotion(e_pos, p_pos, GetLengthObject(p_pos, e_pos), angle, 15);
				enemy->SetPosition(e_pos);
			}
			enemy->Update();
				//正の値なら
				if (pe_len.x > 0)
				{
					e_pos[i].x += e_speed[i];
					if (p_pos.x < e_pos[i].x)
					{
						e_pos[i].x = p_pos.x;
					}
				}
				//負の値なら
				else if (pe_len.x < 0)
				{
					e_pos[i].x -= e_speed[i];
					if (p_pos.x > e_pos[i].x)
					{
						e_pos[i].x = p_pos.x;
					}
				}
			}
			//攻撃状態
			if (is_attack)
			{
				//右向きなら
				if (player->GetRotation().y == 0)
				{
					CircularMotion(e_pos[i], p_pos, 10, angle, -20);
					enemy[i]->SetPosition(e_pos[i]);
				}
				//左向きなら
				else if (player->GetRotation().y == 180)
				{
					CircularMotion(e_pos[i], p_pos, 10, angle, 20);
					enemy[i]->SetPosition(e_pos[i]);
				}
				enemy[i]->Update();

			//マップの当たり判定
			if (MapCollide(enemy, 0))
			{
				e_down = 0;
				is_attack = false;
				//is_catch = false;
			}
		}
		else
		{
			//プレイヤーの自由落下
			//下降度をマイナス
			e_pos = enemy->GetPosition();
			e_down -= gravity;
			e_pos.y += e_down;
			enemy->SetPosition(e_pos);
			enemy->Update();
				//マップの当たり判定
				if (MapCollide(enemy[i], 0))
				{
					e_down[i] = 0;
					is_attack = false;
				}
			}
			else
			{
				//プレイヤーの自由落下
				//下降度をマイナス
				e_pos[i] = enemy[i]->GetPosition();
				e_down[i] -= gravity;
				e_pos[i].y += e_down[i];
				enemy[i]->SetPosition(e_pos[i]);
				enemy[i]->Update();

			if (MapCollide(enemy, 0))
			{
				//初期化
				e_down = 0;
			}
		}
		enemy->Update();
	}

	Rope->SetPosition({ (player->GetPosition().x + enemy->GetPosition().x) / 2, (player->GetPosition().y + enemy->GetPosition().y) / 2, (player->GetPosition().z + enemy->GetPosition().z) / 2 });
				if (MapCollide(enemy[i], 0))
				{
					//初期化
					e_down[i] = 0;
				}
			}
			enemy[i]->Update();

			//ミニマップ用座標変換
			minienemy[i]->SetPosition({ e_pos[i].x , -e_pos[i].y + 28 });
		}
	}

	//ロープの更新
	RopeMove();
	//現在の座標を取得
	XMFLOAT3 playerPosition = player->GetPosition();
	XMFLOAT3 ropePosition = Rope->GetPosition();
	XMFLOAT3 enemyPosition[10];
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		enemyPosition[i] = enemy[i]->GetPosition();
	}

	Rope->SetPosition({ (player->GetPosition().x + enemy->GetPosition().x) / 2, (player->GetPosition().y + enemy->GetPosition().y) / 2, (player->GetPosition().z + enemy->GetPosition().z) / 2 });

	//ミニマップ用座標変換
	minienemy->SetPosition({ enemy->GetPosition().x, -enemy->GetPosition().y + 28 });
	//ロープの座標値
	Rope->SetPosition({ (enemyPosition[0].x + playerPosition.x) / 2,(enemyPosition[0].y + playerPosition.y) / 2,(enemyPosition[0].z + playerPosition.z) / 2 });
	//ロープの大きさ
	Rope->SetScale({ ropeScale.x, ropeScale.y, ropeScale.z });
	//ロープ更新
	Rope->Update();
	//プレイヤーとエネミーをつなぐ
	RopeMove();

	//落下の最大値を超えたら
	float limit_y = player->GetPosition().y;
	if (limit_y < -500.0f)
	{
		player->SetPosition({ 0, 10, 0 });
		p_down = 0;
	}
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		limit_y = enemy[i]->GetPosition().y;
	}
	if (limit_y < -500.0f)
	{
		for (int i = 0; i < EnemySpawnMax; i++)
		{
			enemy[i]->SetPosition({ 0, 10, 0 });
			e_down[i] = 0;
		}
	}
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		MapCollide(enemy[i], 0);
	}

	if (input->TriggerKey(DIK_RETURN))
	{
		//BGM止める
		//Audio::GetInstance()->SoundStop("zaza.wav");
		
		//シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//オブジェクト情報の更新
	camera->Update();
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		enemy[i]->Update();
	}
	player->Update();
	Rope->Update();
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			objBlock[y][x]->Update();
		}
	}

	//プレイヤーの座標（X：Y）
	DebugText::GetInstance()->Print(50, 30 * 1, 2, "%f", objBlock[8][0]->GetPosition().x);
	DebugText::GetInstance()->Print(50, 30 * 2, 2, "%f", objBlock[8][0]->GetPosition().y);
	DebugText::GetInstance()->Print(50, 30 * 3, 2, "rope_X:%f", Rope->GetPosition().x);
	DebugText::GetInstance()->Print(50, 30 * 4, 2, "rope_Y:%f", Rope->GetPosition().y);
	DebugText::GetInstance()->Print(50, 30 * 5, 2, "player_X:%f", player->GetPosition().y);
	DebugText::GetInstance()->Print(50, 30 * 6, 2, "enemy_Y:%f", enemy->GetPosition().y);
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

	// 3Dオブクジェクトの描画
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		if (is_alive[i])
		{
			enemy[i]->Draw();
		}
	}
	player->Draw();
	if (is_catch)
	{
		Rope->Draw();
	}

	//マップチップの描画
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			objBlock[y][x]->Draw();
		}
	}
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	//ミニマップの描画
	minimap->Draw();
	for (int i = 0; i < EnemySpawnMax; i++)
	{
		if (is_alive[i])
		{
			minienemy[i]->Draw();
		}
	}
	miniplayer->Draw();

	// デバッグテキストの描画
	DebugText::GetInstance()->DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
}

void GamePlayScene::MapCreate(int mapNumber)
{
	//マップチップ1つの大きさ
	const float LAND_SCALE = 5.0f;
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
	if (Mapchip::GetChipNum(spawnX, spawnY, map[mapNumber]) == None)
	{
		enemy[enemyNumber]->SetPosition({ spawnX * LAND_SCALE,  -spawnY * LAND_SCALE, 0 });//位置をセット
		is_alive[enemyNumber] = true;//スポーン
	}
}

void GamePlayScene::CircularMotion(XMFLOAT3& pos, const XMFLOAT3 center_pos, const float r, int& angle, const int add)
{
	angle += add;

	pos.x = (cosf(3.14 / 180.0f * angle) * r) + center_pos.x;//円運動の処理
	pos.y = (sinf(3.14 / 180.0f * angle) * r) + center_pos.y;//円運動の処理
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

	for (int b_y = map_max_y - 1; b_y >= 0; b_y--) //yが12
	{
		for (int b_x = map_max_x - 1; b_x >= 0; b_x--) //xが52
		{
			if (Mapchip::GetChipNum(b_x, b_y, map[mapNumber]) == Ground)
			{
				x = objBlock[b_y][b_x]->GetPosition().x;
				y = objBlock[b_y][b_x]->GetPosition().y;
				w = 2.5f * objBlock[b_y][b_x]->GetScale().x;
				h = 2.5f * objBlock[b_y][b_x]->GetScale().y;

				if (powf(y - b, 2) <= powf(h + r, 2) && (x - w <= a && a <= x + w))
				{
					XMFLOAT3 pos = object->GetPosition();
					//下
					if (y - b < 0)
					{
						pos.y = y + h + r;
						is_hit = true;
					}
					//上
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
				else if (powf(x - a, 2) <= powf(w + r, 2) && (y - h <= b  && b <= y + h))
				{
					XMFLOAT3 pos = object->GetPosition();
					//右
					if (x - a < 0)
					{
						pos.x = x + w + r;
						if (!is_jump)
						{
							is_hit = true;
						}
					}
					//左
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

void GamePlayScene::RopeMove()
{
	if (is_catch)
	{
		Rope->Update();

		//プレイヤーの位置を取得
		XMFLOAT3 playerPosition = player->GetPosition();
		//エネミーの位置
		XMFLOAT3 enemyPosition = enemy->GetPosition();
		//ロープの位置を取得
		XMFLOAT3 ropePosition = Rope->GetPosition();

		//プレイヤーとエネミーの距離
		XMFLOAT2 length = { playerPosition.x - enemyPosition.x, playerPosition.y - enemyPosition.y };
		float len = GetLengthObject(playerPosition, enemyPosition);
		//最大値より大きいなら
		if (len > max_rope)
		{
			float wq = len / max_rope;
			len = max_rope;
			enemy->SetPosition({ playerPosition.x - length.x / wq, playerPosition.y - length.y / wq, 0 });
			enemy->Update();
		}

		//ロープの長さ
		Rope->SetScale({ 0.3f, len / 2, 0.3f });

		float angleX = rope_angle->PosForAngle(playerPosition.x, ropePosition.y, ropePosition.x, playerPosition.y);

		Rope->SetRotation({ 0, 0 ,XMConvertToDegrees(angleX) });
	}
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
