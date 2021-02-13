#include "rendering/utility/gl/GLSamplerObject.h"
#include "rendering/utility/UnderlyingEnumType.h"

#include "common/HZeeException.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace tex;


GLSamplerObject::GLSamplerObject()
    :
      m_id( 0 )
{
    initializeOpenGLFunctions();
}

GLSamplerObject::GLSamplerObject( GLSamplerObject&& other )
    :
      m_id( std::move( other.m_id ) )
{
    initializeOpenGLFunctions();

    other.m_id = 0;
}

GLSamplerObject& GLSamplerObject::operator=( GLSamplerObject&& other )
{
    if ( this != &other )
    {
        release();

        std::swap( m_id, other.m_id );
    }

    return *this;
}

GLSamplerObject::~GLSamplerObject()
{
    release();
}

void GLSamplerObject::generate()
{
    glGenSamplers( 1, &m_id );
}

void GLSamplerObject::release()
{
    glDeleteSamplers( 1, &m_id );
    m_id = 0;
}

void GLSamplerObject::bind( uint32_t textureUnit )
{
    /// When a sampler object is bound to a texture image unit, the internal sampling
    /// parameters for a texture bound to the same image unit are all ignored.
    /// Instead, the sampling parameters are taken from this sampler object.

    glBindSampler( textureUnit, m_id );
}

bool GLSamplerObject::isBound( uint32_t textureUnit )
{
    GLint oldTextureUnit = 0;
    glGetIntegerv( GL_ACTIVE_TEXTURE, &oldTextureUnit );

    GLint boundID = 0;
    glActiveTexture( GL_TEXTURE0 + textureUnit );
    {
        glGetIntegerv( GL_SAMPLER_BINDING, &boundID );
    }
    glActiveTexture( GL_TEXTURE0 + static_cast<GLenum>( oldTextureUnit ) );

    return ( static_cast<GLuint>( boundID ) == m_id );
}

void GLSamplerObject::unbind( uint32_t textureUnit )
{
    glBindSampler( textureUnit, 0 );
}

GLuint GLSamplerObject::id() const
{
    return m_id;
}

void GLSamplerObject::setMinificationFilter( const MinificationFilter& filter )
{
    glSamplerParameteri( m_id, GL_TEXTURE_MIN_FILTER, underlyingType_asInt32(filter) );
}

void GLSamplerObject::setMagnificationFilter( const MagnificationFilter& filter )
{
    glSamplerParameteri( m_id, GL_TEXTURE_MAG_FILTER, underlyingType_asInt32(filter) );
}

void GLSamplerObject::setSwizzleMask(
        const SwizzleValue& rValue,
        const SwizzleValue& gValue,
        const SwizzleValue& bValue,
        const SwizzleValue& aValue )
{
    const GLint mask[] = {
        static_cast<GLint>( underlyingType(rValue) ),
        static_cast<GLint>( underlyingType(gValue) ),
        static_cast<GLint>( underlyingType(bValue) ),
        static_cast<GLint>( underlyingType(aValue) ) };

    glSamplerParameteriv( m_id, GL_TEXTURE_SWIZZLE_RGBA, mask );
}

void GLSamplerObject::setWrapMode( const SamplingDirection& dir, const WrapMode& mode )
{
    glSamplerParameteri( m_id, underlyingType_asUInt32( dir ), underlyingType_asInt32( mode ) );
}

void GLSamplerObject::setBorderColor( const glm::vec4& color )
{
    glSamplerParameterfv( m_id, GL_TEXTURE_BORDER_COLOR, glm::value_ptr( color ) );
}
