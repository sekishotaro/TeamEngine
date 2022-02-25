#pragma once
#include <Windows.h>

//WindowsAPI
class WinApp
{
public: //定数
	static const int window_width = 1280;	// ウィンドウ横幅
	static const int window_height = 720;	// ウィンドウ縦幅

public: //静的メンバ関数
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
public: //メンバ関数
	//初期化
	void Initializa();

	//メッセージの処理
	bool ProcessMessage();


	//終了
	void Finalize();

	//getter
	HWND GetHwnd() { return hwnd; }
	HINSTANCE GetHInstance() { return w.hInstance; }

private:
	//ウィンドウハンドル
	HWND hwnd = nullptr;
	WNDCLASSEX w{}; // ウィンドウクラスの設定

};
