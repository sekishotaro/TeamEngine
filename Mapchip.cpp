#include "Mapchip.h"

//CSV�t�@�C���ǂݍ���
void Mapchip::CsvToVector(std::vector<std::vector<int>>& mapData, std::string fName) {
	std::ifstream ifs(fName);//�t�@�C���l�[���̊m�F
	std::string line;
	std::vector<int> csv;
	while (getline(ifs, line))
	{
		std::istringstream stream(line);
		std::string field;
		std::vector<int> result;
		while (std::getline(stream, field, ','))//�J���}��؂��
		{
			result.push_back(stoi(field));
		}
		for (auto i : result)
		{
			csv.push_back(i);
		}
	}
	mapData.push_back(csv);
}

//�}�b�v�i���o�[�w��
int Mapchip::GetChipNum(int x, int y, std::vector<int> map)
{
	const int X = x / map_size;
	const int Y = y / map_size;

	if ((X < 0 || X >= map_max_x) || (Y < 0 || Y >= map_max_y))
	{
		return 0;
	}

	//�����̌x���͋C�ɂ��Ȃ���OK
	return map[Y * map_max_x + X];
}

void Mapchip::SetChipNum(int x, int y, std::vector<int> map)
{
	const int X = x / map_size;
	const int Y = y / map_size;

	if (map[Y * map_max_x + X] = 0)
	{
		map[Y * map_max_x + X] = 1;
	}
	else
	{
		map[Y * map_max_x + X] = 0;
	}
}
