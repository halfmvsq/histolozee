#include "rendering/utility/math/MathUtility.h"
#include "logic/camera/Camera.h"
#include "logic/camera/CameraHelpers.h"

#include <glm/glm.hpp>
#include <glm/vector_relational.hpp>

#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <iostream>
#include <unordered_map>

#define EPSILON 0.000001

#define CROSS(dest, v1, v2){                 \
    dest[0] = v1[1] * v2[2] - v1[2] * v2[1]; \
    dest[1] = v1[2] * v2[0] - v1[0] * v2[2]; \
    dest[2] = v1[0] * v2[1] - v1[1] * v2[0];}

#define DOT(v1, v2) (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2])

#define SUB(dest, v1, v2){       \
    dest[0] = v1[0] - v2[0]; \
    dest[1] = v1[1] - v2[1]; \
    dest[2] = v1[2] - v2[2];}

namespace math
{

/**
* A function for creating a rotation matrix that rotates a vector called
* "from" into another vector called "to".
* Input : from[3], to[3] which both must be *normalized* non-zero vectors
* Output: mtx[3][3] -- a 3x3 matrix in colum-major form
* Authors: Tomas Moller, John Hughes 1999
*/

/**
@article{MollerHughes99,
  author = "Tomas Möller and John F. Hughes",
  title = "Efficiently Building a Matrix to Rotate One Vector to Another",
  journal = "journal of graphics, gpu, and game tools",
  volume = "4",
  number = "4",
  pages = "1-4",
  year = "1999",
}
*/

#if 0
void fromToRotation(float from[3], float to[3], float mtx[3][3])
{
    float v[3];
    float e, h, f;

    CROSS(v, from, to);
    e = DOT(from, to);
    f = (e < 0)? -e:e;
    if (f > 1.0 - EPSILON)     /* "from" and "to"-vector almost parallel */
    {
        float u[3], v_temp[3]; /* temporary storage vectors */
        float x[3];       /* vector most nearly orthogonal to "from" */
        float c1, c2, c3; /* coefficients for later use */
        int i, j;

        x[0] = (from[0] > 0.0)? from[0] : -from[0];
        x[1] = (from[1] > 0.0)? from[1] : -from[1];
        x[2] = (from[2] > 0.0)? from[2] : -from[2];

        if (x[0] < x[1])
        {
            if (x[0] < x[2])
            {
                x[0] = 1.0; x[1] = x[2] = 0.0;
            }
            else
            {
                x[2] = 1.0; x[0] = x[1] = 0.0;
            }
        }
        else
        {
            if (x[1] < x[2])
            {
                x[1] = 1.0; x[0] = x[2] = 0.0;
            }
            else
            {
                x[2] = 1.0; x[0] = x[1] = 0.0;
            }
        }

        u[0] = x[0] - from[0]; u[1] = x[1] - from[1]; u[2] = x[2] - from[2];
        v_temp[0] = x[0] - to[0];   v_temp[1] = x[1] - to[1];   v_temp[2] = x[2] - to[2];

        c1 = 2.0 / DOT(u, u);
        c2 = 2.0 / DOT(v_temp, v_temp);
        c3 = c1 * c2  * DOT(u, v_temp);

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                mtx[i][j] =  - c1 * u[i] * u[j]
                        - c2 * v_temp[i] * v_temp[j]
                        + c3 * v_temp[i] * u[j];
            }
            mtx[i][i] += 1.0;
        }
    }
    else  /* the most common case, unless "from"="to", or "from"=-"to" */
    {
#if 0
        /* unoptimized version - a good compiler will optimize this. */
        /* h = (1.0 - e)/DOT(v, v); old code */
        h = 1.0/(1.0 + e);      /* optimization by Gottfried Chen */
        mtx[0][0] = e + h * v[0] * v[0];
        mtx[0][1] = h * v[0] * v[1] - v[2];
        mtx[0][2] = h * v[0] * v[2] + v[1];

        mtx[1][0] = h * v[0] * v[1] + v[2];
        mtx[1][1] = e + h * v[1] * v[1];
        mtx[1][2] = h * v[1] * v[2] - v[0];

        mtx[2][0] = h * v[0] * v[2] - v[1];
        mtx[2][1] = h * v[1] * v[2] + v[0];
        mtx[2][2] = e + h * v[2] * v[2];
#else
        /* ...otherwise use this hand optimized version (9 mults less) */
        float hvx, hvz, hvxy, hvxz, hvyz;
        /* h = (1.0 - e)/DOT(v, v); old code */
        h = 1.0/(1.0 + e);      /* optimization by Gottfried Chen */
        hvx = h * v[0];
        hvz = h * v[2];
        hvxy = hvx * v[1];
        hvxz = hvx * v[2];
        hvyz = hvz * v[1];
        mtx[0][0] = e + hvx * v[0];
        mtx[0][1] = hvxy - v[2];
        mtx[0][2] = hvxz + v[1];

        mtx[1][0] = hvxy + v[2];
        mtx[1][1] = e + h * v[1] * v[1];
        mtx[1][2] = hvyz - v[0];

        mtx[2][0] = hvxz - v[1];
        mtx[2][1] = hvyz + v[0];
        mtx[2][2] = e + hvz * v[2];
#endif
    }
}
#endif

