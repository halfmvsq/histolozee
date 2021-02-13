#include "rendering/utility/containers/BlankTextures.h"
#include "rendering/utility/CreateGLObjects.h"
#include "rendering/utility/gl/GLTexture.h"
#include "imageio/HZeeTypes.hpp"


struct BlankTextures::Impl
{
    Impl()
        : m_blankImageTexture2D(
              gpuhelper::createBlankRGBATexture(
                  imageio::ComponentType::UInt8,
                  tex::Target::Texture2D ) ),

          m_blankImageTexture3D(
              gpuhelper::createBlankRGBATexture(
                  imageio::ComponentType::Int16,
                  tex::Target::Texture3D ) ),

          m_blankLabelTexture3D(
              gpuhelper::createBlankRGBATexture(
                  imageio::ComponentType::UInt16,
                  tex::Target::Texture3D ) )
    {}

    ~Impl() = default;

    GLTexture m_blankImageTexture2D;
    GLTexture m_blankImageTexture3D;
    GLTexture m_blankLabelTexture3D;
};

BlankTextures::BlankTextures()
    :
      m_impl( nullptr )
{}

BlankTextures::~BlankTextures() = default;

void BlankTextures::initializeGL()
{
    initializeOpenGLFunctions();
    m_impl = std::make_unique<Impl>();
}

void BlankTextures::bindImageTexture2D( boost::optional<uint32_t> textureUnit )
{
    m_impl->m_blankImageTexture2D.bind( textureUnit );
}

void BlankTextures::bindImageTexture3D( boost::optional<uint32_t> textureUnit )
{
    m_impl->m_blankImageTexture3D.bind( textureUnit );
}

void BlankTextures::bindLabelTexture3D( boost::optional<uint32_t> textureUnit )
{
    m_impl->m_blankLabelTexture3D.bind( textureUnit );
}
