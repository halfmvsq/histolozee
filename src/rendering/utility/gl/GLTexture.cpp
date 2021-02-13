#include "rendering/utility/gl/GLTexture.h"
#include "rendering/utility/UnderlyingEnumType.h"

#include "common/HZeeException.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <algorithm>


using namespace tex;

const std::unordered_map< Target, Binding >
GLTexture::s_bindingMap =
{
    { Target::Texture1D, Binding::TextureBinding1D },
    { Target::Texture2D, Binding::TextureBinding2D },
    { Target::Texture3D, Binding::TextureBinding3D },
    { Target::TextureCubeMap, Binding::TextureBindingCubeMap },
    { Target::Texture1DArray, Binding::TextureBinding1DArray },
    { Target::Texture2DArray, Binding::TextureBinding2DArray },
    { Target::Texture2DMultisample, Binding::TextureBinding2DMultisample },
    { Target::TextureRectangle, Binding::TextureBindingRectangle },
    { Target::Texture2DMultisampleArray, Binding::TextureBinding2DMultisampleArray }
};


const std::unordered_map< imageio::ComponentType, SizedInternalFormat >
GLTexture::s_componentTypeToSizedInternalNormalizedRedFormatMap =
{
    { imageio::ComponentType::Int8, SizedInternalFormat::R8_SNorm },
    { imageio::ComponentType::UInt8, SizedInternalFormat::R8_UNorm },
    { imageio::ComponentType::Int16, SizedInternalFormat::R16_SNorm },
    { imageio::ComponentType::UInt16, SizedInternalFormat::R16_UNorm },
    { imageio::ComponentType::Int32, SizedInternalFormat::R32F },
    { imageio::ComponentType::UInt32, SizedInternalFormat::R32F },
    { imageio::ComponentType::Float32, SizedInternalFormat::R32F }
};

const std::unordered_map< imageio::ComponentType, SizedInternalFormat >
GLTexture::s_componentTypeToSizedInternalRedFormatMap =
{
    { imageio::ComponentType::Int8, SizedInternalFormat::R8I },
    { imageio::ComponentType::UInt8, SizedInternalFormat::R8U },
    { imageio::ComponentType::Int16, SizedInternalFormat::R16I },
    { imageio::ComponentType::UInt16, SizedInternalFormat::R16U },
    { imageio::ComponentType::Int32, SizedInternalFormat::R32I },
    { imageio::ComponentType::UInt32, SizedInternalFormat::R32U },
    { imageio::ComponentType::Float32, SizedInternalFormat::R32F }
};

const std::unordered_map< imageio::ComponentType, SizedInternalFormat >
GLTexture::s_componentTypeToSizedInternalRGBAFormatMap =
{
    { imageio::ComponentType::Int8, SizedInternalFormat::RGBA8I },
    { imageio::ComponentType::UInt8, SizedInternalFormat::RGBA8U },
    { imageio::ComponentType::Int16, SizedInternalFormat::RGBA16I },
    { imageio::ComponentType::UInt16, SizedInternalFormat::RGBA16U },
    { imageio::ComponentType::Int32, SizedInternalFormat::RGBA32I },
    { imageio::ComponentType::UInt32, SizedInternalFormat::RGBA32U },
    { imageio::ComponentType::Float32, SizedInternalFormat::RGBA32F }
};


const std::unordered_map< imageio::ComponentType, BufferPixelFormat >
GLTexture::s_componentTypeToBufferPixelRedNormalizedFormatMap =
{
    { imageio::ComponentType::Int8, BufferPixelFormat::Red },
    { imageio::ComponentType::UInt8, BufferPixelFormat::Red },
    { imageio::ComponentType::Int16, BufferPixelFormat::Red },
    { imageio::ComponentType::UInt16, BufferPixelFormat::Red },
    { imageio::ComponentType::Int32, BufferPixelFormat::Red },
    { imageio::ComponentType::UInt32, BufferPixelFormat::Red },
    { imageio::ComponentType::Float32, BufferPixelFormat::Red }
};

