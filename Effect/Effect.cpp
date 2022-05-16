#include "Effect.h"
#include "Vector3.h"

const float Effect::maxTime = 100.0f;
std::vector<float> Effect::nowTime;
std::vector<float> Effect::timeRate;
std::vector<XMFLOAT3> Effect::generationPos;

void Effect::CreateLocus(std::vector<std::unique_ptr<Object3d>> &object, Model &model, XMFLOAT3 &pos)
{
	object.emplace_back(Object3d::Create());			//����
	object.back()->SetModel(&model);					//���f���Z�b�g
	object.back()->SetPosition(pos);					//�ʒu�Z�b�g
	generationPos.emplace_back(pos);					//�����l��ۑ�
	object.back()->SetRotation({ 0.0f, 90.0f, 0.0f });	//�p�x�Z�b�g
	object.back()->SetScale({ 0.5f, 0.5f, 0.5f });		//�T�C�Y�Z�b�g

	nowTime.emplace_back(0.0f);							//���Ԑ���
	timeRate.emplace_back(0.0f);						//���Ԃ̊�������
}

void Effect::DeletLocus(std::vector<std::unique_ptr<Object3d>> &object, Camera *camera, XMFLOAT3& player)
{
	//�ŏI�n�_�̌���
	//�����̒P��
	float IdentityLen = 720 / 2 * sqrtf(3);
	//�^�[�Q�b�g�̉��Əc�̒���
	float eyeLen = camera->GetTarget().z - camera->GetEye().z;
	float targetWidth = 1280 * eyeLen / IdentityLen;
	float targetHeight = 720 * eyeLen / IdentityLen;
	XMFLOAT3 endPos = { player.x + targetWidth / 2 - 5.0f, player.y + targetHeight / 2 - 5.0f, 0 };

	//���ԊǗ�

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
		
			object.erase(object.begin() + i);				//�Ώۂ̍폜
			generationPos.erase(generationPos.begin() + i);	//�Ώۂ̏����l�̍폜
			nowTime.erase(nowTime.begin() + i);				//�Ώۂ̎��ԏ��̍폜
			timeRate.erase(timeRate.begin() + i);			//�Ώۂ̎��Ԋ������̍폜
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
