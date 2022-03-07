#include "Input.h"
#include <assert.h>

using namespace DirectX;

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input* Input::GetInstance()
{
	static Input input;

	return &input;
}

void Input::Initialize(WinApp* win_app)
{
	HRESULT result;

	//�C���^�[�t�F�[�X�쐬
	result = DirectInput8Create(
		win_app->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);
	if (FAILED(result))
	{
		assert(0);
	}

	//�L�[�{�[�h�f�o�C�X�̐���
	devkeyboard = nullptr;
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);

	//�}�E�X�̐���
	devMouse = nullptr;
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);

	//�Q�[���p�b�h�̐���
	devGamePad = nullptr;
	result = dinput->CreateDevice(GUID_Joystick, &devGamePad, NULL);

	if (devkeyboard != nullptr)
	{
		//���̓f�[�^�`���̃Z�b�g
		result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(result))
		{
			assert(0);
		}

		//�r�����䃌�x���Z�b�g
		result = devkeyboard->SetCooperativeLevel(win_app->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		if (FAILED(result))
		{
			assert(0);
		}
	}

	if (devMouse != nullptr)
	{
		//���̓f�[�^�`���̃Z�b�g
		result = devMouse->SetDataFormat(&c_dfDIMouse2);
		if (FAILED(result))
		{
			assert(0);
		}

		//�r�����䃌�x���Z�b�g
		result = devMouse->SetCooperativeLevel(win_app->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		if (FAILED(result))
		{
			assert(0);
		}
	}

	if (devGamePad != nullptr)
	{
		//���̓f�[�^�`���̃Z�b�g
		result = devGamePad->SetDataFormat(&c_dfDIJoystick);
		if (FAILED(result))
		{
			assert(0);
		}

		// �����[�h���Βl���[�h�Ƃ��Đݒ�
		DIPROPDWORD diprop;
		ZeroMemory(&diprop, sizeof(diprop));
		diprop.diph.dwSize = sizeof(diprop);
		diprop.diph.dwHeaderSize = sizeof(diprop.diph);
		diprop.diph.dwHow = DIPH_DEVICE;
		diprop.diph.dwObj = 0;
		diprop.dwData = DIPROPAXISMODE_ABS;

		// �����[�h��ύX
		devGamePad->SetProperty(DIPROP_AXISMODE, &diprop.diph);

		// X���̒l�͈̔͐ݒ�
		DIPROPRANGE diprg;
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.diph.dwObj = DIJOFS_X;
		diprg.lMin = -responsive_range;
		diprg.lMax = responsive_range;

		// X���̒l�͈̔͐ݒ�
		devGamePad->SetProperty(DIPROP_RANGE, &diprg.diph);
		
		// Y���̒l�͈̔͐ݒ�
		diprg.diph.dwObj = DIJOFS_Y;
		devGamePad->SetProperty(DIPROP_RANGE, &diprg.diph);

		//�r�����䃌�x���Z�b�g
		result = devGamePad->SetCooperativeLevel(win_app->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		if (FAILED(result))
		{
			assert(0);
		}
	}

	Update();
}

void Input::Update()
{
	HRESULT result = S_FALSE;

	if (devkeyboard != nullptr)
	{
		//�L�[�{�[�h���̎擾�J�n
		result = devkeyboard->Acquire();
		//�O�t���[���̃L�[���擾
		memcpy(oldkeys, keys, sizeof(keys));
		//�S�L�[�̏����擾����
		result = devkeyboard->GetDeviceState(sizeof(keys), keys);
	}

	if (devMouse != nullptr)
	{
		//�}�E�X���̎擾�J�n
		result = devMouse->Acquire();
		//�O�t���[���̔���
		oldMouseState = mouseState;
		//�S�N���b�N�̏����擾����
		result = devMouse->GetDeviceState(sizeof(mouseState), &mouseState);
	}

	if (devGamePad != nullptr)
	{
		//�Q�[���p�b�h���̎擾�J�n
		result = devGamePad->Acquire();
		//�O�t���[���̔���
		oldGamePadState = gamePadState;
		//�S�{�^���̏����擾����
		result = devGamePad->GetDeviceState(sizeof(DIJOYSTATE), &gamePadState);
		//���Z�b�g
		for (int i = 0; i < 32; i++)
		{
			is_push[i] = false;
		}
	}
}

bool Input::PushKey(BYTE key)
{
	assert(0 <= key && key < 256);

	if (keys[key])
	{
		return true;
	}
	return false;
}

bool Input::TriggerKey(BYTE key)
{
	assert(0 <= key && key < 256);

	if (keys[key] && !oldkeys[key])
	{
		return true;
	}
	return false;
}

bool Input::PushMouse(int Mouse)
{
	assert(0 <= Mouse && Mouse < 3);

	if (mouseState.rgbButtons[Mouse])
	{
		return true;
	}

	return false;
}

bool Input::TriggerMouse(int Mouse)
{
	assert(0 <= Mouse && Mouse < 3);

	if (mouseState.rgbButtons[Mouse] && !oldMouseState.rgbButtons[Mouse])
	{
		return true;
	}

	return false;
}

bool Input::TiltLeftStick(int stick)
{
	assert(0 <= stick && stick < 4);

	//��
	if (gamePadState.lX < -unresponsive_range && stick == S_Left)
	{
		return true;
	}
	//�E
	else if (gamePadState.lX > unresponsive_range && stick == S_Right)
	{
		return true;
	}
	//���
	if (gamePadState.lY > unresponsive_range && stick == S_Down)
	{
		return true;
	}
	//�O
	else if (gamePadState.lY < -unresponsive_range && stick == S_Up)
	{
		return true;
	}

	return false;
}

bool Input::TriggerLeftStick(int stick)
{
	assert(0 <= stick && stick < 4);

	//��
	if (gamePadState.lX < -unresponsive_range && !(oldGamePadState.lX < -unresponsive_range) && stick == S_Left)
	{
		return true;
	}
	//�E
	else if (gamePadState.lX > unresponsive_range && !(oldGamePadState.lX > unresponsive_range) && stick == S_Right)
	{
		return true;
	}
	//���
	if (gamePadState.lY > unresponsive_range && !(oldGamePadState.lY > unresponsive_range) && stick == S_Down)
	{
		return true;
	}
	//�O
	else if (gamePadState.lY < -unresponsive_range && !(oldGamePadState.lY < -unresponsive_range) && stick == S_Up)
	{
		return true;
	}

	return false;
}

XMFLOAT2 Input::LeftStickAngle()
{
	//�X�e�B�b�N�̕�������
	float y_vec = static_cast<float>(-gamePadState.lY / responsive_range);
	float x_vec = static_cast<float>(gamePadState.lX / responsive_range);

	//��
	if (gamePadState.lX > -unresponsive_range && gamePadState.lX < unresponsive_range)
	{
		x_vec = 0.0f;
	}
	//�c
	if (gamePadState.lY < unresponsive_range && gamePadState.lY > -unresponsive_range)
	{
		y_vec = 0.0f;
	}

	return XMFLOAT2(x_vec, y_vec);
}

bool Input::PushButton(int Button)
{
	assert(0 <= Button && Button < Cross_Up);

	for (int i = 0; i < 32; i++)
	{
		if (!(gamePadState.rgbButtons[i] & 0x80))
		{
			continue;
		}

		switch (i)
		{
		case 0:
			is_push[ButtonKind::Button_A] = true;
			break;
		case 1:
			is_push[ButtonKind::Button_B] = true;
			break;
		case 2:
			is_push[ButtonKind::Button_X] = true;
			break;
		case 3:
			is_push[ButtonKind::Button_Y] = true;
			break;
		case 4:
			is_push[ButtonKind::Button_LB] = true;
			break;
		case 5:
			is_push[ButtonKind::Button_RB] = true;
			break;
		case 6:
			is_push[ButtonKind::Select] = true;
			break;
		case 7:
			is_push[ButtonKind::Start] = true;
			break;
		case 8:
			is_push[ButtonKind::Button_LS] = true;
			break;
		case 9:
			is_push[ButtonKind::Button_RS] = true;
			break;
		default:
			break;
		}
	}

	for (int i = 0; i < Cross_Up; i++)
	{
		if (is_push[i] == true)
		{
			if (is_push[i] == is_push[Button])
			{
				return true;
			}
		}
	}

	return false;
}

bool Input::TriggerButton(int Button)
{
	assert(0 <= Button && Button < Cross_Up);

	for (int i = 0; i < 32; i++)
	{
		if (!(gamePadState.rgbButtons[i] & 0x80))
		{
			continue;
		}
		if (oldGamePadState.rgbButtons[i] & 0x80)
		{
			continue;
		}

		switch (i)
		{
		case 0:
			is_push[ButtonKind::Button_A] = true;
			break;
		case 1:
			is_push[ButtonKind::Button_B] = true;
			break;
		case 2:
			is_push[ButtonKind::Button_X] = true;
			break;
		case 3:
			is_push[ButtonKind::Button_Y] = true;
			break;
			is_push[ButtonKind::Button_Y] = true;
			break;
		case 4:
			is_push[ButtonKind::Button_LB] = true;
			break;
		case 5:
			is_push[ButtonKind::Button_RB] = true;
			break;
		case 6:
			is_push[ButtonKind::Select] = true;
			break;
		case 7:
			is_push[ButtonKind::Start] = true;
			break;
		case 8:
			is_push[ButtonKind::Button_LS] = true;
			break;
		case 9:
			is_push[ButtonKind::Button_RS] = true;
			break;
		default:
			break;
		}
	}

	for (int i = 0; i < Cross_Up; i++)
	{
		if (is_push[i] == true)
		{
			if (is_push[i] == is_push[Button])
			{
				return true;
			}
		}
	}

	return false;
}

bool Input::PushCrossKey(int CrossKey)
{
	assert(Cross_Up <= CrossKey && CrossKey < Cross_Left);

	if (gamePadState.rgdwPOV[0] != 0xFFFFFFFF)
	{
		switch (gamePadState.rgdwPOV[0])
		{
		case 0:
			is_push[ButtonKind::Cross_Up] = true;
			break;
		case 4500:
			is_push[ButtonKind::Cross_Up] = true;
			is_push[ButtonKind::Cross_Right] = true;
			break;
		case 9000:
			is_push[ButtonKind::Cross_Right] = true;
			break;
		case 13500:
			is_push[ButtonKind::Cross_Right] = true;
			is_push[ButtonKind::Cross_Down] = true;
			break;
		case 18000:
			is_push[ButtonKind::Cross_Down] = true;
			break;
		case 22500:
			is_push[ButtonKind::Cross_Down] = true;
			is_push[ButtonKind::Cross_Left] = true;
			break;
		case 27000:
			is_push[ButtonKind::Cross_Left] = true;
			break;
		case 31500:
			is_push[ButtonKind::Cross_Left] = true;
			is_push[ButtonKind::Cross_Up] = true;
			break;
		}

		for (int i = Cross_Up; i < ButtonMax; i++)
		{
			if (is_push[i] == true)
			{
				if (is_push[i] == is_push[CrossKey])
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool Input::TriggerCrossKey(int CrossKey)
{
	assert(Cross_Up <= CrossKey && CrossKey < Cross_Left);

	if (gamePadState.rgdwPOV[0] != 0xFFFFFFFF && oldGamePadState.rgdwPOV[0] == 0xFFFFFFFF)
	{
		switch (gamePadState.rgdwPOV[0])
		{
		case 0:
			is_push[ButtonKind::Cross_Up] = true;
			break;
		case 4500:
			is_push[ButtonKind::Cross_Up] = true;
			is_push[ButtonKind::Cross_Right] = true;
			break;
		case 9000:
			is_push[ButtonKind::Cross_Right] = true;
			break;
		case 13500:
			is_push[ButtonKind::Cross_Right] = true;
			is_push[ButtonKind::Cross_Down] = true;
			break;
		case 18000:
			is_push[ButtonKind::Cross_Down] = true;
			break;
		case 22500:
			is_push[ButtonKind::Cross_Down] = true;
			is_push[ButtonKind::Cross_Left] = true;
			break;
		case 27000:
			is_push[ButtonKind::Cross_Left] = true;
			break;
		case 31500:
			is_push[ButtonKind::Cross_Left] = true;
			is_push[ButtonKind::Cross_Up] = true;
			break;
		}

		for (int i = Cross_Up; i < ButtonMax; i++)
		{
			if (is_push[i] == true)
			{
				if (is_push[i] == is_push[CrossKey])
				{
					return true;
				}
			}
		}
	}

	return false;
}