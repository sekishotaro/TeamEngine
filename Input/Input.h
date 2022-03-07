#pragma once
#include "InputList.h"
#include "WinApp.h"
#include <windows.h>
#include <dinput.h>
#include <wrl.h>
#include <DirectXMath.h>

#define DIRECTINPUT_VERSION             0x0800 //Direct Inputのバージョン指定

class Input : protected InputList
{
public: //エイリアス
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;

private: //メンバ変数
	//DirectInputのインスタンス
	ComPtr<IDirectInput8> dinput;
	//キーボードデバイス
	ComPtr<IDirectInputDevice8> devkeyboard;
	//キー判定
	BYTE keys[256] = {};
	//前フレームのキー情報
	BYTE oldkeys[256] = {};
	//マウスデバイス
	ComPtr<IDirectInputDevice8> devMouse;
	//マウス判定
	DIMOUSESTATE2 mouseState = {};
	//前フレームのマウス判定
	DIMOUSESTATE2 oldMouseState = {};
	//ゲームパッドデバイス
	ComPtr<IDirectInputDevice8> devGamePad;
	//ゲームパッドの判定
	DIJOYSTATE gamePadState = {};
	//前フレームのゲームパッドの判定
	DIJOYSTATE oldGamePadState = {};
	//ボタンデータ
	bool is_push[32] = {};
	//スティックの反応範囲
	LONG responsive_range = 100;
	//スティックの無反応範囲
	LONG unresponsive_range = 20;

public: //静的メンバ関数
	static Input* GetInstance();

public: //メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp *win_app);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	//キー操作
	//入力
	bool PushKey(BYTE key);
	//入力（長押し不可）
	bool TriggerKey(BYTE key);

	//クリック
	//左クリック
	bool PushMouse(int Mouse);
	//左クリック（長押し不可）
	bool TriggerMouse(int Mouse);

	//ゲームパッド
	//ゲームパッドスティック
	bool TiltLeftStick(int stick);
	//ゲームパッドスティック（長押し不可）
	bool TriggerLeftStick(int stick);
	//ゲームパッドスティックを倒した比率
	XMFLOAT2 LeftStickAngle();
	//ゲームパッドボタン
	bool PushButton(int Button);
	//ゲームパッドボタン（長押し不可）
	bool TriggerButton(int Button);
	//ゲームパッド十字キー
	bool PushCrossKey(int CrossKey);
	//ゲームパッド十字キー（長押し不可）
	bool TriggerCrossKey(int CrossKey);
};