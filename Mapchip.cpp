#include "Mapchip.h"

//CSVファイル読み込み
void Mapchip::CsvToVector(std::vector<std::vector<int>>& mapData, std::string fName) {
	std::ifstream ifs(fName);//ファイルネームの確認
	std::string line;
	std::vector<int> csv;
	while (getline(ifs, line))
	{
		std::istringstream stream(line);
		std::string field;
		std::vector<int> result;
		while (std::getline(stream, field, ','))//カンマ区切りで
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
int Mapchip::GetChipNum(float x, float y, std::vector<int> map)
{
	const float X = x / map_size;
	const float Y = y / map_size;

	if ((X < 0 || X >= map_max_x) || (Y < 0 || Y >= map_max_y))
	{
		return 0;
	}

	//ここの警告は気にしないでOK
	return map[Y * map_max_x + X];
}