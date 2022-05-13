#pragma once
#include "Sprite.h"
#include <DirectXMath.h>

class Count
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	static void Initilize();

	/// <summary>
	/// 解放
	/// </summary>
	static void Finalize();

	/// <summary>
	/// 3カウントダウン
	/// </summary>
	/// <param name="finishFlag">修了確認</param>
	static void CountDown3(bool &finishFlag);

	/// <summary>
	/// 3カウントダウンの描画
	/// </summary>
	static void CountDown3Draw();

	/// <summary>
	/// 終了～
	/// </summary>
	/// <param name="finishFlag">修了確認</param>
	static void Fnish(bool& finishFlag, const float &x);

	/// <summary>
	/// 終了描画
	/// </summary>
	static void FnishDraw();

	/// <summary>
	/// すべとの描画
	/// </summary>
	static void Draw();

private:
	//static Sprite* count0;
	static Sprite* count1;
	static Sprite* count2;
	static Sprite* count3;
	//static Sprite* count4;
	//static Sprite* count5;
	static Sprite*  countStart;
	static Sprite*  countFinish;


	static float count3Time;
	static bool count3finishFlag;	//3カウントスタート修了確認フラグ

	static float countFinishTime;
	static bool fnishFinishFlag;	//Finish修了確認フラグ
};