const std::unordered_map< imageio::ComponentType, BufferPixelFormat >
GLTexture::s_componentTypeToBufferPixelRedFormatMap =
{
    { imageio::ComponentType::Int8, BufferPixelFormat::Red_Integer },
    { imageio::ComponentType::UInt8, BufferPixelFormat::Red_Integer },
    { imageio::ComponentType::Int16, BufferPixelFormat::Red_Integer },
    { imageio::ComponentType::UInt16, BufferPixelFormat::Red_Integer },
    { imageio::ComponentType::Int32, BufferPixelFormat::Red_Integer },
    { imageio::ComponentType::UInt32, BufferPixelFormat::Red_Integer },
    { imageio::ComponentType::Float32, BufferPixelFormat::Red }
};

const std::unordered_map< imageio::ComponentType, BufferPixelFormat >
GLTexture::s_componentTypeToBufferPixelRGBAFormatMap =
{
    { imageio::ComponentType::Int8, BufferPixelFormat::RGBA_Integer },
    { imageio::ComponentType::UInt8, BufferPixelFormat::RGBA_Integer },
    { imageio::ComponentType::Int16, BufferPixelFormat::RGBA_Integer },
    { imageio::ComponentType::UInt16, BufferPixelFormat::RGBA_Integer },
    { imageio::ComponentType::Int32, BufferPixelFormat::RGBA_Integer },
    { imageio::ComponentType::UInt32, BufferPixelFormat::RGBA_Integer },
    { imageio::ComponentType::Float32, BufferPixelFormat::RGBA }
};


const std::unordered_map< imageio::ComponentType, BufferPixelDataType >
GLTexture::s_componentTypeToBufferPixelDataTypeMap =
{
    { imageio::ComponentType::Int8, BufferPixelDataType::Int8 },
    { imageio::ComponentType::UInt8, BufferPixelDataType::UInt8 },
    { imageio::ComponentType::Int16, BufferPixelDataType::Int16 },
    { imageio::ComponentType::UInt16, BufferPixelDataType::UInt16 },
    { imageio::ComponentType::Int32, BufferPixelDataType::Int32 },
    { imageio::ComponentType::UInt32, BufferPixelDataType::UInt32 },
    { imageio::ComponentType::Float32, BufferPixelDataType::Float32 }
};


/// componentTypeToSizedInternalFormat?
SizedInternalFormat GLTexture::getSizedInternalNormalizedRedFormat( const imageio::ComponentType& componentType )
{
    return s_componentTypeToSizedInternalNormalizedRedFormatMap.at( componentType );
}

SizedInternalFormat GLTexture::getSizedInternalRedFormat( const imageio::ComponentType& componentType )
{
    return s_componentTypeToSizedInternalRedFormatMap.at( componentType );
}

SizedInternalFormat GLTexture::getSizedInternalRGBAFormat( const imageio::ComponentType& componentType )
{
    return s_componentTypeToSizedInternalRGBAFormatMap.at( componentType );
}

BufferPixelFormat GLTexture::getBufferPixelNormalizedRedFormat( const imageio::ComponentType& componentType )
{
    return s_componentTypeToBufferPixelRedNormalizedFormatMap.at( componentType );
}

BufferPixelFormat GLTexture::getBufferPixelRedFormat( const imageio::ComponentType& componentType )
{
    return s_componentTypeToBufferPixelRedFormatMap.at( componentType );
}

BufferPixelFormat GLTexture::getBufferPixelRGBAFormat( const imageio::ComponentType& componentType )
{
    return s_componentTypeToBufferPixelRGBAFormatMap.at( componentType );
}

BufferPixelDataType GLTexture::getBufferPixelDataType( const imageio::ComponentType& componentType )
{
    return s_componentTypeToBufferPixelDataTypeMap.at( componentType );
}


