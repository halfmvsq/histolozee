#include "logic/ui/details/PackageHeader.h"

#include <iomanip>
#include <locale>
#include <sstream>


namespace
{

template< typename T >
std::string formatWithCommas( T value )
{
    std::stringstream ss;
    ss.imbue( std::locale( "" ) );
    ss << std::fixed << value;
    return ss.str();
}

} // anonymous


namespace details
{

std::vector< std::pair< std::string, std::string > >
packageImageHeaderForUi( const imageio::ImageHeader& header,
                         const imageio::ImageSettings& settings )
{
    std::vector< std::pair< std::string, std::string > > items;

    std::ostringstream ss;

    // Pixel type:
    ss.str( std::string() );
    ss << header.m_pixelTypeString << " (" << header.m_numComponents << " component"
       << ( ( header.m_numComponents > 1 ) ? "s)" : ")" ) << std::ends;
    items.emplace_back( std::make_pair( "Pixel type", ss.str() ) );

    // Component type (on disk):
    ss.str( std::string() );
    ss << header.m_componentTypeString << " ("
       << header.m_componentSizeInBytes << " byte"
       << ( ( header.m_componentSizeInBytes > 1 ) ? "s)" : ")" ) << std::ends;
    items.emplace_back( std::make_pair( "Component type", ss.str() ) );

    // Component type (in memory):
//    ss.str( std::string() );
//    ss << header.m_bufferComponentTypeString << " (" << header.m_bufferComponentSizeInBytes << " bytes)" << std::ends;
//    items.emplace_back( std::make_pair( "Components (RAM)", ss.str() ) );

    // Pixel dimensions per axis (i.e. matrix size):
    ss.str( std::string() );
    ss << std::to_string( header.m_pixelDimensions[0] ) << " x "
       << std::to_string( header.m_pixelDimensions[1] ) << " x "
       << std::to_string( header.m_pixelDimensions[2] )
       << " (" << formatWithCommas( header.m_imageSizeInPixels ) << " pixels" << ")" << std::ends;
    items.emplace_back( std::make_pair( "Dimensions", ss.str() ) );

    // Size in bytes and mebibytes (on disk):
    const float MiB_disk = static_cast<float>( header.m_imageSizeInBytes ) / (2 << 19 );
    ss.str( std::string() );
    ss << formatWithCommas( header.m_imageSizeInBytes ) << " bytes" << " ("
       << formatWithCommas( MiB_disk ) << " MiB)" << std::ends;
    items.emplace_back( std::make_pair( "Storage", ss.str() ) );

    // Size in bytes and mebibytes (as loaded in system memory):
//    const float MiB_RAM = static_cast<float>( header.m_bufferSizeInBytes ) / (2 << 19 );
//    ss.str( std::string() );
//    ss << std::to_string( header.m_bufferSizeInBytes ) << " bytes" << " (" << MiB_RAM << " MiB)" << std::ends;
//    items.emplace_back( std::make_pair( "Storage (RAM)", ss.str() ) );

    // Voxel spacing in subject space:
    ss.str( std::string() );
    ss << "(" << header.m_spacing.x << ", " << header.m_spacing.y << ", "
       << header.m_spacing.z << ") mm" << std::ends;
    items.emplace_back( std::make_pair( "Spacing", ss.str() ) );

    // Origin in subject space:
    ss.str( std::string() );
    ss << "(" << header.m_origin.x << ", " << header.m_origin.y << ", "
       << header.m_origin.z << ") mm" << std::ends;
    items.emplace_back( std::make_pair( "Origin", ss.str() ) );

    // Axis directions in subject space:
    ss.str( std::string() );
    ss << "(" << header.m_directions[0].x << ", " << header.m_directions[0].y << ", "
       << header.m_directions[0].z << ")" << std::ends;
    items.emplace_back( std::make_pair( "X direction", ss.str() ) );

    ss.str( std::string() );
    ss << "(" << header.m_directions[1].x << ", " << header.m_directions[1].y << ", "
       << header.m_directions[1].z << ")" << std::ends;
    items.emplace_back( std::make_pair( "Y direction", ss.str() ) );

    ss.str( std::string() );
    ss << "(" << header.m_directions[2].x << ", " << header.m_directions[2].y << ", "
       << header.m_directions[2].z << ")" << std::ends;
    items.emplace_back( std::make_pair( "Z direction", ss.str() ) );

    // SPIRAL code and flag indicating whether the directions are oblique
    ss.str( std::string() );
    if ( header.m_isOblique )
    {
        ss << "Oblique (closest to " << header.m_spiralCode << ")" << std::ends;
    }
    else
    {
        ss << header.m_spiralCode << std::ends;
    }
    items.emplace_back( std::make_pair( "Orientation", ss.str() ) );

    // Min and max axis-aligned bounding box corners in subject space:
    ss.str( std::string() );
    ss << "(" << header.m_boundingBoxMinMaxCorners.first.x << ", "
       << header.m_boundingBoxMinMaxCorners.first.y << ", "
       << header.m_boundingBoxMinMaxCorners.first.z << "), "
       << "(" << header.m_boundingBoxMinMaxCorners.second.x << ", "
       << header.m_boundingBoxMinMaxCorners.second.y << ", "
       << header.m_boundingBoxMinMaxCorners.second.z << ") " << std::ends;

    items.emplace_back( std::make_pair( "AABB corners", ss.str() ) );

    // Center of the axis-aligned bounding box in subject space
    ss.str( std::string() );
    ss << "(" << header.m_boundingBoxCenter.x << ", "
       << header.m_boundingBoxCenter.y << ", "
       << header.m_boundingBoxCenter.z << ")" << std::ends;

    items.emplace_back( std::make_pair( "AABB center", ss.str() ) );

    // Min/max intensities of component 0
    ss.str( std::string() );
    ss << "[" << settings.thresholdLow( 0 ) << ", "
       << settings.thresholdHigh( 0 ) << "]" << std::ends;

    items.emplace_back( std::make_pair( "Value range", ss.str() ) );

    return items;
}

} // namespace details
