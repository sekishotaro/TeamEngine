#include <windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION  0x0800   //DirectInputのバージョン指定
#include <dinput.h>
#include "WinApp.h"
#pragma once

//入力
class Input
{
public:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //メンバ関数

	static Input *GetInstance();

	//初期化
	void Initialize(WinApp *winApp);
	//更新
	void Update();

	///<summary>
	/// キーの押下をチェック
	///</summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

	///<summary>
	/// キーのトリガーをチェック
	///</summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>押されているか</returns>
	bool TriggerKey(BYTE keyNumber);

private: //メンバ変数
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> devkeyboard;
	//DirectInputのインスタンス生成
	ComPtr<IDirectInput8> dinput;

	//全キーの状態
	BYTE key[256] = {};
	//前回の全キーの状態
	BYTE keyPre[256] = {};

	//WindowsAPI
	WinApp *winApp = nullptr;
	
};