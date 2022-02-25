#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include <DirectXMath.h>

#include <memory>

#include "Camera.h"

class GamePlayScene : public BaseScene
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;


public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 解放
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// アップデート
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;



	/// <summary>
	/// ゲームシーン用
	/// </summary>
	Sprite *spriteBG = nullptr;
	std::unique_ptr<Object3d> objectX;
	Model *model = nullptr;
	Camera *camera = nullptr;
};

