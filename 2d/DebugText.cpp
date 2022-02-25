#include "DebugText.h"

DebugText *DebugText::GetInstance()
{
	static DebugText instance;

	return &instance;
}


void DebugText::Initialize(UINT texnumber)
{
	// 全てのスプライトデータについて
	for (int i = 0; i < _countof(spriteDatas); i++)
	{
		// スプライトを生成する
		spriteDatas[i] = Sprite::Create(texnumber, { 0,0 });
	}
}

void DebugText::Finalize()
{
	for (int i = 0; i < _countof(spriteDatas); i++) {
		delete spriteDatas[i];
	}
}

// 1文字列追加
void DebugText::Print(float x, float y, float scale, std::string text, ...)
{
	bool parsentFlag = false;
	va_list args;
	va_start(args, text);
	text.push_back(' ');
	for (size_t i = 0; text[i] != '\0'; ++i)
	{
		printf("%c:", text[i]);
		if (text[i] == '%')
		{
			parsentFlag = true;
		}
		if (parsentFlag == true)
		{
			int temp;
			double tempF = NULL;
			switch (text[i])
			{
			case 'd':
				temp = va_arg(args, int);
				text.replace((size_t)i - 1, 2, std::to_string(temp));
				parsentFlag = false;
				break;
			case 'f':
				tempF = va_arg(args, double);
				tempF = round(tempF * 100);
				tempF = tempF / 100;
				text.replace((size_t)i - 1, 2, std::to_string(tempF));
				parsentFlag = false;
				break;
			default:
				break;
			}
		}
	}
	va_end(args);

	// 全ての文字について
	for (int i = 0; i < text.size(); i++)
	{
		// 最大文字数超過
		if (spriteIndex >= maxCharCount) {
			break;
		}

		// 1文字取り出す(※ASCIIコードでしか成り立たない)
		const unsigned char &character = text[i];

		int fontIndex = character - 32;
		if (character >= 0x7f) {
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		// 座標計算
		spriteDatas[spriteIndex]->SetPosition({ x + fontWidth * scale * i, y });
		spriteDatas[spriteIndex]->SetTextureRect({ (float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight }, { (float)fontWidth, (float)fontHeight });
		spriteDatas[spriteIndex]->SetSize({ fontWidth * scale, fontHeight * scale });

		// 文字を１つ進める
		spriteIndex++;
	}
}

// まとめて描画
void DebugText::DrawAll(ID3D12GraphicsCommandList* cmdList)
{
	// 全ての文字のスプライトについて
	for (int i = 0; i < spriteIndex; i++)
	{
		// スプライト描画
		spriteDatas[i]->Draw();
	}

	spriteIndex = 0;
}