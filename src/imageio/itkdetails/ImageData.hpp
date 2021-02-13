#ifndef IMAGEIO_IMAGE_DATA_H
#define IMAGEIO_IMAGE_DATA_H

#include "itkdetails/ImageBaseData.hpp"

#include <itkImage.h>
#include <itkVectorImage.h>

#include <vector>


namespace itkdetails
{

namespace utility
{
template< class ImageType >
struct PixelStatistics;
}


/**
 * @note Data of multi-component (vector) images are cloned after being loaded:
 * one copy pointed to by base class' \c m_imageBasePtr;
 * the other copy pointed to by this class' \c m_splitImagePtrs
 */
template< typename ComponentType >
class ImageData : public ImageBaseData
{
public:

    explicit ImageData();

    /**
     * @brief Construct with vector of split image pointers and IO information structure
     * @param splitImagePtrs Split image pointers
     * @param ioInfo IO information structure
     */
    ImageData( std::vector< typename image3d::ImageType< ComponentType >::Pointer > splitImagePtrs,
               io::ImageIoInfo ioInfo );

    /**
     * @brief Constructo with IO information structure and default image pixel component value
     * @param ioInfo IO information structure
     * @param defaultValue Default pixel component value
     */
    ImageData( const io::ImageIoInfo& ioInfo,
               const ComponentType& defaultValue );

    ImageData( const ImageData& ) = default;
    ImageData& operator=( const ImageData& ) = default;

    ImageData( ImageData&& ) = default;
    ImageData& operator=( ImageData&& ) = default;

    ~ImageData() override = default;

    bool loadFromImageFile(
            const std::string& fileName,
            const imageio::ComponentNormalizationPolicy& normalizationPolicy ) override;

    bool loadFromDicomSeries(
            const std::vector< std::string >& fileNames,
            const imageio::ComponentNormalizationPolicy& normalizationPolicy ) override;

    const uint8_t* bufferPointer() const override;

    const uint8_t* bufferPointer( const uint32_t componentIndex ) const override;

    vtkSmartPointer< vtkImageData >
    asVTKImageData( const uint32_t componentIndex ) const override;

    std::vector< vtkSmartPointer< vtkImageData > > asVTKImageData() const override;

    bool getPixelAsDouble(
            const uint32_t componentIndex,
            uint32_t i, uint32_t j, uint32_t k, double& value ) const override;

    std::vector< typename image3d::ImageType< ComponentType >::Pointer >
    asSplitITKImage() const;

    typename image3d::ImageType< ComponentType >::Pointer
    asITKImage() const;

    typename image3d::VectorImageType< ComponentType >::Pointer
    asITKVectorImage() const;


private:

    bool splitImageIntoComponents();
    bool computePixelStatistics();
    bool setup();

    /// Image split into vector of \c itkImage pointers
    std::vector< typename image3d::ImageType< ComponentType >::Pointer > m_splitImagePtrs;
};

} // namespace itkdetails


#include "itkdetails/ImageData.tpp"

#endif // IMAGEIO_IMAGE_DATA_H
