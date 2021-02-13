#ifndef BLANK_TEXTURES_H
#define BLANK_TEXTURES_H

#include <boost/optional.hpp>
#include <QOpenGLFunctions_3_3_Core>
#include <memory>


class BlankTextures final :
        protected QOpenGLFunctions_3_3_Core
{
public:

    BlankTextures();

    BlankTextures( const BlankTextures& ) = delete;
    BlankTextures& operator=( const BlankTextures& ) = delete;

    BlankTextures( BlankTextures&& ) = default;
    BlankTextures& operator=( BlankTextures&& ) = default;

    ~BlankTextures();

    void initializeGL();

    void bindImageTexture2D( boost::optional<uint32_t> textureUnit );
    void bindImageTexture3D( boost::optional<uint32_t> textureUnit );
    void bindLabelTexture3D( boost::optional<uint32_t> textureUnit );


private:

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

#endif // BLANK_TEXTURES_H
