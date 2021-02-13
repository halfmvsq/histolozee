#ifndef IMAGEIO_IMAGE_HEADER_H
#define IMAGEIO_IMAGE_HEADER_H

#include "HZeeTypes.hpp"

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_precision.hpp>

#include <array>
#include <string>
#include <utility>


namespace imageio
{

/**
 * @brief Static image header with data set upon creation or loading of image.
 */
class ImageHeader
{
public:

    explicit ImageHeader();

    ImageHeader( const ImageHeader& ) = default;
    ImageHeader& operator=( const ImageHeader& ) = default;

    ImageHeader( ImageHeader&& ) = default;
    ImageHeader& operator=( ImageHeader&& ) = default;

    ~ImageHeader() = default;

    /**
     * @brief Validate the image header data.
     * @param setDefaultsIfInvalid If true, set default values for invalid header data.
     * @return True iff the header data are valid.
     */
    bool validate( bool setDefaultsIfInvalid );


    /// Image file name on disk
    std::string m_fileName;

    /// Image component type, as stored in file on disk
    ComponentType m_componentType;

    /// Image component type as a string, as stored in file on disk
    std::string m_componentTypeString;

    /// Size of image component, as stored in file on disk
    uint32_t m_componentSizeInBytes;

    /// Image size in bytes, as stored in file on disk
    size_t m_imageSizeInBytes;

    /// Image component type, as stored in the application buffer in memory
    ComponentType m_bufferComponentType;

    /// Image component type string, as stored in the application buffer in memory
    std::string m_bufferComponentTypeString;

    /// Size of image component, as stored in the application buffer in memory
    uint32_t m_bufferComponentSizeInBytes;

    /// Image size in bytes, as stored in the application buffer in memory
    size_t m_bufferSizeInBytes;


    /// Image pixel type
    PixelType m_pixelType;

    /// Image pixel type string
    std::string m_pixelTypeString;

    /// Number of components per pixel
    uint32_t m_numComponents;

    /// Image size in pixels
    size_t m_imageSizeInPixels;

    /// Number of image dimensions (e.g. 1D, 2D, 3D, 4D, etc.)
    uint32_t m_numDimensions;

    /// Image pixel dimensions (i.e. matrix size)
    glm::u64vec3 m_pixelDimensions;

    /// Image origin in subject's "physical" space
    glm::dvec3 m_origin;

    /// Image voxel spacing in subject's "physical" space
    glm::dvec3 m_spacing;

    /// Image axis directions in subject's "physical" space, stored column-wise in a 3x3 matrix
    glm::dmat3 m_directions;

    /// Minimum and maximum corners of the image's axis-aligned bounding box in subject's "physical" space
    std::pair< glm::dvec3, glm::dvec3 > m_boundingBoxMinMaxCorners;

    /// All corners of the image's axis-aligned bounding box in subject's "physical" space
    std::array< glm::dvec3, 8 > m_boundingBoxCorners;

    /// Center of the image's axis-aligned bounding box in subject's "physical" space
    glm::dvec3 m_boundingBoxCenter;

    /// Size of the image's axis-aligned bounding box in subject's "physical" space
    glm::dvec3 m_boundingBoxSize;

    /// Three-character "SPIRAL" code defining the anatomical orientation of the image in subject's
    /// "physical" space, where positive X, Y, and Z axes correspond to the physical Left,
    /// Posterior, and Superior directions, respectively. The acroynm stands for Superior, Posterior,
    /// Inferior, Right, Anterior, Left
    std::string m_spiralCode;

    /// Flag indicating whether the image directions are oblique (i.e. skew w.r.t. the physical
    /// X, Y, Z, axes)
    bool m_isOblique;
};

} // namespace imageio


std::ostream& operator<< ( std::ostream&, const imageio::ImageHeader& );

#endif // IMAGEIO_IMAGE_HEADER_H
