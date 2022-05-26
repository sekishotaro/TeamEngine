#include "EndScene.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Input.h"
#include "DebugText.h"
#include "DirectXCommon.h"
#include "ConvertScene.h"
#include "GamePlayScene.h"
//#include "math.h"
#include <fstream> 
#include <string>
#include <iostream>
#include <sstream>

void EndScene::Initialize()
{
	// �e�N�X�`���ǂݍ���
	Sprite::LoadTexture(0, L"Resources/Number0.png");
	Sprite::LoadTexture(1, L"Resources/Number1.png");
	Sprite::LoadTexture(2, L"Resources/Number2.png");
	Sprite::LoadTexture(3, L"Resources/Number3.png");
	Sprite::LoadTexture(4, L"Resources/Number4.png");
	Sprite::LoadTexture(5, L"Resources/Number5.png");
	Sprite::LoadTexture(6, L"Resources/Number6.png");
	Sprite::LoadTexture(7, L"Resources/Number7.png");
	Sprite::LoadTexture(8, L"Resources/Number8.png");
	Sprite::LoadTexture(9, L"Resources/Number9.png");
	Sprite::LoadTexture(10, L"Resources/Titlebackground.png");
	Sprite::LoadTexture(21, L"Resources/switch_In.png");
	Sprite::LoadTexture(22, L"Resources/switch_Out.png");
	// �w�i�X�v���C�g����
	spriteBG = Sprite::Create(10, { 0.0f,0.0f });

	ScoreSpriteInitialize(digitsNum, spriteScoreNo1, WinApp::window_width / 3 * 2, 128);
	ScoreSpriteInitialize(digitsNum, spriteScoreNo2, WinApp::window_width / 3 * 2, 256);
	ScoreSpriteInitialize(digitsNum, spriteScoreNo3, WinApp::window_width / 3 * 2, 384);
	ScoreSpriteInitialize(digitsNum, spriteScore, WinApp::window_width / 3 * 2, 512);
	//spriteScore[1] = Sprite::Create(0, { WinApp::window_width - 32,0 });
	//spriteScore[2] = Sprite::Create(0, { WinApp::window_width - 64,0 });
	//spriteScore[3] = Sprite::Create(0, { WinApp::window_width - 96,0 });
	//spriteScore[4] = Sprite::Create(0, { WinApp::window_width - 128,0 });
	//spriteScore[5] = Sprite::Create(0, { WinApp::window_width - 160,0 });
	//spriteScore[6] = Sprite::Create(0, { WinApp::window_width - 192,0 });
	//spriteScore[7] = Sprite::Create(0, { WinApp::window_width - 224,0 });
	//spriteScore[8] = Sprite::Create(0, { WinApp::window_width - 256,0 });

	ConvertScene::InitializeOut();
}

void EndScene::Finalize()
{
	flag = false;
	rankCheckFlag = false;
}

