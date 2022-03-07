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

	//インターフェース作成
	result = DirectInput8Create(
		win_app->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);
	if (FAILED(result))
	{
		assert(0);
	}

	//キーボードデバイスの生成
	devkeyboard = nullptr;
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);

	//マウスの生成
	devMouse = nullptr;
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);

	//ゲームパッドの生成
	devGamePad = nullptr;
	result = dinput->CreateDevice(GUID_Joystick, &devGamePad, NULL);

	if (devkeyboard != nullptr)
	{
		//入力データ形式のセット
		result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(result))
		{
			assert(0);
		}

		//排他制御レベルセット
		result = devkeyboard->SetCooperativeLevel(win_app->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		if (FAILED(result))
		{
			assert(0);
		}
	}

	if (devMouse != nullptr)
	{
		//入力データ形式のセット
		result = devMouse->SetDataFormat(&c_dfDIMouse2);
		if (FAILED(result))
		{
			assert(0);
		}

		//排他制御レベルセット
		result = devMouse->SetCooperativeLevel(win_app->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		if (FAILED(result))
		{
			assert(0);
		}
	}

	if (devGamePad != nullptr)
	{
		//入力データ形式のセット
		result = devGamePad->SetDataFormat(&c_dfDIJoystick);
		if (FAILED(result))
		{
			assert(0);
		}

		// 軸モードを絶対値モードとして設定
		DIPROPDWORD diprop;
		ZeroMemory(&diprop, sizeof(diprop));
		diprop.diph.dwSize = sizeof(diprop);
		diprop.diph.dwHeaderSize = sizeof(diprop.diph);
		diprop.diph.dwHow = DIPH_DEVICE;
		diprop.diph.dwObj = 0;
		diprop.dwData = DIPROPAXISMODE_ABS;

		// 軸モードを変更
		devGamePad->SetProperty(DIPROP_AXISMODE, &diprop.diph);

		// X軸の値の範囲設定
		DIPROPRANGE diprg;
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.diph.dwObj = DIJOFS_X;
		diprg.lMin = -responsive_range;
		diprg.lMax = responsive_range;

		// X軸の値の範囲設定
		devGamePad->SetProperty(DIPROP_RANGE, &diprg.diph);
		
		// Y軸の値の範囲設定
		diprg.diph.dwObj = DIJOFS_Y;
		devGamePad->SetProperty(DIPROP_RANGE, &diprg.diph);

		//排他制御レベルセット
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
		//キーボード情報の取得開始
		result = devkeyboard->Acquire();
		//前フレームのキー情報取得
		memcpy(oldkeys, keys, sizeof(keys));
		//全キーの情報を取得する
		result = devkeyboard->GetDeviceState(sizeof(keys), keys);
	}

	if (devMouse != nullptr)
	{
		//マウス情報の取得開始
		result = devMouse->Acquire();
		//前フレームの判定
		oldMouseState = mouseState;
		//全クリックの情報を取得する
		result = devMouse->GetDeviceState(sizeof(mouseState), &mouseState);
	}

	if (devGamePad != nullptr)
	{
		//ゲームパッド情報の取得開始
		result = devGamePad->Acquire();
		//前フレームの判定
		oldGamePadState = gamePadState;
		//全ボタンの情報を取得する
		result = devGamePad->GetDeviceState(sizeof(DIJOYSTATE), &gamePadState);
		//リセット
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

	//左
	if (gamePadState.lX < -unresponsive_range && stick == S_Left)
	{
		return true;
	}
	//右
	else if (gamePadState.lX > unresponsive_range && stick == S_Right)
	{
		return true;
	}
	//後ろ
	if (gamePadState.lY > unresponsive_range && stick == S_Down)
	{
		return true;
	}
	//前
	else if (gamePadState.lY < -unresponsive_range && stick == S_Up)
	{
		return true;
	}

	return false;
}

bool Input::TriggerLeftStick(int stick)
{
	assert(0 <= stick && stick < 4);

	//左
	if (gamePadState.lX < -unresponsive_range && !(oldGamePadState.lX < -unresponsive_range) && stick == S_Left)
	{
		return true;
	}
	//右
	else if (gamePadState.lX > unresponsive_range && !(oldGamePadState.lX > unresponsive_range) && stick == S_Right)
	{
		return true;
	}
	//後ろ
	if (gamePadState.lY > unresponsive_range && !(oldGamePadState.lY > unresponsive_range) && stick == S_Down)
	{
		return true;
	}
	//前
	else if (gamePadState.lY < -unresponsive_range && !(oldGamePadState.lY < -unresponsive_range) && stick == S_Up)
	{
		return true;
	}

	return false;
}

XMFLOAT2 Input::LeftStickAngle()
{
	//スティックの方向判定
	float y_vec = static_cast<float>(-gamePadState.lY / responsive_range);
	float x_vec = static_cast<float>(gamePadState.lX / responsive_range);

	//横
	if (gamePadState.lX > -unresponsive_range && gamePadState.lX < unresponsive_range)
	{
		x_vec = 0.0f;
	}
	//縦
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