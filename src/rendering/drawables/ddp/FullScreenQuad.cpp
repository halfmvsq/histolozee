#include "rendering/drawables/ddp/FullScreenQuad.h"
#include "rendering/records/MeshGpuRecord.h"
#include "rendering/utility/containers/VertexAttributeInfo.h"
#include "rendering/utility/containers/VertexIndicesInfo.h"
#include "rendering/utility/gl/GLDrawTypes.h"

#include "common/HZeeException.hpp"

#include <array>


FullScreenQuad::FullScreenQuad( const std::string& name )
    :
      DrawableBase( name, DrawableType::FullScreenQuad ),

      m_vao(),
      m_vaoParams( nullptr ),
      m_meshGpuRecord( nullptr )
{
    initBuffer();
    initVao();
}

FullScreenQuad::~FullScreenQuad() = default;


void FullScreenQuad::initBuffer()
{
    static constexpr int sk_numVerts = 4;
    static constexpr int sk_numPosComps = 3;
    static constexpr int sk_numTCComps = 2;

    static const std::array< float, sk_numVerts * sk_numPosComps >
            sk_clipPositionsBuffer =
    { {
          -1.0f, -1.0f, -1.0f, // bottom left
          1.0f, -1.0f, -1.0f,  // bottom right
          -1.0f,  1.0f, -1.0f, // top left
          1.0f,  1.0f, -1.0f   // top right
      } };

    static const std::array< float, sk_numVerts * sk_numTCComps >
            sk_texCoordsBuffer =
    { {
          0.0f, 0.0f, // bottom left
          1.0f, 0.0f, // bottom right
          0.0f, 1.0f, // top left
          1.0f, 1.0f  // top right
      } };

    static const std::array< uint32_t, sk_numVerts >
            sk_indicesBuffer = { { 0, 1, 2, 3 } };

    VertexAttributeInfo positionsInfo(
                BufferComponentType::Float, BufferNormalizeValues::False,
                sk_numPosComps, sk_numPosComps * sizeof(float), 0, sk_numVerts );

    VertexAttributeInfo texCoordsInfo(
                BufferComponentType::Float, BufferNormalizeValues::False,
                sk_numTCComps, sk_numTCComps * sizeof(float), 0, sk_numVerts );

    VertexIndicesInfo indexInfo(
                IndexType::UInt32, PrimitiveMode::TriangleStrip, sk_numVerts, 0 );

    GLBufferObject positionsBuffer( BufferType::VertexArray, BufferUsagePattern::StaticDraw );
    GLBufferObject texCoordsBuffer( BufferType::VertexArray, BufferUsagePattern::StaticDraw );
    GLBufferObject indicesBuffer( BufferType::Index, BufferUsagePattern::StaticDraw );

    positionsBuffer.generate();
    texCoordsBuffer.generate();
    indicesBuffer.generate();

    positionsBuffer.allocate( sk_numVerts * sk_numPosComps * sizeof( float ), sk_clipPositionsBuffer.data() );
    texCoordsBuffer.allocate( sk_numVerts * sk_numTCComps * sizeof( float ), sk_texCoordsBuffer.data() );
    indicesBuffer.allocate( sk_numVerts * sizeof( uint32_t ), sk_indicesBuffer.data() );

    m_meshGpuRecord = std::make_unique<MeshGpuRecord>(
                std::move(positionsBuffer), std::move(indicesBuffer),
                positionsInfo, indexInfo );

    m_meshGpuRecord->setTexCoords( std::move(texCoordsBuffer), texCoordsInfo );
}


void FullScreenQuad::initVao()
{
    static constexpr GLuint sk_positionsIndex = 0;
    static constexpr GLuint sk_texCoordsIndex = 1;

    if ( ! m_meshGpuRecord )
    {
        return;
    }

    const auto& positionsInfo = m_meshGpuRecord->positionsInfo();
    const auto& texCoordsInfo = m_meshGpuRecord->texCoordsInfo();
    const auto& indicesInfo = m_meshGpuRecord->indicesInfo();

    auto& positionsObject = m_meshGpuRecord->positionsObject();
    auto& texCoordsObject = m_meshGpuRecord->texCoordsObject();
    auto& indicesObject = m_meshGpuRecord->indicesObject();

    if ( ! texCoordsInfo || ! texCoordsObject )
    {
        throw_debug( "No mesh texture data" );
    }

    m_vao.generate();
    m_vao.bind();
    {
        // Bind EBO so that it is part of the VAO state
        indicesObject.bind();

        positionsObject.bind();
        m_vao.setAttributeBuffer( sk_positionsIndex, positionsInfo );
        m_vao.enableVertexAttribute( sk_positionsIndex );

        texCoordsObject->bind();
        m_vao.setAttributeBuffer( sk_texCoordsIndex, *texCoordsInfo );
        m_vao.enableVertexAttribute( sk_texCoordsIndex );
    }
    m_vao.release();

    m_vaoParams = std::make_unique< GLVertexArrayObject::IndexedDrawParams >( indicesInfo );
}


void FullScreenQuad::drawVao()
{
    if ( ! m_vaoParams )
    {
        std::ostringstream ss;
        ss << "Null VAO parameters in " << m_name << std::ends;
        throw_debug( ss.str() );
    }

    m_vao.bind();
    m_vao.drawElements( *m_vaoParams );
    m_vao.release();
}
