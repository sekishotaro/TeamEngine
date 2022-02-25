#pragma once

//前方宣言
class SceneManager;

/// <summary>
/// シーンインターフェイス
/// </summary>
class BaseScene
{
public:

	virtual ~BaseScene() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 解放
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// アップデート
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;
};

