#ifndef DDP_BLEND_PASS_QUAD_H
#define DDP_BLEND_PASS_QUAD_H

#include "rendering/drawables/ddp/FullScreenQuad.h"
#include "rendering/common/ShaderProviderType.h"
#include "rendering/utility/gl/GLTexture.h"

#include <array>


class DdpBlendPassQuad : public FullScreenQuad
{
public:

    DdpBlendPassQuad(
            const std::string& name,
            ShaderProgramActivatorType shaderProgramActivator,
            UniformsProviderType uniformsProvider,
            std::array<GLTexture, 2>& backTempTextures );

    DdpBlendPassQuad( const DdpBlendPassQuad& ) = delete;
    DdpBlendPassQuad& operator=( const DdpBlendPassQuad& ) = delete;

    DdpBlendPassQuad( DdpBlendPassQuad&& ) = delete;
    DdpBlendPassQuad& operator=( DdpBlendPassQuad&& ) = delete;

    ~DdpBlendPassQuad() override = default;

    void setCurrentTextureID( uint i );


private:

    void doRender( const RenderStage& stage ) override;

    ShaderProgramActivatorType m_shaderProgramActivator;
    UniformsProviderType m_uniformsProvider;

    Uniforms m_uniforms;

    std::array<GLTexture, 2>& m_backTempTextures;
    uint m_currentTextureID;
};

#endif // DDPBLENDPASSQUAD_H
