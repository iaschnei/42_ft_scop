#include <cmath>
#include "../include/Mat4.hpp"

// Basically a matrix is a way to transform a 3D point
// It's just numbers in an array but positioned in a certain way
// so our GPU knows what to do with the 3D point (rotate, scale, move...)

// Default status of our matrix, like this : 
//
// 1 0 0 0
// 0 1 0 0
// 0 0 1 0
// 0 0 0 1
Mat4 Mat4::identity() {
    Mat4 r = {};
    r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
    return r;
}

// Multiply two matrices together. To render any object in 3D we need to apply the transformation
// Projection * View * Model   (MVP)
Mat4 Mat4::multiply(const Mat4 &a, const Mat4 &b) {
    Mat4 r;
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++) {
            r.m[col*4 + row] =
                a.m[0*4 + row] * b.m[col*4 + 0] +
                a.m[1*4 + row] * b.m[col*4 + 1] +
                a.m[2*4 + row] * b.m[col*4 + 2] +
                a.m[3*4 + row] * b.m[col*4 + 3];
        }
    return r;
}

// Simulate perspective as if we're looking at an object from a camera, taking into account field of view (fov)
// aspect ratio (aspect), nearest possible object (near) = minimum distance of display, farthest possible object (far) = max distance of display
Mat4 Mat4::perspective(float fov, float aspect, float near, float far) {
    float f = 1.0f / tanf(fov / 2.0f);
    Mat4 r = {};
    r.m[0]  = f / aspect;
    r.m[5]  = f;
    r.m[10] = (far + near) / (near - far);
    r.m[11] = -1.0f;
    r.m[14] = (2 * far * near) / (near - far);
    return r;
}


// Make the camera (eye) look at something (c) according to where up is (up)
Mat4 Mat4::lookAt(float eyeX, float eyeY, float eyeZ,
            float cx, float cy, float cz,
            float upX, float upY, float upZ)
{

    // Compute the direction from the camera to the target 
    // then normalise it so it gives an absolute direction (not based on distance)
    float fX = cx - eyeX;
    float fY = cy - eyeY;
    float fZ = cz - eyeZ;
    float fLen = sqrtf(fX*fX + fY*fY + fZ*fZ);
    fX /= fLen; fY /= fLen; fZ /= fLen;

    // Normalise up
    float uLen = sqrtf(upX*upX + upY*upY + upZ*upZ);
    upX /= uLen; upY /= uLen; upZ /= uLen;

    // Compute the s(ide) vector, perpendicular to f(orward) and up
    float sX = fY*upZ - fZ*upY;
    float sY = fZ*upX - fX*upZ;
    float sZ = fX*upY - fY*upX;

    // Re-compute up to make it perpendicular to s(ide) 
    // If we're looking from an angle, the original up wouldn't necessarily be perpendicular to it 
    float uX = sY*fZ - sZ*fY;
    float uY = sZ*fX - sX*fZ;
    float uZ = sX*fY - sY*fX;

    // Create the new camera matrix with
    // X axis = side
    // Y axis = up
    // Z axis = negative forward 
    // (negative because we're looking at forward so we're behind it if that makes sense)
    Mat4 r = identity();
    r.m[0] = sX; r.m[4] = sY; r.m[8]  = sZ;
    r.m[1] = uX; r.m[5] = uY; r.m[9]  = uZ;
    r.m[2] =-fX; r.m[6] =-fY; r.m[10] =-fZ;

    // Then move the entire world in the opposite direction of the camera (instead of moving the camera)
    // Meaning camera is still at the origin point and everything else translates
    r.m[12] = -(sX*eyeX + sY*eyeY + sZ*eyeZ);
    r.m[13] = -(uX*eyeX + uY*eyeY + uZ*eyeZ);
    r.m[14] =  (fX*eyeX + fY*eyeY + fZ*eyeZ);

    // Return the new view
    return r;
}

Mat4 Mat4::rotateX(float a) {
    float c = cosf(a);
    float s = sinf(a);

    Mat4 r = identity();

    r.m[5]  = c;
    r.m[6]  = s;
    r.m[9]  = -s;
    r.m[10] = c;

    return r;
}

Mat4 Mat4::rotateY(float a) {
    float c = cosf(a);
    float s = sinf(a);

    Mat4 r = identity();

    r.m[0]  = c;
    r.m[2]  = -s;
    r.m[8]  = s;
    r.m[10] = c;

    return r;
}

Mat4 Mat4::rotateZ(float a) {
    float c = cosf(a);
    float s = sinf(a);

    Mat4 r = identity();

    r.m[0] = c;
    r.m[1] = s;
    r.m[4] = -s;
    r.m[5] = c;

    return r;
}

Mat4 Mat4::rotateAxis(float x, float y, float z, float angle) {
    float len = sqrtf(x*x + y*y + z*z);
    if (len == 0.0f) return identity();
    x /= len; y /= len; z /= len;

    float c = cosf(angle);
    float s = sinf(angle);
    float ic = 1.0f - c;

    Mat4 r = identity();

    r.m[0] = c + x*x*ic;
    r.m[4] = x*y*ic - z*s;
    r.m[8] = x*z*ic + y*s;

    r.m[1] = y*x*ic + z*s;
    r.m[5] = c + y*y*ic;
    r.m[9] = y*z*ic - x*s;

    r.m[2] = z*x*ic - y*s;
    r.m[6] = z*y*ic + x*s;
    r.m[10] = c + z*z*ic;

    return r;
}

Mat4 Mat4::translate(float x, float y, float z) {
    Mat4 r = identity();
    r.m[12] = x;
    r.m[13] = y;
    r.m[14] = z;
    return r;
}
