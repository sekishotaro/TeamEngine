#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>


// 3Dモデル

class Model
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス	
// 頂点データ構造体（テクスチャあり）
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos; // xyz座標
		XMFLOAT3 normal; // 法線ベクトル
		XMFLOAT2 uv;  // uv座標
	};

	// 定数バッファ用データ構造体B1
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient;  //アンビエント係数
		float pad1;        //パディング
		XMFLOAT3 diffuse;  //ディフューズ係数
		float pad2;        //パディング
		XMFLOAT3 specular; //スペキュラー係数
		float alpha;       //アルファ
	};

	struct Material
	{
		std::string name;             //マテリアル名
		XMFLOAT3 ambient;             //アンビエント影響度
		XMFLOAT3 diffuse;             //ディフューズ影響度
		XMFLOAT3 specular;            //スペキュラー影響度
		float alpha;                  //アルファ
		std::string textureFilename;  //テクスチャファイル名
		//コンストラクタ
		Material()
		{
			ambient = { 0.3f, 0.3f, 0.3f };
			diffuse = { 0.0f, 0.0f, 0.0f };
			specular = { 0.0f, 0.0f, 0.0f };
			alpha = 1.0f;
		}

	};

public: // 静的メンバ関数
	
	static Model *LoadFromOBJ(const std::string &modelname);


	static void SetDevice(ID3D12Device *device) { Model::device = device; }


	/// <summary>
	/// デスクリプタヒープの初期化
	/// </summary>
	/// <returns></returns>
	void InitializeDescriptorHeap();

	//各種バッファ生成
	void CreateBuffers();

private: //静的メンバ変数
	//デバイス
	static ID3D12Device *device;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;
	// コマンドリスト
	ID3D12GraphicsCommandList *cmdList;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView;
	//マテリアル
	Material material;



private: //メンバ変数
	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices;
	// 頂点インデックス配列
	std::vector<unsigned short> indices;
	ComPtr<ID3D12Resource> constBuffB1; // 定数バッファ (マテリアル)

private: //非公開のメンバ関数
	// OBJファイルから3Dモデルを読み込む (非公開)
	void LoadFormOBJInternal(const std:: string &modelname);


private: // メンバ関数
	/// <summary>
	/// マテリアル読み込み
	/// </summary>
	void LoadMaterial(const std::string &directoryPath, const std::string &filename);

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTexture(const std::string &directoryPath, const std::string &filename);

public: //メンバ関数
	void Draw(ID3D12GraphicsCommandList *cmdList, UINT rootParamIndexMaterial);
};