#include "Effect.h"

void Effect::CreateLocus(std::vector<std::unique_ptr<Object3d>> &object, Model &model, XMFLOAT3 &pos)
{
	object.emplace_back(Object3d::Create());
	object.back()->SetModel(&model);
	object.back()->SetPosition(pos);
	object.back()->SetRotation({ 0.0f, 90.0f, 0.0f });
	object.back()->SetScale({ 0.5f, 0.5f, 0.5f });
}

void Effect::DeletLocus(std::vector<std::unique_ptr<Object3d>> &object, Camera *camera, XMFLOAT3& player)
{
	//長さの単位
	float IdentityLen = 720 / 2 * sqrtf(3);

	//ターゲットの横と縦の長さ
	float eyeLen = camera->GetTarget().z - camera->GetEye().z;
	float targetWidth = 1280 * eyeLen / IdentityLen;
	float targetHeight = 720 * eyeLen / IdentityLen;

	XMFLOAT3 pos = { player.x + targetWidth / 2 - 5.0f, player.y + targetHeight / 2 - 5.0f, 0 };

	for (int i = 0; i < object.size(); i++)
	{
		if (object[i]->GetPosition().x < pos.x)
		{
			float x = object[i]->GetPosition().x;
			x+= 0.5f;
			object[i]->SetPosition({ x, object[i]->GetPosition().y , object[i]->GetPosition().z });
		}

		if (object[i]->GetPosition().y < pos.y)
		{
			float y = object[i]->GetPosition().y;
			y += 0.5f;
			object[i]->SetPosition({ object[i]->GetPosition().x, y , object[i]->GetPosition().z });
		}

		if (object[i]->GetPosition().x >= pos.x && object[i]->GetPosition().y >= pos.y)
		{
			//object.clear();
			object.erase(object.begin() + i);
		}
	}
	
}
 
void Effect::ShockWaveUpdate(std::unique_ptr<Object3d>& object, XMFLOAT3& generationPoint, bool *flag)
{

	XMFLOAT3 size = object->GetScale();
	if ( *flag == true)
	{
		object->SetScale({ size.x + 2.0f, size.y, size.z + 2.0f });
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