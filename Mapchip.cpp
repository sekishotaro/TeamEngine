#include "Mapchip.h"

const int map_size = 1;
const int map_max_x = 47, map_max_y = 7;

//CSVファイル読み込み
void Mapchip::CsvToVector(std::vector<std::vector<int>>& mapData, std::string fName) {
	std::ifstream ifs(fName);
	std::string line;
	std::vector<int> csv;
	while (getline(ifs, line))
	{
		std::istringstream stream(line);
		std::string field;
		std::vector<int> result;
		while (std::getline(stream, field, ','))
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

//マップナンバー指定
int Mapchip::GetChipNum(int x, int y, std::vector<int> map)
{
	const int X = x / map_size;
	const int Y = y / map_size;

	if (X < 0 || X >= map_max_x || Y < 0 || Y >= map_max_y)
	{
		return 0;
	}

	return map[Y * map_max_x + X];
}

void Mapchip::MapCreate(int mapNumber)
{
}