GLTexture::Binder::Binder( GLTexture& texture )
    : m_texture( texture ),
      m_boundID( 0 )
{
    m_texture.glGetIntegerv( underlyingType( s_bindingMap.at(m_texture.m_target) ), &m_boundID );
    m_texture.glBindTexture( m_texture.m_targetEnum, static_cast<GLuint>( m_texture.m_id ) );
}

GLTexture::Binder::~Binder()
{
    m_texture.glBindTexture( m_texture.m_targetEnum, static_cast<GLuint>( m_boundID ) );
}


GLTexture::GLTexture(
        Target target,
        MultisampleSettings multisampleSettings,
        boost::optional<PixelStoreSettings> pixelPackSettings,
        boost::optional<PixelStoreSettings> pixelUnpackSettings )
    :
      m_target( std::move( target ) ),
      m_targetEnum( underlyingType( m_target ) ),
      m_id( 0 ),
      m_size( 1 ),
      m_autoGenerateMipmaps( false ),
      m_samplerID( 0 ),
      m_multisampleSettings( std::move( multisampleSettings ) ),
      m_pixelPackSettings( std::move( pixelPackSettings ) ),
      m_pixelUnpackSettings( std::move( pixelUnpackSettings ) )
{
    initializeOpenGLFunctions();
}

GLTexture::GLTexture( GLTexture&& other )
    :
      m_target( std::move( other.m_target ) ),
      m_targetEnum( std::move( other.m_targetEnum ) ),
      m_id( std::move( other.m_id ) ),
      m_size( std::move( other.m_size ) ),
      m_autoGenerateMipmaps( std::move( other.m_autoGenerateMipmaps ) ),
      m_multisampleSettings( std::move( other.m_multisampleSettings ) ),
      m_pixelPackSettings( std::move( other.m_pixelPackSettings ) ),
      m_pixelUnpackSettings( std::move( other.m_pixelUnpackSettings ) )
{
    initializeOpenGLFunctions();

    other.m_id = 0;
    other.m_size = glm::uvec3{ 1 };
    other.m_autoGenerateMipmaps = false;
    other.m_multisampleSettings = MultisampleSettings();
    other.m_pixelPackSettings = PixelStoreSettings();
    other.m_pixelUnpackSettings = PixelStoreSettings();
}

GLTexture& GLTexture::operator=( GLTexture&& other )
{
    if ( this != &other )
    {
        release();

        std::swap( m_id, other.m_id );
        std::swap( m_size, other.m_size );
        std::swap( m_autoGenerateMipmaps, other.m_autoGenerateMipmaps );
        std::swap( m_multisampleSettings, other.m_multisampleSettings );
        std::swap( m_pixelPackSettings, other.m_pixelPackSettings );
        std::swap( m_pixelUnpackSettings, other.m_pixelUnpackSettings );
    }

    return *this;
}

GLTexture::~GLTexture()
{
    release();
}

void GLTexture::generate()
{
    glGenTextures( 1, &m_id );

    // Generate sampler object for this texture
    glGenSamplers( 1, &m_samplerID );
}

void GLTexture::release( boost::optional<uint32_t> textureUnit )
{
    //    GLint oldTextureUnit = 0;
    //    if (reset == QOpenGLTexture::ResetTextureUnit)
    //        glGetIntegerv(GL_ACTIVE_TEXTURE, &oldTextureUnit);

    //    texFuncs->glActiveTexture(GL_TEXTURE0 + unit);
    //    glBindTexture(target, textureId);

    //    if (reset == QOpenGLTexture::ResetTextureUnit)
    //        texFuncs->glActiveTexture(GL_TEXTURE0 + oldTextureUnit);

    if ( textureUnit )
    {
        glActiveTexture( GL_TEXTURE0 + *textureUnit );
    }

    glDeleteTextures( 1, &m_id );

    glDeleteSamplers( 1, &m_samplerID );

    m_id = 0;
    m_size = glm::uvec3{ 1 };
    m_autoGenerateMipmaps = false;
    m_samplerID = 0;

    m_multisampleSettings = MultisampleSettings();
    m_pixelPackSettings = boost::none;
    m_pixelUnpackSettings = boost::none;
}

