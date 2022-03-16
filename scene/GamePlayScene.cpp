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
	minienemy = Sprite::Create(4, { 40.0f,20.0f });

	// オブジェクト生成
	model = Model::LoadFromOBJ("sphere");
	block = Model::LoadFromOBJ("block");
	rope = Model::LoadFromOBJ("rope");

	enemy = Object3d::Create();
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
		if (input->LeftStickAngle().x)
		{
			p_pos.x += input->LeftStickAngle().x / (1.0f / p_max_speed) * 1.0f; // * 1.0f = 何倍速か

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
		//プレイヤーの攻撃
		if ((input->TriggerKey(DIK_SPACE) || input->PushButton(Button_B)) && is_attack == false)
		{
			is_attack = true;

			//プレイヤーの向きで投げる方向を変える
			if (player->GetRotation().y == 0)
			{
				angle = 180;
			}
			else if (player->GetRotation().y == 180)
			{
				angle = 0;
			}
		}
		//プレイヤーのジャンプ
		if ((input->TriggerKey(DIK_W) || input->TriggerButton(Button_A)) && is_air == false && is_jump == false)
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

		//ミニマップ用座標変換
		miniplayer->SetPosition({ p_pos.x , -p_pos.y + 28 });

		camera->SetTarget(player->GetPosition());
		camera->SetEye({ player->GetPosition().x, player->GetPosition().y, player->GetPosition().z - 60.0f });
	}

	//エネミー処理
	{
		//通常状態
		if (is_normal && is_attack == false)
		{
			//移動
			e_pos.x += e_add;

			//端まで行ったら
			if (e_pos.x >= 50.0f || e_pos.x <= -50.0f)
			{
				e_add = -e_add;

				//向きを変える
				XMFLOAT3 e_rot = enemy->GetRotation();
				e_rot.y += 180.0f;
				if (e_rot.y >= 360)
				{
					e_rot.y = 0;
				}
				enemy->SetRotation(e_rot);
			}
		}
		//プレイヤーを追尾
		else if (is_chase && is_attack == false)
		{
			//プレイヤーとエネミーの距離
			XMFLOAT2 pe_len = { p_pos.x - e_pos.x, p_pos.y - e_pos.y };

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
				CircularMotion(e_pos, p_pos, 10, angle, -20);
				enemy->SetPosition(e_pos);
			} 
			//左向きなら
			else if (player->GetRotation().y == 180)
			{
				CircularMotion(e_pos, p_pos, 10, angle, 20);
				enemy->SetPosition(e_pos);
			}
			enemy->Update();

			//マップの当たり判定
			if (MapCollide(enemy, 0))
			{
				e_down = 0;
				is_attack = false;
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

			if (MapCollide(enemy, 0))
			{
				//初期化
				e_down = 0;
			}
		}
		enemy->Update();

		//ミニマップ用座標変換
		minienemy->SetPosition({ e_pos.x , -e_pos.y + 28 });

	}

	//現在の座標を取得
	XMFLOAT3 playerPosition = player->GetPosition();
	XMFLOAT3 ropePosition = Rope->GetPosition();
	XMFLOAT3 enemyPosition = enemy->GetPosition();

	//現在のスケールを取得
	XMFLOAT3 ropeScale = Rope->GetScale();

	//ロープの座標値
	Rope->SetPosition({ (enemyPosition.x + playerPosition.x) / 2,(enemyPosition.y + playerPosition.y) / 2,(enemyPosition.z + playerPosition.z) / 2 });
	//ロープの大きさ
	Rope->SetScale({ ropeScale.x, ropeScale.y, ropeScale.z });
	//ロープ更新
	Rope->Update();
	//プレイヤーとエネミーをつなぐ
	ropeMove();

	//落下の最大値を超えたら
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

	//プレイヤーの座標（X：Y）
	DebugText::GetInstance()->Print(50, 30 * 1, 2, "%f", objBlock[8][0]->GetPosition().x);
	DebugText::GetInstance()->Print(50, 30 * 2, 2, "%f", objBlock[8][0]->GetPosition().y);
	DebugText::GetInstance()->Print(50, 30 * 3, 2, "%f", enemy->GetPosition().x);
	DebugText::GetInstance()->Print(50, 30 * 4, 2, "%f", enemy->GetPosition().y);
	DebugText::GetInstance()->Print(50, 30 * 5, 2, "%f", e_add);
	DebugText::GetInstance()->Print(50, 30 * 6, 2, "%f", e_down);

	if (input->TriggerKey(DIK_RETURN))
	{
		//BGM止める
		//Audio::GetInstance()->SoundStop("zaza.wav");
		
		//シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//オブジェクト情報の更新
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
	enemy->Draw();
	player->Draw();
	Rope->Draw();

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
	minienemy->Draw();
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

void GamePlayScene::SpawnEnemy()
{
	
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

				if (powf(y - b, 2) < powf(h + r, 2) && (x - w <= a && a <= x + w))
				{
					XMFLOAT3 pos = object->GetPosition();
					//下
					if (y - b < 0)
					{
						pos.y = y + h + r;
						is_hit = true;
						/*if (is_jump)
						{
							this->is_jump = false;
						}*/
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
				else if (powf(x - a, 2) < powf(w + r, 2) && (y - h <= b  && b <= y + h))
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

void GamePlayScene::ropeMove()
{
	if (catched)
	{
		//ロープの位置を取得
		XMFLOAT3 ropePosition = Rope->GetPosition();
		//プレイヤーの位置を取得
		XMFLOAT3 playerPosition = player->GetPosition();
		//エネミーの位置
		XMFLOAT3 enemyPosition = enemy->GetPosition();

		//プレイヤーとエネミーの距離
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