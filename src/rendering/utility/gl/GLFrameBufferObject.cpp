#include "rendering/utility/gl/GLFrameBufferObject.h"
#include "rendering/utility/UnderlyingEnumType.h"

#include "common/HZeeException.hpp"

#include <sstream>


GLFrameBufferObject::GLFrameBufferObject( const std::string& name )
    : m_name( name ),
      m_id( 0u )
{
    initializeOpenGLFunctions();
}

GLFrameBufferObject::GLFrameBufferObject( GLFrameBufferObject&& other )
    : m_name( other.m_name ),
      m_id( other.m_id )
{
    initializeOpenGLFunctions();

    other.m_name = "";
    other.m_id = 0u;
}

GLFrameBufferObject& GLFrameBufferObject::operator=( GLFrameBufferObject&& other )
{
    if ( this != &other )
    {
        destroy();

        std::swap( m_name, other.m_name );
        std::swap( m_id, other.m_id );
    }

    return *this;
}

GLFrameBufferObject::~GLFrameBufferObject()
{
    destroy();
}

void GLFrameBufferObject::generate()
{
    glGenFramebuffers( 1, &m_id );
}

void GLFrameBufferObject::destroy()
{
    glDeleteFramebuffers( 1, &m_id );
}

void GLFrameBufferObject::bind( const fbo::TargetType& target )
{
    glBindFramebuffer( underlyingType( target ), m_id );
}

void GLFrameBufferObject::attach2DTexture(
        const fbo::TargetType& target,
        const fbo::AttachmentType& attachment,
        const GLTexture& texture,
        std::optional<int> colorAttachmentIndex )
{
    if ( fbo::TargetType::DrawAndRead == target )
    {
        throw_debug( "Invalid FBO target" );
    }

    if ( tex::Target::Texture2D != texture.target() &&
         tex::Target::Texture2DMultisample != texture.target() &&
         tex::Target::TextureRectangle != texture.target() )
    {
        throw_debug( "Invalid texture target" );
    }

    int index = 0;

    if ( fbo::AttachmentType::Color == attachment  )
    {
        if ( colorAttachmentIndex )
        {
            // Get maximum color attachment point for the FBO
            GLint maxAttach = 0;
            glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &maxAttach );

            if ( *colorAttachmentIndex < 0 || maxAttach <= *colorAttachmentIndex )
            {
                std::ostringstream ss;
                ss << "Invalid color attachment index " << *colorAttachmentIndex << std::ends;
                throw_debug( ss.str() );
            }

            index = *colorAttachmentIndex;
        }
        else
        {
            std::ostringstream ss;
            ss << "No color attachment index specified" << std::ends;
            throw_debug( ss.str() );
        }
    }

    glFramebufferTexture2D(
                underlyingType( target ),
                underlyingType( attachment ) + static_cast<GLenum>(index),
                underlyingType( texture.target() ),
                texture.id(), 0 );

    checkStatus();
}

//GLint maxDrawBuf = 0;
//glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuf);

void GLFrameBufferObject::attachCubeMapTexture(
        const fbo::TargetType& target,
        const fbo::AttachmentType& attachment,
        const GLTexture& texture,
        const tex::CubeMapFace& cubeMapFace,
        GLint level,
        std::optional<int> colorAttachmentIndex )
{
    if ( tex::Target::TextureCubeMap != texture.target() )
    {
        throw_debug( "Invalid FBO target" );
    }

    int index = 0;

    if ( fbo::AttachmentType::Color == attachment && colorAttachmentIndex )
    {
        // Get maximum color attachment point for the FBO
        GLint maxAttach = 0;
        glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &maxAttach );

        if ( *colorAttachmentIndex < 0 || maxAttach <= *colorAttachmentIndex )
        {
            std::ostringstream ss;
            ss << "Invalid color attachment index " << *colorAttachmentIndex << std::ends;
            throw_debug( ss.str() );
        }

        index = *colorAttachmentIndex;
    }

    glFramebufferTexture2D(
                underlyingType( target ),
                underlyingType( attachment ) + static_cast<GLenum>(index),
                underlyingType( cubeMapFace ),
                texture.id(), level );

    checkStatus();
}

GLuint GLFrameBufferObject::id() const
{
    return m_id;
}

void GLFrameBufferObject::checkStatus()
{
    GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

    if ( GL_FRAMEBUFFER_COMPLETE != status )
    {
        std::ostringstream ss;
        ss << "Framebuffer object " << m_name << " not complete: "
           << glCheckFramebufferStatus( GL_FRAMEBUFFER ) << std::ends;
        throw_debug( ss.str() );
    }
}
