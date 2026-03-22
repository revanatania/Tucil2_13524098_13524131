#include <iostream>
#include <string>
#include <filesystem>
#include <stdexcept>

#include "io/objParser.hpp"
#include "io/objWriter.hpp"
#include "voxel/Voxelizer.hpp"

namespace fs = std::filesystem;

static void printUsage(const char* progName) {
    std::cout << "Usage: " << progName
              << " <input.obj> <maxDepth> [outputPath] [parallelLevel]\n"
              << "\n"
              << "  input.obj      Path to the source .obj file\n"
              << "  maxDepth       Octree maximum depth (integer >= 1)\n"
              << "  outputPath     (optional) Where to write the voxelized .obj\n"
              << "                 Default: <inputStem>-voxelized.obj\n"
              << "  parallelLevel  (optional) Depth up to which async is used (default 2)\n";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    std::string inputPath = argv[1];
    int maxDepth = 0;
    try {
        maxDepth = std::stoi(argv[2]);
    } catch (...) {
        std::cerr << "Error: maxDepth must be an integer.\n";
        return 1;
    }
    if (maxDepth < 1) {
        std::cerr << "Error: maxDepth must be >= 1.\n";
        return 1;
    }

    // default path output :  stem + "-voxelized.obj"
    std::string outputPath;
    if (argc >= 4) {
        outputPath = argv[3];
    } else {
        fs::path p(inputPath);
        outputPath = (p.parent_path() / (p.stem().string() + "-voxelized.obj")).string();
    }

    int parallelLevel = 2;
    if (argc >= 5) {
        try {
            parallelLevel = std::stoi(argv[4]);
        } catch (...) {
            std::cerr << "Warning: invalid parallelLevel, defaulting to 2.\n";
        }
    }

    if (!fs::exists(inputPath)) {
        std::cerr << "Error: input file does not exist: " << inputPath << "\n";
        return 1;
    }
    if (fs::path(inputPath).extension() != ".obj") {
        std::cerr << "Error: input file must have .obj extension.\n";
        return 1;
    }

    std::cout << "Parsing: " << inputPath << " ...\n";
    ParseResult parsed = ObjParser::parse(inputPath);
    if (!parsed.success) {
        std::cerr << "Parse error: " << parsed.errorMessage << "\n";
        return 1;
    }
    std::cout << "  Vertices: " << parsed.mesh.vertices.size()
              << "  Triangles: " << parsed.mesh.triangles.size() << "\n";


    std::cout << "Voxelizing (maxDepth=" << maxDepth
              << ", parallelLevel=" << parallelLevel << ") ...\n";

    VoxelizationResult result;
    try {
        result = Voxelizer::voxelize(parsed.mesh, maxDepth, parallelLevel);
    } catch (const std::exception& ex) {
        std::cerr << "Voxelization error: " << ex.what() << "\n";
        return 1;
    }


    result.stats.outputPath = outputPath;
    std::string writeError;
    if (!Output::writeMesh(outputPath, result.outputMesh, writeError)) {
        std::cerr << "Write error: " << writeError << "\n";
        return 1;
    }
    
    std::cout << "\n=== Hasil ===\n";
    Output::printStats(result.stats);

    return 0;
}
