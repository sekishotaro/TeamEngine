#include "SceneFactory.h"

#include "GamePlayScene.h"
#include "TitleScene.h"
#include "EndScene.h"

BaseScene *SceneFactory::CreateScene(const std::string &sceneName)
{
    //次のシーンを作成
    BaseScene *newScene = nullptr;


    if (sceneName == "TITLE")
    {
        newScene = new TitleScene();
    }
    else if (sceneName == "GAMEPLAY")
    {
        newScene = new GamePlayScene();
    }
    else if (sceneName == "END")
    {
        newScene = new EndScene();
    }
    else
    {
        assert(0);
    }

    return newScene;
}