void GLTexture::bind( boost::optional<uint32_t> textureUnit )
{
    static constexpr bool rebind = false;

    GLint prevUnit = 0;

    if ( textureUnit )
    {
        if ( rebind ) glGetIntegerv( GL_ACTIVE_TEXTURE, &prevUnit );
        glActiveTexture( GL_TEXTURE0 + *textureUnit );
    }

    glBindTexture( m_targetEnum, m_id );

    if ( rebind && textureUnit )
    {
        glActiveTexture( static_cast<GLenum>( GL_TEXTURE0 + prevUnit ) );
    }
}

bool GLTexture::isBound( boost::optional<uint32_t> textureUnit )
{
    GLint prevUnit = 0;

    if ( textureUnit )
    {
        glGetIntegerv( GL_ACTIVE_TEXTURE, &prevUnit );
        glActiveTexture( GL_TEXTURE0 + *textureUnit );
    }

    GLint boundID = 0;
    glGetIntegerv( underlyingType( s_bindingMap.at(m_target) ), &boundID );

    const bool result = ( static_cast<GLuint>( boundID ) == m_id );

    if ( textureUnit )
    {
        glActiveTexture( static_cast<GLenum>( GL_TEXTURE0 + prevUnit ) );
    }

    return result;
}

void GLTexture::unbind()
{
    glBindTexture( m_targetEnum, 0 );
}

void GLTexture::bindSampler( uint32_t textureUnit )
{
    /// When a sampler object is bound to a texture image unit, the internal sampling
    /// parameters for a texture bound to the same image unit are all ignored.
    /// Instead, the sampling parameters are taken from this sampler object.

    glBindSampler( textureUnit, m_samplerID );
}

void GLTexture::unbindSampler( uint32_t textureUnit )
{
    glBindSampler( textureUnit, 0 );
}

Target GLTexture::target() const
{
    return m_target;
}

GLuint GLTexture::id() const
{
    return m_id;
}

glm::uvec3 GLTexture::size() const
{
    return m_size;
}

void GLTexture::setSize( const glm::uvec3& size )
{
    if ( glm::any( glm::lessThan( size, glm::uvec3{1} ) ) )
    {
        std::ostringstream ss;
        ss << "Invalid texture size " << glm::to_string( size ) << std::ends;
        throw_debug( ss.str() );
    }

    m_size = size;
}

