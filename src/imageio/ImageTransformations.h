#ifndef IMAGEIO_IMAGE_TX_H
#define IMAGEIO_IMAGE_TX_H

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_precision.hpp>

#include <ostream>
#include <utility>


namespace imageio
{

/**
 * @brief Container for transformations of an image. There are four image coordinate spaces:
 *
 * - TEXTURE SPACE: Representation of image in GPU texture space. Coordinate axes are normalized
 *   to the range [0.0, 1.0], with 0.0 and 1.0 denoting the EDGES of the first and last image pixels
 *   (not the pixel centers). The three coordinates are often abbreviated (s, t, p).
 *
 * - PIXEL SPACE: Representation of image in memory. Coordinates along an image dimension run
 *   from [0, N-1], where N is the number of pixels along the dimension and where 0 and N-1
 *   denote the CENTERS (not edges) of the first and last pixels. Note: the term "pixel" is used
 *   synonymously with "voxel", even for 3D images. The three coordinates are often abbreviated
 *   (i, j, k).
 *
 * - SUBJECT SPACE: Space of the subject in physical units, commonly millimeters.
 *   The transformation from Pixel space to Subject space is computed from the image pixel size,
 *   origin, and directions. This space is defined such that coordinates (x, y, z) correspond
 *   to physical directions Left, Posterior, and Superior (or, LPS) for human subjects.
 *
 * - WORLD SPACE: Space in which the image is rendered. This is typically identical to
 *   Subject space (i.e. world_O_subject == identity). However, the user may choose to apply a
 *   non-identity transformation between Subject and World space. This is useful when co-registering
 *   images to each other or when otherwise transforming the subject. The world_O_subject
 *   transformation is constrained to be rigid-body (i.e. 3D translation and rotation only).
 *
 * @todo the world_O_subject transformation is not yet fully supported in the application.
 * For instance, by default, crosshairs align to Subject space, not World space.
 */
class ImageTransformations
{
public:

    /**
     * @brief Constructor
     *
     * @param[in] pixelDimensions Image dimensions in pixel units
     * @param[in] spacing Spacings of image pixels in Subject space
     * @param[in] origin Position of image pixel (0, 0, 0) in Subject space
     * @param[in] directions Directions of image pixel axes (x, y, z) in Subject space
     * @param[in] worldSubjectOrigin Origin of Subject in World space
     * @param[in] subjectToWorldRotation Rotation from Subject to World space
     *
     * @throws \c std::invalid_argument
     */
    ImageTransformations(
            const glm::u64vec3& pixelDimensions,
            const glm::dvec3& spacing,
            const glm::dvec3& origin,
            const glm::dmat3& directions,
            const glm::vec3& getWorldSubjectOrigin,
            const glm::quat& subjectToWorldRotation );

    ImageTransformations( const ImageTransformations& ) = default;
    ImageTransformations& operator=( const ImageTransformations& ) = default;

    ImageTransformations( ImageTransformations&& ) = default;
    ImageTransformations& operator=( ImageTransformations&& ) = default;

    ~ImageTransformations() = default;


    /// Set origin of Subject in World space
    void setWorldSubjectOrigin( glm::vec3 getWorldSubjectOrigin );

    /// Get origin of Subject in World space
    glm::vec3 getWorldSubjectOrigin() const;

    /// Set rotation from Subject to World space
    void setSubjectToWorldRotation( glm::quat world_O_subject_rotation );

    /// Get rotation from Subject to World space
    glm::quat getSubjectToWorldRotation() const;


    const glm::mat4& world_O_subject() const; //!< Get tx from image Subject to World space
    const glm::mat4& subject_O_world() const; //!< Get tx from World to image Subject space

    const glm::vec3& subjectDimensions() const; //!< Get dimensions of image in Subject space

    const glm::mat4& subject_O_pixel() const; //!< Get tx from image Pixel to Subject space
    const glm::mat4& pixel_O_subject() const; //!< Get tx from image Subject to Pixel space

    const glm::mat4& pixel_O_texture() const; //!< Get tx from image Texture to Pixel space
    const glm::mat4& texture_O_pixel() const; //!< Get tx from image Pixel to Texture space

    const glm::mat4& subject_O_texture() const; //!< Get tx from image Texture to Subject space
    const glm::mat4& texture_O_subject() const; //!< Get tx from image Subject to Texture space

    const glm::mat4& world_O_texture() const; //!< Get tx from image Texture to World space
    const glm::mat4& texture_O_world() const; //!< Get tx from World to image Texture space

    const glm::mat4& world_O_pixel() const; //!< Get tx from image Pixel to World space
    const glm::mat4& pixel_O_world() const; //!< Get tx from World to image Pixel space

    /// Get inverse-transpose of tx from World to image Pixel space
    glm::mat4 pixel_O_world_invTranspose() const;


private:

    /// Update the world_O_subject (and its inverse) matrices from the Subject origin position
    /// and Subject to World rotation quaternion
    void update_world_O_subject();

    /// @note The constant variables here never change for an image

    glm::vec3 m_subjectDimensions; //!< Dimensions of image in Subject space

    glm::mat4 m_subject_O_pixel; //!< Tx from Pixel to Subject space
    glm::mat4 m_pixel_O_subject; //!< Subject to Pixel space

    glm::mat4 m_texture_O_pixel; //!< Pixel to Texture space
    glm::mat4 m_pixel_O_texture; //!< Texture to Pixel space

    glm::mat4 m_texture_O_subject; //!< Subject to Texture space
    glm::mat4 m_subject_O_texture; //!< Texture to Subject space

    glm::vec3 m_worldSubjectOrigin;   //!< Subject origin defined in World space
    glm::quat m_subjectToWorldRotation; //!< Rotation from Subject to World space

    glm::mat4 m_world_O_subject; //!< Subject to World space
    glm::mat4 m_subject_O_world; //!< World to Subject space

    glm::mat4 m_world_O_texture; //!< Texture to World space
    glm::mat4 m_texture_O_world; //!< World to Texture space

    glm::mat4 m_world_O_pixel; //!< World to Pixel space
    glm::mat4 m_pixel_O_world; //!< Pixel to World space
};

} // namespace imageio


std::ostream& operator<< ( std::ostream&, const imageio::ImageTransformations& );

#endif // IMAGEIO_IMAGE_TX_H
