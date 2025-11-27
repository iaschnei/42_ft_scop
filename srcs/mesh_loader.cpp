#include "../include/include.hpp"

// ---------------- Normal generation ----------------
void generateNormals(Mesh &mesh) {
    size_t vertexCount = mesh.vertices.size() / 3;
    mesh.normals.resize(mesh.vertices.size(), 0.0f); // initialize

    // Compute face normals for each triangle
    for(size_t i=0; i < vertexCount; i += 3) {
        float* v0 = &mesh.vertices[i*3 + 0];
        float* v1 = &mesh.vertices[(i+1)*3 + 0];
        float* v2 = &mesh.vertices[(i+2)*3 + 0];

        float edge1[3] = { v1[0]-v0[0], v1[1]-v0[1], v1[2]-v0[2] };
        float edge2[3] = { v2[0]-v0[0], v2[1]-v0[1], v2[2]-v0[2] };

        float nx = edge1[1]*edge2[2] - edge1[2]*edge2[1];
        float ny = edge1[2]*edge2[0] - edge1[0]*edge2[2];
        float nz = edge1[0]*edge2[1] - edge1[1]*edge2[0];

        float length = std::sqrt(nx*nx + ny*ny + nz*nz);
        if(length != 0.0f) { nx/=length; ny/=length; nz/=length; }

        for(int j=0;j<3;j++){
            mesh.normals[(i+j)*3 + 0] = nx;
            mesh.normals[(i+j)*3 + 1] = ny;
            mesh.normals[(i+j)*3 + 2] = nz;
        }
    }
}

// ---------------- Compute bounding box and scaling ----------------
void computeCenterScale(const Mesh &mesh, float &cx, float &cy, float &cz, float &scale) {
    float minX=1e9,minY=1e9,minZ=1e9;
    float maxX=-1e9,maxY=-1e9,maxZ=-1e9;

    for(size_t i=0;i<mesh.vertices.size();i+=3){
        float x=mesh.vertices[i], y=mesh.vertices[i+1], z=mesh.vertices[i+2];
        if(x<minX) minX=x; 
        if(x>maxX) maxX=x;
        if(y<minY) minY=y; 
        if(y>maxY) maxY=y;
        if(z<minZ) minZ=z; 
        if(z>maxZ) maxZ=z;
    }

    cx = (minX+maxX)*0.5f;
    cy = (minY+maxY)*0.5f;
    cz = (minZ+maxZ)*0.5f;

    float dx = maxX-minX;
    float dy = maxY-minY;
    float dz = maxZ-minZ;
    float radius = std::sqrt(dx*dx + dy*dy + dz*dz)*0.5f;

    float desiredRadius = 1.5f;
    scale = desiredRadius / radius;
}

// ---------------- Interleave vertex and normal data ----------------
std::vector<float> interleaveMesh(const Mesh &mesh, float cx, float cy, float cz, float scale) {
    size_t vertexCount = mesh.vertices.size()/3;
    std::vector<float> interleaved;
    interleaved.reserve(vertexCount*8); // 3 pos + 3 normal + 2 UV

    // Compute bounding box for UV mapping
    float minX=1e9,minY=1e9,maxX=-1e9,maxY=-1e9;
    for(size_t i=0;i<mesh.vertices.size();i+=3){
        float x=(mesh.vertices[i]-cx)*scale;
        float y=(mesh.vertices[i+1]-cy)*scale;
        if(x<minX) minX=x; 
        if(x>maxX) maxX=x;
        if(y<minY) minY=y; 
        if(y>maxY) maxY=y;
    }
    float dx = maxX - minX;
    float dy = maxY - minY;

    for(size_t i=0;i<vertexCount;i++){
        float x = (mesh.vertices[i*3+0]-cx)*scale;
        float y = (mesh.vertices[i*3+1]-cy)*scale;
        float z = (mesh.vertices[i*3+2]-cz)*scale;

        float nx = mesh.normals[i*3+0];
        float ny = mesh.normals[i*3+1];
        float nz = mesh.normals[i*3+2];

        // Generate UVs in XY plane [0,1]
        float u = (x - minX) / dx;
        float v = (y - minY) / dy;

        interleaved.push_back(x); interleaved.push_back(y); interleaved.push_back(z);
        interleaved.push_back(nx); interleaved.push_back(ny); interleaved.push_back(nz);
        interleaved.push_back(u); interleaved.push_back(v);
    }

    return interleaved;
}
