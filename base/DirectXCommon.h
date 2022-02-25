#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3dx12.h>
#include <cstdlib>
#include "WinApp.h"

class DirectXCommon
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:

	static DirectXCommon *GetInstance();

	//メンバ関数	
	/// <summary>
	/// 全てのDirectXの初期化
	/// </summary>
	/// <param name="winApp"></param>
	void Initialize(WinApp *winApp);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// レンダーターゲットのクリア
	/// </summary>
	void ClearRenderTarget();

	/// <summary>
	/// 深度バッファのクリア
	/// </summary>
	void ClearDepthBuffer();

	/// <summary>
	/// デバイス取得
	/// </summary>
	/// <returns>デバイス</returns>
	ID3D12Device *GetDev() { return dev.Get(); }

	/// <summary>
	/// コマンドリスト取得
	/// </summary>
	/// <returns></returns>
	ID3D12GraphicsCommandList *GetCmdList() { return cmdList.Get(); }



private:
	//メンバ変数
	//WindowsAPI
	WinApp *winApp = nullptr;

	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<IDXGISwapChain4> swapchain;
	ComPtr<ID3D12Resource> depthBuffer;
	ComPtr<ID3D12Fence> fence;
	//フェンス値
	UINT64 fenceVal = 0;
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	//デバイス
	ComPtr<ID3D12Device> dev;
	// DXGIファクトリー
	ComPtr<IDXGIFactory6> dxgiFactory;
	//バックバッファ
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	
	//メンバ関数
private:

	//フェンスの初期化
	void InitializeFence();
	/// <summary>
	/// レンダーターゲットビューのクリア
	/// </summary>
	void InitializeRenderTargetView();

	/// <summary>
	/// 深度バッファの初期化
	/// </summary>
	void InitializeDepthBuffer();

	/// <summary>
	/// DXGIデバイス初期化
	/// </summary>
	/// <returns>成否</returns>
	bool InitializeDevice();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	/// <returns>成否</returns>
	bool CreateSwapChain();

	/// <summary>
	/// コマンド関連初期化
	/// </summary>
	/// <returns>成否</returns>
	bool InitializeCommand();

	/// <summary>
	/// レンダーターゲット生成
	/// </summary>
	/// <returns>成否</returns>
	bool CreateFinalRenderTargets();

	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	/// <returns>成否</returns>
	bool CreateDepthBuffer();

	/// <summary>
	/// フェンス生成
	/// </summary>
	/// <returns>成否</returns>
	bool CreateFence();


};