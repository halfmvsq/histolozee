#ifndef CREATE_PARCELLATION_IMAGE_H
#define CREATE_PARCELLATION_IMAGE_H

#include "ImageCpuRecord.h"
#include "ParcellationCpuRecord.h"

#include "itkdetails/ImageTypes.hpp"
#include "itkdetails/ImageData.hpp"
#include "itkbridge/ITKBridge.hpp"

#include <itkImage.h>
#include <itkImportImageFilter.h>

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>


namespace
{

// HistoloZee works with 3D images:
static constexpr unsigned int Dim = 3;

static constexpr bool importImageFilterWillOwnTheBuffer = false;


/**
 * @brief Helper function to update ImageIOInfo object with a new component type
 * @param[in] oldInfo Object to update
 * @param[in] newComponentType New component type
 * @param[in] newComponentSizeInBytes Size of new component in bytes
 * @return Updated object
 */
itkdetails::io::ImageIoInfo updateImageIOInfo(
        const itkdetails::io::ImageIoInfo& oldInfo,
        const ::itk::ImageIOBase::IOComponentType& newComponentType,
        uint32_t newComponentSizeInBytes )
{
    itkdetails::io::ImageIoInfo newInfo = oldInfo;

    newInfo.m_componentInfo.m_componentType = newComponentType;
    newInfo.m_componentInfo.m_componentSizeInBytes = newComponentSizeInBytes;

    newInfo.m_componentInfo.m_componentTypeString =
            itk::ImageIOBase::GetComponentTypeAsString(
                newInfo.m_componentInfo.m_componentType );

    newInfo.m_pixelInfo.m_pixelStrideInBytes =
            newInfo.m_componentInfo.m_componentSizeInBytes *
            newInfo.m_pixelInfo.m_numComponents;

    newInfo.m_sizeInfo.m_imageSizeInBytes =
            newInfo.m_sizeInfo.m_imageSizeInPixels *
            newInfo.m_pixelInfo.m_pixelStrideInBytes;

    return newInfo;
}


/**
 * @brief Template function to create a new ITK image from a buffer of 'old' pixel values.
 * A hash map is provided to convert 'old' pixel values to 'new' pixel values. The spatial
 * information of the new ITK image is provided as input.
 *
 * @param[in] oldBuffer Raw pointer to old pixel buffer
 * @param[in] newIoInfo ImageIoInfo object for new image
 * @param[in] newRegion Region for new image
 * @param[in] newOrigin Origin for new image
 * @param[in] newSpacing Pixel spacing for new image
 * @param[in] newDirections Direction vectors for new image
 * @param[in] oldToNewPixelValueMap Hash map from old to new image pixel values
 *
 * @return Unique pointer to new image
 */
template< typename OldPixelType, typename NewPixelType >
std::unique_ptr< ::itkdetails::ImageBaseData > convertOldBufferToNewImage(
        const OldPixelType* oldBuffer,
        const ::itkdetails::io::ImageIoInfo& newIoInfo,
        const itk::ImageBase<3>::RegionType& newRegion,
        const itk::ImageBase<3>::PointType& newOrigin,
        const itk::ImageBase<3>::SpacingType& newSpacing,
        const itk::ImageBase<3>::DirectionType& newDirections,
        const std::unordered_map< OldPixelType, size_t >& oldToNewPixelValueMap )
{
    if ( ! oldBuffer )
    {
        return nullptr;
    }

    const size_t numTotalPixels = newRegion.GetNumberOfPixels();

    // Create new image buffer with size matching old image buffer.
    // Pixels values in old buffer are converted to new values and cast to new type.
    auto newBuffer = std::make_unique< NewPixelType[] >( numTotalPixels );
    for ( size_t i = 0; i < numTotalPixels; ++i )
    {
        newBuffer[i] = static_cast<NewPixelType>( oldToNewPixelValueMap.at( oldBuffer[i] ) );

//        std::cout << int(oldBuffer[i]) << " --> " << int(newBuffer[i]) << std::endl;
    }

    // Create new ITK image holding new buffer and matching old image's spatial information
    using ImportFilterType = itk::ImportImageFilter< NewPixelType, itkdetails::image3d::NDIM >;
    typename ImportFilterType::Pointer importFilter = ImportFilterType::New();
    importFilter->SetRegion( newRegion );
    importFilter->SetOrigin( newOrigin );
    importFilter->SetSpacing( newSpacing );
    importFilter->SetDirection( newDirections );
    importFilter->SetImportPointer( newBuffer.get(), numTotalPixels, importImageFilterWillOwnTheBuffer );
    importFilter->Update();

    // Create vector with single component
    std::vector< typename ::itkdetails::image3d::ImageType<NewPixelType>::Pointer > componentImages;
    componentImages.push_back( importFilter->GetOutput() );

    // Package into ImageData and cast to ImageBaseData
    return std::make_unique< itkdetails::ImageData<NewPixelType> >(
                std::move( componentImages ), newIoInfo );
}


/**
 * @brief Given an image buffer of integer pixels, this template function creates new image with
 * pixel values that are indices into the original image buffer. The new image pixels index
 * the old image pixels in increasing order, with one exception: Index 0 is forced to map to
 * value 0, even if value 0 is not present in the input image.
 *
 * The new image is a "squashed" version of the original that hopefully requires a smaller number of
 * bytes per pixel for storage. The returned image uses the least number of storage bytes per pixel.
 *
 * @note The intent of this function is to squash a parcellation image (with a potentially large
 * range of label values) into a new image that takes less space.
 *
 * @note Index 0 is forced to map to value 0 due to the special meaning of label 0 as "background" /
 * "no label". We wanted the index 0 to preserve this special meaning.
 *
 * @param[in] buffer Input image buffer
 * @param[in] ioInfo Input image's ImageIoInfo object
 * @param[in] region Input image region
 * @param[in] origin Input image origin
 * @param[in] spacing Input image pixel spacing
 * @param[in] directions Input image direction vectors
 *
 * @return Pair consisting of the new image of indices and a vector that maps the new
 * image's pixel values (the "indices") to old image pixel values.
 */
template< typename LabelValueType >
std::pair< std::unique_ptr< itkdetails::ImageBaseData >, std::vector<int64_t> >
createSquashedImage(
        const LabelValueType* buffer,
        const itkdetails::io::ImageIoInfo& ioInfo,
        const itkdetails::image3d::ImageBaseType::RegionType& region,
        const itkdetails::image3d::ImageBaseType::PointType& origin,
        const itkdetails::image3d::ImageBaseType::SpacingType& spacing,
        const itkdetails::image3d::ImageBaseType::DirectionType& directions )
{
    // Hard limit on the maximum number of unique labels allowed:
    // (Note: this means that the parcellation should fit into a ushort image.)
    static constexpr size_t sk_maxNumLabels = ( 1 << 16 );

    const size_t numTotalPixels = region.GetNumberOfPixels();

    // Set of all unique pixel (parcellation label) values:
    std::set<LabelValueType> labels;
    for ( size_t i = 0; i < numTotalPixels; ++i )
    {
        labels.insert( buffer[i] );
    }

    // Create sorted vector of all unique label values:
    std::vector<LabelValueType> sortedLabels( std::begin( labels ), std::end( labels ) );
    std::sort( std::begin( sortedLabels ), std::end( sortedLabels ) );

    const size_t numUniqueLabels = sortedLabels.size();
    if ( numUniqueLabels >= sk_maxNumLabels )
    {
        std::cerr << "Number of unique parcellation label values (" << numUniqueLabels
                  << ") exceeds the maximum number allowed (" << sk_maxNumLabels << ")" << std::endl;

        return { nullptr, std::vector<int64_t>{} };
    }

    // Map from label value to label index:
    std::unordered_map< LabelValueType, size_t > labelToIndex;

    // Vector from label index to label value.
    // Element i of this map is the i'th sorted label value.
    std::vector<int64_t> indexToLabel;

    std::cout << "\nRemapping parcellation label values:" << std::endl;
    std::cout << "Index\tValue" << std::endl;

    // Explicitly associate label index 0 with label value 0:
    size_t index = 0;
    labelToIndex.insert( { 0, index } );
    indexToLabel.push_back( 0 );
    ++index;

    std::cout << "0\t0" << std::endl;

    static constexpr int64_t sk_lowInt64 = std::numeric_limits<int64_t>::lowest();
    static constexpr int64_t sk_maxInt64 = std::numeric_limits<int64_t>::max();

    for ( const LabelValueType labelValue : sortedLabels )
    {
        // Skip case of label value 0, since it was taken care of above:
        if ( 0 == labelValue )
        {
            continue;
        }

        int64_t labelValueClipped; // Label value, clipped to int64_t range

        // Replace labels that exceed the int64_t range with 0 (blank label)
        if ( std::is_signed<LabelValueType>::value &&
             ( labelValue < sk_lowInt64 || sk_maxInt64 < labelValue ) )
        {
            labelValueClipped = 0;
        }
        else if ( sk_maxInt64 < labelValue )
        {
            labelValueClipped = 0;
        }
        else
        {
            labelValueClipped = static_cast<int64_t>( labelValue );
        }

        // Map label value to index:
        labelToIndex.insert( { labelValue, index } );

        // Copy labels into vector, casting old types to int64_t:
        indexToLabel.push_back( labelValueClipped );

        std::cout << index << "\t" << labelValueClipped << std::endl;

        ++index;
    }
    std::cout << std::endl;

    // Number of unique label values:
    const size_t numLabels = indexToLabel.size();

    if ( numLabels != numUniqueLabels )
    {
        std::cerr << "Error has occurred while squashing parcellation image." << std::endl;
        return { nullptr, std::vector<int64_t>{} };
    }

    // Create new ITK image with least number of bytes per pixel in order to represent the new indices:
    if ( numLabels <= std::numeric_limits<uint8_t>::max() + 1 )
    {
        // Allocate uint8_t indices
        const auto newIoInfo = updateImageIOInfo( ioInfo, ::itk::ImageIOBase::IOComponentType::UCHAR, 1 );

        return { convertOldBufferToNewImage< LabelValueType, uint8_t >(
                        buffer, newIoInfo, region, origin, spacing, directions, labelToIndex ),
                    indexToLabel };
    }
    else if ( numLabels <= std::numeric_limits<uint16_t>::max() + 1 )
    {
        // Allocate uint16_t indices
        const auto newIoInfo = updateImageIOInfo( ioInfo, ::itk::ImageIOBase::IOComponentType::USHORT, 2 );

        return { convertOldBufferToNewImage< LabelValueType, uint16_t >(
                        buffer, newIoInfo, region, origin, spacing, directions, labelToIndex ),
                    indexToLabel };
    }
    else if ( numLabels <= std::numeric_limits<uint32_t>::max() + 1 )
    {
        // Note: this case should not happen based on max allowable number of labels!
        // Allocate uint32_t indices
        const auto newIoInfo = updateImageIOInfo( ioInfo, ::itk::ImageIOBase::IOComponentType::UINT, 4 );

        return { convertOldBufferToNewImage< LabelValueType, uint32_t >(
                        buffer, newIoInfo, region, origin, spacing, directions, labelToIndex ),
                    indexToLabel };
    }
    else if ( numLabels <= std::numeric_limits<uint64_t>::max() + 1 )
    {
        // Note: this case should not happen based on max allowable number of labels!
        // Allocate uint64_t indices
        const auto newIoInfo = updateImageIOInfo( ioInfo, ::itk::ImageIOBase::IOComponentType::ULONG, 8 );

        return { convertOldBufferToNewImage< LabelValueType, uint64_t >(
                        buffer, newIoInfo, region, origin, spacing, directions, labelToIndex ),
                    indexToLabel };
    }
    else
    {
        // Outside permitted range: return null pointer instead of image,
        // as well as empty mapping from indices to values.
        return { nullptr, std::vector<int64_t>{} };
    }
}

} // anonymous


