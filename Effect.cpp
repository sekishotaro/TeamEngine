#include "Effect.h"

void Effect::CreateLocus(std::vector<std::unique_ptr<Object3d>> &object, Model &model, XMFLOAT3 &pos)
{
	object.emplace_back(Object3d::Create());
	object.back()->SetModel(&model);
	object.back()->SetPosition(pos);
	object.back()->SetRotation({ 0.0f, 90.0f, 0.0f });
	object.back()->SetScale({ 0.5f, 0.5f, 0.5f });
}

void Effect::DeletLocus(std::vector<std::unique_ptr<Object3d>> &object)
{
	//for (int i = 0; i < locus.size(); i++)
	//{
	//	XMFLOAT3 pos = locus[i]->GetPosition();
	//	pos.x += 0.1f;
	//	pos.y += 0.1f;
	//	locus[i]->SetPosition({ pos.x, pos.y, pos.z });
	//}

	object.clear();
}
