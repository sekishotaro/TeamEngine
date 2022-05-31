#include "MyGame.h"

#include "SafeDelete.h"
#include "SceneFactory.h"

void MyGame::Initialize()
{
	//基底クラスの初期化
	Framework::Initialize();

	//シーンの初期化
	//シーンファクトリ生成
	sceneFactory_ = std::make_unique<SceneFactory>();
	
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
	
	//シーンマネージャーに最初のシーンをセット
	SceneManager::GetInstance()->ChangeScene("TITLE");
}

void MyGame::Finalize()
{
	// 基底クラスの終了
	Framework::Finalize();
}

void MyGame::Update()
{
	//基底クラスのアップデート
	Framework::Update();
}

void MyGame::Draw()
{
	//基底クラスのアップデート
	Framework::Draw();
}
