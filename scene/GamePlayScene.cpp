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

	enemy = Object3d::Create();

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

	//オブジェクトにモデルをひも付ける
	enemy->SetModel(model);

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
	}

	//プレイヤー処理
	{
		//下降度の初期化
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
		if ((input->PushKey(DIK_W) || input->TriggerButton(Button_A)) && is_jump == false)
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

			//地面に当たったら
			if (p_pos.y < 0)
			{
				p_pos.y = 0;
				p_add = 0;
				is_jump = false;
			}
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
		//プレイヤーの落下
		if (is_jump == false && !MapCollide(player, 0))
		{
			//下降度をマイナス
			p_down += gravity;
			p_pos.y -= p_down;
		}
		//ブロックに当たったら
		if (MapCollide(player, 0))
		{
			//押し戻し
			p_pos.y = 0;
			p_down = 0;
		}

		player->SetPosition(p_pos);
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

		enemy->SetPosition(e_pos);
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

	// デバッグテキストの描画
	DebugText::GetInstance()->DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
}

void GamePlayScene::MapCreate(int mapNumber)
{
	//マップチップ1つの大きさ(playerが5なので5の倍数で指定すること)
	const float LAND_SCALE = 5.0f;
	for (int y = 0; y < map_max_y; y++) {//(yが12)
		for (int x = 0; x < map_max_x; x++) {//(xが52)

			if (Mapchip::GetChipNum(x, y, map[mapNumber]) == Ground)
			{
				//位置と大きさの変更(今は大きさは変更しないで)
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
	pos.x = (cosf(3.14 / 180.0f * angle) * r) + center_pos.x;//円運動の処理
	pos.y = (sinf(3.14 / 180.0f * angle) * r) + center_pos.y;//円運動の処理

	angle += add;
}

bool GamePlayScene::MapCollide(const std::unique_ptr<Object3d>& object, int mapNumber)
{
	for (int y = 0; y < map_max_y; y++) //yが12
	{
		for (int x = 0; x < map_max_x; x++) //xが52
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
//半径
				float block_r = 2.5f * objBlock[y][x]->GetScale().x;
				float obj_r = 1.0f * object->GetScale().x;
				//XとYの長さ
				float len_x = objBlock[y][x]->GetPosition().x - object->GetPosition().x;
				float len_y = objBlock[y][x]->GetPosition().y - object->GetPosition().y;
				//それぞれが半径以下である
				if (block_r + obj_r >= sqrtf(len_x * len_x) && block_r + obj_r >= sqrtf(len_y * len_y))
				{
					return true;
				}
				//四角の隅から円の中心までのベクトル
				XMFLOAT2 vec = {0, 0};
				//長さ
				float len_r = 0;
				//四つの隅を調べる
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
					//長さを求める
					len_r = sqrtf(vec.x * vec.x + vec.y * vec.y);
					//円の半径よりも長さが短い
					if (len_r <= obj_r)
					{
						return true;
					}
				}
*/