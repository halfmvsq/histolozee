#ifndef MESH_GPU_RECORD_H
#define MESH_GPU_RECORD_H

#include "rendering/utility/containers/VertexAttributeInfo.h"
#include "rendering/utility/containers/VertexIndicesInfo.h"
#include "rendering/utility/gl/GLBufferObject.h"

#include <boost/optional.hpp>

#include <memory>


class MeshGpuRecord
{
public:

    MeshGpuRecord(
            GLBufferObject positionsObject,
            GLBufferObject indicesObject,
            VertexAttributeInfo positionsInfo,
            VertexIndicesInfo indicesInfo );

    MeshGpuRecord(
            GLBufferObject positionsObject,
            GLBufferObject normalsObject,
            GLBufferObject texCoordsObject,
            GLBufferObject indicesObject,
            VertexAttributeInfo positionsInfo,
            VertexAttributeInfo normalsInfo,
            VertexAttributeInfo texCoordsInfo,
            VertexIndicesInfo indicesInfo );

    MeshGpuRecord() = delete;

    MeshGpuRecord( const MeshGpuRecord& ) = delete;
    MeshGpuRecord& operator=( const MeshGpuRecord& ) = delete;

    MeshGpuRecord( MeshGpuRecord&& ) = default;
    MeshGpuRecord& operator=( MeshGpuRecord&& ) = default;

    ~MeshGpuRecord() = default;

    void setNormals( GLBufferObject normalsObject,
                     VertexAttributeInfo normalsInfo );

    void setTexCoords( GLBufferObject texCoordsObject,
                       VertexAttributeInfo texCoordsInfo );

    void setColors( GLBufferObject colorsObject,
                    VertexAttributeInfo colorsInfo );

    // Return as non-const reference, since users need access to
    // non-const member functions of GLTexture
    GLBufferObject& positionsObject();
    boost::optional<GLBufferObject>& normalsObject();
    boost::optional<GLBufferObject>& texCoordsObject();
    boost::optional<GLBufferObject>& colorsObject();
    GLBufferObject& indicesObject();

    const VertexAttributeInfo& positionsInfo() const;
    const boost::optional<VertexAttributeInfo>& normalsInfo() const;
    const boost::optional<VertexAttributeInfo>& texCoordsInfo() const;
    const boost::optional<VertexAttributeInfo>& colorsInfo() const;
    const VertexIndicesInfo& indicesInfo() const;

    BufferComponentType componentType() const;
    BufferNormalizeValues normalizeValues() const;


private:

    GLBufferObject m_positionsObject;
    boost::optional<GLBufferObject> m_normalsObject;
    boost::optional<GLBufferObject> m_texCoordsObject;
    boost::optional<GLBufferObject> m_colorsObject;
    GLBufferObject m_indicesObject;

    VertexAttributeInfo m_positionsInfo;
    boost::optional<VertexAttributeInfo> m_normalsInfo;
    boost::optional<VertexAttributeInfo> m_texCoordsInfo;
    boost::optional<VertexAttributeInfo> m_colorsInfo;
    VertexIndicesInfo m_indicesInfo;
};

#endif // MESH_GPU_RECORD_H
