#pragma once
#include "Sprite.h"
#include <DirectXMath.h>

class ConvertScene
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
	/// 初期化開く
	/// </summary>
	static void InitializeOut();

	/// <summary>
	/// 初期化閉じる
	/// </summary>
	static void InitializeIn();

	/// <summary>
	/// 解放
	/// </summary>
	void Finalize();

	/// <summary>
	/// 横開く
	/// </summary>
	static void besideOut();

	/// <summary>
	/// 横に閉じる
	/// </summary>
	static void besideIn();

	/// <summary>
	/// 描画
	/// </summary>
	static void Draw();

	static Sprite* switchIn; //シーン切り替え用1
	static Sprite* switchOut; //シーン切り替え用2

};

