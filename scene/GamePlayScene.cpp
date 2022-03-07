#include "GamePlayScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"
#include "DirectXCommon.h"

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

	enemy = Object3d::Create();

	player = Object3d::Create();

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

	//カメラ移動
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A))
	{
		// 現在の座標を取得
		XMFLOAT3 position = camera->GetEye();

		// 移動後の座標を計算
		if (input->PushKey(DIK_W)) { position.y += 1.0f; }
		else if (input->PushKey(DIK_S)) { position.y -= 1.0f; }
		if (input->PushKey(DIK_D)) { position.x += 1.0f; }
		else if (input->PushKey(DIK_A)) { position.x -= 1.0f; }

		// 座標の変更を反映
		camera->SetEye(position);
	}

	//プレイヤー処理
	{
		//プレイヤーの移動
		if (input->LeftStickAngle().x/* || input->LeftStickAngle().y*/)
		{
			p_pos.x += input->LeftStickAngle().x / (1 / p_max_speed) * 1;
			/*if (input->LeftStickAngle().y > 0)
			{
				p_pos.y += input->LeftStickAngle().y / 2;
			}*/
		}
		//ジャンプフラグ
		if (input->TriggerButton(Button_A) && is_jump == false)
		{
			is_jump = true;
			p_add = 4.0f;
			p_gravity = 0.3f;
		}
		//ジャンプ処理
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

	//エネミー処理
	{
		//プレイヤーを追尾
		if (chase == true)
		{
			//プレイヤーとエネミーの距離
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
		//BGM止める
		//Audio::GetInstance()->SoundStop("zaza.wav");
		
		//シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//アップデート
	camera->Update();
	enemy->Update();
	player->Update();
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
