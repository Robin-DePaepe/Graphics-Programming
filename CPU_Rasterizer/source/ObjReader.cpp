#include "catch.hpp"
#include <regex>
#include "ObjReader.h"
#include <iostream>
#include <fstream>
#include "EMath.h"

using namespace Elite;

void ObjReader::ReadFile(const std::string& filename, Mesh& mesh)
{
	std::cout << "Attempting to parse file: " << filename << std::endl;

	std::ifstream reader{ filename };
	std::string line{};
	int lineNumber{ 0 }, skippedLines{ 0 };

	if (reader.is_open())
	{
		std::cout << "Parsing started.\n";

		while (!reader.eof())
		{
			std::getline(reader, line);

			if (IsLineValid(line))
			{
				//faces
				if (line[0] == 'f')
				{
					std::vector<std::vector<int>> numbers{};
					std::vector<int> indices;

					ParseFace(line, numbers);

					//add face to mesh
					for (size_t i = 0; i < numbers.size(); i++) //loop over the vertices
					{
						InputVertex vertex{ m_Vertices.at(numbers[i][0] - 1),m_VertexTextures.at(numbers[i][1] - 1),m_VertexNormals.at(numbers[i][2] - 1) };

						std::pair<bool, int> dublicate{ mesh.CheckForDublicate(vertex) };

						if (!dublicate.first) //test if its a  dublicates
						{
							indices.push_back(int((mesh.AddPoint(vertex)))); //not a dublicate
						}
						else
						{
							indices.push_back(dublicate.second); //dublicate
						}
					}
					mesh.AddTriangeIndices(IPoint3{ indices.at(0),indices.at(1),indices.at(2) });
				}

				else if (line[0] == 'v')
				{
					// vertex normals
					if (line[1] == 'n')
					{
						ParseVertexNormal(line);
					}
					//uv coordiantes
					else if (line[1] == 't')
					{
						ParseVertexTexture(line);
					}
					//vertices
					else if (line[1] == ' ')
					{
						ParseVertex(line);
					}
				}
				++lineNumber;
			}
			else
			{
				std::cout << "Line " << (lineNumber + skippedLines) << " is invaled and is ignored \n";
				++skippedLines;
			}
		}
		reader.close();
		std::cout << "Parsing has ended.\n";
	}
	else
	{
		std::cout << "Parsing failed.\n";
	}
	//calculate the tangents after all the vertecis are added
	mesh.CalculateAndInitializeVertexTangent();
}

void ObjReader::GetNumbersOutLine(const std::string& line) // parses the numbers of a line and places it in m_Numbers
{
	m_Numbers.clear();

	size_t start{ line.find(' ') };

	while (start < line.size() - 1)
	{
		while (line.at(start + 1) == ' ' && start < line.size()) //check for multple spaces
		{
			start = line.find(' ', start + 1);
		}
		size_t end{ line.find(' ',start + 1) };

		std::string subString{ line.substr(start + 1, end - start - 1) };
		size_t backslashPos{ subString.find('/') };

		if (backslashPos < line.size())
		{
			while (backslashPos < line.size() - 1)
			{
				subString.replace(backslashPos, 1, " ");
				backslashPos = subString.find('/', backslashPos + 1);
			}
		}
		m_Numbers.push_back(subString);

		start = end;
	}
}

bool ObjReader::IsLineValid(const std::string& line)
{
	std::vector< std::regex> checks;
	checks.push_back(std::regex("vn?t?\\s*(-?\\d+\\.?\\d*e?-?\\d*\\s*){3}\\s*")); //vertex , vertex normal and vertex texture regex
	checks.push_back(std::regex("f\\s((\\d+/?\\s*){1,}){1,}\\s*")); //face regex
	checks.push_back(std::regex("#.*")); //comments regex

	for (const std::regex& regex : checks)
	{
		if (regex_match(line, regex))
		{
			return true;
		}
	}
	return false;
}

void ObjReader::ParseVertex(const std::string& data)
{
	FPoint3 numbers;
	GetNumbersOutLine(data);

	numbers.x = std::stof(m_Numbers[0]);
	numbers.y = std::stof(m_Numbers[1]);
	numbers.z = std::stof(m_Numbers[2]);

	m_Vertices.push_back(numbers);
}

void ObjReader::ParseVertexNormal(const std::string& data)
{
	FVector3 numbers{};

	GetNumbersOutLine(data);

	numbers.x = std::stof(m_Numbers[0]);
	numbers.y = std::stof(m_Numbers[1]);
	numbers.z = std::stof(m_Numbers[2]);

	Normalize(numbers);

	m_VertexNormals.push_back(numbers);
}

void ObjReader::ParseVertexTexture(const std::string& data)
{
	FVector2 numbers{};

	GetNumbersOutLine(data);

	numbers.x = std::stof(m_Numbers[0]);
	numbers.y = 1.f - std::stof(m_Numbers[1]);

	m_VertexTextures.push_back(numbers);
}

void ObjReader::ParseFace(const std::string& data, std::vector<std::vector<int>>& numbers)
{
	GetNumbersOutLine(data);

	for (const std::string& number : m_Numbers) //each number is a vertex
	{
		std::vector<int> currentNumbers;
		size_t space{ number.find(' ') };

		//seperate the indexes 
		currentNumbers.push_back(std::stoi(number.substr(0, space)));

		while (space < number.size() - 1)
		{
			size_t nextSpace = number.find(' ', space + 1);
			currentNumbers.push_back(std::stoi(number.substr(space + 1, nextSpace - space + 1)));
			space = nextSpace;
		}
		numbers.push_back(currentNumbers);
	}
}



