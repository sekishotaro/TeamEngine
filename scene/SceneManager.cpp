#include "SceneManager.h"
#include <cassert>

SceneManager::~SceneManager()
{
	//�Ō�̃V�[���̏I���Ɖ��
	scene_->Finalize();
	delete scene_;
}

SceneManager *SceneManager::GetInstance()
{
	static SceneManager instance;

	return &instance;
}

void SceneManager::ChangeScene(const std::string &sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	//���V�[���𐶐�
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}

void SceneManager::Update()
{
	//�V�[���؂�ւ�������Ȃ�
	if (nextScene_)
	{
		//���V�[���̏I��
		if (scene_)
		{
			scene_->Finalize();
			delete scene_;
		}

		//�V�[���؂�ւ�
		scene_ = nextScene_;
		nextScene_ = nullptr;

		//�V�V�[���̏�����
		scene_->Initialize();
	}

	scene_->Update();
}

void SceneManager::Draw()
{
	scene_->Draw();
}