void GLTexture::setData(
        GLint level,
        const SizedInternalFormat& internalFormat,
        const BufferPixelFormat& format,
        const BufferPixelDataType& type,
        const GLvoid* data )
{
    if ( Target::TextureCubeMap == m_target ||
         Target::TextureBuffer == m_target )
    {
        throw_debug( "Invalid texture target type ");
    }

    const GLint _internalFormat = underlyingType_asInt32( internalFormat );
    const GLenum _format = underlyingType( format );
    const GLenum _type = underlyingType( type );
    const glm::ivec3 _size( m_size );

    Binder binder( *this );

    boost::optional<PixelStoreSettings> oldUnpackSettings = boost::none;

    if ( m_pixelUnpackSettings )
    {
        oldUnpackSettings = getPixelUnpackSettings();
        applyPixelUnpackSettings( *m_pixelUnpackSettings );
    }

    switch ( m_target )
    {
    case Target::Texture1D :
    {
        glTexImage1D( m_targetEnum, level, _internalFormat,
                      _size.x,
                      0, _format, _type, data );
        break;
    }
    case Target::Texture2D :
    {
        glTexImage2D( m_targetEnum, level, _internalFormat,
                      _size.x, _size.y,
                      0, _format, _type, data );

        /// @todo This needs its own function
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4 );

        // new!
        glSamplerParameteri( m_samplerID, GL_TEXTURE_MAX_LEVEL, 4 );
        break;
    }
    case Target::Texture3D :
    {
        glTexImage3D( m_targetEnum, level, _internalFormat,
                      _size.x, _size.y, _size.z,
                      0, _format, _type, data );
        break;
    }
    case Target::Texture1DArray :
    {
        glTexImage2D( m_targetEnum, level, _internalFormat,
                      _size.x, _size.y,
                      0, _format, _type, data );
        break;
    }
    case Target::Texture2DArray :
    {
        glTexImage3D( m_targetEnum, level, _internalFormat,
                      _size.x, _size.y, _size.z,
                      0, _format, _type, data );
        break;
    }
    case Target::Texture2DMultisample :
    {
        glTexImage2DMultisample(
                    m_targetEnum, m_multisampleSettings.m_numSamples, _internalFormat,
                    _size.x, _size.y,
                    m_multisampleSettings.m_fixedSampleLocations );
        break;
    }
    case Target::TextureRectangle :
    {
        glTexImage2D( m_targetEnum, 0, _internalFormat,
                      _size.x, _size.y,
                      0, _format, _type, data );
        break;
    }
    case Target::Texture2DMultisampleArray :
    {
        glTexImage3DMultisample(
                    m_targetEnum, m_multisampleSettings.m_numSamples, _internalFormat,
                    _size.x, _size.y, _size.z,
                    m_multisampleSettings.m_fixedSampleLocations );
        break;
    }
    case Target::TextureCubeMap :
    case Target::TextureBuffer :
    {
        break;
    }
    }

    if ( ! ( Target::Texture2DMultisample == m_target ||
             Target::TextureRectangle == m_target ||
             Target::Texture2DMultisampleArray == m_target ) )
    {
        if ( m_autoGenerateMipmaps )
        {
            glGenerateMipmap( m_targetEnum );
        }
    }

    if ( oldUnpackSettings )
    {
        applyPixelUnpackSettings( *oldUnpackSettings );
    }

    CHECK_GL_ERROR( m_errorChecker );
}

void GLTexture::setSubData(
        GLint level,
        const glm::uvec3& offset,
        const glm::uvec3& size,
        const BufferPixelFormat& format,
        const BufferPixelDataType& type,
        const GLvoid* data )
{
    if ( Target::Texture2DMultisample == m_target ||
         Target::TextureRectangle == m_target ||
         Target::Texture2DMultisampleArray == m_target ||
         Target::TextureCubeMap == m_target ||
         Target::TextureBuffer == m_target )
    {
        throw_debug( "Invalid texture target type ");
    }

    const GLenum _format = underlyingType(format);
    const GLenum _type = underlyingType(type);
    const glm::ivec3 _offset( offset );
    const glm::ivec3 _size( size );

    Binder binder( *this );

    boost::optional<PixelStoreSettings> oldUnpackSettings = boost::none;

    if ( m_pixelUnpackSettings )
    {
        oldUnpackSettings = getPixelUnpackSettings();
        applyPixelUnpackSettings( *m_pixelUnpackSettings );
    }

    switch ( m_target )
    {
    case Target::Texture1D :
        glTexSubImage1D( m_targetEnum, level, _offset.x, _size.x, _format, _type, data );
        break;

    case Target::Texture2D :
        glTexSubImage2D( m_targetEnum, level, _offset.x, _offset.y,
                         _size.x, _size.y, _format, _type, data );
        break;

    case Target::Texture3D :
        glTexSubImage3D( m_targetEnum, level, _offset.x, _offset.y, _offset.z,
                         _size.x, _size.y, _size.z, _format, _type, data );
        break;

    case Target::Texture1DArray :
        glTexSubImage2D( m_targetEnum, level, _offset.x, _offset.y,
                         _size.x, _size.y, _format, _type, data );
        break;

    case Target::Texture2DArray :
        glTexSubImage3D( m_targetEnum, level, _offset.x, _offset.y, _offset.z,
                         _size.x, _size.y, _size.z, _format, _type, data );
        break;

    case Target::Texture2DMultisample :
    case Target::TextureRectangle :
    case Target::Texture2DMultisampleArray :
    case Target::TextureCubeMap :
    case Target::TextureBuffer :
        break;
    }

    if ( oldUnpackSettings )
    {
        applyPixelUnpackSettings( *oldUnpackSettings );
    }

    CHECK_GL_ERROR( m_errorChecker );
}

