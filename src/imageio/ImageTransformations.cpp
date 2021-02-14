#include "ImageTransformations.h"

#include "util/HZeeException.hpp"
#include "util/MathFuncs.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>


namespace imageio
{

ImageTransformations::ImageTransformations(
        const glm::u64vec3& pixelDimensions,
        const glm::dvec3& spacing,
        const glm::dvec3& origin,
        const glm::dmat3& directions,
        const glm::vec3& worldSubjectOrigin,
        const glm::quat& subjectToWorldRotation )
    :
      m_subjectDimensions( math::subjectImageDimensions( pixelDimensions, spacing ) ),

      m_subject_O_pixel( math::computeImagePixelToSubjectTransformation( directions, spacing, origin ) ),
      m_pixel_O_subject( glm::inverse( m_subject_O_pixel ) ),

      m_texture_O_pixel( math::computeImagePixelToTextureTransformation( pixelDimensions ) ),
      m_pixel_O_texture( glm::inverse( m_texture_O_pixel ) ),

      m_texture_O_subject( m_texture_O_pixel * m_pixel_O_subject ),
      m_subject_O_texture( glm::inverse( m_texture_O_subject ) ),

      m_worldSubjectOrigin( worldSubjectOrigin ),
      m_subjectToWorldRotation( subjectToWorldRotation ),

      m_world_O_subject( 1.0f ),
      m_subject_O_world( 1.0f ),

      m_world_O_texture( 1.0f ),
      m_texture_O_world( 1.0f ),

      m_world_O_pixel( 1.0f ),
      m_pixel_O_world( 1.0f )
{   
    if ( 0.0f == glm::determinant( m_subject_O_pixel ) ||
         0.0f == glm::determinant( m_texture_O_pixel ) ||
         0.0f == glm::determinant( m_texture_O_subject ) )
    {
        throw std::invalid_argument( "Exception while constructing ImageTransformations" );
    }

    update_world_O_subject();
}


void ImageTransformations::setWorldSubjectOrigin( glm::vec3 worldSubjectOrigin )
{
    m_worldSubjectOrigin = std::move( worldSubjectOrigin );
    update_world_O_subject();
}

glm::vec3 ImageTransformations::getWorldSubjectOrigin() const
{
    return m_worldSubjectOrigin;
}

void ImageTransformations::setSubjectToWorldRotation( glm::quat subjectToWorldRotation )
{
    m_subjectToWorldRotation = std::move( subjectToWorldRotation );
    update_world_O_subject();
}

glm::quat ImageTransformations::getSubjectToWorldRotation() const
{
    return m_subjectToWorldRotation;
}


void ImageTransformations::update_world_O_subject()
{
    m_world_O_subject = glm::translate( m_worldSubjectOrigin ) *
            glm::toMat4( m_subjectToWorldRotation );

    m_subject_O_world = glm::inverse( m_world_O_subject );

    m_world_O_texture = m_world_O_subject * m_subject_O_texture;
    m_texture_O_world = glm::inverse( m_world_O_texture );

    m_world_O_pixel = m_world_O_subject * m_subject_O_pixel;
    m_pixel_O_world = glm::inverse( m_world_O_pixel );
}


const glm::vec3& ImageTransformations::subjectDimensions() const
{
    return m_subjectDimensions;
}

const glm::mat4& ImageTransformations::subject_O_pixel() const
{
    return m_subject_O_pixel;
}

const glm::mat4& ImageTransformations::pixel_O_subject() const
{
    return m_pixel_O_subject;
}

const glm::mat4& ImageTransformations::pixel_O_texture() const
{
    return m_pixel_O_texture;
}

const glm::mat4& ImageTransformations::texture_O_pixel() const
{
    return m_texture_O_pixel;
}

const glm::mat4& ImageTransformations::subject_O_texture() const
{
    return m_subject_O_texture;
}

const glm::mat4& ImageTransformations::texture_O_subject() const
{
    return m_texture_O_subject;
}

const glm::mat4& ImageTransformations::world_O_subject() const
{
    return m_world_O_subject;
}

const glm::mat4& ImageTransformations::subject_O_world() const
{
    return m_subject_O_world;
}

const glm::mat4& ImageTransformations::world_O_texture() const
{
    return m_world_O_texture;
}

const glm::mat4& ImageTransformations::texture_O_world() const
{
    return m_texture_O_world;
}

const glm::mat4& ImageTransformations::world_O_pixel() const
{
    return m_world_O_pixel;
}

const glm::mat4& ImageTransformations::pixel_O_world() const
{
    return m_pixel_O_world;
}

glm::mat4 ImageTransformations::pixel_O_world_invTranspose() const
{
    return glm::transpose( m_world_O_pixel );
}

} // namespace imageio


std::ostream& operator<< ( std::ostream& os, const imageio::ImageTransformations& tx )
{
    os << "Subject dimensions: "   << glm::to_string( tx.subjectDimensions() ) << std::endl
       << "subject_O_pixel tx: "   << glm::to_string( tx.subject_O_pixel() ) << std::endl
       << "pixel_O_subject tx: "   << glm::to_string( tx.pixel_O_subject() ) << std::endl
       << "texture_O_pixel tx: "   << glm::to_string( tx.texture_O_pixel() ) << std::endl
       << "pixel_O_texture tx: "   << glm::to_string( tx.pixel_O_texture() ) << std::endl
       << "subject_O_texture tx: " << glm::to_string( tx.world_O_texture() ) << std::endl
       << "texture_O_subject tx: " << glm::to_string( tx.texture_O_world() ) << std::endl
       << std::endl;

    return os;
}
