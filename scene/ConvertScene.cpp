#include "ConvertScene.h"
#include <safe_delete.h>

Sprite* ConvertScene::switchIn;
Sprite* ConvertScene::switchOut;

void ConvertScene::InitializeOut()
{
	//テクスチャのロード
	Sprite::LoadTexture(21, L"Resources/switch_In.png");
	Sprite::LoadTexture(22, L"Resources/switch_Out.png");
	//切り替え用画像
	switchIn = Sprite::Create(21, { 0.0f, 0.0f });
	switchOut = Sprite::Create(22, { 0.0f, 0.0f });
}

void ConvertScene::InitializeIn()
{
	//テクスチャのロード
	Sprite::LoadTexture(21, L"Resources/switch_In.png");
	Sprite::LoadTexture(22, L"Resources/switch_Out.png");
	//切り替え用画像
	switchIn = Sprite::Create(21, { -1280.0f, 0.0f });
	switchOut = Sprite::Create(22, { 1280.0f, 0.0f });
}

void ConvertScene::Finalize()
{
	safe_delete(switchIn);
	safe_delete(switchOut);
}

void ConvertScene::besideOut()
{
	XMFLOAT2 pos = switchIn->GetPosition();
	if (pos.x <= 1280)
	{
		switchIn->SetPosition({ pos.x + 10.0f, pos.y });
	}
	pos = switchOut->GetPosition();
	if (pos.x >= -1280)
	{
		switchOut->SetPosition({ pos.x - 10.0f, pos.y });
	}
}

void ConvertScene::besideIn()
{
	XMFLOAT2 pos = switchIn->GetPosition();
	if (pos.x < 0)
	{
		switchIn->SetPosition({ pos.x + 10.0f, pos.y });
	}
	pos = switchOut->GetPosition();
	if (pos.x > 0)
	{
		switchOut->SetPosition({ pos.x - 10.0f, pos.y });
	}
}

void ConvertScene::Draw()
{
	switchIn->Draw();
	switchOut->Draw();
}
