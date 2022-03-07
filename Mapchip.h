#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

const int map_size = 1;
const int map_max_x = 52, map_max_y = 12;

class Mapchip
{
public:
	static void CsvToVector(std::vector<std::vector<int>>& mapData, std::string fName);

	static int GetChipNum(int x, int y, std::vector<int> map);

	static void MapCreate(int mapNumber);
};

