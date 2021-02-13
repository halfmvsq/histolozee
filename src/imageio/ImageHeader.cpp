#include "ImageHeader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <ostream>


namespace imageio
{

/// @todo Pass all values in constructor
ImageHeader::ImageHeader()
    :
      m_componentType( ComponentType::Int8 ),
      m_componentTypeString( "Undefined"),
      m_componentSizeInBytes( 0u ),
      m_imageSizeInBytes( 0u ),

      m_bufferComponentType( ComponentType::Int8 ),
      m_bufferComponentTypeString( "Undefined"),
      m_bufferComponentSizeInBytes( 0u ),
      m_bufferSizeInBytes( 0u ),

      m_pixelType( PixelType::Undefined ),
      m_pixelTypeString( "Undefined" ),
      m_numComponents( 0u ),

      m_imageSizeInPixels( 0u ),
      m_numDimensions( 0u ),
      m_pixelDimensions( 0u ),

      m_origin( 0.0 ),
      m_spacing( 0.0 ),
      m_directions( 0.0 ),

      m_boundingBoxMinMaxCorners{ glm::dvec3{0.0}, glm::dvec3{0.0} },
      m_boundingBoxCorners( { glm::dvec3{0.0} } ),
      m_boundingBoxCenter( 0.0 ),
      m_boundingBoxSize( 0.0 ),

      m_spiralCode( "" ),
      m_isOblique( false )
{}

/// @todo This function is not done
bool ImageHeader::validate( bool setDefaultsIfInvalid )
{
    if ( PixelType::Undefined == m_pixelType )
    {
        return false;
    }

    if ( glm::any( glm::equal( m_pixelDimensions, glm::u64vec3{ 0 } ) ) )
    {
        return false;

        if ( setDefaultsIfInvalid )
        {
            // only set default for the wrong dimension?
            m_pixelDimensions = glm::u64vec3{ 1u };
        }
    }

    if ( glm::any( glm::lessThanEqual( m_spacing, glm::dvec3{ 0.0 } ) ) )
    {
        return false;

        if ( setDefaultsIfInvalid )
        {
            // only set default for the wrong spacing?
            m_spacing = glm::dvec3{ 1.0 };
        }
    }

    if ( 0.0 == glm::determinant( m_directions ) )
    {
        return false;

        if ( setDefaultsIfInvalid )
        {
            m_directions = glm::dmat3{ 1.0 };
        }
    }

    return false;
}

} // namespace imageio


std::ostream& operator<< ( std::ostream& os, const imageio::ImageHeader& header )
{
    os << "File name: "                     << header.m_fileName << std::endl
       << "Component type: "                << header.m_componentTypeString << std::endl
       << "Component size (bytes): "        << header.m_componentSizeInBytes << std::endl
       << "Buffer component type: "         << header.m_bufferComponentTypeString << std::endl
       << "Buffer component size (bytes): " << header.m_bufferComponentSizeInBytes << std::endl
       << "Pixel type: "                    << header.m_pixelTypeString << std::endl
       << "Num. components: "               << header.m_numComponents << std::endl
       << "Image size (pixels): "           << header.m_imageSizeInPixels << std::endl
       << "Image size (bytes): "            << header.m_imageSizeInBytes << std::endl
       << "Image buffer size (bytes): "     << header.m_bufferSizeInBytes << std::endl
       << "Num. dimensions: "               << header.m_numDimensions << std::endl
       << "Dimensions (pixels): "           << glm::to_string( header.m_pixelDimensions ) << std::endl
       << "Origin (mm): "                   << glm::to_string( header.m_origin ) << std::endl
       << "Spacing (mm): "                  << glm::to_string( header.m_spacing ) << std::endl
       << "Directions: "                    << glm::to_string( header.m_directions ) << std::endl
       << "Bounding box corners (mm): "
            << glm::to_string( header.m_boundingBoxMinMaxCorners.first ) << ", "
            << glm::to_string( header.m_boundingBoxMinMaxCorners.second ) << std::endl
       << "Bounding box center (mm): "      << glm::to_string( header.m_boundingBoxCenter ) << std::endl
       << "Bounding box size (mm): "        << glm::to_string( header.m_boundingBoxSize ) << std::endl
       << "SPIRAL code: "                   << header.m_spiralCode << std::endl
       << "Is oblique: "                    << std::boolalpha << header.m_isOblique << std::endl;

    os << std::endl;

    return os;
}
