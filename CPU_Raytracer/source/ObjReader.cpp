#include "pch.h"
#include "catch.hpp"
#include <regex>
#include "ObjReader.h"
#include <iostream>
#include <fstream>

ObjData ObjReader::ReadFile(const std::string& filename)
{
	std::ifstream reader{ filename };
	std::string line{};
	ObjData data{};

	if (reader.is_open())
	{
		while (!reader.eof())
		{
			std::getline(reader, line);

			if (IsLineValid(line))
			{
				if (line.size())
				{
					if (line[0] == 'f')
					{
						Elite::IPoint3 numbers{};
						GetNumbersOutLine(line);

						numbers.x = std::stoi(m_Numbers[0]);
						numbers.y = std::stoi(m_Numbers[1]);
						numbers.z = std::stoi(m_Numbers[2]);

						data.m_Faces.push_back(numbers);
					}
					else if (line[0] == 'v')
					{
						Elite::FPoint3 numbers{};
						GetNumbersOutLine(line);

						numbers.x = std::stof(m_Numbers[0]);
						numbers.y = std::stof(m_Numbers[1]);
						numbers.z = std::stof(m_Numbers[2]);

						data.m_Vertexes.push_back(numbers);
					}
				}
			}
		}

		reader.close();
	}
	return data;
}

void ObjReader::GetNumbersOutLine(const std::string& line) 
{
	m_Numbers.clear();
	size_t start{ line.find(' ') };

	while (start < line.size() - 1)
	{
		while (line.at(start + 1) == ' ' && start < line.size()) //check for double spaces
		{
			start = GetNextSpace(line, start);
		}
		size_t end{ line.find(' ',start + 1) };

		m_Numbers.push_back(line.substr(start + 1, end - start - 1));

		start = end;
	}
}

size_t ObjReader::GetNextSpace(const std::string& line, size_t currentPos) const
{
	return line.find(' ', currentPos + 1);
}

bool ObjReader::IsLineValid(const std::string& line) const
{
	std::vector< std::regex> checks;
	checks.push_back(std::regex("v\\s*(-?\\d+\\.?\\d*e?-?\\d*\\s*){3}\\s*")); //vertex regex
	checks.push_back(std::regex("f\\s((\\d+/?\\s*){1,}){1,}\\s*")); //face regex

	for (const std::regex& regex : checks)
	{
		if (regex_match(line, regex))
		{
			return true;
		}
	}
	return false;
}





