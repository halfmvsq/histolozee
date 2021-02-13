#ifndef DDP_FINAL_PASS_QUAD_H
#define DDP_FINAL_PASS_QUAD_H

#include "rendering/drawables/ddp/FullScreenQuad.h"
#include "rendering/utility/gl/GLTexture.h"
#include "rendering/common/ShaderProviderType.h"

#include <array>


class DdpFinalPassQuad : public FullScreenQuad
{
public:

    DdpFinalPassQuad(
            const std::string& name,
            ShaderProgramActivatorType shaderProgramActivator,
            UniformsProviderType uniformsProvider,
            std::array<GLTexture, 2>& frontBlenderTextures,
            GLTexture& backBlenderTexture );

    DdpFinalPassQuad( const DdpFinalPassQuad& ) = delete;
    DdpFinalPassQuad& operator=( const DdpFinalPassQuad& ) = delete;

    DdpFinalPassQuad( DdpFinalPassQuad&& ) = delete;
    DdpFinalPassQuad& operator=( DdpFinalPassQuad&& ) = delete;

    ~DdpFinalPassQuad() override = default;

    void setCurrentTextureID( uint i );


private:

    void doRender( const RenderStage& stage ) override;

    ShaderProgramActivatorType m_shaderProgramActivator;
    UniformsProviderType m_uniformsProvider;

    Uniforms m_uniforms;

    std::array<GLTexture, 2>& m_frontBlenderTextures;
    GLTexture& m_backBlenderTexture;

    uint m_currentTextureID;
};

#endif // DDPFINALPASSQUAD_H
