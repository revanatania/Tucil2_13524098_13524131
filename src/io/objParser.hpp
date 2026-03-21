#pragma once
#include <string>
#include "../geometry/Mesh.hpp"

class ObjParser {
public:
    static ParseResult parse(const std::string& filePath);

private:
    static bool parseVertexLine(const std::string& line, Vector3D& outVertex);
    static bool parseFaceLine(const std::string& line, int& i, int& j, int& k);
};