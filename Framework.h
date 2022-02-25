#pragma once

#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#include "Audio.h"
#include "Sprite.h"
#include "Object3d.h"
#include "DebugText.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"

#include <memory>
#include "Camera.h"

/// <summary>
/// �t���[�����[�N
/// </summary>
class Framework
{
public:
	
	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// �I��
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// �}�C�t���[���X�V
	/// </summary>
	virtual void Update();

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// ���s
	/// </summary>
	void Run();

	virtual bool IsEndRequest() { return endRequest_; }

protected:
	bool endRequest_ = false;
	WinApp *winApp = nullptr;
	DirectXCommon *dxCommon = nullptr;
	//����
	Input *input = nullptr;
	//�I�[�f�B�I
	Audio *audio = nullptr;

	Camera *camera = nullptr;

	//�f�o�b�N�e�L�X�g
	DebugText *debugText = nullptr;
	static const int debugTextTexNumber = 0;

	//�V�[���t�@�N�g���[
	std::unique_ptr<AbstractSceneFactory> sceneFactory_;
};

