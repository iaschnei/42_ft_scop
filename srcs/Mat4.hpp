#ifndef MAT4_HPP
#define MAT4_HPP

// Our 4x4 matrix, in a 16 float array, visual representation would be :
//
//  a b c d
//  e f g h
//  i j k l
//  m n o p
class Mat4 {
    
    public:

    float m[16];

    static Mat4 identity();
    static Mat4 multiply(const Mat4 &a, const Mat4 &b);
    static Mat4 perspective(float fov, float aspect, float near, float far);
    static Mat4 lookAt(float eyeX, float eyeY, float eyeZ,
            float cx, float cy, float cz,
            float upX, float upY, float upZ);
};


#endif