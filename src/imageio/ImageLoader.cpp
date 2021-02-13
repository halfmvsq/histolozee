#include "ImageLoader.h"
#include "ImageHeader.h"
#include "ImageSettings.h"
#include "ImageTransformations.hpp"
#include "ImageCpuRecord.h"
#include "ParcellationCpuRecord.h"

#include "HZeeTypes.hpp"

#include "itkbridge/ImageDataFactory.hpp"
#include "itkbridge/ITKBridge.hpp"

#include "itkdetails/ImageData.hpp"
#include "itkdetails/ImageUtility.hpp"
#include "itkdetails/ImageIOInfo.hpp"

#include "itkImage.h"

#include <boost/filesystem.hpp>

#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <utility>


namespace
{

static const glm::vec3 sk_origin{ 0.0f, 0.0f, 0.0f };
static const glm::quat sk_ident{ 1.0f, 0.0f, 0.0f, 0.0f };

/**
 * @brief Get the collection of image file names associated with a single input image file
 * and an optional DICOM series UID. For the case of regular image files, the output collection
 * consists of the single file name itself. The function also returns a boolean flag indicating
 * whether the image is in DICOM format. DICOM images may correspond to multiple image
 * files on disk.
 *
 * @param[in] inputFileName Input image file name
 * @param[in] inputDicomSeriesUID Optional input DICOM series UID
 *
 * @return Pair consisting of
 * 1. Vector of image file names: this will be empty if the image does not exist
 * 2. Boolean flag indicating whether this is a DICOM image
 */
std::pair< std::vector< std::string >, bool >
getImageFileNames( const std::string& inputFileName,
                   const boost::optional< std::string >& inputDicomSeriesUID )
{
    static const std::pair< std::vector< std::string >, bool > EMPTY =
            std::make_pair( std::vector< std::string >{}, false );

    std::vector< std::string > fileNames;
    bool isDicom = false;

    const ::itkdetails::utility::ImageFileType imageFileType =
            ::itkdetails::utility::getImageFileType( inputFileName.c_str() );

    if ( ::itkdetails::utility::ImageFileType::SingleImage == imageFileType )
    {
        /// @todo Logging
        std::cout << "> Loading standard image" << std::endl;

        // There is a single file name
        fileNames = { inputFileName };
        isDicom = false;
    }
    else if ( ::itkdetails::utility::ImageFileType::DICOMSeries == imageFileType )
    {
        /// @todo Logging
        std::cout << "> Loading DICOM series" << std::endl;

        std::vector< std::string > foundDicomSeriesUIDs;
        ::itkdetails::utility::dicom::NameGeneratorType::Pointer nameGenerator;

        std::tie( foundDicomSeriesUIDs, nameGenerator ) =
                ::itkdetails::utility::dicom::seriesSearch( inputFileName.c_str() );

        if ( nameGenerator.IsNull() )
        {
            std::cerr << "DICOM file name generator is invalid." << std::endl;
            return EMPTY;
        }

        std::string selectedSeriesUID;

        if ( inputDicomSeriesUID )
        {
            // Check whether the input DICOM series UID is among those found by GDCM.
            // If so, then use the input UID, otherwise error out.
            if ( std::count( std::begin( foundDicomSeriesUIDs ),
                             std::end( foundDicomSeriesUIDs ),
                             *inputDicomSeriesUID ) > 0 )
            {
                selectedSeriesUID = *inputDicomSeriesUID;
            }
            else
            {
                std::ostringstream ss;
                ss << "Input DICOM series UID "
                   << *inputDicomSeriesUID << " is invalid " << std::ends;
                std::cerr << ss.str() << std::endl;
                return EMPTY;
            }
        }
        else if ( ! foundDicomSeriesUIDs.empty() )
        {
            // No series UID was input, but GDCM has found a set of UIDs.
            // It is common to find multiple DICOM series in the same directory,
            // so select the specific series that is to be read in.

            /// @note Currently defaulting to the use the first series UID found during
            /// exploration of the directory. The UI will allow the user to pick which ever
            /// series they like.

            std::cout << "> Available DICOM series UIDs in this directory:" << std::endl;
            for ( const auto& uid : foundDicomSeriesUIDs )
            {
                std::cout << uid << std::endl;
            }
            std::cout << std::endl;

            selectedSeriesUID = foundDicomSeriesUIDs[0];
        }
        else
        {
            std::cerr << "No DICOM series found in directory." << std::endl;
            return EMPTY;
        }

        std::cout << "> Selected series UID: " << selectedSeriesUID << std::endl << std::endl;

        fileNames = nameGenerator->GetFileNames( selectedSeriesUID );

        if ( fileNames.empty() )
        {
            std::cerr << "Directory does not contain a DICOM series with UID '"
                      << selectedSeriesUID << "'" << std::endl;
            return EMPTY;
        }

        isDicom = true;
    }
    else
    {
        return EMPTY;
    }

    return std::make_pair( fileNames, isDicom );
}

} // anonymous namespace