void EndScene::Update()
{
	// �Q�[���V�[���̖��t���[������
	nowPlayScore = GamePlayScene::GetScoreNum();
	LoadText();

	time += 0.166;

	Input* input = Input::GetInstance();


	
	if (flag == false)
	{
		for (int i = 0; i < 3; i++)
		{
			score_listDisplay[i] = score_list[i];
			flag = true;
		}
		RankCheck();
		writeText();
	}
	
	if (rankCheckFlag == true)
	{
		if (rankFluctuationNum == 1)
		{
			ScoreSpriteBB(digitsNum, spriteScoreNo1, nowPlayScore);
			ScoreSpriteBB(digitsNum, spriteScoreNo2, score_listDisplay[0]);
			ScoreSpriteBB(digitsNum, spriteScoreNo3, score_listDisplay[1]);
			ScoreSpriteBB(digitsNum, spriteScore, nowPlayScore);
		}
		else if (rankFluctuationNum == 2)
		{
			ScoreSpriteBB(digitsNum, spriteScoreNo1, score_listDisplay[0]);
			ScoreSpriteBB(digitsNum, spriteScoreNo2, nowPlayScore);
			ScoreSpriteBB(digitsNum, spriteScoreNo3, score_listDisplay[1]);
			ScoreSpriteBB(digitsNum, spriteScore, nowPlayScore);
		}
		else if (rankFluctuationNum == 3)
		{
			ScoreSpriteBB(digitsNum, spriteScoreNo1, score_listDisplay[0]);
			ScoreSpriteBB(digitsNum, spriteScoreNo2, score_listDisplay[1]);
			ScoreSpriteBB(digitsNum, spriteScoreNo3, nowPlayScore);
			ScoreSpriteBB(digitsNum, spriteScore, nowPlayScore);
		}
	}
	else
	{
		ScoreSpriteBB(digitsNum, spriteScoreNo1, score_listDisplay[0]);
		ScoreSpriteBB(digitsNum, spriteScoreNo2, score_listDisplay[1]);
		ScoreSpriteBB(digitsNum, spriteScoreNo3, score_listDisplay[2]);
		ScoreSpriteBB(digitsNum, spriteScore, nowPlayScore);
	}


	
	ConvertScene::besideOut(ConvertFlag);

	if (input->TriggerKey(DIK_RETURN) || input->TriggerButton(Select))
	{
		//�V�[���؂�ւ�
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	DebugText::GetInstance()->Print(50, 20, 3, "fafafafafafafafafafafafafafaf");
	DebugText::GetInstance()->Print(50, 20 * 5, 3, "%d", nowPlayScore);
}

void EndScene::Draw()
{
	// �Q�[���V�[���̕`��

// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCmdList();

	// �w�i�X�v���C�g�`��O����
	Sprite::PreDraw(cmdList);
	// �w�i�X�v���C�g�`��
	spriteBG->Draw();

	/// <summary>
	/// �����ɔw�i�X�v���C�g�̕`�揈����ǉ��ł���
	/// </summary>

	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
	// �[�x�o�b�t�@�N���A
	DirectXCommon::GetInstance()->ClearDepthBuffer();

	// 3D�I�u�W�F�N�g�`��O����
	Object3d::PreDraw(cmdList);

	/// <summary>
	/// ������3D�I�u�W�F�N�g�̕`�揈����ǉ��ł���
	/// </summary>

	// 3D�I�u�W�F�N�g�`��㏈��
	Object3d::PostDraw();

	// �O�i�X�v���C�g�`��O����
	Sprite::PreDraw(cmdList);

	//�X�R�A�̕\��
	if (rankCheckFlag == true && rankFluctuationNum == 1)
	{
		if ((int)time % 5 != 0)
		{
			ScoreSpriteDraw(digitsNum, spriteScoreNo1);
		}
	}
	else
	{
		ScoreSpriteDraw(digitsNum, spriteScoreNo1);
	}

	if (rankCheckFlag == true && rankFluctuationNum == 2)
	{
		if ((int)time % 5 != 0)
		{
			ScoreSpriteDraw(digitsNum, spriteScoreNo2);
		}
	}
	else
	{
		ScoreSpriteDraw(digitsNum, spriteScoreNo2);
	}
	
	if (rankCheckFlag == true && rankFluctuationNum == 3)
	{
		if ((int)time % 5 != 0)
		{
			ScoreSpriteDraw(digitsNum, spriteScoreNo3);
		}
	}
	else
	{
		ScoreSpriteDraw(digitsNum, spriteScoreNo3);
	}
	
	if ((int)time % 5 != 0)
	{
		ScoreSpriteDraw(digitsNum, spriteScore);
	}
	

	// �f�o�b�O�e�L�X�g�̕`��
	DebugText::GetInstance()->DrawAll(cmdList);

	ConvertScene::Draw();

	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
}

void EndScene::writeText()
{
	for (int i = 0; i < 3; i++)
	{
		if (nowPlayScore >= score_list[i])
		{
			if (i == 0)
			{
				score_list[2] = score_list[1];
				score_list[1] = score_list[0];
			}
			else if (i == 1)
			{
				score_list[2] = score_list[1];
			}
			score_list[i] = nowPlayScore;
			break;
		}
	}

	std::string first = std::to_string(score_list[0]);
	std::string second = std::to_string(score_list[1]);
	std::string third = std::to_string(score_list[2]);

	std::ofstream ofs("Resources/ScoreList.txt");
	if (!ofs)
	{
		assert(0);
	}
	else
	{
		ofs << " " << first << " " << second << " " << third;
	}
	ofs.close();
}

void EndScene::LoadText()
{
	std::ifstream file;
	const std::string fileName = "Resources/ScoreList.txt";
	file.open(fileName);
	if (file.fail())
	{
		int tmp = nowPlayScore;
		nowPlayScore = 0;
		writeText();
		nowPlayScore = tmp;
	}

	//1�s���ǂݍ���
	std::string line;
	while (getline(file, line))
	{
		//1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		//���p�X�y�[�X�D�؂�ōs�̐擪��������擾
		std::string key;
		getline(line_stream, key, ' ');

		line_stream >> score_list[0];
		line_stream >> score_list[1];
		line_stream >> score_list[2];
	}
	//�t�@�C�������
	file.close();
}

void EndScene::ScoreSpriteInitialize(int digits, Sprite *scoreSprite[], float x, float y)
{
	for (int i = 1; i < digits + 1; i++)
	{
		scoreSprite[i] = Sprite::Create(0, { x - (32.0f * i), y });
	}
}

void EndScene::ScoreSpriteBB(int digits,Sprite *scoreSprite[], int score)
{
	scoreSprite[1]->ChangeTex((int)score % 10);
	for (int i = 2; i < digits + 1; i++)
	{
		int hundredScore = 0;
		hundredScore = score / pow(10, i - 1);
		scoreSprite[i]->ChangeTex((int)hundredScore % 10);
	}
}

void EndScene::ScoreSpriteDraw(int digits, Sprite *scoreSprite[])
{
	for (int i = 1; i < digits + 1; i++)
	{
		scoreSprite[i]->Draw();
	}
}

void EndScene::RankCheck()
{
	for (int i = 0; i < 3; i++)
	{
		if (rankCheckFlag == false)
		{
			if (nowPlayScore >= score_list[i])
			{
				rankCheckFlag = true;
				rankFluctuationNum = i + 1;
			}
		}
	}
}
