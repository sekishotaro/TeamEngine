#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

/// <summary>
/// �V�[���Ǘ�
/// </summary>
class SceneManager
{

public:

	static SceneManager *GetInstance();

	
	
	/// <summary>
	/// ���̃V�[���\��
	/// </summary>
	/// <param name="sceneName">�V�[����</param>
	void ChangeScene(const std::string &sceneName);

	/// <summary>
	/// �V�[���t�@�N�g���̃Z�b�^�[
	/// </summary>
	/// <param name="sceneFactory"></param>
	void SetSceneFactory(AbstractSceneFactory *sceneFactory) { sceneFactory_ = sceneFactory; }

	/// <summary>
/// �}�C�t���[���X�V
/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();


private:

	/// <summary>
	/// ���̃Q�[���V�[��
	/// </summary>
	BaseScene *scene_ = nullptr;
	
	/// <summary>
	/// ���̃Q�[���V�[��
	/// </summary>
	BaseScene *nextScene_ = nullptr;
	
	/// <summary>
	/// �V�[���t�@�N�g���[
	/// </summary>
	AbstractSceneFactory *sceneFactory_ = nullptr;


	//�R���X�g���N�^�A�f�X�g���N�^�̉B��
	SceneManager() = default;
	~SceneManager();
	//������Z�q�̉B��
	SceneManager(const SceneManager &) = delete;
	//�R�s�[���Z�q�̉B��
	void operator = (const SceneManager &) = delete;

};

