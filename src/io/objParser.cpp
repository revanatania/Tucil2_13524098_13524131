#include "objParser.hpp"
#include <fstream>
#include <sstream>

using namespace std;

ParseResult ObjParser::parse(const string& filePath) {
    ParseResult result;

    ifstream file(filePath);
    if (!file.is_open()) {
        result.errorMessage = "Failed to open file: " + filePath;
        return result;
    }

    Mesh mesh;
    string line;
    int lineNumber = 0;

    while (getline(file, line)) {
        ++lineNumber;

        if (line.empty()) continue;

        istringstream iss(line);
        string prefix;
        iss >> prefix;

        if (prefix.empty()) continue;

        if (prefix == "v") {
            Vector3D vertex;
            if (!parseVertexLine(line, vertex)) {
                result.errorMessage = "Invalid vertex at line " + to_string(lineNumber);
                return result;
            }
            mesh.vertices.push_back(vertex);
        }
        else if (prefix == "f") {
            vector<int> indices;
            if (!parseFaceLine(line, indices)) {
                result.errorMessage = "Invalid face at line " + to_string(lineNumber);
                return result;
            }

            // Validasi semua index
            for (int idx : indices) {
                if (idx < 1 || idx > (int)mesh.vertices.size()) {
                    result.errorMessage = "Face index out of range at line " + to_string(lineNumber);
                    return result;
                }
            }

            // Fan triangulation: buat segitiga 1 triangle, buat quad 2 triangle, dst.
            for (int i = 1; i < (int)indices.size() - 1; i++) {
                mesh.triangles.emplace_back(
                    mesh.vertices[indices[0] - 1],
                    mesh.vertices[indices[i] - 1],
                    mesh.vertices[indices[i + 1] - 1]
                );
            }
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
    result.mesh = move(mesh);
    return result;
}

bool ObjParser::parseVertexLine(const string& line, Vector3D& outVertex) {
    istringstream iss(line);
    string prefix;
    double x, y, z;

    if (!(iss >> prefix >> x >> y >> z)) return false;

    string extra;
    if (iss >> extra) return false;

    outVertex = Vector3D(x, y, z);
    return true;
}

static int parseVertexIndex(const string& token) {
    // Ambil angka hanya sebelum '/' pertama
    string indexStr = token.substr(0, token.find('/'));
    return stoi(indexStr);
}

bool ObjParser::parseFaceLine(const string& line, vector<int>& outIndices) {
    istringstream iss(line);
    string prefix;
    iss >> prefix; // buang "f"

    string token;
    while (iss >> token) {
        try {
            int index = parseVertexIndex(token);
            outIndices.push_back(index);
        } catch (...) {
            return false;
        }
    }

    // Minimal harus ada 3 index (segitiga)
    return outIndices.size() >= 3;
}