#include "ConvertScene.h"
#include <safe_delete.h>

Sprite* ConvertScene::switchIn;
Sprite* ConvertScene::switchOut;

void ConvertScene::InitializeOut()
{
	//Ø‚è‘Ö‚¦—p‰æ‘œ
	switchIn = Sprite::Create(21, { 0.0f, 0.0f });
	switchOut = Sprite::Create(22, { 0.0f, 0.0f });
}

void ConvertScene::InitializeIn()
{
	//Ø‚è‘Ö‚¦—p‰æ‘œ
	switchIn = Sprite::Create(21, { -1280.0f, 0.0f });
	switchOut = Sprite::Create(22, { 1280.0f, 0.0f });
}

void ConvertScene::Finalize()
{
	safe_delete(switchIn);
	safe_delete(switchOut);
}

void ConvertScene::besideOut(bool &flag)
{
	XMFLOAT2 pos = switchIn->GetPosition();
	if (pos.x >= -1280)
	{
		switchIn->SetPosition({ pos.x - 10.0f, pos.y });
	}
	pos = switchOut->GetPosition();
	if (pos.x <= 1280)
	{
		switchOut->SetPosition({ pos.x + 10.0f, pos.y });
	}
	else
	{
		flag = true;
	}
}

void ConvertScene::besideIn(bool &flag)
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
	else
	{
		flag = true;
	}
	
}

void ConvertScene::Draw()
{
	switchIn->Draw();
	switchOut->Draw();
}
