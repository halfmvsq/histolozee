#include "rendering/drawables/ddp/DdpFinalPassQuad.h"
#include "rendering/ShaderNames.h"
#include "rendering/utility/gl/GLShaderProgram.h"

#include "common/HZeeException.hpp"


namespace
{

static const Uniforms::SamplerIndexType s_frontTexSamplerIndex{ 0 };
static const Uniforms::SamplerIndexType s_backTexSamplerIndex{ 1 };

} // anonymous


DdpFinalPassQuad::DdpFinalPassQuad(
        const std::string& name,
        ShaderProgramActivatorType shaderProgramActivator,
        UniformsProviderType uniformsProvider,
        std::array<GLTexture, 2>& frontBlenderTextures,
        GLTexture& backBlenderTexture )
    :
      FullScreenQuad( name ),

      m_shaderProgramActivator( shaderProgramActivator ),
      m_uniformsProvider( uniformsProvider ),
      m_uniforms(),

      m_frontBlenderTextures( frontBlenderTextures ),
      m_backBlenderTexture( backBlenderTexture ),
      m_currentTextureID( 0 )
{
    if ( m_uniformsProvider )
    {
        m_uniforms = m_uniformsProvider( DDPFinalProgram::name );
    }
    else
    {
        throw_debug( "Unable to access UniformsProvider" );
    }
}

void DdpFinalPassQuad::setCurrentTextureID( uint i )
{
    m_currentTextureID = i;
}

void DdpFinalPassQuad::doRender( const RenderStage& /*stage*/ )
{
    if ( ! m_shaderProgramActivator )
    {
        throw_debug( "Unable to access ShaderProgramActivator" );
    }

    if ( auto program = m_shaderProgramActivator( DDPFinalProgram::name ) )
    {
        m_frontBlenderTextures[m_currentTextureID].bind( s_frontTexSamplerIndex.index );
        m_backBlenderTexture.bind( s_backTexSamplerIndex.index );

        m_uniforms.setValue( DDPFinalProgram::frag::frontBlenderTexture, s_frontTexSamplerIndex );
        m_uniforms.setValue( DDPFinalProgram::frag::backBlenderTexture, s_backTexSamplerIndex );

        program->applyUniforms( m_uniforms );

        // This prevents infinite loop during occlusion query
        glClear( GL_COLOR_BUFFER_BIT );

        drawVao();
    }
}