namespace imageio
{

/**
 * @brief Given the CPU record of an input (integer scalar) 3D image, this function
 * outputs a CPU record that represents it as a new entity called a "parcellation" image.
 * The values in the output parcellation image are indices into the original image values
 * (i.e. the original label values are remapped to indices). The purpose of this remapping is to
 * "compress" the label values, so that the output parcellation image can use a smaller data type
 * to represent than the original image. This is especially useful if the original parcellation
 * image contains label values separated by large gaps.
 *
 * @note The output parcellation image contains a vector that maps indices into the original
 * parcellation image values. It is guaranteed that label index 0 maps to label value 0,
 * which always denotes the background label.
 *
 * @param[in] cpuRecord Input image with pixel components of integer type
 *
 * @return The parcellation image CPU record
 */
std::unique_ptr< ParcellationCpuRecord >
createParcellationCpuRecord( const ImageCpuRecord& cpuRecord )
{
    using namespace itkdetails::image3d;

    if ( cpuRecord.header().m_numComponents > 1 ||
         PixelType::Scalar != cpuRecord.header().m_pixelType ||
         3 != cpuRecord.header().m_numDimensions )
    {
        // Only accept 3D scalar, integer images
        return nullptr;
    }

    const auto imageBaseData = cpuRecord.imageBaseData();
    if ( ! imageBaseData )
    {
        return nullptr;
    }

    const auto imageBase = imageBaseData->imageBase();
    if ( ! imageBase )
    {
        return nullptr;
    }

    // The old image's raw pixel buffer and I/O information structure
    const uint8_t* oldBuffer = imageBaseData->bufferPointer();
    const itkdetails::io::ImageIoInfo oldIoInfo = imageBaseData->imageIOInfo();

    // Spatial information for the old image
    const ImageBaseType::RegionType region = imageBase->GetLargestPossibleRegion();
    const ImageBaseType::PointType origin = imageBase->GetOrigin();
    const ImageBaseType::SpacingType spacing = imageBase->GetSpacing();
    const ImageBaseType::DirectionType directions = imageBase->GetDirection();

    // Pointer to ImageBaseData of the new image to be created
    std::unique_ptr< itkdetails::ImageBaseData > newImageBaseData = nullptr;

    // Mapping from new to old pixel values. The "new" pixel values are indices.
    std::vector< int64_t > newToOldPixelValuesVector;

    // Cast the old buffer to the appropriate type
    switch ( cpuRecord.header().m_bufferComponentType )
    {
    case ComponentType::Int8 :
    {
        std::tie( newImageBaseData, newToOldPixelValuesVector ) =
                createSquashedImage( reinterpret_cast<const int8_t*>( oldBuffer ),
                                     oldIoInfo, region, origin, spacing, directions );
        break;
    }
    case ComponentType::UInt8 :
    {
        std::tie( newImageBaseData, newToOldPixelValuesVector ) =
                createSquashedImage( reinterpret_cast<const uint8_t*>( oldBuffer ),
                                     oldIoInfo, region, origin, spacing, directions );
        break;
    }
    case ComponentType::Int16 :
    {
        std::tie( newImageBaseData, newToOldPixelValuesVector ) =
                createSquashedImage( reinterpret_cast<const int16_t*>( oldBuffer ),
                                     oldIoInfo, region, origin, spacing, directions );
        break;
    }
    case ComponentType::UInt16 :
    {
        std::tie( newImageBaseData, newToOldPixelValuesVector ) =
                createSquashedImage( reinterpret_cast<const uint16_t*>( oldBuffer ),
                                     oldIoInfo, region, origin, spacing, directions );
        break;
    }
    case ComponentType::Int32 :
    {
        std::tie( newImageBaseData, newToOldPixelValuesVector ) =
                createSquashedImage( reinterpret_cast<const int32_t*>( oldBuffer ),
                                     oldIoInfo, region, origin, spacing, directions );
        break;
    }
    case ComponentType::UInt32 :
    {
        std::tie( newImageBaseData, newToOldPixelValuesVector ) =
                createSquashedImage( reinterpret_cast<const uint32_t*>( oldBuffer ),
                                     oldIoInfo, region, origin, spacing, directions );
        break;
    }
    case ComponentType::Int64 :
    {
        std::tie( newImageBaseData, newToOldPixelValuesVector ) =
                createSquashedImage( reinterpret_cast<const int64_t*>( oldBuffer ),
                                     oldIoInfo, region, origin, spacing, directions );
        break;
    }
    case ComponentType::UInt64 :
    {
        std::tie( newImageBaseData, newToOldPixelValuesVector ) =
                createSquashedImage( reinterpret_cast<const uint64_t*>( oldBuffer ),
                                     oldIoInfo, region, origin, spacing, directions );
        break;
    }
    case ComponentType::Float32 :
    {
        // Invalid data type for label image pixels
        return nullptr;
    }
    case ComponentType::Double64 :
    {
        // Invalid data type for label image pixels
        return nullptr;
    }
    }

    if ( ! newImageBaseData )
    {
        // Something failed
        return nullptr;
    }

    const auto componentType = newImageBaseData->imageIOInfo().m_componentInfo.m_componentType;
    if ( ::itk::ImageIOBase::IOComponentType::UCHAR != componentType &&
         ::itk::ImageIOBase::IOComponentType::USHORT != componentType &&
         ::itk::ImageIOBase::IOComponentType::UINT != componentType )
    {
        std::cerr << "Only 8, 16, and 32-bit unsigned integer images are acceptable as parcellation images." << std::endl;
        return nullptr;
    }

    // Convert component type to HZee's format
    auto newHzeeComponentType = itkbridge::fromITKComponentType( componentType );
    if ( ! newHzeeComponentType )
    {
        // Invalid component type
        return nullptr;
    }

    // Create new header and setting objects for the new parcellation image
    auto newHeader = cpuRecord.header();
    auto newSettings = cpuRecord.settings();

    // Update the new header with the new component type information.
    // The newly created image was (obviously) not loaded from a file,
    // so its "fileComponentType" matches its "bufferComponentType".
    bool success = itkbridge::createImageHeader(
                newImageBaseData->imageIOInfo(),
                *newHzeeComponentType, // file component type
                *newHzeeComponentType, // buffer component type
                newHeader );

    if ( ! success )
    {
        return nullptr;
    }

    ImageCpuRecord newImageCpuRecord(
                std::move( newImageBaseData ),
                newHeader, newSettings, cpuRecord.transformations() );

    return std::make_unique<ParcellationCpuRecord>(
                std::move( newImageCpuRecord ), newToOldPixelValuesVector );
}

} // namespace imageio

#endif // CREATE_PARCELLATION_IMAGE_H
