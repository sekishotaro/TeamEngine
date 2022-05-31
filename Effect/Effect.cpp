#include "Effect.h"
#include "Vector3.h"
#include "Audio.h"
#include <safe_delete.h>

const float Effect::maxTime = 100.0f;
std::vector<float> Effect::nowTime;
std::vector<float> Effect::timeRate;
std::vector<XMFLOAT3> Effect::generationPos;
Sprite* Effect::timeLimitSprite;
std::vector<float> Effect::delayTime;
std::vector<bool> Effect::processStartFlag;

std::vector<std::unique_ptr<Object3d>> Effect::destroyEffect;
Model *Effect::destroyEffectModel;
std::vector<float> Effect::nowTimeD;
std::vector<float> Effect::timeRateD;
std::vector<float> Effect::delayTimeD;
std::vector<bool> Effect::processStartFlagD;
std::vector<XMFLOAT3> Effect::generationPosD;

void Effect::CreateLocus(std::vector<std::unique_ptr<Object3d>> &object, Model &model, XMFLOAT3 &pos)
{
	object.emplace_back(Object3d::Create());			//生成
	object.back()->SetModel(&model);					//モデルセット
	object.back()->SetPosition(pos);					//位置セット
	generationPos.emplace_back(pos);					//初期値を保存
	object.back()->SetRotation({ 0.0f, 90.0f, 0.0f });	//角度セット
	object.back()->SetScale({ 0.5f, 0.5f, 0.5f });		//サイズセット

	nowTime.emplace_back(0.0f);							//時間生成
	timeRate.emplace_back(0.0f);						//時間の割合生成
	delayTime.emplace_back(2.0f);						//遅延の生成
	processStartFlag.emplace_back(false);				//処理開始
}

void Effect::DeletLocus(std::vector<std::unique_ptr<Object3d>> &object, Camera *camera, XMFLOAT3& player)
{
	for (int i = 0; i < object.size(); i++)
	{
		if (delayTime[i] > 0)
		{
			delayTime[i] -= 0.1f;
		}
		else
		{
			processStartFlag[i] = true;
		}
	}

	//最終地点の検索
	//長さの単位
	float IdentityLen = 720 / 2 * sqrtf(3);
	//ターゲットの横と縦の長さ
	float eyeLen = camera->GetTarget().z - camera->GetEye().z;
	float targetWidth = 1280 * eyeLen / IdentityLen;
	float targetHeight = 720 * eyeLen / IdentityLen;
	XMFLOAT3 endPos = { player.x + targetWidth / 2 - 5.0f, player.y + targetHeight / 2 - 5.0f, 0 };

	//時間管理

	for (int i = 0; i < object.size(); i++)
	{
		if (processStartFlag[i] == true)
		{
			nowTime[i] += 2.0f;

			timeRate[i] = min(nowTime[i] / maxTime, 1.0f);

			XMFLOAT3 rot = object[i]->GetRotation();
			rot.x += 1.0f;
			object[i]->SetRotation(rot);

			XMFLOAT3 position = lerp(generationPos[i], endPos, timeRate[i]);

			object[i]->SetPosition(position);

			if (object[i]->GetPosition().x >= endPos.x && object[i]->GetPosition().y >= endPos.y)
			{
				object.erase(object.begin() + i);				//対象の削除
				generationPos.erase(generationPos.begin() + i);	//対象の初期値の削除
				nowTime.erase(nowTime.begin() + i);				//対象の時間情報の削除
				timeRate.erase(timeRate.begin() + i);			//対象の時間割合情報の削除
				delayTime.erase(delayTime.begin() + i);
				processStartFlag.erase(processStartFlag.begin() + i);
			}
		}
	}
}
 
void Effect::ShockWaveUpdate(std::unique_ptr<Object3d>& object, Camera *camera, float &radius, bool *flag)
{
	XMFLOAT3 size = object->GetScale();


	//長さの単位
	float IdentityLen = 720 / 2 * sqrtf(3);

	//ターゲットの横と縦の長さ
	float eyeLen = camera->GetTarget().z - camera->GetEye().z;
	float targetWidth = radius * eyeLen / IdentityLen;

	if (size.x > targetWidth / 2)
	{
		*flag = false;
		radius = 0;
	}

	if ( *flag == true)
	{
		object->SetScale({ size.x + 2.5f, size.y, size.z + 2.5f });
	}
	else if ( *flag == false)
	{
		object->SetScale({ 0.0f, 0.0f, 0.0f });
	}
}

