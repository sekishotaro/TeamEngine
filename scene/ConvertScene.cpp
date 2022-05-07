#include "ConvertScene.h"
#include <safe_delete.h>

void ConvertScene::Initialize()
{
	//テクスチャのロード
	Sprite::LoadTexture(21, L"Resources/switch_In.png");
	Sprite::LoadTexture(22, L"Resources/switch_Out.png");
	//切り替え用画像
	switchIn = Sprite::Create(21, { 0.0f, 0.0f });
	switchOut = Sprite::Create(22, { 0.0f, 0.0f });
}

void ConvertScene::Finalize()
{
	safe_delete(switchIn);
	safe_delete(switchOut);
}

void ConvertScene::Update()
{
	XMFLOAT2 pos = switchIn->GetPosition();
	switchIn->SetPosition({ pos.x + 10.0f, pos.y });
	pos = switchOut->GetPosition();
	switchOut->SetPosition({ pos.x - 10.0f, pos.y });
}

void ConvertScene::Draw()
{
	switchIn->Draw();
	switchOut->Draw();
}
