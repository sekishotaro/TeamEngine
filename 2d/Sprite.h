#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

class Sprite
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: //�e�\����
	
	/// <summary>
	/// ���_�f�[�^�\����
	/// </summary>
	struct VertexPosUv
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	/// <summary>
	/// �萔�o�b�t�@�p�f�[�^�\����
	/// </summary>
	struct ConstBufferData
	{
		XMFLOAT4 color;
		XMMATRIX mat;
	};

public: //�ÓI�����o�֐�

	/// <summary>
	/// ������(�`��O)
	/// </summary>
	/// <param name="dev">�f�o�C�X</param>
	/// <param name="window_width">��ʕ�</param>
	/// <param name="window_height">��ʍ���</param>
	/// <returns>����</returns>
	static bool StaticInitialize(ID3D12Device *dev, int window_width, int window_height);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <param name="texnumber">�e�N�X�`���ԍ�</param>
	/// <param name="filename">�摜�t�@�C����</param>
	static void LoadTexture(UINT texnumber, const wchar_t *filename);

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	static void PreDraw(ID3D12GraphicsCommandList *cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// �X�v���C�g����
	/// </summary>
	/// <param name="texNumber">�e�N�X�`���ԍ�</param>
	/// <param name="position">���W</param>
	/// <param name="color">�F</param>
	/// <param name="anchorpoint">�A���J�[�|�C���g</param>
	/// <param name="isFlipX">���E���]</param>
	/// <param name="isFlipY">�㉺���]</param>
	/// <returns>�������ꂽ�X�v���C�g</returns>
	static Sprite *Create(UINT texNumber, XMFLOAT2 position, XMFLOAT4 color = { 1, 1, 1, 1 }, XMFLOAT2 anchorpoint = { 0.0f, 0.0f }, bool isFlipX = false, bool isFlipY = false);







private: //�ÓI�����o�ϐ�

	//�f�o�C�X
	static ID3D12Device *dev;
	//�R�}���h���X�g
	static ID3D12GraphicsCommandList *cmdList;
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;
	// �e�N�X�`���̍ő喇��
	static const int spriteSRVCount = 512;
	// ���_��
	static const int vertNum = 4;
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootSignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelineState;
	// �ˉe�s��
	static XMMATRIX matProjection;
	// �f�X�N���v�^�q�[�v
	static ComPtr<ID3D12DescriptorHeap> descHeap;
	// �e�N�X�`���o�b�t�@
	static ComPtr<ID3D12Resource> texBuff[spriteSRVCount];

public: //�����o�֐�

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Sprite(UINT texNumber, XMFLOAT2 position, XMFLOAT2 size, XMFLOAT4 color, XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY);

	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����</returns>
	bool Initialize();

	/// <summary>
	/// �p�x�̐ݒ�
	/// </summary>
	/// <param name="rotation">�p�x</param>
	void SetRotation(float rotation);

	/// <summary>
	/// ���W�̐ݒ�
	/// </summary>
	/// <param name="position">���W</param>
	void SetPosition(XMFLOAT2 position);

	/// <summary>
	/// �T�C�Y�̐ݒ�
	/// </summary>
	/// <param name="size">�T�C�Y</param>
	void SetSize(XMFLOAT2 size);

	/// <summary>
	/// �A���J�[�|�C���g�̐ݒ�
	/// </summary>
	/// <param name="anchorpoint">�A���J�[�|�C���g</param>
	void SetAnchorPoint(XMFLOAT2 anchorpoint);

	/// <summary>
	/// ���E���]�̐ݒ�
	/// </summary>
	/// <param name="isFlipX">���E���]</param>
	void SetIsFlipX(bool isFlipX);

	/// <summary>
	/// �㉺���]�̐ݒ�
	/// </summary>
	/// <param name="isFlipX">�㉺���]</param>
	void SetIsFlipY(bool isFlipY);

	/// <summary>
	/// �e�N�X�`���͈͐ݒ�
	/// </summary>
	/// <param name="texLeftTop">�e�N�X�`��������W</param>
	/// <param name="texSize">�e�N�X�`���T�C�Y</param>
	void SetTextureRect(XMFLOAT2 texLeftTop, XMFLOAT2 texSize);

	void SetIsInvisible(bool isInvisible);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();



private: //�����o�ϐ�
	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	//Z�����̉�]�p
	float rotation = 0.0f;
	//���W
	XMFLOAT2 position = { 0,0};
	//���[���h�s��
	XMMATRIX matWorld{};
	//�F(RGBA)
	XMFLOAT4 color = { 1, 1, 1, 1 };
	//�e�N�X�`���ԍ�
	UINT texNumber = 0;
	//�傫��
	XMFLOAT2 size = { 100,100 };
	//�A���J�[�|�C���g
	XMFLOAT2 anchorpoint = { 0.0f, 0.0f };
	//���E���]
	bool isFlipX = false;
	//�㉺���]
	bool isFlipY = false;
	//�e�N�X�`���̍�����W
	XMFLOAT2 texLeftTop = { 0,0 };
	//�e�N�X�`���̐؂�o���T�C�Y
	XMFLOAT2 texSize = { 100,100 };
	//��\��
	bool isInvisible = false;

private: // �����o�֐�
	/// <summary>
	/// ���_�f�[�^�]��
	/// </summary>
	void TransferVertices();
};