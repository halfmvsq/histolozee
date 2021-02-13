#ifndef IMAGEIO_IMAGE_BASE_DATA_H
#define IMAGEIO_IMAGE_BASE_DATA_H

#include "itkdetails/ImageIOInfo.hpp"
#include "itkdetails/ImageTypes.hpp"
#include "itkdetails/ImageUtility.hpp"

#include "HZeeTypes.hpp"

#include <itkImageBase.h>

#include <vtkImageData.h>
#include <vtkSmartPointer.h>

#include <boost/optional.hpp>
#include <boost/range/any_range.hpp>

#include <string>
#include <vector>


namespace itkdetails
{

/**
 * @brief Base class of ITK image wrapper.
 * When it loads images, \c m_imageBasePtr is a pointer to a child ImageData object
 * templated over the input component type.
 *
 * This base/derived pair (ImageBaseData/ImageData) was created so that we could
 * use a factory function up higher...
 */
class ImageBaseData
{
private:

    /// @todo put in another header
    using statistics_range_t = boost::any_range<
        utility::PixelStatistics<double>,
        boost::forward_traversal_tag,
        utility::PixelStatistics<double>&,
        std::ptrdiff_t >;


public:

    explicit ImageBaseData();

    explicit ImageBaseData( io::ImageIoInfo );

    ImageBaseData( const ImageBaseData& ) = default;
    ImageBaseData& operator=( const ImageBaseData& ) = default;

    ImageBaseData( ImageBaseData&& ) = default;
    ImageBaseData& operator=( ImageBaseData&& ) = default;

    virtual ~ImageBaseData() = default;


    virtual bool loadFromImageFile(
            const std::string& fileName,
            const imageio::ComponentNormalizationPolicy& normalizationPolicy ) = 0;

    virtual bool loadFromDicomSeries(
            const std::vector< std::string >& fileNames,
            const imageio::ComponentNormalizationPolicy& normalizationPolicy ) = 0;

    virtual const uint8_t* bufferPointer() const = 0;

    virtual const uint8_t* bufferPointer( const uint32_t componentIndex ) const = 0;

    virtual vtkSmartPointer< vtkImageData >
    asVTKImageData( const uint32_t componentIndex ) const = 0;

    virtual std::vector< vtkSmartPointer< vtkImageData > >
    asVTKImageData() const = 0;

    virtual bool getPixelAsDouble(
            const uint32_t componentIndex,
            uint32_t i, uint32_t j, uint32_t k, double& value ) const = 0;

    const io::ImageIoInfo& imageIOInfo() const;

    statistics_range_t pixelStatistics() const;

    boost::optional< utility::PixelStatistics<double> >
    pixelStatistics( uint32_t componentIndex ) const;

    bool isVectorImage() const;

    typename image3d::ImageBaseType::Pointer
    imageBase() const;

    /**
     * @brief numPixels
     * @return unsigned long int (uint64_t)
     */
    typename image3d::ImageRegionType::SizeValueType
    numPixels() const;

    bool isFullyBuffered() const;

    /** Get the index (discrete) of a voxel from a physical point.
     * Floating point index results are rounded to integers
     * Returns true if the resulting index is within the image, false otherwise
     * \sa Transform */
    bool transformPhysicalPointToIndex(
            const image3d::PointType& point,
            image3d::IndexType& index ) const;

    /** \brief Get the continuous index from a physical point
     *
     * Returns true if the resulting index is within the image, false otherwise.
     * \sa Transform */
    bool transformPhysicalPointToContinuousIndex(
            const image3d::PointType& point,
            image3d::ContinuousIndexType& index ) const;

    /** Get a physical point (in the space which
     * the origin and spacing information comes from)
     * from a discrete index (in the index space)
     *
     * \sa Transform */
    bool transformIndexToPhysicalPoint(
            const image3d::IndexType& index,
            image3d::PointType& point ) const;

    bool transformContinuousIndexToPhysicalPoint(
            const image3d::ContinuousIndexType& index,
            image3d::PointType& point ) const;


protected:

    io::ImageIoInfo m_imageIOInfo;

    std::vector< utility::PixelStatistics<double> > m_pixelStatistics;

    /// Store base pointer to the image, which can be either an
    /// \c itkImage or an \c itkVectorImage
    image3d::ImageBaseType::Pointer m_imageBasePtr;
};

} // namespace itkdetails

#endif // IMAGEIO_IMAGE_BASE_DATA_H
