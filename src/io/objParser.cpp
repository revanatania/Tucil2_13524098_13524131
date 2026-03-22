#include "objParser.hpp"
#include <fstream>
#include <sstream>

ParseResult ObjParser::parse(const std::string& filePath) {
    ParseResult result;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        result.errorMessage = "Failed to open file: " + filePath;
        return result;
    }

    Mesh mesh;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;

        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix.empty()) continue;

        if (prefix == "v") {
            Vector3D vertex;
            if (!parseVertexLine(line, vertex)) {
                result.errorMessage = "Invalid vertex at line " + std::to_string(lineNumber);
                return result;
            }
            mesh.vertices.push_back(vertex);
        }
        else if (prefix == "f") {
            int i, j, k;
            if (!parseFaceLine(line, i, j, k)) {
                result.errorMessage = "Invalid face at line " + std::to_string(lineNumber);
                return result;
            }

            if (i < 1 || j < 1 || k < 1 ||
                i > (int)mesh.vertices.size() ||
                j > (int)mesh.vertices.size() ||
                k > (int)mesh.vertices.size()) {
                result.errorMessage = "Face index is out of range at line " + std::to_string(lineNumber);
                return result;
            }

            mesh.triangles.emplace_back(
                mesh.vertices[i - 1],
                mesh.vertices[j - 1],
                mesh.vertices[k - 1]
            );
        }
    }

    if (mesh.vertices.empty()) {
        result.errorMessage = "No vertices are found.";
        return result;
    }

    if (mesh.triangles.empty()) {
        result.errorMessage = "No triangles are found.";
        return result;
    }

    result.success = true;
    result.mesh = std::move(mesh);
    return result;
}

bool ObjParser::parseVertexLine(const std::string& line, Vector3D& outVertex) {
    std::istringstream iss(line);
    std::string prefix;
    double x, y, z;

    if (!(iss >> prefix >> x >> y >> z)) return false;

    std::string extra;
    if (iss >> extra) return false;

    outVertex = Vector3D(x, y, z);
    return true;
}

bool ObjParser::parseFaceLine(const std::string& line, int& i, int& j, int& k) {
    std::istringstream iss(line);
    std::string prefix;

    if (!(iss >> prefix >> i >> j >> k)) return false;

    std::string extra;
    if (iss >> extra) return false;

    return true;
}