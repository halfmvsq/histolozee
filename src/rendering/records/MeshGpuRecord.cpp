#include "rendering/records/MeshGpuRecord.h"

MeshGpuRecord::MeshGpuRecord(
        GLBufferObject positionsObject,
        GLBufferObject indicesObject,
        VertexAttributeInfo positionsInfo,
        VertexIndicesInfo indicesInfo )
    :
      m_positionsObject( std::move( positionsObject ) ),
      m_normalsObject( boost::none ),
      m_texCoordsObject( boost::none ),
      m_colorsObject( boost::none ),
      m_indicesObject( std::move( indicesObject ) ),

      m_positionsInfo( std::move( positionsInfo ) ),
      m_normalsInfo( boost::none ),
      m_texCoordsInfo( boost::none ),
      m_colorsInfo( boost::none ),
      m_indicesInfo( std::move( indicesInfo ) )
{}

MeshGpuRecord::MeshGpuRecord(
        GLBufferObject positionsObject,
        GLBufferObject normalsObject,
        GLBufferObject texCoordsObject,
        GLBufferObject indicesObject,
        VertexAttributeInfo positionsInfo,
        VertexAttributeInfo normalsInfo,
        VertexAttributeInfo texCoordsInfo,
        VertexIndicesInfo indicesInfo )
    :
      MeshGpuRecord( std::move(positionsObject),
                     std::move(indicesObject),
                     std::move( positionsInfo ),
                     std::move( indicesInfo ) )
{
    m_normalsObject = std::move( normalsObject );
    m_texCoordsObject = std::move( texCoordsObject );
    m_normalsInfo = std::move( normalsInfo );
    m_texCoordsInfo = std::move( texCoordsInfo );
}

void MeshGpuRecord::setNormals(
        GLBufferObject normalsObject,
        VertexAttributeInfo normalsInfo )
{
    m_normalsObject = std::move( normalsObject );
    m_normalsInfo = std::move( normalsInfo );
}

void MeshGpuRecord::setTexCoords(
        GLBufferObject texCoordsObject,
        VertexAttributeInfo texCoordsInfo )
{
    m_texCoordsObject = std::move( texCoordsObject );
    m_texCoordsInfo = std::move( texCoordsInfo );
}

void MeshGpuRecord::setColors(
        GLBufferObject colorsObject,
        VertexAttributeInfo colorsInfo )
{
    m_colorsObject = std::move( colorsObject );
    m_colorsInfo = std::move( colorsInfo );
}

GLBufferObject& MeshGpuRecord::positionsObject()
{
    return m_positionsObject;
}

boost::optional<GLBufferObject>&
MeshGpuRecord::normalsObject()
{
    return m_normalsObject;
}

boost::optional<GLBufferObject>&
MeshGpuRecord::texCoordsObject()
{
    return m_texCoordsObject;
}

boost::optional<GLBufferObject>&
MeshGpuRecord::colorsObject()
{
    return m_colorsObject;
}

GLBufferObject& MeshGpuRecord::indicesObject()
{
    return m_indicesObject;
}

const VertexAttributeInfo&
MeshGpuRecord::positionsInfo() const
{
    return m_positionsInfo;
}

const boost::optional< VertexAttributeInfo >&
MeshGpuRecord::normalsInfo() const
{
    return m_normalsInfo;
}

const boost::optional< VertexAttributeInfo >&
MeshGpuRecord::texCoordsInfo() const
{
    return m_texCoordsInfo;
}

const boost::optional< VertexAttributeInfo >&
MeshGpuRecord::colorsInfo() const
{
    return m_colorsInfo;
}

const VertexIndicesInfo&
MeshGpuRecord::indicesInfo() const
{
    return m_indicesInfo;
}
