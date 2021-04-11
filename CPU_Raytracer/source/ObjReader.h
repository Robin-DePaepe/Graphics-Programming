#pragma once
#include <string>
#include "Structs.h"


class ObjReader
{
public:
	//functions
	ObjData ReadFile(const std::string& filename);
private:
	//datamembers
	std::vector<std::string> m_Numbers;

	//functions
	void GetNumbersOutLine(const std::string& line);
	size_t GetNextSpace(const std::string& line, size_t currentPos) const;
	bool IsLineValid(const std::string& line) const;
};

