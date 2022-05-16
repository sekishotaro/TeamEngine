#include "Effect.h"
#include "Vector3.h"

const float Effect::maxTime = 100.0f;
std::vector<float> Effect::nowTime;
std::vector<float> Effect::timeRate;
std::vector<XMFLOAT3> Effect::generationPos;

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
}

void Effect::DeletLocus(std::vector<std::unique_ptr<Object3d>> &object, Camera *camera, XMFLOAT3& player)
{
	//最終地点の検索
	//長さの単位
	float IdentityLen = 720 / 2 * sqrtf(3);
	//ターゲットの横と縦の長さ
	float eyeLen = camera->GetTarget().z - camera->GetEye().z;
	float targetWidth = 1280 * eyeLen / IdentityLen;
	float targetHeight = 720 * eyeLen / IdentityLen;
	XMFLOAT3 endPos = { player.x + targetWidth / 2 - 5.0f, player.y + targetHeight / 2 - 5.0f, 0 };

	//時間管理

	if (object.size() != nowTime.size())
	{
		//assert(0);
	}

	for (int i = 0; i < object.size(); i++)
	{
		nowTime[i] += 2.0f;

		timeRate[i] = min(nowTime[i] / maxTime, 1.0f);
	}




	for (int i = 0; i < object.size(); i++)
	{
		XMFLOAT3 position = lerp(generationPos[i], endPos, timeRate[i]);

		object[i]->SetPosition(position);

		if (object[i]->GetPosition().x >= endPos.x && object[i]->GetPosition().y >= endPos.y)
		{
		
			object.erase(object.begin() + i);				//対象の削除
			generationPos.erase(generationPos.begin() + i);	//対象の初期値の削除
			nowTime.erase(nowTime.begin() + i);				//対象の時間情報の削除
			timeRate.erase(timeRate.begin() + i);			//対象の時間割合情報の削除
		}
	}
	
}
 
void Effect::ShockWaveUpdate(std::unique_ptr<Object3d>& object, XMFLOAT3& generationPoint, bool *flag)
{

	XMFLOAT3 size = object->GetScale();
	if ( *flag == true)
	{
		object->SetScale({ size.x + 5.0f, size.y, size.z + 5.0f });
	}
	else if ( *flag == false)
	{
		object->SetScale({ 1.0f, 1.0f, 1.0f });
	}

	if (size.x >= 150.0f)
	{
		*flag = false;
	}
}
