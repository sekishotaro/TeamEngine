#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

class Mapchip
{
public:
	void CsvToVector(std::vector<std::vector<int>>& mapData, std::string fName);

	int GetChipNum(int x, int y, std::vector<int> map);

	void MapCreate(int mapNumber);
};

