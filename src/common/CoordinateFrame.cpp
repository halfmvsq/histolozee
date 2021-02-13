#include "common/CoordinateFrame.h"
#include "common/HZeeException.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/vector_query.hpp>


CoordinateFrame::CoordinateFrame()
    :
      m_worldFrameOrigin( 0.0f, 0.0f, 0.0f ),
      m_world_O_frame_rotation( 1.0f, 0.0f, 0.0f, 0.0f )
{
}


CoordinateFrame::CoordinateFrame(
        glm::vec3 worldOrigin,
        glm::quat world_O_frame_rotation )
    :
      m_worldFrameOrigin( std::move( worldOrigin ) ),
      m_world_O_frame_rotation( std::move( world_O_frame_rotation ) )
{
}


CoordinateFrame::CoordinateFrame( glm::vec3 worldOrigin, float angle, const glm::vec3& worldAxis )
    :
      m_worldFrameOrigin( std::move( worldOrigin ) )
{
    setFrameToWorldRotation( angle, worldAxis );
}


CoordinateFrame::CoordinateFrame(
        glm::vec3 worldOrigin,
        const glm::vec3& frameAxis1, const glm::vec3& worldAxis1,
        const glm::vec3& frameAxis2, const glm::vec3& worldAxis2 )
    :
      m_worldFrameOrigin( std::move( worldOrigin ) )
{
    static constexpr bool k_requireEqualAngles = false;
    setFrameToWorldRotation( frameAxis1, worldAxis1, frameAxis2, worldAxis2, k_requireEqualAngles );
}


void CoordinateFrame::setWorldOrigin( glm::vec3 origin )
{
    m_worldFrameOrigin = std::move( origin );
}


void CoordinateFrame::setFrameToWorldRotation( glm::quat world_O_frame_rotation )
{
    m_world_O_frame_rotation = std::move( world_O_frame_rotation );
}


void CoordinateFrame::setFrameToWorldRotation( float angleDegrees, const glm::vec3& worldAxis )
{
    static const glm::quat sk_ident( 1.0f, 0.0f, 0.0f, 0.0f );

    m_world_O_frame_rotation = glm::rotateNormalizedAxis( sk_ident, angleDegrees, worldAxis );
}


void CoordinateFrame::setFrameToWorldRotation(
        const glm::vec3& frameAxis1, const glm::vec3& worldAxis1,
        const glm::vec3& frameAxis2, const glm::vec3& worldAxis2,
        bool requireEqualAngles )
{
    const float frameAngle = glm::angle( frameAxis1, frameAxis2 );
    const float worldAngle = glm::angle( worldAxis1, worldAxis2 );

    if ( requireEqualAngles &&
         ! glm::epsilonEqual( frameAngle, worldAngle, glm::epsilon<float>() ) )
    {
        throw_debug( "Angle between input frame and world axes are not equal." );
    }

    if ( glm::epsilonEqual( frameAngle, 0.0f, glm::epsilon<float>() ) ||
         glm::epsilonEqual( worldAngle, 0.0f, glm::epsilon<float>() ) )
    {
        throw_debug( "Input axes are equal." );
    }

    const glm::mat3 frame_O_ident{ frameAxis1, frameAxis2, glm::cross( frameAxis1, frameAxis2 ) };
    const glm::mat3 world_O_ident{ worldAxis1, worldAxis2, glm::cross( worldAxis1, worldAxis2 ) };

    const glm::mat3 world_O_frame = glm::orthonormalize(
                world_O_ident * glm::inverse( frame_O_ident ) );

    m_world_O_frame_rotation = glm::normalize( glm::quat_cast( world_O_frame ) );
}


void CoordinateFrame::setIdentity()
{
    m_worldFrameOrigin = { 0.0f, 0.0f, 0.0f };
    m_world_O_frame_rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
}


glm::vec3 CoordinateFrame::worldOrigin() const
{
    return m_worldFrameOrigin;
}

glm::quat CoordinateFrame::world_O_frame_rotation() const
{
    return m_world_O_frame_rotation;
}

glm::mat4 CoordinateFrame::world_O_frame() const
{
    return glm::translate( m_worldFrameOrigin ) * glm::toMat4( m_world_O_frame_rotation );
}

glm::mat4 CoordinateFrame::frame_O_world() const
{
    return glm::affineInverse( world_O_frame() );
}


CoordinateFrame& CoordinateFrame::operator+=( const CoordinateFrame& rhs )
{
    return ( *this = *this + rhs );
}

CoordinateFrame CoordinateFrame::operator+( const CoordinateFrame& rhs ) const
{
    CoordinateFrame res;
    res.setWorldOrigin( worldOrigin() + rhs.worldOrigin() );
    res.setFrameToWorldRotation( world_O_frame_rotation() * rhs.world_O_frame_rotation() );
    return res;
}


#if 0
// Code for setting frame transformation from a matrix (not used):
void CoordinateFrame::set_world_O_frame( const glm::mat4& world_O_frame )
{
    m_worldOriginOfFrame = glm::vec3( world_O_frame[3] / world_O_frame[3].w );

    static constexpr bool k_requireEqualAngles = false;
    static const glm::vec3 frameAxis1( 1.0f, 0.0f, 0.0f );
    static const glm::vec3 frameAxis2( 0.0f, 1.0f, 0.0f );

    const glm::vec3 worldAxis1 = glm::vec3( world_O_frame[0] );
    const glm::vec3 worldAxis2 = glm::vec3( world_O_frame[1] );

    setRotation( frameAxis1, worldAxis1,
                 frameAxis2, worldAxis2,
                 k_requireEqualAngles );
}
#endif
