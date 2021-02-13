#include "rendering/utility/math/SliceIntersector.h"
#include "rendering/utility/math/MathUtility.h"

#include <glm/glm.hpp>

#include <glm/gtc/matrix_inverse.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>


SliceIntersector::SliceIntersector()
    :
      m_positioningMethod( PositioningMethod::FrameOrigin ),
      m_alignmentMethod( AlignmentMethod::CameraZ ),

      m_cameraSliceOffset( 0.0f, 0.0f, -1.0f ),
      m_userSlicePosition( 0.0f, 0.0f, 0.0f ),
      m_userSliceNormal( 1.0f, 0.0f, 0.0f ),

      m_modelPlaneEquation( 1.0f, 0.0f, 0.0f, 0.0f )
{}


void SliceIntersector::setPositioningMethod(
        const PositioningMethod& method,
        const boost::optional<glm::vec3>& p )
{
    m_positioningMethod = method;

    if ( p )
    {
        switch ( m_positioningMethod )
        {
        case PositioningMethod::UserDefined :
        {
            m_userSlicePosition = *p;
            break;
        }
        case PositioningMethod::OffsetFromCamera :
        {
            m_cameraSliceOffset = *p;
            break;
        }
        case PositioningMethod::FrameOrigin :
            break;
        }
    }
}


void SliceIntersector::setAlignmentMethod(
        const AlignmentMethod& method,
        const boost::optional<glm::vec3>& worldNormal )
{
    m_alignmentMethod = method;

    if ( AlignmentMethod::UserDefined == method )
    {
        if ( worldNormal )
        {
            if ( 0.0f < glm::length2( *worldNormal ) )
            {
                m_userSliceNormal = glm::normalize( *worldNormal );
            }
        }
    }
}


std::pair< boost::optional< SliceIntersector::IntersectionVertices >, glm::vec4 >
SliceIntersector::computePlaneIntersections(
        const glm::mat4& model_O_camera,
        const glm::mat4& model_O_frame,
        const std::array< glm::vec3, 8 >& modelBoxCorners )
{
    updatePlaneEquation( model_O_camera, model_O_frame );

    return std::make_pair( math::computeAABBoxPlaneIntersections<float>(
                               modelBoxCorners, m_modelPlaneEquation ),
                           m_modelPlaneEquation );
}


const SliceIntersector::PositioningMethod& SliceIntersector::positioningMethod() const
{
    return m_positioningMethod;
}


const SliceIntersector::AlignmentMethod& SliceIntersector::alignmentMethod() const
{
    return m_alignmentMethod;
}


void SliceIntersector::updatePlaneEquation(
        const glm::mat4& model_O_camera,
        const glm::mat4& model_O_frame )
{
    glm::vec3 position;
    glm::vec3 normal;

    switch ( m_positioningMethod )
    {
    case PositioningMethod::OffsetFromCamera :
    {
        const glm::vec4 p = model_O_camera * glm::vec4{ m_cameraSliceOffset, 1.0f };
        position = glm::vec3{ p / p.w };
        break;
    }
    case PositioningMethod::FrameOrigin :
    {
        const glm::vec4 p = model_O_frame[3];
        position = glm::vec3{ p / p.w };
        break;
    }
    case PositioningMethod::UserDefined :
    {
        position = m_userSlicePosition;
        break;
    }
    }


    switch ( m_alignmentMethod )
    {
    case AlignmentMethod::CameraZ :
    {
        normal = glm::vec3( glm::inverseTranspose( model_O_camera )[2] );
        break;
    }
    case AlignmentMethod::FrameX :
    {
        normal = glm::vec3( glm::inverseTranspose( model_O_frame )[0] );
        break;
    }
    case AlignmentMethod::FrameY :
    {
        normal = glm::vec3( glm::inverseTranspose( model_O_frame )[1] );
        break;
    }
    case AlignmentMethod::FrameZ :
    {
        normal = glm::vec3( glm::inverseTranspose( model_O_frame )[2] );
        break;
    }
    case AlignmentMethod::UserDefined :
    {
        normal = m_userSliceNormal;
        break;
    }
    }

    m_modelPlaneEquation = math::makePlane( glm::normalize( normal ), position );
}
