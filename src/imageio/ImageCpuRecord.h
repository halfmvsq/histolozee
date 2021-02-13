#ifndef IMAGEIO_IMAGE_CPU_RECORD_H
#define IMAGEIO_IMAGE_CPU_RECORD_H

#include "ImageHeader.h"
#include "ImageSettings.h"
#include "ImageTransformations.hpp"

#include "itkdetails/ImageBaseData.hpp"

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>


namespace imageio
{

/**
 * @brief Record of an image stored in this class: it consists of the
 * image data itself, the header information, and associated spatial transformations.
 *
 * @note Due to unique data ownership, image records cannot be copied
 *
 * @todo ImageCpuRecord and ParcellationCpuRecord do NOT belong in the imageio library.
 */
class ImageCpuRecord
{
public:

    /**
     * @brief Construct a record from existing image data, information, and
     * transformation objects
     *
     * @param[in] data Unique pointer to the data object: The pointer to the
     * data object will be moved and uniquely owned by this record.
     * @param[in] Header information object
     * @param[in] Settings information object
     * @param[in] Image spatial transformations object
     */
    ImageCpuRecord( std::unique_ptr< ::itkdetails::ImageBaseData >,
                    ImageHeader,
                    ImageSettings,
                    ImageTransformations );

    ImageCpuRecord( const ImageCpuRecord& ) = delete;
    ImageCpuRecord& operator=( const ImageCpuRecord& ) = delete;

    ImageCpuRecord( ImageCpuRecord&& ) = default;
    ImageCpuRecord& operator=( ImageCpuRecord&& ) = default;

    virtual ~ImageCpuRecord() = default;

    const ::itkdetails::ImageBaseData* imageBaseData() const;

    /// Get raw pointer to the pixel buffer of the whole image
    const uint8_t* buffer() const;

    /// Get raw pointer to the pixel buffer of a given component of the image
    const uint8_t* buffer( uint32_t componentIndex ) const;

    /**
     * @brief Get a single pixel value. The function returns false if a pixel
     * index outside the image was requested.
     *
     * @param[in] componentIndex Image component
     * @param[in] pixelIndex Pixel index in image matrix
     * @param[out] value Pixel value cast to double-precision floating point
     *
     * @return True iff the pixel value was retrieved successfully
     */
    bool pixelValue( uint32_t componentIndex, const glm::uvec3& pixelIndex, double& value ) const;

    const ImageHeader& header() const;
    const ImageSettings& settings() const;
    const ImageTransformations& transformations() const;

    /// Set display name
    void setDisplayName( std::string name );

    /// Set opacity
    void setOpacity( uint32_t component, double opacity );

    /// Set window width
    void setWindowWidth( uint32_t component, double window );

    /// Set level (i.e. window center)
    void setLevel( uint32_t component, double level );

    /// Set low threshold
    void setThresholdLow( uint32_t component, double thresh );

    /// Set high threshold
    void setThresholdHigh( uint32_t component, double thresh );

    /// Set interpolation mode
    void setInterpolationMode( uint32_t component, const ImageSettings::InterpolationMode& mode );

    /// Set origin of Subject in World space
    void setWorldSubjectOrigin( glm::vec3 worldSubjectOrigin );

    /// Set rotation from Subject to World space
    void setSubjectToWorldRotation( glm::quat world_O_subject_rotation );

    /// Reset Subject to World transformation to identity
    void resetSubjectToWorld();


private:

    std::unique_ptr< ::itkdetails::ImageBaseData > m_data; //!< Image data
    ImageHeader m_header; //!< Image header
    ImageSettings m_settings; //!< Image settings
    ImageTransformations m_transformations; //!< Image transformations

    /// @todo also hold ImageDicomInfo in here for dicom images
};

} // namespace imageio

#endif // IMAGEIO_IMAGE_CPU_RECORD_H
