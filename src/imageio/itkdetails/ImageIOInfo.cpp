#include "itkdetails/ImageIOInfo.hpp"

#include "util/HZeeException.hpp"

#include <itkIOCommon.h> // for itk::SpatialOrientation

#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>

#include <exception>


namespace
{

namespace so = ::itk::SpatialOrientation;

std::unordered_map< so::ValidCoordinateOrientationFlags, std::string >
//HashType< so::ValidCoordinateOrientationFlags > >
spiralCodeMap = {
    { so::ITK_COORDINATE_ORIENTATION_RIP, "RIP" },
    { so::ITK_COORDINATE_ORIENTATION_LIP, "LIP" },
    { so::ITK_COORDINATE_ORIENTATION_RSP, "RSP" },
    { so::ITK_COORDINATE_ORIENTATION_LSP, "LSP" },
    { so::ITK_COORDINATE_ORIENTATION_RIA, "RIA" },
    { so::ITK_COORDINATE_ORIENTATION_LIA, "LIA" },
    { so::ITK_COORDINATE_ORIENTATION_RSA, "RSA" },
    { so::ITK_COORDINATE_ORIENTATION_LSA, "LSA" },
    { so::ITK_COORDINATE_ORIENTATION_IRP, "IRP" },
    { so::ITK_COORDINATE_ORIENTATION_ILP, "ILP" },
    { so::ITK_COORDINATE_ORIENTATION_SRP, "SRP" },
    { so::ITK_COORDINATE_ORIENTATION_SLP, "SLP" },
    { so::ITK_COORDINATE_ORIENTATION_IRA, "IRA" },
    { so::ITK_COORDINATE_ORIENTATION_ILA, "ILA" },
    { so::ITK_COORDINATE_ORIENTATION_SRA, "SRA" },
    { so::ITK_COORDINATE_ORIENTATION_SLA, "SLA" },
    { so::ITK_COORDINATE_ORIENTATION_RPI, "RPI" },
    { so::ITK_COORDINATE_ORIENTATION_LPI, "LPI" },
    { so::ITK_COORDINATE_ORIENTATION_RAI, "RAI" },
    { so::ITK_COORDINATE_ORIENTATION_LAI, "LAI" },
    { so::ITK_COORDINATE_ORIENTATION_RPS, "RPS" },
    { so::ITK_COORDINATE_ORIENTATION_LPS, "LPS" },
    { so::ITK_COORDINATE_ORIENTATION_RAS, "RAS" },
    { so::ITK_COORDINATE_ORIENTATION_LAS, "LAS" },
    { so::ITK_COORDINATE_ORIENTATION_PRI, "PRI" },
    { so::ITK_COORDINATE_ORIENTATION_PLI, "PLI" },
    { so::ITK_COORDINATE_ORIENTATION_ARI, "ARI" },
    { so::ITK_COORDINATE_ORIENTATION_ALI, "ALI" },
    { so::ITK_COORDINATE_ORIENTATION_PRS, "PRS" },
    { so::ITK_COORDINATE_ORIENTATION_PLS, "PLS" },
    { so::ITK_COORDINATE_ORIENTATION_ARS, "ARS" },
    { so::ITK_COORDINATE_ORIENTATION_ALS, "ALS" },
    { so::ITK_COORDINATE_ORIENTATION_IPR, "IPR" },
    { so::ITK_COORDINATE_ORIENTATION_SPR, "SPR" },
    { so::ITK_COORDINATE_ORIENTATION_IAR, "IAR" },
    { so::ITK_COORDINATE_ORIENTATION_SAR, "SAR" },
    { so::ITK_COORDINATE_ORIENTATION_IPL, "IPL" },
    { so::ITK_COORDINATE_ORIENTATION_SPL, "SPL" },
    { so::ITK_COORDINATE_ORIENTATION_IAL, "IAL" },
    { so::ITK_COORDINATE_ORIENTATION_SAL, "SAL" },
    { so::ITK_COORDINATE_ORIENTATION_PIR, "PIR" },
    { so::ITK_COORDINATE_ORIENTATION_PSR, "PSR" },
    { so::ITK_COORDINATE_ORIENTATION_AIR, "AIR" },
    { so::ITK_COORDINATE_ORIENTATION_ASR, "ASR" },
    { so::ITK_COORDINATE_ORIENTATION_PIL, "PIL" },
    { so::ITK_COORDINATE_ORIENTATION_PSL, "PSL" },
    { so::ITK_COORDINATE_ORIENTATION_AIL, "AIL" },
    { so::ITK_COORDINATE_ORIENTATION_ASL, "ASL" }
};

} // anonymous namespace


