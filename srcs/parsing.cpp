// parsing.cpp
#include "../include/include.hpp"

bool loadOBJ(const std::string& path, Mesh& outMesh) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[OBJ] Cannot open file: " << path << "\n";
        return false;
    }

    std::vector<float> temp_v;
    std::vector<float> temp_vt;
    std::vector<float> temp_vn;

    std::string line;
    while (std::getline(file, line)) {

        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            temp_v.push_back(x);
            temp_v.push_back(y);
            temp_v.push_back(z);
        }
        else if (type == "vt") {
            float u, v;
            ss >> u >> v;
            temp_vt.push_back(u);
            temp_vt.push_back(v);
        }
        else if (type == "vn") {
            float nx, ny, nz;
            ss >> nx >> ny >> nz;
            temp_vn.push_back(nx);
            temp_vn.push_back(ny);
            temp_vn.push_back(nz);
        }
        else if (type == "f") {
            // Faces: v, v/vt, v//vn, v/vt/vn
            std::string vStr;
            std::vector<std::string> face;

            // Collect all vertices of the face (supports quads)
            while (ss >> vStr)
                face.push_back(vStr);

            // Convert quad → two triangles
            int triCount = (face.size() == 4) ? 2 : 1;
            int indices[6] = {0,1,2, 0,2,3};

            for (int t = 0; t < triCount; t++) {
                for (int i = 0; i < 3; i++) {

                    std::string vert = face[indices[t*3 + i]];

                    int vi = -1, ti = -1, ni = -1;

                    // Parse v/vt/vn
                    sscanf(vert.c_str(), 
                        "%d/%d/%d", &vi, &ti, &ni);

                    // v//vn
                    if (ti == -1) {
                        sscanf(vert.c_str(), "%d//%d", &vi, &ni);
                    }

                    // v only
                    if (vi == -1) {
                        sscanf(vert.c_str(), "%d", &vi);
                    }

                    // OBJ indices start at 1
                    vi -= 1;
                    if (ti > 0) ti -= 1;
                    if (ni > 0) ni -= 1;

                    // POSITION
                    outMesh.vertices.push_back(temp_v[vi*3 + 0]);
                    outMesh.vertices.push_back(temp_v[vi*3 + 1]);
                    outMesh.vertices.push_back(temp_v[vi*3 + 2]);

                    // AUTO-COLORS (optional)
                    outMesh.colors.push_back(0.8f);
                    outMesh.colors.push_back(0.8f);
                    outMesh.colors.push_back(0.8f);

                    // NORMALS (optional)
                    if (!temp_vn.empty() && ni >= 0) {
                        outMesh.normals.push_back(temp_vn[ni*3 + 0]);
                        outMesh.normals.push_back(temp_vn[ni*3 + 1]);
                        outMesh.normals.push_back(temp_vn[ni*3 + 2]);
                    }

                    // UVS (optional)
                    if (!temp_vt.empty() && ti >= 0) {
                        outMesh.uvs.push_back(temp_vt[ti*2 + 0]);
                        outMesh.uvs.push_back(temp_vt[ti*2 + 1]);
                    }
                }
            }
        }
    }

    std::cout << "[OBJ] Loaded " << outMesh.vertices.size()/3 << " vertices\n";
    return true;
}
