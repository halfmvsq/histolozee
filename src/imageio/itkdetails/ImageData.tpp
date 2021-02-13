#include "itkdetails/ImageData.hpp" // for IDE
#include "itkdetails/ImageIOInfo.hpp"
#include "itkdetails/ImageReading.hpp"
#include "itkdetails/ImageUtility.hpp"

#include "util/HZeeException.hpp"

#include <exception>
#include <sstream>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <vector> // for IDE


namespace itkdetails
{

template< typename ComponentType >
bool normalizeImageIntensities(
        image3d::ImageBaseType::Pointer& imageBase,
        const imageio::ComponentNormalizationPolicy& normalizationPolicy )
{
    switch ( normalizationPolicy )
    {
    case imageio::ComponentNormalizationPolicy::SignedNormalizedFloating :
    {
        return utility::rescaleIntensities< ComponentType >( imageBase, -1.0, 1.0 );
    }
    case imageio::ComponentNormalizationPolicy::UnsignedNormalizedFloating :
    {
        return utility::rescaleIntensities< ComponentType >( imageBase, 0.0, 1.0 );
    }
    case imageio::ComponentNormalizationPolicy::None :
    {
        return true;
    }
    }
    return false;
}


template< class ComponentType >
ImageData< ComponentType >::ImageData()
    :
      ImageBaseData(),
      m_splitImagePtrs()
{}


template< class ComponentType >
ImageData< ComponentType >::ImageData(
        std::vector< typename image3d::ImageType< ComponentType >::Pointer > splitImagePtrs,
        io::ImageIoInfo ioInfo )
    :
      ImageBaseData( std::move( ioInfo ) ),
      m_splitImagePtrs( std::move( splitImagePtrs ) )
{
    // No need to call splitImageIntoComponents() here, since the split image components
    // are passed in to the constructor
    computePixelStatistics();
}


template< class ComponentType >
ImageData< ComponentType >::ImageData(
        const io::ImageIoInfo& ioInfo,
        const ComponentType& defaultValue )
{
    if ( image3d::NDIM != ioInfo.m_spaceInfo.m_numDimensions )
    {
        std::ostringstream ss;
        ss << "Unable to setup and construct ImageData object of dimension "
           << ioInfo.m_spaceInfo.m_numDimensions << std::ends;

        throw std::invalid_argument( ss.str() );
    }

    using ImageType = image3d::ImageType< ComponentType >;

    typename ImageType::IndexType start;
    typename ImageType::SizeType size;
    typename ImageType::SpacingType spacing;
    typename ImageType::PointType origin;
    typename ImageType::DirectionType directions;

    for ( uint i = 0; i < image3d::NDIM; ++i )
    {
        start[i] = 0;
        size[i] = ioInfo.m_spaceInfo.m_dimensions[i];
        spacing[i] = ioInfo.m_spaceInfo.m_spacing[i];
        origin[i] = ioInfo.m_spaceInfo.m_origin[i];

        for ( uint j = 0; j < image3d::NDIM; ++j )
        {
            directions[i][j] = ioInfo.m_spaceInfo.m_directions[i][j];
        }
    }

    typename ImageType::RegionType region;
    region.SetSize( size );
    region.SetIndex( start );

    typename ImageType::Pointer image = ImageType::New();
    image->SetRegions( region );
    image->Allocate();
    image->FillBuffer( defaultValue );

    image->SetSpacing( spacing );
    image->SetOrigin( origin );
    image->SetDirection( directions );

    m_imageBasePtr = static_cast< image3d::ImageBaseType::Pointer >( image );
    m_imageIOInfo = ioInfo;

    if ( ! setup() )
    {
        throw std::runtime_error( "Unable to setup and construct ImageData object" );
    }
}


template< class ComponentType >
bool ImageData< ComponentType >::setup()
{
    bool success = ( splitImageIntoComponents() && computePixelStatistics() );
    return success;
}


template< class ComponentType >
bool ImageData< ComponentType >::loadFromImageFile(
        const std::string& fileName,
        const imageio::ComponentNormalizationPolicy& normalizationPolicy )
{
    const ::itk::ImageIOBase::Pointer imageIO =
            itkdetails::utility::createStandardImageIO( fileName.c_str() );

    if ( imageIO.IsNull() )
    {
        std::cerr << "Error creating ImageIOBase." << std::endl;
        return false;
    }

    if ( ! m_imageIOInfo.set( imageIO ) )
    {
        std::cerr << "Error setting imageIO information" << std::endl;
        return false;
    }

#if 0
    /// @note This is only needed if the read component type SHOULD equal the image component type
    const auto expectedComponentType = io::itkComponentTypeMap[
            std::type_index( typeid( ComponentType ) ) ];

    if ( expectedComponentType != m_imageIOInfo.m_componentInfo.m_componentType )
    {
        std::cerr << "Component type read from image '"
                  << fileName << "' is not of type "
                  << m_imageIOInfo.m_componentInfo.m_componentTypeString << std::endl;

        return false;
    }
#endif

    m_imageBasePtr = reader::read< ComponentType >( imageIO );

    if ( m_imageBasePtr.IsNull() )
    {
        std::cerr << "Error reading image '" << fileName << "'" << std::endl;
        return false;
    }

    if ( ::itkdetails::image3d::NDIM != m_imageBasePtr->GetImageDimension() )
    {
        std::cerr << "Error reading image '" << fileName << "' as 3D volume" << std::endl;
        m_imageBasePtr = ITK_NULLPTR;
        return false;
    }

    const bool normalized = normalizeImageIntensities< ComponentType >(
                m_imageBasePtr, normalizationPolicy );

    if ( ! normalized )
    {
        // Failure during intensity normalization step
        return false;
    }

    return setup();
}


template< class ComponentType >
bool ImageData< ComponentType >::loadFromDicomSeries(
        const std::vector< std::string >& fileNames,
        const imageio::ComponentNormalizationPolicy& normalizationPolicy )
{
    if ( fileNames.empty() )
    {
        return false;
    }

    const ::itk::ImageIOBase::Pointer imageIO =
            itkdetails::utility::dicom::createDicomImageIO( fileNames[0].c_str() );

    if ( imageIO.IsNull() )
    {
        std::cerr << "Error creating GDCMImageIO." << std::endl;
        return false;
    }

    if ( ! m_imageIOInfo.set( imageIO ) )
    {
        std::cerr << "Error setting imageIO information" << std::endl;
        return false;
    }

#if 0
    /// @note This is only needed if the read component type SHOULD equal the image component type
    const auto expectedComponentType = io::itkComponentTypeMap[
            std::type_index( typeid( ComponentType ) ) ];

    if ( expectedComponentType != m_imageIOInfo.m_componentInfo.m_componentType )
    {
        std::cerr << "Component type read from DICOM series image '"
                  << fileNames[0] << "' is not of type "
                  << m_imageIOInfo.m_componentInfo.m_componentTypeString << std::endl;

        return false;
    }
#endif

    m_imageBasePtr = reader::read< ComponentType >( imageIO, fileNames );

    if ( m_imageBasePtr.IsNull() )
    {
        std::cerr << "Error reading DICOM series starting with file '"
                  << fileNames[0] << "'" << std::endl;

        return false;
    }

    if ( ::itkdetails::image3d::NDIM != m_imageBasePtr->GetImageDimension() )
    {
        std::cerr << "Error reading DICOM series starting with file '"
                  << fileNames[0] << "' as 3D volume" << std::endl;

        m_imageBasePtr = ITK_NULLPTR;
        return false;
    }

    const bool normalized = normalizeImageIntensities< ComponentType >(
                m_imageBasePtr, normalizationPolicy );

    if ( ! normalized )
    {
        // Failure during intensity normalization step
        return false;
    }

    /// @internal Set the size and space information using itkImageBase directly,
    /// since these may have been set incorrectly using itkImageIOBase

    m_imageIOInfo.m_sizeInfo.set( m_imageBasePtr, imageIO->GetComponentSize() );
    m_imageIOInfo.m_spaceInfo.set( m_imageBasePtr );

    return setup();
}


template< class ComponentType >
const uint8_t* ImageData< ComponentType >::bufferPointer() const
{
    if ( m_imageBasePtr.IsNotNull() )
    {
        return reinterpret_cast< const uint8_t* >(
                    ( isVectorImage() )
                    ? asITKVectorImage()->GetBufferPointer()
                    : asITKImage()->GetBufferPointer() );
    }
    else
    {
        return nullptr;
    }
}


template< class ComponentType >
const uint8_t* ImageData< ComponentType >::bufferPointer(
        const uint32_t componentIndex ) const
{
    if ( componentIndex >= m_splitImagePtrs.size() )
    {
        return nullptr;
    }

    const auto& image = m_splitImagePtrs[ componentIndex ];

    if ( image.IsNotNull() )
    {
        return reinterpret_cast< const uint8_t* >(
                    image->GetBufferPointer() );
    }
    else
    {
        return nullptr;
    }
}


template< class ComponentType >
vtkSmartPointer< vtkImageData >
ImageData< ComponentType >::asVTKImageData(
        const uint32_t componentIndex ) const
{
    if ( componentIndex >= m_splitImagePtrs.size() )
    {
        return nullptr;
    }

    const auto& image = m_splitImagePtrs[ componentIndex ];

    const vtkSmartPointer< vtkImageData > imageData =
            utility::convertITKImageToVTKImageData< ComponentType >( image );

    return imageData;
}


template< class ComponentType >
std::vector< vtkSmartPointer< vtkImageData > >
ImageData< ComponentType >::asVTKImageData() const
{
    std::vector< vtkSmartPointer< vtkImageData > > data(
                m_imageIOInfo.m_pixelInfo.m_numComponents );

    for ( uint32_t i = 0; i < m_imageIOInfo.m_pixelInfo.m_numComponents; ++i )
    {
        data[i] = asVTKImageData( i );
    }

    return data;
}


template< class ComponentType >
bool ImageData< ComponentType >::getPixelAsDouble(
        const uint32_t componentIndex,
        uint32_t i, uint32_t j, uint32_t k,
        double& value ) const
{
    if ( componentIndex >= m_splitImagePtrs.size() )
    {
        std::ostringstream ss;
        ss << "Attempting to access invalid image component " << componentIndex << std::ends;
        throw std::runtime_error( ss.str() );
    }

    const auto& image = m_splitImagePtrs[ componentIndex ];

    if ( ! image )
    {
        std::ostringstream ss;
        ss << "Null image component " << componentIndex << std::ends;
        throw std::runtime_error( ss.str() );
    }

    typename image3d::ImageType< ComponentType >::RegionType region =
            image->GetLargestPossibleRegion();

    typename image3d::ImageType< ComponentType >::IndexType index = {{ i, j, k }};

    if ( region.IsInside( index ) )
    {
        value = static_cast<double>( image->GetPixel( index ) );
        return true;
    }
    else
    {
        return false;
    }
}


template< class ComponentType >
std::vector< typename image3d::ImageType< ComponentType >::Pointer >
ImageData< ComponentType >::asSplitITKImage() const
{
    return m_splitImagePtrs;
}


template< class ComponentType >
typename image3d::ImageType< ComponentType >::Pointer
ImageData< ComponentType >::asITKImage() const
{
    return utility::downcastImageBaseToImage< ComponentType, 3 >( m_imageBasePtr );
}


template< class ComponentType >
typename image3d::VectorImageType< ComponentType >::Pointer
ImageData< ComponentType >::asITKVectorImage() const
{
    return utility::downcastImageBaseToVectorImage< ComponentType, 3 >( m_imageBasePtr );
}


/**
 * @note Data of multi-component (vector) images gets duplicated by this function:
 * one copy pointed to by base class' \c m_imageBasePtr;
 * the other copy pointed to by this class' \c m_splitImagePtrs
 */
template< class ComponentType >
bool ImageData< ComponentType >::splitImageIntoComponents()
{
    if ( isVectorImage() )
    {
        const typename image3d::VectorImageType< ComponentType >::Pointer
                vectorImage = asITKVectorImage();

        if ( vectorImage.IsNull() )
        {
            std::cerr << "Error obtaining vector image." << std::endl;
            return false;
        }

        /// @internal Same as m_imageInfo.m_pixelInfo.m_numComponents:
        const uint32_t numComponents = vectorImage->GetVectorLength();

        m_splitImagePtrs.resize( numComponents );

        for ( uint32_t i = 0; i < numComponents; ++i )
        {
            m_splitImagePtrs[i] = image3d::ImageType< ComponentType >::New();

            m_splitImagePtrs[i]->CopyInformation( vectorImage );
            m_splitImagePtrs[i]->SetRegions( vectorImage->GetBufferedRegion() );
            m_splitImagePtrs[i]->Allocate();

            ComponentType* source = vectorImage->GetBufferPointer() + i;
            ComponentType* dest = m_splitImagePtrs[i]->GetBufferPointer();

            const ComponentType* end = dest + numPixels();

            /// @internal Copy pixels from component \c i of \c vectorImage (the source),
            /// which are offset from each other by a stride of \c numComponents,
            /// into pixels of the i'th split image (the destination)
            while ( dest < end )
            {
                *dest = *source;
                ++dest;
                source += numComponents;
            }
        }
    }
    else
    {
        const typename image3d::ImageType< ComponentType >::Pointer
                image = asITKImage();

        if ( image.IsNull() )
        {
            std::cerr << "Error obtaining image." << std::endl;
            return false;
        }

        /// @internal Image has only one component
        m_splitImagePtrs.resize( 1 );
        m_splitImagePtrs[0] = image;
    }

    return true;
}


template< class ComponentType >
bool ImageData< ComponentType >::computePixelStatistics()
{
    try
    {
        m_pixelStatistics.clear();

        for ( const auto& image : m_splitImagePtrs )
        {
            auto stats = utility::computeImagePixelStatistics<
                    image3d::ImageType< ComponentType > >( image );

            utility::PixelStatistics< double > cs;
            cs.m_minimum = static_cast< double >( stats.m_minimum );
            cs.m_maximum = static_cast< double >( stats.m_maximum );
            cs.m_mean = stats.m_mean;
            cs.m_stdDeviation = stats.m_stdDeviation;
            cs.m_variance = stats.m_variance;
            cs.m_sum = stats.m_sum;
            cs.m_histogram = std::move( stats.m_histogram );
            cs.m_quantiles = std::move( stats.m_quantiles );

            m_pixelStatistics.push_back( cs );
        }

        return true;
    }
    catch ( const ::itk::ExceptionObject& e )
    {
        std::cerr << "Exception while computing image statistics: " << e.what() << std::endl;
        return false;
    }
    catch ( ... )
    {
        std::cerr << "Exception while computing image statistics." << std::endl;
        return false;
    }
}

} // namespace itkdetails


#if 0
template<class TPixel, unsigned int VDim>
void
cloneImage()
{
    // Simply make a copy of the input image on the stack
    ImagePointer output = ImageType::New();
    output->SetRegions(input->GetBufferedRegion());
    output->SetSpacing(input->GetSpacing());
    output->SetOrigin(input->GetOrigin());
    output->SetDirection(input->GetDirection());
    output->SetMetaDataDictionary(input->GetMetaDataDictionary());
    output->Allocate();

    size_t n = input->GetBufferedRegion().GetNumberOfPixels();
    for(size_t i = 0; i < n; i++)
        output->GetBufferPointer()[i] = input->GetBufferPointer()[i];
}
#endif