void GLTexture::setCubeMapFaceData(
        const CubeMapFace& face,
        GLint level,
        const SizedInternalFormat& internalFormat,
        const BufferPixelFormat& format,
        const BufferPixelDataType& type,
        const GLvoid* data )
{
    //    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    const glm::ivec3 _size( m_size );

    Binder binder( *this );

    boost::optional<PixelStoreSettings> oldUnpackSettings = boost::none;

    if ( m_pixelUnpackSettings )
    {
        oldUnpackSettings = getPixelUnpackSettings();
        applyPixelUnpackSettings( *m_pixelUnpackSettings );
    }

    glTexImage2D( underlyingType(face), level, underlyingType_asInt32(internalFormat),
                  _size.x, _size.y,
                  0, underlyingType(format), underlyingType(type), data );

//    if ( m_autoGenerateMipmaps )
//    {
//        glGenerateMipmap( m_targetEnum );
//    }

    if ( oldUnpackSettings )
    {
        applyPixelUnpackSettings( *oldUnpackSettings );
    }

    CHECK_GL_ERROR( m_errorChecker );
}

void GLTexture::readData(
        GLint level,
        const BufferPixelFormat& format,
        const BufferPixelDataType& type,
        GLvoid* data )
{
    if ( Target::Texture2DMultisample == m_target ||
         Target::Texture2DMultisampleArray == m_target ||
         Target::TextureCubeMap == m_target )
    {
        throw_debug( "Invalid texture target type ");
    }

    // How slow is this?
    Binder binder( *this );

    boost::optional<PixelStoreSettings> oldPackSettings = boost::none;

    if ( m_pixelPackSettings )
    {
        oldPackSettings = getPixelPackSettings();
        applyPixelPackSettings( *m_pixelPackSettings );
    }

    glGetTexImage( m_targetEnum, level, underlyingType(format),
                   underlyingType(type), data );

    if ( oldPackSettings )
    {
        applyPixelPackSettings( *oldPackSettings );
    }
}

void GLTexture::readCubeMapFaceData(
        const CubeMapFace& face,
        GLint level,
        const BufferPixelFormat& format,
        const BufferPixelDataType& type,
        GLvoid* data )
{
    Binder binder( *this );

    boost::optional<PixelStoreSettings> oldPackSettings = boost::none;

    if ( m_pixelPackSettings )
    {
        oldPackSettings = getPixelPackSettings();
        applyPixelPackSettings( *m_pixelPackSettings );
    }

    glGetTexImage( underlyingType(face), level,
                   underlyingType(format), underlyingType(type), data );

    if ( oldPackSettings )
    {
        applyPixelPackSettings( *oldPackSettings );
    }

    CHECK_GL_ERROR( m_errorChecker );
}

void GLTexture::setMinificationFilter( const MinificationFilter& filter )
{
    if ( Target::Texture2DMultisample == m_target ||
         Target::Texture2DMultisampleArray == m_target )
    {
        throw_debug( "Invalid texture target type ");
    }

    Binder binder( *this );

    if ( ! ( Target::Texture2DMultisample == m_target ||
             Target::TextureRectangle == m_target ||
             Target::Texture2DMultisampleArray == m_target ) )
    {
        if ( m_autoGenerateMipmaps )
        {
            glGenerateMipmap( m_targetEnum );
        }
    }

    glTexParameteri( m_targetEnum, GL_TEXTURE_MIN_FILTER, underlyingType_asInt32(filter) );

    // new!
    glSamplerParameteri( m_samplerID, GL_TEXTURE_MIN_FILTER, underlyingType_asInt32(filter) );
}

