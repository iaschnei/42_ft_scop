// parsing.hpp
#pragma once
#include <vector>
#include <string>

struct Mesh {
    std::vector<float> vertices;
    std::vector<float> colors;
    std::vector<float> normals;
    std::vector<float> uvs;
};
 
bool loadOBJ(const std::string& path, Mesh& outMesh);
