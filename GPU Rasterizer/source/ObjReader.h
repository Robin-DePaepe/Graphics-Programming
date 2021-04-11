#pragma once
#include <string>
#include "Mesh.h"

class ObjReader
{
public:
	void ReadFile(const std::string& filename, Mesh& mesh);
private:
	//datamembers
	std::vector<std::string> m_Numbers;
	std::vector<FPoint3> m_Vertices;
	std::vector<FVector3> m_VertexNormals;
	std::vector<FVector2> m_VertexTextures;

	//functions
	void GetNumbersOutLine(const std::string& line);
	bool IsLineValid(const std::string& line);
	void ParseVertex(const std::string& data);
	void ParseVertexNormal(const std::string& data);
	void ParseVertexTexture(const std::string& data);
	void ParseFace(const std::string& data, std::vector<std::vector<int>>& numbers);
};

