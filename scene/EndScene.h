#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include <DirectXMath.h>

class EndScene : public BaseScene
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;


public:

	/// <summary>
	/// ������
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// ���
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// �A�b�v�f�[�g
	/// </summary>
	void Update() override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() override;



	/// <summary>
	/// �Q�[���V�[���p
	/// </summary>
	Sprite* spriteBG = nullptr;
	Sprite* totitle = nullptr;
	Sprite* newrecord = nullptr;
	Sprite* spriteScore[10]; //�X�R�A
	Sprite* spriteScoreNo1[10]; //���1�ʃX�R�A
	Sprite* spriteScoreNo2[10]; //���2�ʃX�R�A
	Sprite* spriteScoreNo3[10]; //���3�ʃX�R�A

	//�V�[���ڍs�����m�F�t���O
	bool ConvertFlag = false;

	float time = 0.0f;

	//�X�R�A
	int nowPlayScore;
	int score_list[3] = { 0, 0, 0 };
	int score_listDisplay[3] = { 0, 0, 0 };
	const int digitsNum = 8; //8��

	bool flag = false;
	bool rankCheckFlag = false;
	int rankFluctuationNum = 0;
	
	void writeText();
	void LoadText();


	void ScoreSpriteInitialize(int digits, Sprite* scoreSprite[], float x, float y);

	/// <summary>
	/// �X�R�A�X�v���C�g�\���̏���
	/// </summary>
	/// <param name="digits">����</param>
	void ScoreSpriteBB(int digits, Sprite *scoreSprite[], int score);

	/// <summary>
	/// �X�R�A�̕`��
	/// </summary>
	/// <param name="digits">����</param>
	/// <param name="scoreSprite">�w��X�R�A�̃X�v���C�g</param>
	void ScoreSpriteDraw(int digits, Sprite* scoreSprite[]);

	//�����N�`�F�b�N
	void RankCheck();
};


