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
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:

	static DirectXCommon *GetInstance();

	//�����o�֐�	
	/// <summary>
	/// �S�Ă�DirectX�̏�����
	/// </summary>
	/// <param name="winApp"></param>
	void Initialize(WinApp *winApp);

	/// <summary>
	/// �`��O����
	/// </summary>
	void PreDraw();

	/// <summary>
	/// �`��㏈��
	/// </summary>
	void PostDraw();

	/// <summary>
	/// �����_�[�^�[�Q�b�g�̃N���A
	/// </summary>
	void ClearRenderTarget();

	/// <summary>
	/// �[�x�o�b�t�@�̃N���A
	/// </summary>
	void ClearDepthBuffer();

	/// <summary>
	/// �f�o�C�X�擾
	/// </summary>
	/// <returns>�f�o�C�X</returns>
	ID3D12Device *GetDev() { return dev.Get(); }

	/// <summary>
	/// �R�}���h���X�g�擾
	/// </summary>
	/// <returns></returns>
	ID3D12GraphicsCommandList *GetCmdList() { return cmdList.Get(); }



private:
	//�����o�ϐ�
	//WindowsAPI
	WinApp *winApp = nullptr;

	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<IDXGISwapChain4> swapchain;
	ComPtr<ID3D12Resource> depthBuffer;
	ComPtr<ID3D12Fence> fence;
	//�t�F���X�l
	UINT64 fenceVal = 0;
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	//�f�o�C�X
	ComPtr<ID3D12Device> dev;
	// DXGI�t�@�N�g���[
	ComPtr<IDXGIFactory6> dxgiFactory;
	//�o�b�N�o�b�t�@
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	
	//�����o�֐�
private:

	//�t�F���X�̏�����
	void InitializeFence();
	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[�̃N���A
	/// </summary>
	void InitializeRenderTargetView();

	/// <summary>
	/// �[�x�o�b�t�@�̏�����
	/// </summary>
	void InitializeDepthBuffer();

	/// <summary>
	/// DXGI�f�o�C�X������
	/// </summary>
	/// <returns>����</returns>
	bool InitializeDevice();

	/// <summary>
	/// �X���b�v�`�F�[���̐���
	/// </summary>
	/// <returns>����</returns>
	bool CreateSwapChain();

	/// <summary>
	/// �R�}���h�֘A������
	/// </summary>
	/// <returns>����</returns>
	bool InitializeCommand();

	/// <summary>
	/// �����_�[�^�[�Q�b�g����
	/// </summary>
	/// <returns>����</returns>
	bool CreateFinalRenderTargets();

	/// <summary>
	/// �[�x�o�b�t�@����
	/// </summary>
	/// <returns>����</returns>
	bool CreateDepthBuffer();

	/// <summary>
	/// �t�F���X����
	/// </summary>
	/// <returns>����</returns>
	bool CreateFence();


};