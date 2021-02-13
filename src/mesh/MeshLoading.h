#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include <memory>
#include <string>


namespace imageio
{
class ImageHeader;
}

class MeshCpuRecord;
class vtkImageData;


namespace meshgen
{

std::unique_ptr<MeshCpuRecord> generateIsoSurface(
        vtkImageData* imageData,
        const imageio::ImageHeader& imageHeader,
        const double isoValue );

std::unique_ptr<MeshCpuRecord> generateLabelMesh(
        vtkImageData* imageData,
        const imageio::ImageHeader& imageHeader,
        const uint32_t labelIndex );

/// @todo Put this function here
//std::map< int64_t, double >
//generateImageHistogramAtLabelValues(
//        vtkImageData* imageData,
//        const std::unordered_set<int64_t>& labelValues );

bool writeMeshToFile( const MeshCpuRecord&, const std::string& fileName );

} // namespace meshgen

#endif // MESH_LOADER_H
