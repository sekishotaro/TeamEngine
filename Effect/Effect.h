#pragma once
#include "Object3d.h"
#include <DirectXMath.h>
#include <vector>
#include "Camera.h"
#include "Sprite.h"
#include "Object3d.h"

class Effect
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
	/// 軌跡生成
	/// </summary>
	/// <param name="object">軌跡</param>
	/// <param name="model">軌跡モデル</param>
	/// <param name="pos">軌跡発生地点</param>
	static void CreateLocus(std::vector<std::unique_ptr<Object3d>> &object, Model &model, XMFLOAT3& pos);

	/// <summary>
	/// 軌跡削除
	/// </summary>
	/// <param name="object">軌跡</param>
	/// <param name="camera">カメラ</param>
	/// <param name="player">自機(画面中心部)</param>
	static void DeletLocus(std::vector<std::unique_ptr<Object3d>>& object, Camera *camera, XMFLOAT3& player);

	/// <summary>
	/// 波動
	/// </summary>
	/// <param name="object">波動</param>
	/// <param name="generationPoint">発生地点</param>
	/// <param name="flag">フラグ</param>
	static void ShockWaveUpdate(std::unique_ptr<Object3d>& object, Camera *camera, float &radius, bool *flag);

	/// <summary>
	/// 初期化
	/// </summary>
	static void Initialize();

	/// <summary>
	/// 解放
	/// </summary>
	static void Finalize();

	/// <summary>
	/// タイムリミット
	/// </summary>
	/// <param name="time">時間</param>
	static void TimeLimitEffectDraw(float& time);

	/// <summary>
	/// 撃破時エフェクト生成
	/// </summary>
	/// <param name="enemyNum"></param>
	/// <param name="pos"></param>
	static void DestroyEffectCreate(int enemyNum, const XMFLOAT3 &pos);

	/// <summary>
	/// 撃破時エフェクト更新
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="player">自機の位置</param>
	static void DestroyEffectUpdate(Camera* camera, XMFLOAT3& player);

	/// <summary>
	/// 撃破時エフェクトの描画
	/// </summary>
	static void DestroyEffectDraw();

	/// <summary>
	/// 点滅オブジェクト
	/// </summary>
	/// <param name="object"></param>
	static void flashingEffectDraw(const std::unique_ptr<Object3d>& object, float flashingTime);


public:
	static const float maxTime;					//最大値
	static std::vector<float> nowTime;			//今
	static std::vector<float> timeRate;			//軌跡の何%時間が進んだか(率)
	static std::vector<XMFLOAT3> generationPos; //初期生成場所
	static std::vector<float> delayTime;		//遅延処理
	static std::vector <bool> processStartFlag;
	static Sprite* timeLimitSprite;

	static Model* destroyEffectModel;
	static std::vector<std::unique_ptr<Object3d>> destroyEffect;
	static std::vector<float> nowTimeD;
	static std::vector<float> timeRateD;
	static std::vector<float> delayTimeD;
	static std::vector <bool> processStartFlagD;
	static std::vector<XMFLOAT3> generationPosD;
};

 