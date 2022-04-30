#pragma once

#include "Object3d.h"
#include <DirectXMath.h>
#include <vector>
#include "Camera.h"
#include "Sprite.h"

class Effect
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
	/// 軌跡生成
	/// </summary>
	/// <param name="object">軌跡</param>
	/// <param name="model">軌跡モデル</param>
	/// <param name="pos">軌跡発生地点</param>
	static void CreateLocus(std::vector<std::unique_ptr<Object3d>> &object, Model &model, XMFLOAT3& pos);

	/// <summary>
	/// 軌跡削除
	/// </summary>
	/// <param name="object">軌跡</param>
	/// <param name="camera">カメラ</param>
	/// <param name="player">自機(画面中心部)</param>
	static void DeletLocus(std::vector<std::unique_ptr<Object3d>>& object, Camera *camera, XMFLOAT3& player);

	/// <summary>
	/// 波動
	/// </summary>
	/// <param name="object">波動</param>
	/// <param name="generationPoint">発生地点</param>
	/// <param name="flag">フラグ</param>
	static void ShockWaveUpdate(std::unique_ptr<Object3d>& object, XMFLOAT3& generationPoint, bool *flag);
};

