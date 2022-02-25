#include "SceneFactory.h"

#include "GamePlayScene.h"
#include "TitleScene.h"

BaseScene *SceneFactory::CreateScene(const std::string &sceneName)
{
    //Ÿ‚ÌƒV[ƒ“‚ğì¬
    BaseScene *newScene = nullptr;


    if (sceneName == "TITLE")
    {
        newScene = new TitleScene();
    }
    else if (sceneName == "GAMEPLAY")
    {
        newScene = new GamePlayScene();
    }

    return newScene;
}