void GLTexture::setMagnificationFilter( const MagnificationFilter& filter )
{
    if ( Target::Texture2DMultisample == m_target ||
         Target::Texture2DMultisampleArray == m_target )
    {
        throw_debug( "Invalid texture target type ");
    }

    Binder binder( *this );

    if ( ! ( Target::Texture2DMultisample == m_target ||
             Target::TextureRectangle == m_target ||
             Target::Texture2DMultisampleArray == m_target ) )
    {
        if ( m_autoGenerateMipmaps )
        {
            glGenerateMipmap( m_targetEnum );
        }
    }

    glTexParameteri( m_targetEnum, GL_TEXTURE_MAG_FILTER, underlyingType_asInt32(filter) );

    // new!
    glSamplerParameteri( m_samplerID, GL_TEXTURE_MAG_FILTER, underlyingType_asInt32(filter) );
}

void GLTexture::setSwizzleMask(
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

    Binder binder( *this );
    glTexParameteriv( m_targetEnum, GL_TEXTURE_SWIZZLE_RGBA, mask );

    // new!
    glSamplerParameteriv( m_samplerID, GL_TEXTURE_SWIZZLE_RGBA, mask );
}

void GLTexture::setWrapMode( const WrapMode& mode )
{
    Binder binder( *this );

    if ( Target::Texture1D == m_target ||
         Target::Texture1DArray == m_target )
    {
        glTexParameteri( m_targetEnum, GL_TEXTURE_WRAP_S, underlyingType_asInt32(mode) );

        // new!
        glSamplerParameteri( m_samplerID, GL_TEXTURE_WRAP_S, underlyingType_asInt32(mode) );
    }
    else if ( Target::Texture2D == m_target ||
              Target::Texture2DArray == m_target ||
              Target::Texture2DMultisample == m_target ||
              Target::TextureRectangle == m_target ||
              Target::Texture2DMultisampleArray == m_target )
    {
        glTexParameteri( m_targetEnum, GL_TEXTURE_WRAP_S, underlyingType_asInt32(mode) );
        glTexParameteri( m_targetEnum, GL_TEXTURE_WRAP_T, underlyingType_asInt32(mode) );

        // new!
        glSamplerParameteri( m_samplerID, GL_TEXTURE_WRAP_S, underlyingType_asInt32(mode) );
        glSamplerParameteri( m_samplerID, GL_TEXTURE_WRAP_T, underlyingType_asInt32(mode) );
    }
    else if ( Target::Texture3D == m_target )
    {
        glTexParameteri( m_targetEnum, GL_TEXTURE_WRAP_S, underlyingType_asInt32(mode) );
        glTexParameteri( m_targetEnum, GL_TEXTURE_WRAP_T, underlyingType_asInt32(mode) );
        glTexParameteri( m_targetEnum, GL_TEXTURE_WRAP_R, underlyingType_asInt32(mode) );

        // new!
        glSamplerParameteri( m_samplerID, GL_TEXTURE_WRAP_S, underlyingType_asInt32(mode) );
        glSamplerParameteri( m_samplerID, GL_TEXTURE_WRAP_T, underlyingType_asInt32(mode) );
        glSamplerParameteri( m_samplerID, GL_TEXTURE_WRAP_R, underlyingType_asInt32(mode) );
    }
}

void GLTexture::setBorderColor( const glm::vec4& color )
{
    Binder binder( *this );
    glTexParameterfv( m_targetEnum, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color) );

    // new!
    glSamplerParameterfv( m_samplerID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color) );
}

void GLTexture::setAutoGenerateMipmaps( bool set )
{
    m_autoGenerateMipmaps = set;

    if ( ! ( Target::Texture2DMultisample == m_target ||
             Target::TextureRectangle == m_target ||
             Target::Texture2DMultisampleArray == m_target ) )
    {
        if ( m_autoGenerateMipmaps )
        {
            Binder binder( *this );
            glGenerateMipmap( m_targetEnum );
        }
    }
}

void GLTexture::setMultisampleSettings( const MultisampleSettings& settings )
{
    m_multisampleSettings = settings;
}

