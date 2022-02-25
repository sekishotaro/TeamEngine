#include "MyGame.h"

#include "SafeDelete.h"
#include "SceneFactory.h"

void MyGame::Initialize()
{
	//���N���X�̏�����
	Framework::Initialize();

	//�V�[���̏�����
	//�V�[���t�@�N�g������
	sceneFactory_ = std::make_unique<SceneFactory>();
	
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
	
	//�V�[���}�l�[�W���[�ɍŏ��̃V�[�����Z�b�g
	SceneManager::GetInstance()->ChangeScene("TITLE");
}

void MyGame::Finalize()
{
	// ���N���X�̏I��
	Framework::Finalize();
}

void MyGame::Update()
{
	//���N���X�̃A�b�v�f�[�g
	Framework::Update();
}

void MyGame::Draw()
{
	//���N���X�̃A�b�v�f�[�g
	Framework::Draw();
}
