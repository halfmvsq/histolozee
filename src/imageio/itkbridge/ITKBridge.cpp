#include "itkbridge/ITKBridge.hpp"
#include "itkdetails/ImageUtility.hpp"

#include "util/MathFuncs.hpp"

#include <itkGDCMImageIO.h>

#include <glm/glm.hpp>

#include <algorithm>
#include <array>
#include <utility>


namespace
{

::imageio::PixelType
fromITKPixelType( const ::itk::ImageIOBase::IOPixelType& pixelType )
{
    using IOB = ::itk::ImageIOBase;

    switch ( pixelType )
    {
    case IOB::SCALAR:     return ::imageio::PixelType::Scalar;
    case IOB::COMPLEX:    return ::imageio::PixelType::Complex;
    case IOB::RGB:        return ::imageio::PixelType::RGB;
    case IOB::RGBA:       return ::imageio::PixelType::RGBA;
    case IOB::VECTOR:     return ::imageio::PixelType::Vector;
    case IOB::COVARIANTVECTOR:
        return ::imageio::PixelType::CovariantVector;
    case IOB::OFFSET:     return ::imageio::PixelType::Offset;
    case IOB::POINT:      return ::imageio::PixelType::Point;
    case IOB::FIXEDARRAY: return ::imageio::PixelType::FixedArray;
    case IOB::MATRIX:     return ::imageio::PixelType::Matrix;
    case IOB::DIFFUSIONTENSOR3D:
        return ::imageio::PixelType::DiffusionTensor3D;
    case IOB::SYMMETRICSECONDRANKTENSOR:
        return ::imageio::PixelType::SymmetricSecondRankTensor;
    case IOB::UNKNOWNPIXELTYPE:
    default:
        return ::imageio::PixelType::Undefined;
    }
}


std::pair< std::string, bool >
getSpiralCodeFromDirectionMatrix( const glm::dmat3& glmMatrix )
{
    vnl_matrix_fixed< double, 3, 3 > vnlMatrix;

    for ( uint8_t row = 0; row < 3; ++row )
    {
        for ( uint8_t col = 0; col < 3; ++col )
        {
            vnlMatrix.set( row, col, glmMatrix[col][row] );
        }
    }

    return ::itkdetails::utility::getSpiralCodeFromDirectionMatrix( vnlMatrix );
}

std::array< glm::dvec3, 8 >
computeAllBoxCorners( const std::pair< glm::dvec3, glm::dvec3 > boxMinMaxCorners )
{
    const glm::dvec3 size = boxMinMaxCorners.second - boxMinMaxCorners.first;

    std::array< glm::dvec3, 8 > corners =
    {
        {
            glm::dvec3{ 0, 0, 0 },
            glm::dvec3{ size.x, 0, 0 },
            glm::dvec3{ 0, size.y, 0 },
            glm::dvec3{ 0, 0, size.z },
            glm::dvec3{ size.x, size.y, 0 },
            glm::dvec3{ size.x, 0, size.z },
            glm::dvec3{ 0, size.y, size.z },
            glm::dvec3{ size.x, size.y, size.z }
        }
    };

    std::for_each( std::begin(corners), std::end(corners),
                   [ &boxMinMaxCorners ] ( glm::dvec3& corner )
    {
        corner = corner + boxMinMaxCorners.first;
    } );

    return corners;
}

} // anonymous namespace


