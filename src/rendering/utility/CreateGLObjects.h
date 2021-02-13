#ifndef CREATE_GL_OBJECTS_H
#define CREATE_GL_OBJECTS_H

#include "logic/colormap/ImageColorMap.h"
#include "logic/colormap/ParcellationLabelTable.h"

#include "logic/records/ImageRecord.h"
#include "logic/records/MeshRecord.h"
#include "logic/records/SlideAnnotationRecord.h"
#include "logic/records/SlideRecord.h"

#include "rendering/utility/gl/GLBufferTexture.h"
#include "rendering/utility/gl/GLTexture.h"
#include "rendering/utility/gl/GLTextureTypes.h"

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <memory>


class Polygon;


namespace gpuhelper
{

std::unique_ptr<ImageGpuRecord> createImageGpuRecord(
        const imageio::ImageCpuRecord* imageCpuRecord,
        const uint32_t componentIndex,
        const tex::MinificationFilter& minFilter,
        const tex::MagnificationFilter& magFilter,
        bool useNormalizedIntegers );

std::unique_ptr<MeshGpuRecord> createSliceMeshGpuRecord(
        const BufferUsagePattern& bufferUsagePattern = BufferUsagePattern::DynamicDraw );

std::unique_ptr<MeshGpuRecord> createBoxMeshGpuRecord(
        const BufferUsagePattern& bufferUsagePattern = BufferUsagePattern::StreamDraw );

std::unique_ptr<MeshGpuRecord> createSphereMeshGpuRecord();

std::unique_ptr<MeshGpuRecord> createCylinderMeshGpuRecord(
        const glm::dvec3& center, double radius, double height );

std::unique_ptr<MeshGpuRecord> createCrosshairMeshGpuRecord( double coneToCylinderRatio );

std::unique_ptr<MeshGpuRecord> createMeshGpuRecord(
        size_t vertexCount, size_t indexCount,
        const PrimitiveMode& primitiveMode,
        const BufferUsagePattern& bufferUsagePattern = BufferUsagePattern::DynamicDraw );

std::unique_ptr<MeshGpuRecord> createMeshGpuRecordFromVtkPolyData(
        vtkSmartPointer<vtkPolyData> polyData,
        const MeshPrimitiveType& primitiveType,
        const BufferUsagePattern& bufferUsagePattern = BufferUsagePattern::StreamDraw );

std::unique_ptr<SlideGpuRecord> createSlideGpuRecord( const slideio::SlideCpuRecord* );


/**
 * @brief createSlideAnnotationGpuRecord
 *
 * @param[in] polygon
 *
 * @return
 */
std::unique_ptr<SlideAnnotationGpuRecord> createSlideAnnotationGpuRecord( const Polygon& polygon );

std::unique_ptr<GLTexture> createImageColorMapTexture( const ImageColorMap* );

std::unique_ptr<GLBufferTexture> createLabelColorTableTextureBuffer( const ParcellationLabelTable* );

GLTexture createBlankRGBATexture( const imageio::ComponentType& compType, const tex::Target& texTarget );

void createTestColorBuffer( MeshRecord& meshRecord );

} // namespace gpuhelper

#endif // CREATE_GL_OBJECTS_H
