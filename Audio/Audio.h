#pragma once

#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include <cstdint>
#include <wrl.h>
#include <map>
#include <string>


class Audio
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // �T�u�N���X
// �`�����N�w�b�_
	struct ChunkHeader
	{
		char	id[4]; // �`�����N����ID
		int32_t size;  // �`�����N�T�C�Y
	};

	// RIFF�w�b�_�`�����N
	struct RiffHeader
	{
		ChunkHeader	chunk;   // "RIFF"
		char	type[4]; // "WAVE"
	};

	// FMT�`�����N
	struct FormatChunk
	{
		ChunkHeader		chunk; // "fmt "
		WAVEFORMATEX	fmt;   // �g�`�t�H�[�}�b�g
	};


	//�����f�[�^
	struct SoundData
	{
		//�g�`�t�H�[�}�b�g
		WAVEFORMATEX wfex;
		//�o�b�t�@�̐擪�A�h���X
		BYTE *pBuffer;
		//�o�b�t�@�̃T�C�Y
		unsigned int bufferSize;
		//�g�`�t�H�[�}�b�g������SourceVoice�̐�������z
		IXAudio2SourceVoice *pSourceVoice;
		//���[�v�Đ��p�t���O
		bool loop = false;
		//���Đ����Ă邩�m�F�t���O
		bool playNowFlag = false;
	};


	static Audio *GetInstance();

public: //�����o�֐�

	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����</returns>
	bool Initialize(const std::string &directoryPath = "Resources/");

	/// <summary>
	/// �I������
	/// </summary>
	/// <returns>����</returns>
	void Finalize();

	/// <summary>
	/// WAV�����ǂݍ���
	/// </summary>
	/// <param name="filename">WAV�t�@�C����</param>
	void LoadWave(const std::string &filename);

	/// <summary>
	/// �T�E���h�f�[�^�̉��
	/// </summary>
	/// <param name="soundData">�T�E���h�f�[�^</param>
	void Unload(SoundData *soundData);

	/// <summary>
	/// �����f�[�^�쐬
	/// </summary>
	/// <param name="soundData">�T�E���h�f�[�^</param>
	void CreateSoundData(SoundData &soundData);

	/// <summary>
	/// �����Đ�
	/// </summary>
	/// <param name="filename">�T�E���h�f�[�^��</param>
	void PlayWave(const std::string &filename, bool LoopFlag = false);

	/// <summary>
	/// �T�E���h���~�߂�
	/// </summary>
	/// <param name="filename"></param>
	void SoundStop(const std::string &filename);

private: // �����o�ϐ�
	
	//XAudio2�̃C���X�^���X
	ComPtr<IXAudio2> xAudio2;
	//�T�E���h�f�[�^�̘A�z�z��
	std::map<std::string, SoundData> soundDatas_;
	//�T�E���h�i�[�f�B���N�g��
	std::string directoryPath_;

	XAUDIO2_BUFFER buf{};
};
