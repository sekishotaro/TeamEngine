#pragma once

#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include <DirectXMath.h>
#include <memory>
#include "Camera.h"
#include <vector>

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
	Sprite *spriteBG = nullptr;
	Model *model = nullptr;
	Camera *camera = nullptr;
	Model* block = nullptr;
	std::vector<std::vector<int>> map;
	std::vector<Object3d*> box;
	std::unique_ptr<Object3d> objBlock[12][52];
	enum MapNumber //マップチップの番号
	{
		None, Ground
	};

	//汎用変数
	float gravity = 0.15f; //重力加速度
	int angle = 0; //円運動の角度

	//プレイヤー
	std::unique_ptr<Object3d> player = nullptr; //オブジェクトクラス
	XMFLOAT3 p_pos = {0, 0, 0}; //座標
	float p_max_speed = 0.5f; //最高速度
	bool is_jump = false; // ジャンプフラグ
	float p_add = 0; //上昇度
	bool is_attack = false; //攻撃フラグ

	//エネミー
	std::unique_ptr<Object3d> enemy; //オブジェクトクラス
	XMFLOAT3 e_pos = {0, 0, 0}; //座標
	bool is_normal = false; //通常状態
	bool is_chase = false; //追跡状態
	float e_add = 0.25f; //移動量
	int max_spawn = 25; //最大スポーン数

	/// <summary>
	/// エネミー生成
	/// </summary>
	void SpawnEnemy(bool& active, int& spawn_num);

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
	bool MapCollide(const std::unique_ptr<Object3d>& object, int mapNumber);
};

