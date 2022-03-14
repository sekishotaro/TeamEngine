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

	// カメラ生成
	camera = new Camera(WinApp::window_width, WinApp::window_height);

	// カメラセット
	Object3d::SetCamera(camera);

	// テクスチャ読み込み

	Sprite::LoadTexture(1, L"Resources/background.png");
	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });

	// オブジェクト生成
	model = Model::LoadFromOBJ("sphere");
	block = Model::LoadFromOBJ("block");

	mini_enemy = Object3d::Create();
	enemy = Object3d::Create();

	mini_player = Object3d::Create();
	player = Object3d::Create();

	//マップチップ用のCSV読み込み
	//(map, "Resource/scv/なんたら.csv")で追加可能
	Mapchip::CsvToVector(map, "Resources/csv/demo.csv");//mapNum=0


	//マップチップ用のオブジェクトの初期化
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

	//オブジェクトにモデルをひも付ける
	mini_enemy->SetModel(model);
	mini_enemy->SetScale({ 0.2f,0.2f,0.2f });
	enemy->SetModel(model);

	mini_player->SetModel(model);
	mini_player->SetScale({ 0.2f,0.2f,0.2f });
	player->SetModel(model);
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
		MiniMapCreate(0);
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
		//プレイヤーのジャンプ
		if ((input->PushKey(DIK_W) || input->TriggerButton(Button_A)) && is_jump == false && MapCollide(player, 0))
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
		}
		//プレイヤーの攻撃
		if ((input->TriggerKey(DIK_RETURN) || input->PushButton(Button_B)) && is_attack == false)
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
		//座標をセット
		player->SetPosition(p_pos);
		//ブロックに当たったら
		if (MapCollide(player, 0))
		{
			//初期化
			is_jump = false;
			p_add = 0;
			p_down = 0;
		}
		//プレイヤーの自由落下
		if (is_jump == false && !MapCollide(player, 0))
		{
			p_pos = player->GetPosition();
			//下降度をマイナス
			p_down -= gravity;
			p_pos.y += p_down;
			//座標をセット
			player->SetPosition(p_pos);
		}

		//ミニマップ用座標変換
		mini_p_pos.x = (p_pos.x / 5) - 52.5f;
		mini_p_pos.y = (p_pos.y / 5) + 27.5f;
		mini_p_pos.z = p_pos.z / 5;
		mini_player->SetPosition(mini_p_pos);
		player->SetPosition(p_pos);

		camera->SetTarget(player->GetPosition());
		camera->SetEye({ player->GetPosition().x, player->GetPosition().y, player->GetPosition().z - 60.0f });
	}

	//エネミー処理
	{
		//通常状態
		if (is_normal)
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
		else if (is_chase)
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
		else if (is_attack)
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
			//マップの当たり判定
			if (MapCollide(enemy, 0))
			{
				is_attack = false;
			}
		}

		//ミニマップ用座標変換
		mini_e_pos.x = (e_pos.x / 5) - 52.5f;
		mini_e_pos.y = (e_pos.y / 5) + 27.5f;
		mini_e_pos.z = e_pos.z / 5;
		enemy->SetPosition(e_pos);
		mini_enemy->SetPosition(mini_e_pos);
	}

	//プレイヤーの座標（X：Y）
	DebugText::GetInstance()->Print(50, 30 * 1, 2, "%f", objBlock[8][0]->GetPosition().x);
	DebugText::GetInstance()->Print(50, 30 * 2, 2, "%f", objBlock[8][0]->GetPosition().y);
	DebugText::GetInstance()->Print(50, 30 * 3, 2, "%f", player->GetPosition().x);
	DebugText::GetInstance()->Print(50, 30 * 4, 2, "%f", player->GetPosition().y);

	if (input->TriggerKey(DIK_SPACE))
	{
		//BGM止める
		//Audio::GetInstance()->SoundStop("zaza.wav");
		
		//シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//オブジェクト情報の更新
	camera->Update();
	enemy->Update();
	mini_enemy->Update();
	player->Update();
	mini_player->Update();
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
	mini_enemy->Draw();
	player->Draw();
	mini_player->Draw();

	//マップチップの描画
	for (int y = 0; y < map_max_y; y++)
	{
		for (int x = 0; x < map_max_x; x++)
		{
			objBlock[y][x]->Draw();
			objMiniBlock[y][x]->Draw();
		}
	}
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

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
	pos.x = (cosf(3.14 / 180.0f * angle) * r) + center_pos.x;//円運動の処理
	pos.y = (sinf(3.14 / 180.0f * angle) * r) + center_pos.y;//円運動の処理

	angle += add;
}

