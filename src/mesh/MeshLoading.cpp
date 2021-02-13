#include "mesh/MeshLoading.h"
#include "mesh/MeshCpuRecord.h"
#include "mesh/vtkdetails/MeshGeneration.hpp"

#include "imageio/ImageHeader.h"
#include "imageio/util/MathFuncs.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vnl/vnl_matrix_fixed.h>

#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <iostream>
#include <limits>
#include <utility>


namespace meshgen
{

std::unique_ptr<MeshCpuRecord> generateIsoSurface(
        vtkImageData* imageData,
        const imageio::ImageHeader& imageHeader,
        const double isoValue )
{
    // Note: triangle strips offer no speed advantage over indexed triangles on modern hardware
    static const MeshPrimitiveType sk_primitiveType = MeshPrimitiveType::Triangles;

    if ( ! imageData )
    {
        std::cerr << "Error generating iso-surface mesh: "
                  << "Image data is null!" << std::endl;
        return nullptr;
    }

    const vnl_matrix_fixed< double, 3, 3 > imageDirections =
            imageio::convert::toVnlMatrixFixed( imageHeader.m_directions );

    vtkSmartPointer< vtkPolyData > polyData = nullptr;

    try
    {
        auto polyData = ::vtkdetails::generateIsoSurfaceMesh(
                    imageData, imageDirections, isoValue, sk_primitiveType );
    }
    catch ( const std::exception& e )
    {
        std::cerr << "Error generating iso-surface mesh: "
                  << e.what() << std::endl;
        return nullptr;
    }
    catch ( ... )
    {
        std::cerr << "Error generating iso-surface mesh." << std::endl;
        return nullptr;
    }

    if ( ! polyData )
    {
        std::cerr << "Error generating iso-surface mesh: "
                  << "vtkPolyData is null" << std::endl;
        return nullptr;
    }

    return std::make_unique<MeshCpuRecord>(
                polyData, MeshInfo( MeshSource::IsoSurface, sk_primitiveType, isoValue ) );
}


std::unique_ptr<MeshCpuRecord> generateLabelMesh(
        vtkImageData* imageData,
        const imageio::ImageHeader& imageHeader,
        const uint32_t labelIndex )
{
    // Note: triangle strips offer no speed advantage over indexed triangles on modern hardware
    static const MeshPrimitiveType sk_primitiveType = MeshPrimitiveType::Triangles;

    // Parcellation image must have exactly one scalar component
    if ( 1 != imageHeader.m_numComponents ||
         imageio::PixelType::Scalar != imageHeader.m_pixelType )
    {
        std::cerr << "Error computing mesh at label index " << labelIndex
                  << ": Pixel type must be single-component scalar." << std::endl;
        return nullptr;
    }

    // Parcellation pixels are indices that must be of unsigned integer type
    if ( ! imageio::isUnsignedIntegerType( imageHeader.m_componentType ) )
    {
        std::cerr << "Error computing label mesh: "
                  << "parcellation component type must be unsigned integral." << std::endl;
        return nullptr;
    }

    if ( ! imageData )
    {
        std::cerr << "Error: Parcellation image data is null!" << std::endl;
        return nullptr;
    }

    const vnl_matrix_fixed< double, 3, 3 > imageDirections =
            imageio::convert::toVnlMatrixFixed( imageHeader.m_directions );

    try
    {
        auto polyData = ::vtkdetails::generateLabelMesh(
                    imageData, imageDirections, labelIndex, sk_primitiveType );

        if ( ! polyData )
        {
            std::cerr << "Error generating label mesh at index " << labelIndex << std::endl;
            return nullptr;
        }

        return std::make_unique<MeshCpuRecord>(
                    polyData, MeshInfo( MeshSource::Label, sk_primitiveType, labelIndex ) );
    }
    catch ( const std::exception& e )
    {
        std::cerr << "Error generating label mesh: " << e.what() << std::endl;
        return nullptr;
    }
    catch ( ... )
    {
        std::cerr << "Error generating label mesh." << std::endl;
        return nullptr;
    }
}


bool writeMeshToFile( const MeshCpuRecord& record, const std::string& fileName )
{
    if ( record.polyData().GetPointer() )
    {
        return ( ::vtkdetails::writePolyData( record.polyData(), fileName.c_str() ) );
    }

    return false;
}

} // namespace meshgen
