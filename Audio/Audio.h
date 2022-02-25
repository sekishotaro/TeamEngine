#pragma once

#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include <cstdint>
#include <wrl.h>
#include <map>
#include <string>


class Audio
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // サブクラス
// チャンクヘッダ
	struct ChunkHeader
	{
		char	id[4]; // チャンク毎のID
		int32_t size;  // チャンクサイズ
	};

	// RIFFヘッダチャンク
	struct RiffHeader
	{
		ChunkHeader	chunk;   // "RIFF"
		char	type[4]; // "WAVE"
	};

	// FMTチャンク
	struct FormatChunk
	{
		ChunkHeader		chunk; // "fmt "
		WAVEFORMATEX	fmt;   // 波形フォーマット
	};


	//音声データ
	struct SoundData
	{
		//波形フォーマット
		WAVEFORMATEX wfex;
		//バッファの先頭アドレス
		BYTE *pBuffer;
		//バッファのサイズ
		unsigned int bufferSize;
		//波形フォーマットを元にSourceVoiceの生成する奴
		IXAudio2SourceVoice *pSourceVoice;
		//ループ再生用フラグ
		bool loop = false;
		//今再生してるか確認フラグ
		bool playNowFlag = false;
	};


	static Audio *GetInstance();

public: //メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Initialize(const std::string &directoryPath = "Resources/");

	/// <summary>
	/// 終了処理
	/// </summary>
	/// <returns>成否</returns>
	void Finalize();

	/// <summary>
	/// WAV音声読み込み
	/// </summary>
	/// <param name="filename">WAVファイル名</param>
	void LoadWave(const std::string &filename);

	/// <summary>
	/// サウンドデータの解放
	/// </summary>
	/// <param name="soundData">サウンドデータ</param>
	void Unload(SoundData *soundData);

	/// <summary>
	/// 音声データ作成
	/// </summary>
	/// <param name="soundData">サウンドデータ</param>
	void CreateSoundData(SoundData &soundData);

	/// <summary>
	/// 音声再生
	/// </summary>
	/// <param name="filename">サウンドデータ名</param>
	void PlayWave(const std::string &filename, bool LoopFlag = false);

	/// <summary>
	/// サウンドを止める
	/// </summary>
	/// <param name="filename"></param>
	void SoundStop(const std::string &filename);

private: // メンバ変数
	
	//XAudio2のインスタンス
	ComPtr<IXAudio2> xAudio2;
	//サウンドデータの連想配列
	std::map<std::string, SoundData> soundDatas_;
	//サウンド格納ディレクトリ
	std::string directoryPath_;

	XAUDIO2_BUFFER buf{};
};
