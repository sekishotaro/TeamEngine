#pragma once

#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#include "Audio.h"
#include "Sprite.h"
#include "Object3d.h"
#include "DebugText.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"

#include <memory>
#include "Camera.h"

/// <summary>
/// フレームワーク
/// </summary>
class Framework
{
public:
	
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// マイフレーム更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// 実行
	/// </summary>
	void Run();

	virtual bool IsEndRequest() { return endRequest_; }

protected:
	bool endRequest_ = false;
	WinApp *winApp = nullptr;
	DirectXCommon *dxCommon = nullptr;
	//入力
	Input *input = nullptr;
	//オーディオ
	Audio *audio = nullptr;

	Camera *camera = nullptr;

	//デバックテキスト
	DebugText *debugText = nullptr;
	static const int debugTextTexNumber = 0;

	//シーンファクトリー
	std::unique_ptr<AbstractSceneFactory> sceneFactory_;
};

