#ifndef HZEE_TYPES_H
#define HZEE_TYPES_H

#include <array>
#include <vector>

namespace imageio
{

/**
 * @brief Pixel component types used in HistoloZee;
 * they have one-to-one correspondence with types in ITK.
 */
enum class ComponentType
{
    Int8,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float32,
    Double64
};

bool isIntegerType( const ComponentType& );
bool isSignedIntegerType( const ComponentType& );
bool isUnsignedIntegerType( const ComponentType& );
bool isFloatingType( const ComponentType& );


/**
 * @brief Rules for normalization of image components upon their loading
 */
enum class ComponentNormalizationPolicy
{
    /// Components are not normalized
    None,

    /// Components are normalized to signed [-1.0, 1.0] range.
    /// Only works for images with float or double components.
    SignedNormalizedFloating,

    /// Components are normalized to unsigned [0.0, 1.0] range
    /// Only works for images with float or double components.
    UnsignedNormalizedFloating,
};


/// Policy defining how to cast pixel component types when reading images
enum class ComponentTypeCastPolicy
{
    /// Components are kept native and not cast
    Identity,

    /// Components are directly cast to 32-bit floating point type
    ToFloat32,

    /// Components are cast to their nearest OpenGL-compatible type
    ToOpenGLCompatible,

    /// Components are always cast to the most sensible unsigned integer type
    /// that is also OpenGL-compatible: Primary use case is for label images
    ToOpenGLCompatibleUInt
};


/**
 * @brief Pixel types used in HistoloZee;
 * they have one-to-one correspondence with types in ITK.
 */
enum class PixelType
{
    Scalar,
    Complex,
    RGB,
    RGBA,
    Vector,
    CovariantVector,
    Offset,
    Point,
    FixedArray,
    Matrix,
    DiffusionTensor3D,
    SymmetricSecondRankTensor,
    Undefined
};


struct ComponentStatistics
{
    double m_minimum;
    double m_maximum;
    double m_mean;
    double m_stdDeviation;
    std::vector< double > m_histogram;
    std::array< double, 101 > m_quantiles;
};

} // namespace imageio

#endif // HZEE_TYPES_H