namespace imageio
{

ImageLoader::ImageLoader( const ComponentTypeCastPolicy& castPolicy )
    : m_imageDataFactory( std::make_unique<ImageDataFactory>( castPolicy ) )
{}

ImageLoader::~ImageLoader() = default;


std::unique_ptr<ImageCpuRecord> ImageLoader::load(
        const std::string& inputFileName,
        const boost::optional<std::string>& inputDicomSeriesUID,
        const ComponentNormalizationPolicy& normalizationPolicy ) const
{
    std::vector< std::string > inputFileNames;
    bool isDicom;

    std::tie( inputFileNames, isDicom ) =
            getImageFileNames( inputFileName, inputDicomSeriesUID );

    if ( inputFileNames.empty() )
    {
        return nullptr;
    }

    return ( doLoadFiles( inputFileNames, isDicom, normalizationPolicy ) );
}


std::unique_ptr<ParcellationCpuRecord> ImageLoader::generateClearParcellationRecord(
        const ImageCpuRecord* sourceRecord ) const
{
    if ( ! sourceRecord )
    {
        return nullptr;
    }

    // Get ImageIOInfo from source record. Modify it to match the clear label image.
    // ioInfo.m_spaceInfo is unchanged.
    itkdetails::io::ImageIoInfo ioInfo = sourceRecord->imageBaseData()->imageIOInfo();

    // Remove file information, since this image is not loaded from a file.
    ioInfo.m_fileInfo = itkdetails::io::FileInfo();
    ioInfo.m_fileInfo.m_fileType = ::itk::ImageIOBase::FileType::TypeNotApplicable;
    ioInfo.m_metaData.clear();

    // Force uint8_t component type
    ioInfo.m_componentInfo.m_componentType = ::itk::ImageIOBase::IOComponentType::UCHAR;
    ioInfo.m_componentInfo.m_componentTypeString = itk::ImageIOBase::GetComponentTypeAsString(
                ioInfo.m_componentInfo.m_componentType );
    ioInfo.m_componentInfo.m_componentSizeInBytes = 1;

    ioInfo.m_pixelInfo.m_pixelType = ::itk::ImageIOBase::IOPixelType::SCALAR;
    ioInfo.m_pixelInfo.m_pixelTypeString = itk::ImageIOBase::GetPixelTypeAsString(
                ioInfo.m_pixelInfo.m_pixelType );
    ioInfo.m_pixelInfo.m_numComponents = 1;
    ioInfo.m_pixelInfo.m_pixelStrideInBytes = 1;

    ioInfo.m_sizeInfo.m_imageSizeInComponents = ioInfo.m_sizeInfo.m_imageSizeInPixels;

    // Each pixel is one byte, so size in bytes and pixels are equivalent:
    ioInfo.m_sizeInfo.m_imageSizeInBytes = ioInfo.m_sizeInfo.m_imageSizeInPixels;


    // All pixels of the uint8_t image are 0:
    static constexpr uint8_t sk_pixelValue = 0;

    auto imageData = std::make_unique< ::itkdetails::ImageData<uint8_t> >(
                sourceRecord->imageBaseData()->imageIOInfo(), sk_pixelValue );

    ImageHeader header;

    if ( ! itkbridge::createImageHeader(
             ioInfo,
             std::bind( &ImageDataFactory::getComponentTypeCast,
                        m_imageDataFactory.get(), std::placeholders::_1 ),
             header ) )
    {
        return nullptr;
    }


    ImageTransformations tx( header.m_pixelDimensions, header.m_spacing,
                             header.m_origin, header.m_directions,
                             sk_origin, sk_ident );

    ImageSettings settings(
                "None", // Name of the clear parcellation
                imageData->pixelStatistics(),
                header.m_bufferComponentType,
                ImageSettings::InterpolationMode::NearestNeighbor );

    ImageCpuRecord imageCpuRecord( std::move( imageData ), std::move( header ),
                                   std::move( settings ), std::move( tx ) );

    // The only value in the clear label image is 0:
    std::vector<int64_t> labelValues{ 0 };

    return std::make_unique< ParcellationCpuRecord >(
                std::move( imageCpuRecord ), std::move( labelValues ) );
}


std::unique_ptr<ImageCpuRecord> ImageLoader::doLoadFiles(
        const std::vector< std::string >& fileNames,
        bool isDicom,
        const ComponentNormalizationPolicy& normalizationPolicy ) const
{
    if ( ! m_imageDataFactory )
    {
        return nullptr;
    }

    if ( fileNames.empty() )
    {
        return nullptr;
    }

    auto componentType = itkbridge::sniffComponentType( fileNames[0].c_str() );

    if ( ! componentType )
    {
        std::cerr << "Unable to read image with undefined pixel component type." << std::endl;
        return nullptr;
    }

    if ( ComponentNormalizationPolicy::SignedNormalizedFloating == normalizationPolicy ||
         ComponentNormalizationPolicy::UnsignedNormalizedFloating == normalizationPolicy )
    {
        // Force cast to Float32
        componentType = ComponentType::Float32;
    }

    /// @internal The actual \c ImageData object for the pixel component type is
    /// created by the factory
    auto imageBaseData = m_imageDataFactory->createImageData( *componentType, false );

    if ( ! imageBaseData )
    {
        std::cerr << "Factory unable to create ImageData." << std::endl;
        return nullptr;
    }

    /// @internal Load the image data
    const bool isLoaded = ( isDicom )
            ? imageBaseData->loadFromDicomSeries( fileNames, normalizationPolicy )
            : imageBaseData->loadFromImageFile( fileNames[0], normalizationPolicy );

    if ( ! isLoaded )
    {
        std::cerr << "Error while loading image." << std::endl;
        return nullptr;
    }

    ImageHeader header;

    if ( ! itkbridge::createImageHeader(
             imageBaseData->imageIOInfo(),
             std::bind( &ImageDataFactory::getComponentTypeCast,
                        m_imageDataFactory.get(), std::placeholders::_1 ),
             header ) )
    {
        std::cerr << "Error while creating ImageInfo." << std::endl;
        return nullptr;
    }

    try
    {
        ImageTransformations tx( header.m_pixelDimensions, header.m_spacing,
                                 header.m_origin, header.m_directions,
                                 sk_origin, sk_ident );

        // Use stem of first image filename as its "display name"
        const boost::filesystem::path path( fileNames[0] );

        const auto interpMode = ( isFloatingType( *componentType ) )
                ? ImageSettings::InterpolationMode::Linear
                : ImageSettings::InterpolationMode::NearestNeighbor;

        // Remove all extensions to create image's display name
        auto baseName = path.stem();
        while ( baseName != baseName.stem() )
        {
            baseName = baseName.stem();
        }

        ImageSettings settings(
                    baseName.string(),
                    imageBaseData->pixelStatistics(),
                    header.m_bufferComponentType,
                    interpMode );

        return std::make_unique<ImageCpuRecord>(
                    std::move( imageBaseData ),
                    std::move( header ),
                    std::move( settings ),
                    std::move( tx ) );
    }
    catch ( const std::exception& e )
    {
        std::cerr << "Error while creating ImageRecord: " << e.what() << std::endl;
        return nullptr;
    }
    catch ( ... )
    {
        std::cerr << "Error while creating ImageRecord." << std::endl;
        return nullptr;
    }
}

} // namespace imageio
