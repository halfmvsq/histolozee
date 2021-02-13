#ifndef GL_SAMPLER_OBJECT_H
#define GL_SAMPLER_OBJECT_H

#include "rendering/utility/gl/GLTextureTypes.h"
#include "rendering/utility/gl/GLErrorChecker.h"

#include <glm/fwd.hpp>

#include <QOpenGLFunctions_3_3_Core>


class GLSamplerObject final : protected QOpenGLFunctions_3_3_Core
{
public:

    explicit GLSamplerObject();

    GLSamplerObject( const GLSamplerObject& ) = delete;
    GLSamplerObject( GLSamplerObject&& );

    GLSamplerObject& operator=( const GLSamplerObject& ) = delete;
    GLSamplerObject& operator=( GLSamplerObject&& );

    ~GLSamplerObject();

    void generate();
    void release();

    // Bind/bind sampler object to/from a texture unit
    /// @todo Store Sampler Object as member of GPU image record
    void bind( uint32_t textureUnit );
    void unbind( uint32_t textureUnit );

    // Whether the sampler object is currently bound to the active texture unit
    bool isBound( uint32_t textureUnit );

    GLuint id() const;

    void setMinificationFilter( const tex::MinificationFilter& filter );
    void setMagnificationFilter( const tex::MagnificationFilter& filter );

    void setSwizzleMask(
            const tex::SwizzleValue& rValue,
            const tex::SwizzleValue& gValue,
            const tex::SwizzleValue& bValue,
            const tex::SwizzleValue& aValue );

    void setWrapMode( const tex::SamplingDirection& dir, const tex::WrapMode& mode );

    void setBorderColor( const glm::vec4& color );


private:

    GLuint m_id;
};

#endif // GL_SAMPLER_OBJECT_H
