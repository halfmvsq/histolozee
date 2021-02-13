#ifndef GL_UNIFORM_TYPES_H
#define GL_UNIFORM_TYPES_H

#include <QOpenGLFunctions_3_3_Core>

#include <cstdint>

enum class UniformType : uint32_t
{
    Bool = GL_BOOL,
    Int = GL_INT,
    UInt = GL_UNSIGNED_INT,
    Float = GL_FLOAT,
    Double = GL_DOUBLE,
    Vec2 = GL_FLOAT_VEC2,
    Vec3 = GL_FLOAT_VEC3,
    Vec4 = GL_FLOAT_VEC4,
    Mat2 = GL_FLOAT_MAT2,
    Mat3 = GL_FLOAT_MAT3,
    Mat4 = GL_FLOAT_MAT4,

    // Special types defined by HZee:
    Sampler = 1,
    FloatArray2 = 2,
    FloatArray3 = 3,
    FloatArray4 = 4,
    FloatArray5 = 5,
    UIntArray5 = 6,
    Vec3Array8 = 8,
    Undefined = 0
};

#endif // GL_UNIFORM_TYPES_H
