#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <memory>
#include "Camera.h"
#include "Angle.h"
#include <vector>
#include "FbxObject3d.h"
#include "Mapchip.h"

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
	using XMVECTOR = DirectX::XMVECTOR;

public: //サブクラス
	enum EnemyType
	{
		NORMAL, TWICE, JUMP, FIN
	};

	struct EnemyData
	{
		XMFLOAT3 e_pos; //座標
		XMFLOAT3 old_e_pos; //1フレーム前の座標

		float e_x_radius; //中心点からxの最大値まで
		float e_y_radius; //中心点からyの最大値まで

		bool is_normal; //徘徊状態
		bool is_bounce; //跳ねる状態
		bool is_catch; //捕縛状態
		bool is_alive; //生死
		bool is_grand; //地面についているか
		bool is_turn; //攻撃を受けたか

		float e_speed; //移動量
		float e_down; //下降度

		float e_acc; //加速度

		int angle; //円運動の角度

		int enemy_type; //エネミーの種類
		bool can_catch; //捕縛可能か

		bool is_add; //加算するか

		float turn_move; //裏返るときの動き

		int escape_time; //逃げるまでの時間

		float max_rope; //ロープの最大
	};

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
	
	//定数
	static const int EnemySpawnMax = 50;
	//マップチップ1つの大きさ
	const float LAND_SCALE = 5.0f;

	//マップチップの番号
	enum MapNumber
	{
		None, Ground
	};

	//クラス
	Angle* rope_angle = new Angle(); //Angle
	Camera* camera = nullptr; //カメラ

	//モデル
	Model* model = nullptr;
	Model* modelPlayer = nullptr;
	Model* enemy_model_2 = nullptr;
	Model* block = nullptr;
	Model* rope = nullptr;

	//オブジェクト
	std::vector<std::vector<int>> map; //マップチップ
	std::unique_ptr<Object3d> objBlock[map_max_y][map_max_x]; //ステージブロック
	std::unique_ptr<Object3d> player = nullptr; //プレイヤー
	std::unique_ptr<Object3d> enemy[EnemySpawnMax]; //エネミー
	std::unique_ptr<Object3d> Rope[EnemySpawnMax]; //ロープ

	//FBX実装
	FbxModel* fbxModel1 = nullptr;
	FbxObject3d* fbxObject1 = nullptr;

	//エフェクト
	int maxLocus = 0;
	std::vector<std::unique_ptr<Object3d>>locus;
	std::unique_ptr<Object3d> shock = nullptr;
	//locus = new std::unique_ptr<Object3d>[MaxLocus];
	//std::unique_ptr<Object3d> locus[MaxLocus]; //軌跡用板ポリ
	Model* locusModel = nullptr; //軌跡
	Model* shockWaveModel = nullptr; //波動
	bool shockFlag = false; //波動が開始される

	//シーン移行完了確認フラグ
	bool ConvertFlag = false;
	//ゲーム開始カウント完了確認フラグ
	bool countFinishFlag = false;
	//ゲーム終了後エンドシーンに移行確認用フラグ
	bool finishFinishFlag = false;
	//エンドシーンにコンバートする
	bool endConvertflag = false;
	//ゲームプレイ可能フラグ
	bool PlayPossibleflag = false;

	//スプライト
	Sprite* spriteBG = nullptr; //背景
	Sprite* miniplayer = nullptr; //プレイヤー(ミニマップ)
	Sprite* minienemy[EnemySpawnMax]; //エネミー(ミニマップ)
	Sprite* spriteTime[10]; //スコア
	Sprite* spriteScore[10]; //スコア
	Sprite* spriteLevel[10]; //スコア
	Sprite* spriteCoron = nullptr;
	Sprite* texScore = nullptr;
	Sprite* texLevel = nullptr;
	Sprite* spriteTimer = nullptr;
	Sprite* spriteLevelUp = nullptr;
	Sprite* minimap = nullptr; //ステージ(ミニマップ)
	Sprite* finish = nullptr; //終わり

	//UI・スコアetc
	int score; //スコア
	int scoreTick[EnemySpawnMax]; //スコア計算用
	bool is_shake; //シェイクフラグ
	int shake_power; //シェイクの強さ
	int shake_time; //シェイク時間
	float shake_x; //x軸の幅
	float shake_y; //y軸の幅
	float lastTime; //残り時間
	int level;
	int oldLevel;
	int enemySpawn; //敵の数
	int catch_count; //何個持ってるか
	float levelTime;


	//汎用変数
	float gravity; //重力加速度

	//プレイヤー
	XMFLOAT3 p_pos; //座標
	XMFLOAT3 old_p_pos; //1フレーム前の座標

	float p_x_radius; //中心点からxの最大値まで
	float p_y_radius; //中心点からyの最大値まで

	bool is_jump; // ジャンプフラグ

	float p_add; //上昇度
	float p_down; //下降度

	bool is_attack; //攻撃フラグ
	bool is_air;//空中フラグ

	bool is_damage; //攻撃を受けたか
	int damage_time; //ダメージリアクションの時間
	float damage_move; //ダメージの移動

	//無敵時間
	bool is_invincible; //無敵状態
	float invincible_time; //無敵時間

	//エネミー
	EnemyData enemy_data[EnemySpawnMax]; //エネミーのデータ

	XMFLOAT3 c_pos; //カメラの主点座標
	bool camera_chase; //カメラが動き出すまでの時間

	/// <summary>
	/// エネミー生成
	/// </summary>
	void SpawnEnemy(int mapNumber, int enemyNumber);

	/// <summary>
	/// 円運動
	/// </summary>
	void CircularMotion(XMFLOAT3& pos, const XMFLOAT3 center_pos, const float r, int& angle, const int add);

	/// <summary>
	/// //マップチップ生成
	/// </summary>
	void MapCreate(int mapNumber);

	/// <summary>
	/// マップチップ当たり判定
	/// </summary>
	bool MapCollide(XMFLOAT3& pos, float radiusX, float radiusY, float &add, int mapNumber,const XMFLOAT3 old_pos, bool is_jump = false);

	/// <summary>
	/// ロープの角度変更
	/// <summary>
	void RopeMove(const int num);

	//オブジェクト同士の当たり判定
	bool CollisionObject(const std::unique_ptr<Object3d>& object_a, const std::unique_ptr<Object3d>& object_b);

	//長さを求める
	float GetObjectLength(XMFLOAT3 pos_a, XMFLOAT3 pos_b);

	/// <summary>
	/// 視錐台の中にあるか
	/// </summary>
	/// <param name="playerPosition">自機</param>
	/// <param name="negativePoint">対象の負の頂点</param>
	/// <param name="positivePoint">対象の正の頂点</param>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <returns>成否</returns>
	bool inFrustum(XMFLOAT3 playerPosition, XMFLOAT3 negativePoint, XMFLOAT3 positivePoint, const float width, const float height);
};