namespace imageio
{

namespace itkbridge
{

const std::unordered_map< std::type_index, ComponentType > k_hzeeComponentTypeMap =
{
    { std::type_index( typeid(   int8_t ) ), ComponentType::Int8 },
    { std::type_index( typeid(  uint8_t ) ), ComponentType::UInt8 },
    { std::type_index( typeid(  int16_t ) ), ComponentType::Int16 },
    { std::type_index( typeid( uint16_t ) ), ComponentType::UInt16 },
    { std::type_index( typeid(  int32_t ) ), ComponentType::Int32 },
    { std::type_index( typeid( uint32_t ) ), ComponentType::UInt32 },
    { std::type_index( typeid(  int64_t ) ), ComponentType::Int64 },
    { std::type_index( typeid( uint64_t ) ), ComponentType::UInt64 },
    { std::type_index( typeid(    float ) ), ComponentType::Float32 },
    { std::type_index( typeid(   double ) ), ComponentType::Double64 }
};

const std::unordered_map< ComponentType, uint32_t > k_bytesPerComponentMap =
{
    { ComponentType::Int8,     1 },
    { ComponentType::UInt8,    1 },
    { ComponentType::Int16,    2 },
    { ComponentType::UInt16,   2 },
    { ComponentType::Int32,    4 },
    { ComponentType::UInt32,   4 },
    { ComponentType::Int64,    8 },
    { ComponentType::UInt64,   8 },
    { ComponentType::Float32,  4 },
    { ComponentType::Double64, 8 }
};

const std::unordered_map< ComponentType, std::string > k_componentStringMap =
{
    { ComponentType::Int8,     "int8" },
    { ComponentType::UInt8,    "uint8_t" },
    { ComponentType::Int16,    "int16" },
    { ComponentType::UInt16,   "uint16" },
    { ComponentType::Int32,    "int32" },
    { ComponentType::UInt32,   "uint32" },
    { ComponentType::Int64,    "int64" },
    { ComponentType::UInt64,   "uint64" },
    { ComponentType::Float32,  "float32" },
    { ComponentType::Double64, "double64" }
};


boost::optional<::imageio::ComponentType>
fromITKComponentType( const ::itk::ImageIOBase::IOComponentType& componentType )
{
    using IOB = ::itk::ImageIOBase;

    switch ( componentType )
    {
    case IOB::CHAR:   return ::imageio::ComponentType::Int8;
    case IOB::UCHAR:  return ::imageio::ComponentType::UInt8;
    case IOB::SHORT:  return ::imageio::ComponentType::Int16;
    case IOB::USHORT: return ::imageio::ComponentType::UInt16;
    case IOB::INT:    return ::imageio::ComponentType::Int32;
    case IOB::UINT:   return ::imageio::ComponentType::UInt32;
    case IOB::LONG:   return ::imageio::ComponentType::Int64;
    case IOB::ULONG:  return ::imageio::ComponentType::UInt64;
    case IOB::FLOAT:  return ::imageio::ComponentType::Float32;
    case IOB::DOUBLE: return ::imageio::ComponentType::Double64;
    case IOB::UNKNOWNCOMPONENTTYPE:
    default:
        return boost::none;
    }
}


boost::optional<ComponentType> sniffComponentType( const char* fileName )
{
    ::itk::ImageIOBase::Pointer imageIO =
            ::itkdetails::utility::createStandardImageIO( fileName );

    if ( imageIO.IsNull() )
    {
        imageIO = ::itkdetails::utility::dicom::createDicomImageIO( fileName );

        if ( imageIO.IsNull() )
        {
            return boost::none;
        }
    }

    return fromITKComponentType( imageIO->GetComponentType() );
}


/// @note There is no support for images of dimension > 3, so:
/// -Fill unused dimensions with 1
/// -Fill unused origin coordinates with 0.0
/// -Fill unused spacing values with 1.0
/// -Default direction matrix to identity
bool createImageHeader(
        const ::itkdetails::io::ImageIoInfo& itkImageIOInfo,
        const std::function< ComponentType ( ComponentType ) >& componentTypeCaster,
        ImageHeader& header )
{
    auto compType = fromITKComponentType( itkImageIOInfo.m_componentInfo.m_componentType );

    if ( ! compType )
    {
        return false;
    }

    const auto fileComponentType = *compType;
    const auto bufferComponentType = componentTypeCaster( *compType );

    return createImageHeader( itkImageIOInfo, fileComponentType, bufferComponentType, header );
}


bool createImageHeader(
        const ::itkdetails::io::ImageIoInfo& itkImageIOInfo,
        const ComponentType& fileComponentType,
        const ComponentType& bufferComponentType,
        ImageHeader& header )
{
    header.m_fileName = itkImageIOInfo.m_fileInfo.m_fileName;

    header.m_componentType = fileComponentType;
    header.m_componentTypeString = itkImageIOInfo.m_componentInfo.m_componentTypeString;
    header.m_componentSizeInBytes = itkImageIOInfo.m_componentInfo.m_componentSizeInBytes;

    header.m_imageSizeInBytes = static_cast< size_t >( itkImageIOInfo.m_sizeInfo.m_imageSizeInBytes );

    header.m_bufferComponentType = bufferComponentType;
    header.m_bufferComponentTypeString = k_componentStringMap.at( header.m_bufferComponentType );
    header.m_bufferComponentSizeInBytes = k_bytesPerComponentMap.at( header.m_bufferComponentType );

    header.m_bufferSizeInBytes = (header.m_imageSizeInBytes / header.m_componentSizeInBytes) *
            header.m_bufferComponentSizeInBytes;

    header.m_pixelType = fromITKPixelType( itkImageIOInfo.m_pixelInfo.m_pixelType );
    header.m_pixelTypeString = itkImageIOInfo.m_pixelInfo.m_pixelTypeString;
    header.m_numComponents = itkImageIOInfo.m_pixelInfo.m_numComponents;

    header.m_imageSizeInPixels = static_cast< size_t >( itkImageIOInfo.m_sizeInfo.m_imageSizeInPixels );

    header.m_numDimensions = itkImageIOInfo.m_spaceInfo.m_numDimensions;

    // Identity initialization of all spatial information.
    // These are filled in below.
    header.m_pixelDimensions = glm::u64vec3{ 1u };
    header.m_origin = glm::dvec3{ 0.0 };
    header.m_spacing = glm::dvec3{ 1.0 };
    header.m_directions = glm::dmat3{ 1.0 };

    header.m_boundingBoxMinMaxCorners =
            std::make_pair( glm::dvec3{ 0.0 }, glm::dvec3{ 1.0 } );

    header.m_boundingBoxCenter = glm::dvec3{ 0.5 };

    header.m_boundingBoxSize =
            header.m_boundingBoxMinMaxCorners.second -
            header.m_boundingBoxMinMaxCorners.first;

    if ( header.m_numDimensions > 3 )
    {
        return false;
    }

    for ( uint32_t i = 0; i < header.m_numDimensions; ++i )
    {
        int ii = static_cast<int>( i );

        header.m_pixelDimensions[ii] = itkImageIOInfo.m_spaceInfo.m_dimensions[i];
        header.m_origin[ii] = itkImageIOInfo.m_spaceInfo.m_origin[i];
        header.m_spacing[ii] = itkImageIOInfo.m_spaceInfo.m_spacing[i];

        for ( uint32_t j = 0; j < header.m_numDimensions; ++j )
        {
            int jj = static_cast<int>( j );

            /// @note glm::tmat3x3 has column-major order indexing
            header.m_directions[ii][jj] = itkImageIOInfo.m_spaceInfo.m_directions[i][j];
        }
    }

    header.m_boundingBoxMinMaxCorners =
            math::computeImageSubjectAABBoxCorners(
                header.m_pixelDimensions,
                header.m_directions,
                header.m_spacing,
                header.m_origin );

    header.m_boundingBoxCorners = computeAllBoxCorners(
                header.m_boundingBoxMinMaxCorners );

    header.m_boundingBoxCenter =
            0.5 * ( header.m_boundingBoxMinMaxCorners.first +
                    header.m_boundingBoxMinMaxCorners.second );

    header.m_boundingBoxSize =
            header.m_boundingBoxMinMaxCorners.second -
            header.m_boundingBoxMinMaxCorners.first;

    std::tie( header.m_spiralCode, header.m_isOblique ) =
            getSpiralCodeFromDirectionMatrix( header.m_directions );

    header.validate( true );

    return true;
}

} // namespace itkbridge

} // namespace imageio
