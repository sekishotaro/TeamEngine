#include "Count.h"
#include <safe_delete.h>

//Sprite* Count::count0;
Sprite* Count::count1;
Sprite* Count::count2;
Sprite* Count::count3;
//Sprite* Count::count4;
//Sprite* Count::count5;
Sprite* Count::countStart;
Sprite* Count::countFinish;
float Count::count3Time = 4.0f;
bool Count::count3finishFlag = false;
float Count::countFinishTime = 2.0f;
bool Count::fnishFinishFlag = false;

void Count::Initilize()
{
	count1 = Sprite::Create(24, { 1280.0f/ 2, 720.0f/ 2 });
	count1->SetAnchorPoint({ 0.5f, 0.5f });
	count2 = Sprite::Create(25, { 1280.0f / 2, 720.0f / 2 });
	count2->SetAnchorPoint({ 0.5f, 0.5f });
	count3 = Sprite::Create(26, { 1280.0f / 2, 720.0f / 2 });
	count3->SetAnchorPoint({ 0.5f, 0.5f });
	countStart = Sprite::Create(27, { 0.0f, 0.0f });
	countFinish = Sprite::Create(23, { 0.0f, 0.0f });
}

void Count::Finalize()
{
	count3Time = 4.0f;
	count3finishFlag = false;
	countFinishTime = 2.0f;
	fnishFinishFlag = false;
	safe_delete(count1);
	safe_delete(count2);
	safe_delete(count3);
	safe_delete(countStart);
	safe_delete(countFinish);
}

void Count::CountDown3(bool &finishFlag)
{

	if (finishFlag == true)
	{
		return;
	}
	else
	{
		count3finishFlag = false;
	}

	count3Time -= 0.016f;

	if (count3Time < 0.2f)
	{
		finishFlag = true;
		count3finishFlag = true;
		count3Time = 12.0f;
		count1->SetSize({ 100.0f, 100.0f });
		count2->SetSize({ 100.0f, 100.0f });
		count3->SetSize({ 100.0f, 100.0f });
	}


	if (3.0f < count3Time)
	{
		if (3.0f < count3Time && count3Time < 3.5f)
		{
			XMFLOAT2 size = count3->GetSize();

			count3->SetSize({ size.x, size.y - 22.0f });
		}
	}
	else if (2.0f < count3Time && count3Time <= 3.0f)
	{
		if (2.0f < count3Time && count3Time < 2.5f)
		{
			XMFLOAT2 size = count2->GetSize();

			count2->SetSize({ size.x, size.y - 22.0f });
		}
	}
	else if (1.0f < count3Time && count3Time <= 2.0f)
	{
		if (1.0f < count3Time && count3Time < 1.5f)
		{
			XMFLOAT2 size = count1->GetSize();

			count1->SetSize({ size.x, size.y - 22.0f });
		}
		
	}
}

void Count::CountDown3Draw()
{
	if (3.0f < count3Time)
	{
		count3->Draw();
	}
	else if (2.0f < count3Time && count3Time <= 3.0f)
	{
		count2->Draw();
	}
	else if (1.0f < count3Time && count3Time <= 2.0f)
	{
		count1->Draw();
	}
	else if (0.0f <= count3Time && count3Time <= 1.0f)
	{
		countStart->Draw();
	}
}

void Count::Fnish(bool& finishFlag, const float& x)
{
	countFinishTime -= 0.016f;
	if (x < 0)
	{
		fnishFinishFlag = true;
	}

	if (countFinishTime < 0)
	{
		finishFlag = true;
		fnishFinishFlag = false;
	}
}

void Count::FnishDraw()
{
	countFinish->Draw();
}

void Count::Draw()
{
	if (count3finishFlag == false)
	{
		Count::CountDown3Draw();
	}

	if (fnishFinishFlag == true)
	{
		Count::FnishDraw();
	}
}




