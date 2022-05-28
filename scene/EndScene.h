#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include <DirectXMath.h>

class EndScene : public BaseScene
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
	Sprite* spriteBG = nullptr;
	Sprite* totitle = nullptr;
	Sprite* newrecord = nullptr;
	Sprite* spriteScore[10]; //スコア
	Sprite* spriteScoreNo1[10]; //歴代1位スコア
	Sprite* spriteScoreNo2[10]; //歴代2位スコア
	Sprite* spriteScoreNo3[10]; //歴代3位スコア

	//シーン移行完了確認フラグ
	bool ConvertFlag = false;

	float time = 0.0f;

	//スコア
	int nowPlayScore;
	int score_list[3] = { 0, 0, 0 };
	int score_listDisplay[3] = { 0, 0, 0 };
	const int digitsNum = 8; //8桁

	bool flag = false;
	bool rankCheckFlag = false;
	int rankFluctuationNum = 0;
	
	void writeText();
	void LoadText();


	void ScoreSpriteInitialize(int digits, Sprite* scoreSprite[], float x, float y);

	/// <summary>
	/// スコアスプライト表示の処理
	/// </summary>
	/// <param name="digits">桁数</param>
	void ScoreSpriteBB(int digits, Sprite *scoreSprite[], int score);

	/// <summary>
	/// スコアの描画
	/// </summary>
	/// <param name="digits">桁数</param>
	/// <param name="scoreSprite">指定スコアのスプライト</param>
	void ScoreSpriteDraw(int digits, Sprite* scoreSprite[]);

	//ランクチェック
	void RankCheck();
};