bool GamePlayScene::MapCollide(const std::unique_ptr<Object3d>& object, int mapNumber)
{
	float a = object->GetPosition().x;
	float b = object->GetPosition().y;
	float r = 1.0f * object->GetScale().x;
	float x = 0;
	float y = 0;
	float x_w = 0;
	float y_h = 0;

	bool is_hit = false;

	for (int b_y = 0; b_y < map_max_y; b_y++) //yが12
	{
		for (int b_x = 0; b_x < map_max_x; b_x++) //xが52
		{
			if (Mapchip::GetChipNum(b_x, b_y, map[mapNumber]) == Ground)
			{
				x = objBlock[b_y][b_x]->GetPosition().x - 2.5f * objBlock[b_y][b_x]->GetScale().x;
				y = objBlock[b_y][b_x]->GetPosition().y - 2.5f * objBlock[b_y][b_x]->GetScale().x;
				x_w = objBlock[b_y][b_x]->GetPosition().x + 2.5f * objBlock[b_y][b_x]->GetScale().x;
				y_h = objBlock[b_y][b_x]->GetPosition().y + 2.5f * objBlock[b_y][b_x]->GetScale().x;

				//縦
				if ((x <= a && a <= x_w) && (y - r <= b && b <= y_h + r))
				{
					is_hit = true;
					if (b_y != map_max_y && Mapchip::GetChipNum(b_x, b_y - 1, map[mapNumber]) == None)
					{
						if (Mapchip::GetChipNum(b_x, b_y, map[mapNumber]) == Ground)
						{
							XMFLOAT3 pos = object->GetPosition();
							pos.y = y_h + r + gravity;
							object->SetPosition(pos);
							object->Update();
						}
					}
				}
				//横
				if ((x - r <= a && a <= x_w + r) && (y <= b && b <= y_h))
				{
					is_hit = true;
					if (x > a)
					{
						XMFLOAT3 pos = object->GetPosition();
						pos.x = x - r;
						object->SetPosition(pos);
						object->Update();
					}
					else
					{
						XMFLOAT3 pos = object->GetPosition();
						pos.x = x_w + r;
						object->SetPosition(pos);
						object->Update();
					}
				}
				XMFLOAT3 pos_s = object->GetPosition();
				//四隅
				if (powf(x - a, 2) + powf(y - b, 2) <= powf(r, 2))
				{
					is_hit = true;
					/*if (y > b)
					{
						XMFLOAT3 pos = object->GetPosition();
						pos.y = y - r;
						object->SetPosition(pos);
						object->Update();
					} 
					else
					{
						XMFLOAT3 pos = object->GetPosition();
						pos.y = y_h + r;
						object->SetPosition(pos);
						object->Update();
					}*/
				}
				if (powf(x_w - a, 2) + powf(y - b, 2) <= powf(r, 2))
				{
					is_hit = true;
					/*if (y > b)
					{
						XMFLOAT3 pos = object->GetPosition();
						pos.y = y - r;
						object->SetPosition(pos);
						object->Update();
					}
					else
					{
						XMFLOAT3 pos = object->GetPosition();
						pos.y = y_h + r;
						object->SetPosition(pos);
						object->Update();
					}*/
				}
				if (powf(x - a, 2) + powf(y_h - b, 2) <= powf(r, 2))
				{
					is_hit = true;
					/*if (y > b)
					{
						XMFLOAT3 pos = object->GetPosition();
						pos.y = y - r;
						object->SetPosition(pos);
						object->Update();
					} 
					else
					{
						XMFLOAT3 pos = object->GetPosition();
						pos.y = y_h + r;
						object->SetPosition(pos);
						object->Update();
					}*/
				}
				if (powf(x_w - a, 2) + powf(y_h - b, 2) <= powf(r, 2))
				{
					is_hit = true;
					/*if (y > b)
					{
						XMFLOAT3 pos = object->GetPosition();
						pos.y = y - r;
						object->SetPosition(pos);
						object->Update();
					} 
					else
					{
						XMFLOAT3 pos = object->GetPosition();
						pos.y = y_h + r;
						object->SetPosition(pos);
						object->Update();
					}*/
				}
			}
		}
	}
}

void GamePlayScene::MiniMapCreate(int mapNumber)
{
	//マップチップ1つの大きさ(playerが5なので5の倍数で指定すること)
	const float LAND_SCALE = 1.0f;
	for (int y = 0; y < map_max_y; y++) {//(yが12)
		for (int x = 0; x < map_max_x; x++) {//(xが52)

			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == Ground)
			{
				//位置と大きさの変更
				objMiniBlock[y][x]->SetScale({ LAND_SCALE / 5, LAND_SCALE / 5, LAND_SCALE / 5 });
				objMiniBlock[y][x]->SetPosition({ x * LAND_SCALE - 60,  y * -LAND_SCALE + 35, 0 });
			}
		}
	}
}