/**
Building an Orthonormal Basis, Revisited
Tom Duff, James Burgess, Per Christensen, Christophe Hery, Andrew Kensler, Max Liani, and Ryusuke Villemin

Journal of Computer Graphics Techniques Vol. 6, No. 1, 2017
 */
/// Use this to create a camera basis with a lookat direction without any priority axes
void buildONB( const glm::vec3& n, glm::vec3& b1, glm::vec3& b2 )
{
    float sign = std::copysign( 1.0f, n.z );
    const float a = -1.0f / ( sign + n.z );
    const float b = n.x * n.y * a;

    b1 = glm::vec3{ 1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x };
    b2 = glm::vec3{ b, sign + n.y * n.y * a, -n.y };
}


glm::vec3 convertVecToRGB( const glm::vec3& v )
{
    const glm::vec3 c = glm::abs( v );
    return c / glm::compMax( c );
}


glm::u8vec3 convertVecToRGB_uint8( const glm::vec3& v )
{
    return glm::u8vec3{ 255.0f * convertVecToRGB( v ) };
}


/*
bool isLeft( const glm::vec2& start, const glm::vec2& end, const glm::vec2& query )
{
    return glm::determinant( glm::mat2{ end - start, query - start } ) > 0;
}


std::array< uint32_t, 4 > reorder( const glm::vec2& A, const glm::vec2& B,
                              const glm::vec2& C, const glm::vec2& D )
{
    static const std::array< uint32_t, 4 > swapBC{ 0, 2, 1, 3 };
    static const std::array< uint32_t, 4 > swapCD{ 0, 1, 3, 2 };
    static const std::array< uint32_t, 4 > noSwap{ 0, 1, 2, 3 };

    if ( isLeft( C, D, A ) ^ isLeft( C, D, B ) )
    {
        // AB crosses CD
        return swapBC;
    }
    else if ( isLeft( B, C, A ) ^ isLeft( B, C, D ) )
    {
        // AD crosses BC
        return swapCD;
    }
    else
    {
        return noSwap;
    }
}
*/


std::vector<uint32_t> sortCounterclockwise( const std::vector<glm::vec2>& points )
{
    if ( points.empty() )
    {
        return std::vector<uint32_t>{};
    }
    else if ( 1 == points.size() )
    {
        return std::vector<uint32_t>{ 0 };
    }

    glm::vec2 center{ 0.0f, 0.0f };

    for ( const auto& p : points )
    {
        center += p;
    }
    center = center / static_cast<float>( points.size() );

    const glm::vec2 a = points[0] - center;

    std::vector<float> angles;

    for ( uint32_t i = 0; i < points.size(); ++i )
    {
        const glm::vec2 b = points[i] - center;
        const float dot = a.x * b.x + a.y * b.y;
        const float det = a.x * b.y - b.x * a.y;
        const float angle = std::atan2( det, dot );
        angles.emplace_back( angle );
    }

    std::vector<uint32_t> indices( points.size() );
    std::iota( std::begin(indices), std::end(indices), 0 );

    std::sort( std::begin( indices ), std::end( indices ),
               [&angles] ( const uint32_t& a, const uint32_t& b ) {
                return ( angles[a] <= angles[b] );
                }
    );

    return indices;
}


std::vector< glm::vec2 > project3dPointsToPlane( const std::vector< glm::vec3 >& A )
{
    const glm::vec3 normal = glm::cross( A[1] - A[0], A[2] - A[0] );

    const glm::mat4 M = glm::lookAt( A[0] - normal, A[0], A[1] - A[0] );

    std::vector< glm::vec2 > B;

    for ( auto& a : A )
    {
        B.emplace_back( glm::vec2{ M * glm::vec4{ a, 1.0f } } );
    }

    return B;
}


void applyLayeringOffsetsToModelPositions(
        const camera::Camera& camera,
        const glm::mat4& model_O_world,
        uint32_t layer,
        std::vector<glm::vec3>& modelPositions )
{
    if ( modelPositions.empty() )
    {
        return;
    }

    // Matrix for transforming vectors from Camera to Model space:
    const glm::mat3 model_O_camera_invTrans =
            glm::inverseTranspose( glm::mat3{ model_O_world * camera.world_O_camera() } );

    // The view's Back direction transformed to Model space:
    const glm::vec3 modelTowardsViewer = glm::normalize(
                model_O_camera_invTrans * Directions::get( Directions::View::Back ) );

    // Compute offset in World units based on first position (this choice is arbitrary)
    const float worldDepth = camera::computeSmallestWorldDepthOffset( camera, modelPositions.front() );

    // Proportionally offset higher layers by more distance
    const float offsetMag = layer * worldDepth;
    const glm::vec3 modelOffset = offsetMag * modelTowardsViewer;

    for ( auto& p : modelPositions )
    {
        p += modelOffset;
    }
}


} // namespace math
