#include "../include/include.hpp"

// Load our mesh from a file path (argv)
// Then parse it to get its data
bool loadOBJ(const std::string& path, Mesh& outMesh) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[OBJ] Cannot open file: " << path << "\n";
        return false;
    }

    std::vector<float> temp_pos;
    std::vector<float> temp_uvs;
    std::vector<float> temp_normals;

    std::string line;
    while (std::getline(file, line)) {

        std::stringstream ss(line);
        std::string type;
        ss >> type;

        // v = position
        if (type == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            temp_pos.push_back(x);
            temp_pos.push_back(y);
            temp_pos.push_back(z);
        }
        // vt = texture coordinate (uv)
        else if (type == "vt") {
            float u, v;
            ss >> u >> v;
            temp_uvs.push_back(u);
            temp_uvs.push_back(v);
        }
        // vn = normals
        else if (type == "vn") {
            float nx, ny, nz;
            ss >> nx >> ny >> nz;
            temp_normals.push_back(nx);
            temp_normals.push_back(ny);
            temp_normals.push_back(nz);
        }
        // f = face (triangle or quad)
        else if (type == "f") {
            
            std::string vStr;
            std::vector<std::string> face;

            // Collect all vertices of the face
            while (ss >> vStr)
                face.push_back(vStr);

            // Convert quad → two triangles
            int triCount = (face.size() == 4) ? 2 : 1;
            int indices[6] = {0,1,2, 0,2,3};

            for (int t = 0; t < triCount; t++) {
                for (int i = 0; i < 3; i++) {

                    std::string vert = face[indices[t*3 + i]];

                    int vi = -1, ti = -1, ni = -1;

                    // Try to find v/vt/vn
                    sscanf(vert.c_str(), 
                        "%d/%d/%d", &vi, &ti, &ni);

                    // Then only v//vn
                    if (ti == -1) {
                        sscanf(vert.c_str(), "%d//%d", &vi, &ni);
                    }

                    // And v only
                    if (vi == -1) {
                        sscanf(vert.c_str(), "%d", &vi);
                    }

                    // OBJ indices start at 1 (unlike C++ vectors)
                    vi -= 1;
                    if (ti > 0) ti -= 1;
                    if (ni > 0) ni -= 1;

                    // Position
                    outMesh.vertices.push_back(temp_pos[vi*3 + 0]);
                    outMesh.vertices.push_back(temp_pos[vi*3 + 1]);
                    outMesh.vertices.push_back(temp_pos[vi*3 + 2]);

                    // Give it a default color
                    outMesh.colors.push_back(0.8f);
                    outMesh.colors.push_back(0.8f);
                    outMesh.colors.push_back(0.8f);

                    // Normals (if they exist)
                    if (!temp_normals.empty() && ni >= 0) {
                        outMesh.normals.push_back(temp_normals[ni*3 + 0]);
                        outMesh.normals.push_back(temp_normals[ni*3 + 1]);
                        outMesh.normals.push_back(temp_normals[ni*3 + 2]);
                    }

                    // UVS (if they exist)
                    if (!temp_uvs.empty() && ti >= 0) {
                        outMesh.uvs.push_back(temp_uvs[ti*2 + 0]);
                        outMesh.uvs.push_back(temp_uvs[ti*2 + 1]);
                    }
                }
            }
        }
    }

    std::cout << "[OBJ] Loaded " << outMesh.vertices.size()/3 << " vertices\n";
    return true;
}
