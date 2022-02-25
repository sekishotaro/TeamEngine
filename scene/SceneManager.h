#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

/// <summary>
/// シーン管理
/// </summary>
class SceneManager
{

public:

	static SceneManager *GetInstance();

	
	
	/// <summary>
	/// 次のシーン予約
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	void ChangeScene(const std::string &sceneName);

	/// <summary>
	/// シーンファクトリのセッター
	/// </summary>
	/// <param name="sceneFactory"></param>
	void SetSceneFactory(AbstractSceneFactory *sceneFactory) { sceneFactory_ = sceneFactory; }

	/// <summary>
/// マイフレーム更新
/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


private:

	/// <summary>
	/// 今のゲームシーン
	/// </summary>
	BaseScene *scene_ = nullptr;
	
	/// <summary>
	/// 次のゲームシーン
	/// </summary>
	BaseScene *nextScene_ = nullptr;
	
	/// <summary>
	/// シーンファクトリー
	/// </summary>
	AbstractSceneFactory *sceneFactory_ = nullptr;


	//コンストラクタ、デストラクタの隠蔽
	SceneManager() = default;
	~SceneManager();
	//代入演算子の隠蔽
	SceneManager(const SceneManager &) = delete;
	//コピー演算子の隠蔽
	void operator = (const SceneManager &) = delete;

};

