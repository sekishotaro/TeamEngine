#include "EndScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"
#include "DirectXCommon.h"
#include "ConvertScene.h"

void EndScene::Initialize()
{
	// テクスチャ読み込み
	Sprite::LoadTexture(1, L"Resources/Titlebackground.png");
	Sprite::LoadTexture(21, L"Resources/switch_In.png");
	Sprite::LoadTexture(22, L"Resources/switch_Out.png");
	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });

	ConvertScene::InitializeOut();
}

void EndScene::Finalize()
{
}

void EndScene::Update()
{
	// ゲームシーンの毎フレーム処理

	Input* input = Input::GetInstance();

	ConvertScene::besideOut(ConvertFlag);

	if (input->TriggerKey(DIK_RETURN) || input->TriggerButton(Start))
	{
		//シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	DebugText::GetInstance()->Print(50, 20, 3, "fafafafafafafafafafafafafafaf");
}

void EndScene::Draw()
{
	// ゲームシーンの描画

// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCmdList();

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

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	// デバッグテキストの描画
	DebugText::GetInstance()->DrawAll(cmdList);

	ConvertScene::Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
}
