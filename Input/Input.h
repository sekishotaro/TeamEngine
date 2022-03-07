#pragma once
#include "InputList.h"
#include "WinApp.h"
#include <windows.h>
#include <dinput.h>
#include <wrl.h>
#include <DirectXMath.h>

#define DIRECTINPUT_VERSION             0x0800 //Direct Input�̃o�[�W�����w��

class Input : protected InputList
{
public: //�G�C���A�X
	//namespace�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;

private: //�����o�ϐ�
	//DirectInput�̃C���X�^���X
	ComPtr<IDirectInput8> dinput;
	//�L�[�{�[�h�f�o�C�X
	ComPtr<IDirectInputDevice8> devkeyboard;
	//�L�[����
	BYTE keys[256] = {};
	//�O�t���[���̃L�[���
	BYTE oldkeys[256] = {};
	//�}�E�X�f�o�C�X
	ComPtr<IDirectInputDevice8> devMouse;
	//�}�E�X����
	DIMOUSESTATE2 mouseState = {};
	//�O�t���[���̃}�E�X����
	DIMOUSESTATE2 oldMouseState = {};
	//�Q�[���p�b�h�f�o�C�X
	ComPtr<IDirectInputDevice8> devGamePad;
	//�Q�[���p�b�h�̔���
	DIJOYSTATE gamePadState = {};
	//�O�t���[���̃Q�[���p�b�h�̔���
	DIJOYSTATE oldGamePadState = {};
	//�{�^���f�[�^
	bool is_push[32] = {};
	//�X�e�B�b�N�̔����͈�
	LONG responsive_range = 100;
	//�X�e�B�b�N�̖������͈�
	LONG unresponsive_range = 20;

public: //�ÓI�����o�֐�
	static Input* GetInstance();

public: //�����o�֐�
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(WinApp *win_app);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	//�L�[����
	//����
	bool PushKey(BYTE key);
	//���́i�������s�j
	bool TriggerKey(BYTE key);

	//�N���b�N
	//���N���b�N
	bool PushMouse(int Mouse);
	//���N���b�N�i�������s�j
	bool TriggerMouse(int Mouse);

	//�Q�[���p�b�h
	//�Q�[���p�b�h�X�e�B�b�N
	bool TiltLeftStick(int stick);
	//�Q�[���p�b�h�X�e�B�b�N�i�������s�j
	bool TriggerLeftStick(int stick);
	//�Q�[���p�b�h�X�e�B�b�N��|�����䗦
	XMFLOAT2 LeftStickAngle();
	//�Q�[���p�b�h�{�^��
	bool PushButton(int Button);
	//�Q�[���p�b�h�{�^���i�������s�j
	bool TriggerButton(int Button);
	//�Q�[���p�b�h�\���L�[
	bool PushCrossKey(int CrossKey);
	//�Q�[���p�b�h�\���L�[�i�������s�j
	bool TriggerCrossKey(int CrossKey);
};