void GLTexture::setPixelPackSettings( const PixelStoreSettings& settings )
{
    m_pixelPackSettings = settings;
}

void GLTexture::setPixelUnpackSettings( const PixelStoreSettings& settings )
{
    m_pixelUnpackSettings = settings;
}

GLTexture::PixelStoreSettings GLTexture::getPixelPackSettings()
{
    PixelStoreSettings settings;

    glGetIntegerv( GL_PACK_ALIGNMENT, &settings.m_alignment );
    glGetIntegerv( GL_PACK_SKIP_IMAGES, &settings.m_skipImages );
    glGetIntegerv( GL_PACK_SKIP_ROWS, &settings.m_skipRows );
    glGetIntegerv( GL_PACK_SKIP_PIXELS, &settings.m_skipPixels );
    glGetIntegerv( GL_PACK_IMAGE_HEIGHT, &settings.m_imageHeight );
    glGetIntegerv( GL_PACK_ROW_LENGTH, &settings.m_rowLength );
    glGetBooleanv( GL_PACK_LSB_FIRST, &settings.m_lsbFirst );
    glGetBooleanv( GL_PACK_SWAP_BYTES, &settings.m_swapBytes );

    return settings;
}

GLTexture::PixelStoreSettings GLTexture::getPixelUnpackSettings()
{
    PixelStoreSettings settings;

    glGetIntegerv( GL_UNPACK_ALIGNMENT, &settings.m_alignment );
    glGetIntegerv( GL_UNPACK_SKIP_IMAGES, &settings.m_skipImages );
    glGetIntegerv( GL_UNPACK_SKIP_ROWS, &settings.m_skipRows );
    glGetIntegerv( GL_UNPACK_SKIP_PIXELS, &settings.m_skipPixels );
    glGetIntegerv( GL_UNPACK_IMAGE_HEIGHT, &settings.m_imageHeight );
    glGetIntegerv( GL_UNPACK_ROW_LENGTH, &settings.m_rowLength );
    glGetBooleanv( GL_UNPACK_LSB_FIRST, &settings.m_lsbFirst );
    glGetBooleanv( GL_UNPACK_SWAP_BYTES, &settings.m_swapBytes );

    return settings;
}

void GLTexture::applyPixelPackSettings( const PixelStoreSettings& settings )
{
    glPixelStorei( GL_UNPACK_ALIGNMENT, settings.m_alignment );
    glPixelStorei( GL_UNPACK_SKIP_IMAGES, settings.m_skipImages );
    glPixelStorei( GL_UNPACK_SKIP_ROWS, settings.m_skipRows );
    glPixelStorei( GL_UNPACK_SKIP_PIXELS, settings.m_skipPixels );
    glPixelStorei( GL_UNPACK_IMAGE_HEIGHT, settings.m_imageHeight );
    glPixelStorei( GL_UNPACK_ROW_LENGTH, settings.m_rowLength );
    glPixelStorei( GL_UNPACK_LSB_FIRST, settings.m_lsbFirst );
    glPixelStorei( GL_UNPACK_SWAP_BYTES, settings.m_swapBytes );
}

void GLTexture::applyPixelUnpackSettings( const PixelStoreSettings& settings )
{
    glPixelStorei( GL_PACK_ALIGNMENT, settings.m_alignment );
    glPixelStorei( GL_PACK_SKIP_IMAGES, settings.m_skipImages );
    glPixelStorei( GL_PACK_SKIP_ROWS, settings.m_skipRows );
    glPixelStorei( GL_PACK_SKIP_PIXELS, settings.m_skipPixels );
    glPixelStorei( GL_PACK_IMAGE_HEIGHT, settings.m_imageHeight );
    glPixelStorei( GL_PACK_ROW_LENGTH, settings.m_rowLength );
    glPixelStorei( GL_PACK_LSB_FIRST, settings.m_lsbFirst );
    glPixelStorei( GL_PACK_SWAP_BYTES, settings.m_swapBytes );
}

