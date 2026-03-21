#pragma once
#include <string>
#include "../geometry/Mesh.hpp"

class ObjWriter {
public:
    static bool writeMesh(const std::string& filePath, const Mesh& mesh, std::string& errorMessage);
};