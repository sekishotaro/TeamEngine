#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <memory>
#include "Angle.h"
#include "Mapchip.h"
#include "Camera.h"
#include <vector>
#include "Effect.h"
#include "Vector3.h"

class TitleScene : public BaseScene
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
	};

	struct RopeData
	{
		XMFLOAT3 r_pos; // ロープの位置
		XMFLOAT3 r_vec; // ロープの速度
	};

	int num = 0;

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

	//定数
	static const int Point = 10;
	static const int EnemySpawnMax = 40;
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

	Model* modelBoard = nullptr;

	//オブジェクト
	std::vector<std::vector<int>> map; //マップチップ
	std::unique_ptr<Object3d> objBlock[map_max_y][map_max_x]; //ステージブロック
	std::unique_ptr<Object3d> player = nullptr; //プレイヤー
	std::unique_ptr<Object3d> enemy[EnemySpawnMax]; //エネミー
	std::unique_ptr<Object3d> Rope[EnemySpawnMax][Point]; //ロープ
	std::unique_ptr<Object3d> board = nullptr; //いた

	//エフェクト
	int maxLocus = 0;
	std::vector<std::unique_ptr<Object3d>>locus;
	std::unique_ptr<Object3d> shock = nullptr;
	//locus = new std::unique_ptr<Object3d>[MaxLocus];
	//std::unique_ptr<Object3d> locus[MaxLocus]; //軌跡用板ポリ
	Model* locusModel = nullptr; //軌跡
	Model* shockWaveModel = nullptr; //波動
	bool shockFlag = false; //波動が開始される

	//UI・スコアetc
	int score; //スコア
	int tenScore; //仮スコア(代入する値)
	bool is_shake; //シェイクフラグ
	int shake_time; //シェイク時間
	float shake_x; //x軸の幅
	float shake_y; //y軸の幅
	float lastTime; //残り時間
	int level;
	int enemySpawn; //敵の数


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
	int invincible_time; //無敵時間

	//エネミー
	EnemyData enemy_data[EnemySpawnMax]; //エネミーのデータ

	//ロープ
	float max_rope; //ロープの最大
	RopeData rope_data[EnemySpawnMax][Point];
	float rope_gravity = 0.125f;
	float mass[Point];
	float stiffness = 0.7f;
	float damping = 0.3f;
	float timeRate[Point];
	float angleX[Point];

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	Sprite *spriteBG = nullptr;
	Sprite* text = nullptr;
	bool startFlag = false;
	bool Convertflag = false;

	Sprite* titleText = nullptr;

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
	bool MapCollide(XMFLOAT3& pos, float radiusX, float radiusY, int mapNumber, const XMFLOAT3 old_pos, bool is_jump = false);

	/// <summary>
	/// ロープの更新
	/// </summary>
	void RopeUpdate(float targetX, float targetY, const int enemy_index, const int rope_num);

	/// <summary>
	/// ロープの角度変更
	/// <summary>
	void RopeMove(XMFLOAT3& pos, const int enemy_index);

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
	/// <returns>成否</returns>
	bool inFrustum(XMFLOAT3 playerPosition, XMFLOAT3 negativePoint, XMFLOAT3 positivePoint);
};