void Effect::Initialize()
{
	Sprite::LoadTexture(19, L"Resources/timeLimit.png");
	timeLimitSprite = Sprite::Create(19, {0.0f, 0.0f});

	destroyEffectModel = Model::LoadFromOBJ("locus");
}

void Effect::Finalize()
{
	safe_delete(timeLimitSprite);
	safe_delete(destroyEffectModel);
}


void Effect::TimeLimitEffectDraw(float& time)
{
	if (time <= 10.0)
	{
		Audio::GetInstance()->PlayWave("BGM/alarm.wav", 0.2, false);
		if ((int)time % 2 == 0)
		{
			//timeLimitSprite->Draw();
		}
		else
		{
			timeLimitSprite->Draw();
		}
	}
}

void Effect::DestroyEffectCreate(int enemyNum, const XMFLOAT3& pos)
{
	for (int i = 0; i < enemyNum; i++)
	{
		destroyEffect.emplace_back(Object3d::Create());				//生成
		nowTimeD.emplace_back(0.0f);
		destroyEffect.back()->SetModel(destroyEffectModel);		//モデルセット
		float nx = (rand() % 7);
		float ny = (rand() % 7);
		XMFLOAT3 position = { pos.x - 3 + nx, pos.y - 3 + ny, pos.z };
		destroyEffect.back()->SetPosition(position);						//位置セット
		generationPosD.emplace_back(position);
		destroyEffect.back()->SetRotation({ 0.0f, 90.0f, 0.0f });	//角度セット
		destroyEffect.back()->SetScale({ 0.5f, 0.5f, 0.5f });		//サイズセット

		nowTimeD.emplace_back(0.0f);							//時間生成
		timeRateD.emplace_back(0.0f);						//時間の割合生成
		delayTimeD.emplace_back(2.0f);						//遅延の生成
		processStartFlagD.emplace_back(false);				//処理開始
	}
}

void Effect::DestroyEffectUpdate(Camera* camera, XMFLOAT3& player)
{
	for (int i = 0; i < destroyEffect.size(); i++)
	{
		if (delayTimeD[i] > 0)
		{
			delayTimeD[i] -= 0.1f;
		}
		else
		{
			processStartFlagD[i] = true;
		}
	}

	//最終地点の検索
	//長さの単位
	float IdentityLen = 720 / 2 * sqrtf(3);
	//ターゲットの横と縦の長さ
	float eyeLen = camera->GetTarget().z - camera->GetEye().z;
	float targetWidth = 1280 * eyeLen / IdentityLen;
	float targetHeight = 720 * eyeLen / IdentityLen;
	XMFLOAT3 endPos = { player.x + targetWidth / 2 - 5.0f, player.y + targetHeight / 2 - 5.0f, 0 };

	
	for (int i = 0; i < destroyEffect.size(); i++)
	{
		destroyEffect[i]->Update();

		if (processStartFlagD[i] == true)
		{
			nowTimeD[i] += 2.0f;

			XMFLOAT3 rot = destroyEffect[i]->GetRotation();
			rot.x += 5.0f;
			destroyEffect[i]->SetRotation(rot);

			timeRateD[i] = min(nowTimeD[i] / maxTime, 1.0f);

			XMFLOAT3 position = lerp(generationPosD[i], endPos, timeRateD[i]);

			destroyEffect[i]->SetPosition(position);
			if (destroyEffect[i]->GetPosition().x >= endPos.x && destroyEffect[i]->GetPosition().y >= endPos.y)
			{
				destroyEffect.erase(destroyEffect.begin() + i);				//対象の削除
				generationPosD.erase(generationPosD.begin() + i);	//対象の初期値の削除
				nowTimeD.erase(nowTimeD.begin() + i);				//対象の時間情報の削除
				timeRateD.erase(timeRateD.begin() + i);			//対象の時間割合情報の削除
				delayTimeD.erase(delayTimeD.begin() + i);
				processStartFlagD.erase(processStartFlagD.begin() + i);
			}
		}
	}
}


void Effect::DestroyEffectDraw()
{
	for (int i = 0; i < destroyEffect.size(); i++)
	{
		destroyEffect[i]->Draw();
	}
}

void Effect::flashingEffectDraw(const std::unique_ptr<Object3d>& object, float flashingTime)
{	
	if ( (int)flashingTime % 2 == 0)
	{
		object->Draw();
	}
	else
	{
		//object->Draw();
	}
}