namespace itkdetails
{

/// @brief Definitions in this namespace are for ITK image IO.
/// With the exception of SPIRAL codes, nothing is specific to image type or dimension.
namespace io
{

template< class T >
bool setMetaDataEntry(
        MetaDataMap& metaDataMap,
        const itk::MetaDataDictionary& dictionary,
        const std::string& key )
{
    T value;

    if ( itk::ExposeMetaData< T >( dictionary, key, value ) )
    {
        metaDataMap[key] = value;
        return true;
    }
    else
    {
        return false;
    }
}


MetaDataMap getMetaDataMap( const ::itk::ImageIOBase::Pointer imageIO )
{
    MetaDataMap metaDataMap;

    if ( imageIO.IsNull() )
    {
        return metaDataMap;
    }

    const itk::MetaDataDictionary& dictionary = imageIO->GetMetaDataDictionary();
    itk::MetaDataDictionary::ConstIterator itr;

    for ( itr = dictionary.Begin(); itr != dictionary.End(); ++itr )
    {
        const std::string key = itr->first;
        std::string value;

        itk::SpatialOrientation::ValidCoordinateOrientationFlags orientFlagsValue =
                itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_INVALID;

        if ( itk::ExposeMetaData< std::string >( dictionary, key, value ) )
        {
            /// @note For some weird reason, some of the strings returned by this method
            /// contain '\0' characters. We replace them by spaces:
            std::ostringstream sout("");
            for ( std::size_t i = 0; i < value.length(); ++i )
            {
                if ( value[i] >= ' ' )
                {
                    sout << value[i];
                }
            }
            sout << std::ends;

            metaDataMap[key] = sout.str();
        }
        else if ( itk::ExposeMetaData( dictionary, key, orientFlagsValue ) )
        {
            metaDataMap[key] = spiralCodeMap[ orientFlagsValue ];
        }
        else
        {
            if ( setMetaDataEntry<   int8_t >( metaDataMap, dictionary, key ) ) { continue; }
            if ( setMetaDataEntry<  uint8_t >( metaDataMap, dictionary, key ) ) { continue; }
            if ( setMetaDataEntry<  int16_t >( metaDataMap, dictionary, key ) ) { continue; }
            if ( setMetaDataEntry< uint16_t >( metaDataMap, dictionary, key ) ) { continue; }
            if ( setMetaDataEntry<  int32_t >( metaDataMap, dictionary, key ) ) { continue; }
            if ( setMetaDataEntry< uint32_t >( metaDataMap, dictionary, key ) ) { continue; }
            if ( setMetaDataEntry<  int64_t >( metaDataMap, dictionary, key ) ) { continue; }
            if ( setMetaDataEntry< uint64_t >( metaDataMap, dictionary, key ) ) { continue; }
            if ( setMetaDataEntry<    float >( metaDataMap, dictionary, key ) ) { continue; }
            if ( setMetaDataEntry<   double >( metaDataMap, dictionary, key ) ) { continue; }

            std::cerr << key << " is of unsupported type "
                      << itr->second->GetMetaDataObjectTypeName() << std::endl;
        }
    }

    return metaDataMap;
}


FileInfo::FileInfo()
    :
      m_fileName( "" ),
      m_byteOrder( ::itk::ImageIOBase::OrderNotApplicable ),
      m_byteOrderString( "OrderNotApplicable" ),
      m_useCompression( false ),
      m_fileType( ::itk::ImageIOBase::TypeNotApplicable ),
      m_fileTypeString( "TypeNotApplicable" ),
      m_supportedReadExtensions(),
      m_supportedWriteExtensions()
{}

FileInfo::FileInfo( const ::itk::ImageIOBase::Pointer imageIO )
{
    if ( ! set( imageIO ) )
    {
        throw std::invalid_argument( "Exception while instantiating FileInfo" );
    }
}

bool FileInfo::set( const itk::ImageIOBase::Pointer imageIO )
{
    if ( imageIO.IsNull() )
    {
        return false;
    }

    m_fileName = imageIO->GetFileName();

    m_byteOrder = imageIO->GetByteOrder();
    m_byteOrderString = imageIO->GetByteOrderAsString( m_byteOrder );
    m_useCompression = imageIO->GetUseCompression();

    m_fileType = imageIO->GetFileType();
    m_fileTypeString = imageIO->GetFileTypeAsString( m_fileType );

    m_supportedReadExtensions = imageIO->GetSupportedReadExtensions();
    m_supportedWriteExtensions = imageIO->GetSupportedWriteExtensions();

    return true;
}

bool FileInfo::validate() { return true; }


ComponentInfo::ComponentInfo()
    : m_componentType( ::itk::ImageIOBase::UNKNOWNCOMPONENTTYPE ),
      m_componentTypeString( "UNKNOWNCOMPONENTTYPE" ),
      m_componentSizeInBytes( 0u )
{}

ComponentInfo::ComponentInfo( const ::itk::ImageIOBase::Pointer imageIO )
{
    if ( ! set( imageIO ) )
    {
        throw std::invalid_argument( "Exception while instantiating ComponentInfo" );
    }
}

bool ComponentInfo::set( const itk::ImageIOBase::Pointer imageIO )
{
    if ( imageIO.IsNull() )
    {
        return false;
    }

    m_componentType = imageIO->GetComponentType();
    m_componentTypeString = itk::ImageIOBase::GetComponentTypeAsString( m_componentType );
    m_componentSizeInBytes = static_cast< uint32_t >( imageIO->GetComponentSize() );

    return true;
}

bool ComponentInfo::validate() { return true; }


PixelInfo::PixelInfo()
    : m_pixelType( ::itk::ImageIOBase::UNKNOWNPIXELTYPE ),
      m_pixelTypeString( "UNKNOWNPIXELTYPE" ),
      m_numComponents( 0u ),
      m_pixelStrideInBytes( 0 )
{}

PixelInfo::PixelInfo( const ::itk::ImageIOBase::Pointer imageIO )
{
    if ( ! set( imageIO ) )
    {
        throw std::invalid_argument( "Exception while instantiating PixelInfo" );
    }
}

bool PixelInfo::set( const itk::ImageIOBase::Pointer imageIO )
{
    if ( imageIO.IsNull() )
    {
        return false;
    }

    m_pixelType = imageIO->GetPixelType();
    m_pixelTypeString = itk::ImageIOBase::GetPixelTypeAsString( m_pixelType );
    m_numComponents = static_cast< uint32_t >( imageIO->GetNumberOfComponents() );
    m_pixelStrideInBytes = imageIO->GetPixelStride();

    return true;
}

bool PixelInfo::validate() { return true; }


SizeInfo::SizeInfo()
    : m_imageSizeInComponents( 0 ),
      m_imageSizeInPixels( 0 ),
      m_imageSizeInBytes( 0 )
{}

SizeInfo::SizeInfo( const ::itk::ImageIOBase::Pointer imageIO )
{
    if ( ! set( imageIO ) )
    {
        throw std::invalid_argument( "Exception while instantiating SizeInfo" );
    }
}

bool SizeInfo::set( const itk::ImageIOBase::Pointer imageIO )
{
    if ( imageIO.IsNull() )
    {
        return false;
    }

    m_imageSizeInComponents = imageIO->GetImageSizeInComponents();
    m_imageSizeInPixels = imageIO->GetImageSizeInPixels();
    m_imageSizeInBytes = imageIO->GetImageSizeInBytes();

    return true;
}

bool SizeInfo::set(
        const typename ::itk::ImageBase< 3 >::Pointer imageBase,
        const uint32_t componentSizeInBytes )
{
    if ( imageBase.IsNull() )
    {
        return false;
    }

    m_imageSizeInPixels = imageBase->GetLargestPossibleRegion().GetNumberOfPixels();
    m_imageSizeInComponents = m_imageSizeInPixels * imageBase->GetNumberOfComponentsPerPixel();
    m_imageSizeInBytes = m_imageSizeInComponents * componentSizeInBytes;

    return true;
}

bool SizeInfo::validate() { return true; }


SpaceInfo::SpaceInfo()
    : m_numDimensions( 0u ),
      m_dimensions(),
      m_origin(),
      m_spacing(),
      m_directions()
{}

SpaceInfo::SpaceInfo( const ::itk::ImageIOBase::Pointer imageIO )
{
    if ( ! set( imageIO ) )
    {
        throw std::invalid_argument( "Exception while instantiating SpaceInfo" );
    }
}

bool SpaceInfo::set( const itk::ImageIOBase::Pointer imageIO )
{
    if ( imageIO.IsNull() )
    {
        return false;
    }

    m_numDimensions = static_cast< uint32_t >( imageIO->GetNumberOfDimensions() );

    if ( 3 < m_numDimensions )
    {
        return false;
    }

    m_dimensions.resize( m_numDimensions );
    m_origin.resize( m_numDimensions );
    m_spacing.resize( m_numDimensions );
    m_directions.resize( m_numDimensions );

    for ( uint32_t i = 0; i < m_numDimensions; ++i )
    {
        m_dimensions[i] = static_cast< uint64_t >( imageIO->GetDimensions( i ) );
        m_origin[i] = imageIO->GetOrigin( i );
        m_spacing[i] = imageIO->GetSpacing( i );
        m_directions[i] = imageIO->GetDirection( i );
    }

    return true;
}

bool SpaceInfo::set( const typename ::itk::ImageBase< 3 >::Pointer imageBase )
{
    if ( imageBase.IsNull() )
    {
        return false;
    }

    m_numDimensions = 3;

    m_dimensions.resize( m_numDimensions );
    m_origin.resize( m_numDimensions );
    m_spacing.resize( m_numDimensions );
    m_directions.resize( m_numDimensions );

    const typename ::itk::ImageBase<3>::RegionType region = imageBase->GetLargestPossibleRegion();
    const typename ::itk::ImageBase<3>::RegionType::SizeType size = region.GetSize();;
    const typename ::itk::ImageBase<3>::PointType origin = imageBase->GetOrigin();
    const typename ::itk::ImageBase<3>::SpacingType spacing = imageBase->GetSpacing();
    const typename ::itk::ImageBase<3>::DirectionType direction = imageBase->GetDirection();

    for ( uint32_t i = 0; i < m_numDimensions; ++i )
    {
        m_dimensions[i] = static_cast< uint64_t >( size[i] );
        m_origin[i] = origin[i];
        m_spacing[i] = spacing[i];

        for ( uint32_t j = 0; j < m_numDimensions; ++j )
        {
            /// @internal The j'th component of i'th direction vector is the
            /// direction matrix element at row j and column i
            m_directions[i][j] = direction( j, i );
        }
    }

    return true;
}

bool SpaceInfo::validate() { return true; }


ImageIoInfo::ImageIoInfo( const ::itk::ImageIOBase::Pointer imageIO )
{
    if ( ! set( imageIO ) )
    {
        throw std::invalid_argument( "Exception while instantiating ImageIOInfo" );
    }
}

bool ImageIoInfo::set( const itk::ImageIOBase::Pointer imageIO )
{
    if ( imageIO.IsNull() )
    {
        return false;
    }

    m_metaData = getMetaDataMap( imageIO );

    return ( m_fileInfo.set( imageIO ) &&
             m_componentInfo.set( imageIO ) &&
             m_pixelInfo.set( imageIO ) &&
             m_sizeInfo.set( imageIO ) &&
             m_spaceInfo.set( imageIO ) );
}

bool ImageIoInfo::validate()
{
    return ( m_fileInfo.validate() &&
             m_componentInfo.validate() &&
             m_pixelInfo.validate() &&
             m_sizeInfo.validate() &&
             m_spaceInfo.validate() );
}

} // namespace io

} // namespace itkdetails
