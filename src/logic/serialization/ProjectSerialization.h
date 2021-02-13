#ifndef PARSE_ARGUMENTS_H
#define PARSE_ARGUMENTS_H

#include "imageio/ImageSettings.h"

#include <nlohmann/json.hpp>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <boost/optional.hpp>

#include <string>
#include <vector>


namespace serialize
{

/// @todo This section is of course not done; needs expansion for serialization of all app data
/// @todo Create enum for all image color maps


/// Serialized data for reference image settings
struct ImageSettings
{
    std::string m_displayName;

    double m_level;         //! Window center value in image units
    double m_window;        //! Window width in image units

    double m_slope;         //!< Slope computed from window
    double m_intercept;     //!< Intercept computed from window and level

    double m_thresholdLow;  //!< Values below threshold not displayed
    double m_thresholdHigh; //!< Values above threshold not displayed

    double m_opacity;       //!< Opacity [0, 1]

    /// Interpolation mode for voxels
    imageio::ImageSettings::InterpolationMode m_interpolationMode;

    /// Short name of color map
    boost::optional< std::string > m_colorMapName;
};


/// Serialized data for a reference image or parcellation
struct Image
{
    /// Image file name on disk
    std::string m_fileName;

    /// Display name in application
    std::string m_displayName;

    /// Image subject origin defined in World space
    glm::vec3 m_worldSubjectOrigin;

    /// Rotation from Image Subject to World space
    glm::quat m_subjectToWorldRotation;

    /// Image settings
    ImageSettings m_settings;
};


/// Serialized data for a slide
struct Slide
{
    /// Slide file name on disk
    std::string m_fileName;
};


/// Serialized data for a HistoloZee project
struct HZeeProject
{
    /// Reference images
    std::vector<Image> m_refImages;

    /// Parcellation images
    std::vector<Image> m_parcellations;

    /// Slide images
    std::vector<Slide> m_slides;

    /// Index to the active reference image, if there is at least one
    boost::optional<uint32_t> m_activeImage;

    /// Index to the active parcellation, if there is at least one
    boost::optional<uint32_t> m_activeParcellation;
};


/// Open a project from a file
void open( HZeeProject& project, const std::string& fileName );

/// Save a project to a file
void save( const HZeeProject& project, const std::string& fileName );


// make one that takes in imageio::ImageSettings
} // namespace serialize

#endif // PARSE_ARGUMENTS_H
