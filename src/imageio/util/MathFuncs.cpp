#include "MathFuncs.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <algorithm>
#include <array>
#include <limits>


namespace imageio
{

namespace math
{

glm::dvec3 subjectImageDimensions(
        const glm::u64vec3& pixelDimensions,
        const glm::dvec3& pixelSpacing )
{
    return glm::dvec3{ static_cast<double>( pixelDimensions.x ) * pixelSpacing.x,
                       static_cast<double>( pixelDimensions.y ) * pixelSpacing.y,
                       static_cast<double>( pixelDimensions.z ) * pixelSpacing.z };
}


glm::dmat4 computeImagePixelToSubjectTransformation(
        const glm::dmat3& directions,
        const glm::dvec3& pixelSpacing,
        const glm::dvec3& origin )
{
    return glm::dmat4{
        glm::dvec4{ pixelSpacing.x * directions[0], 0.0 }, // column 0
        glm::dvec4{ pixelSpacing.y * directions[1], 0.0 }, // column 1
        glm::dvec4{ pixelSpacing.z * directions[2], 0.0 }, // column 2
        glm::dvec4{ origin, 1.0 } };                       // column 3
}


glm::dmat4 computeImagePixelToTextureTransformation(
        const glm::u64vec3& pixelDimensions )
{
    const glm::dvec3 invDim( 1.0 / pixelDimensions.x,
                             1.0 / pixelDimensions.y,
                             1.0 / pixelDimensions.z );

    return glm::translate( 0.5 * invDim ) * glm::scale( invDim );
}


std::pair< glm::dvec3, glm::dvec3 > computeImageSubjectAABBoxCorners(
        const glm::u64vec3& pixelDimensions,
        const glm::dmat3& directions,
        const glm::dvec3& pixelSpacing,
        const glm::dvec3& origin )
{
    // Image has 8 corners
    static constexpr size_t N = 8;

    const glm::dmat4 subject_O_pixel = computeImagePixelToSubjectTransformation(
                directions, pixelSpacing, origin );

    const glm::u64vec3 D = pixelDimensions - glm::u64vec3{ 1, 1, 1 };

    const std::array< glm::dvec3, N > pixelCorners =
    {
        glm::dvec3{ 0.0, 0.0, 0.0 },
        glm::dvec3{ D.x, 0.0, 0.0 },
        glm::dvec3{ 0.0, D.y, 0.0 },
        glm::dvec3{ D.x, D.y, 0.0 },
        glm::dvec3{ 0.0, 0.0, D.z },
        glm::dvec3{ D.x, 0.0, D.z },
        glm::dvec3{ 0.0, D.y, D.z },
        glm::dvec3{ D.x, D.y, D.z }
    };

    std::array< glm::dvec3, N > subjectCorners;

    std::transform( std::begin( pixelCorners ),
                    std::end( pixelCorners ),
                    std::begin( subjectCorners ),
                    [ &subject_O_pixel ]( const glm::dvec3& v )
    {
        return glm::dvec3{ subject_O_pixel * glm::dvec4{ v, 1.0 } };
    } );

    glm::dvec3 minSubjectCorner{ std::numeric_limits<double>::max() };
    glm::dvec3 maxSubjectCorner{ std::numeric_limits<double>::lowest() };

    for ( uint32_t c = 0; c < N; ++c )
    {
        for ( int i = 0; i < 3; ++i )
        {
            if ( subjectCorners[c][i] < minSubjectCorner[i] )
            {
                minSubjectCorner[i] = subjectCorners[c][i];
            }

            if ( subjectCorners[c][i] > maxSubjectCorner[i] )
            {
                maxSubjectCorner[i] = subjectCorners[c][i];
            }
        }
    }

    return std::make_pair( minSubjectCorner, maxSubjectCorner );
}

} // namespace math

} // namespace imageio
