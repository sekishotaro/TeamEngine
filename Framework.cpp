#include "Framework.h"

#include "SafeDelete.h"

void Framework::Initialize()
{
	//WindowsAPI�̏�����
	winApp = new WinApp();
	winApp->Initializa();

	//DirectX�̏�����
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(winApp);

	//���͂̏�����
	input = Input::GetInstance();
	input->Initialize(winApp);

	//�I�[�f�B�I�̏�����
	audio = Audio::GetInstance();
	audio->Initialize();

	//�X�v���C�g�ÓI������
	if (!Sprite::StaticInitialize(dxCommon->GetDev(), WinApp::window_width, WinApp::window_height))
	{
		assert(0);
	}

	// �f�o�b�N�e�L�X�g
	Sprite::LoadTexture(debugTextTexNumber, L"Resources/ASCII.png");
	DebugText::GetInstance()->Initialize(debugTextTexNumber);

	camera = new Camera(WinApp::window_width, WinApp::window_height);

	//3D�I�u�W�F�N�g�ÓI������
	if (!Object3d::StaticInitialize(dxCommon->GetDev(), camera))
	{
		assert(0);
	}
}

void Framework::Finalize()
{
	//�I�[�f�B�I�J��
	audio->Finalize();
	//�f�o�b�N�e�L�X�g�J��
	//debugText->Finalize();

	// �Q�[���E�B���h�E�̔j��
	winApp->Finalize();
	safe_delete(winApp);
}

void Framework::Update()
{
	// ���b�Z�[�W����
	if (winApp->ProcessMessage())
	{
		// �Q�[�����[�v�𔲂���
		endRequest_ = true;
		return;
	}

	// ���͊֘A�̖��t���[������
	input->Update();

	//�V�[���̍X�V
	SceneManager::GetInstance()->Update();

}

void Framework::Draw()
{
	// �`��J�n
	dxCommon->PreDraw();
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList *cmdList = dxCommon->GetCmdList();

	SceneManager::GetInstance()->Draw();

	// �`��I��
	dxCommon->PostDraw();
}

void Framework::Run()
{
	//�Q�[���̏�����
	Initialize();

	// ���C�����[�v
	while (true)
	{
		//���t���[���X�V
		Update();
		//�I�����N�G�X�g�������甲����
		if (IsEndRequest())
		{
			break;
		}

		//�`��
		Draw();
	}

	//�Q�[���̏I��
	Finalize();
}
