#include "MyGame.h"

//# Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//生成
	Framework *game = new MyGame();
	//実行
	game->Run();
	//解放
	delete(game);

	return 0;
}