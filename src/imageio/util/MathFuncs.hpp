#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/type_precision.hpp>

#include <vnl/vnl_matrix_fixed.h>

#include <cmath>
#include <cstdlib>
#include <limits>
#include <utility>


namespace imageio
{

namespace math
{

/**
 * @brief Compute dimensions of image in Subject space
 * @param[in] pixelDimensions Number of pixels per image dimensions
 * @param[in] pixelSpacing Pixel spacing in Subject space
 *
 * @return Vector of image dimensions in Subject space
 */
glm::dvec3 subjectImageDimensions(
        const glm::u64vec3& pixelDimensions,
        const glm::dvec3& pixelSpacing );


/**
 * @brief Compute transformation from image Pixel space to Subject space.
 *
 * @param[in] directions Directions of image Pixel axes in Subject space
 * @param[in] pixelSpacing Pixel spacing in Subject space
 * @param[in] origin Image origin in Subject space
 *
 * @return 4x4 matrix transforming image Pixel to Subject space
 */
glm::dmat4 computeImagePixelToSubjectTransformation(
        const glm::dmat3& directions,
        const glm::dvec3& pixelSpacing,
        const glm::dvec3& origin );


/**
 * @brief Compute transformation from image Pixel space, with coordinates (i, j, k) representing
 * pixel indices in [0, N-1] range,  to image Texture coordinates (s, t, p) in
 * [1/(2N), 1 - 1/(2N)] range
 *
 * @param[in] pixelDimensions Number of pixels per image dimensions
 *
 * @return 4x4 matrix transforming image Pixel to Texture space
 */
glm::dmat4 computeImagePixelToTextureTransformation(
        const glm::u64vec3& pixelDimensions );


/**
 * @brief Compute the axis-aligned bounding box of the image in Subject space.
 *
 * @param[in] pixelDimensions Number of pixels per image dimension
 * @param[in] directions Directions of image Pixel axes in Subject space
 * @param[in] pixelSpacing Pixel spacing in Subject space
 * @param[in] origin Image origin in Subject space
 *
 * @return Pair of minimum and maximum corners of the image AABB in Subject space
 */
std::pair< glm::dvec3, glm::dvec3 > computeImageSubjectAABBoxCorners(
        const glm::u64vec3& pixelDimensions,
        const glm::dmat3& directions,
        const glm::dvec3& pixelSpacing,
        const glm::dvec3& origin );

} // namespace math


namespace convert
{

/**
 * @brief Convert a 3x3 GLM matrix to a 3x3 VNL matrix
 */
template< class T >
vnl_matrix_fixed< T, 3, 3 > toVnlMatrixFixed( const glm::tmat3x3< T, glm::highp >& glmMatrix )
{
    const vnl_matrix_fixed< T, 3, 3 > vnlMatrixTransposed( glm::value_ptr( glmMatrix ) );
    return vnlMatrixTransposed.transpose();
}

} // namespace convert


#if 0
namespace comparison
{

//implements relative method - do not use for comparing with zero
//use this most of the time, tolerance needs to be meaningful in your context
template<typename TReal>
bool
isApproximatelyEqual(TReal a, TReal b, TReal tolerance = std::numeric_limits<TReal>::epsilon())
{
    TReal diff = std::abs(a - b);
    if (diff <= tolerance)
        return true;

    if (diff < std::fmax(std::abs(a), std::abs(b)) * tolerance)
        return true;

    return false;
}

//supply tolerance that is meaningful in your context
//for example, default tolerance may not work if you are comparing double with float
template<typename TReal>
bool
isApproximatelyZero(TReal a, TReal tolerance = std::numeric_limits<TReal>::epsilon())
{
    if (std::abs(a) <= tolerance)
        return true;
    return false;
}


//use this when you want to be on safe side
//for example, don't start rover unless signal is above 1
template<typename TReal>
bool
isDefinitelyLessThan(TReal a, TReal b, TReal tolerance = std::numeric_limits<TReal>::epsilon())
{
    TReal diff = a - b;
    if (diff < tolerance)
        return true;

    if (diff < std::fmax(std::abs(a), std::abs(b)) * tolerance)
        return true;

    return false;
}
template<typename TReal>
bool
isDefinitelyGreaterThan(TReal a, TReal b, TReal tolerance = std::numeric_limits<TReal>::epsilon())
{
    TReal diff = a - b;
    if (diff > tolerance)
        return true;

    if (diff > std::fmax(std::abs(a), std::abs(b)) * tolerance)
        return true;

    return false;
}

//implements ULP method
//use this when you are only concerned about floating point precision issue
//for example, if you want to see if a is 1.0 by checking if its within
//10 closest representable floating point numbers around 1.0.
template<typename TReal>
bool
isWithinPrecisionInterval(TReal a, TReal b, unsigned int interval_size = 1)
{
    TReal min_a = a - (a - std::nextafter(a, std::numeric_limits<TReal>::lowest())) * interval_size;
    TReal max_a = a + (std::nextafter(a, std::numeric_limits<TReal>::max()) - a) * interval_size;

    return min_a <= b && max_a >= b;
}

} // namespace comparison
#endif

} // namespace imageio
