#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>


// 3D���f��

class Model
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // �T�u�N���X	
// ���_�f�[�^�\���́i�e�N�X�`������j
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos; // xyz���W
		XMFLOAT3 normal; // �@���x�N�g��
		XMFLOAT2 uv;  // uv���W
	};

	// �萔�o�b�t�@�p�f�[�^�\����B1
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient;  //�A���r�G���g�W��
		float pad1;        //�p�f�B���O
		XMFLOAT3 diffuse;  //�f�B�t���[�Y�W��
		float pad2;        //�p�f�B���O
		XMFLOAT3 specular; //�X�y�L�����[�W��
		float alpha;       //�A���t�@
	};

	struct Material
	{
		std::string name;             //�}�e���A����
		XMFLOAT3 ambient;             //�A���r�G���g�e���x
		XMFLOAT3 diffuse;             //�f�B�t���[�Y�e���x
		XMFLOAT3 specular;            //�X�y�L�����[�e���x
		float alpha;                  //�A���t�@
		std::string textureFilename;  //�e�N�X�`���t�@�C����
		//�R���X�g���N�^
		Material()
		{
			ambient = { 0.3f, 0.3f, 0.3f };
			diffuse = { 0.0f, 0.0f, 0.0f };
			specular = { 0.0f, 0.0f, 0.0f };
			alpha = 1.0f;
		}

	};

public: // �ÓI�����o�֐�
	
	static Model *LoadFromOBJ(const std::string &modelname);


	static void SetDevice(ID3D12Device *device) { Model::device = device; }


	/// <summary>
	/// �f�X�N���v�^�q�[�v�̏�����
	/// </summary>
	/// <returns></returns>
	void InitializeDescriptorHeap();

	//�e��o�b�t�@����
	void CreateBuffers();

private: //�ÓI�����o�ϐ�
	//�f�o�C�X
	static ID3D12Device *device;
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;
	// �R�}���h���X�g
	ID3D12GraphicsCommandList *cmdList;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView;
	//�}�e���A��
	Material material;



private: //�����o�ϐ�
	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;
	ComPtr<ID3D12Resource> constBuffB1; // �萔�o�b�t�@ (�}�e���A��)

private: //����J�̃����o�֐�
	// OBJ�t�@�C������3D���f����ǂݍ��� (����J)
	void LoadFormOBJInternal(const std:: string &modelname);


private: // �����o�֐�
	/// <summary>
	/// �}�e���A���ǂݍ���
	/// </summary>
	void LoadMaterial(const std::string &directoryPath, const std::string &filename);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	void LoadTexture(const std::string &directoryPath, const std::string &filename);

public: //�����o�֐�
	void Draw(ID3D12GraphicsCommandList *cmdList, UINT rootParamIndexMaterial);
};