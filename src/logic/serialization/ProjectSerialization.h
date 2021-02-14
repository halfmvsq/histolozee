#ifndef PARSE_ARGUMENTS_H
#define PARSE_ARGUMENTS_H

#include "common/CoordinateFrame.h"

#include "imageio/ImageSettings.h"

#include "slideio/SlideProperties.h"
#include "slideio/SlideTransformation.h"

#include <nlohmann/json.hpp>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <optional>
#include <string>
#include <vector>


namespace serialize
{

/// @todo Serialization of all application data


/// Image display settings. All settings are OPTIONAL and need not be
/// provided in the JSON. If a settings is not in the JSON, then it will assume
/// default values AFTER the image is loaded.
///
/// @note Currently, only values for image component 0 are serialized
/// @todo Serialize the other components, too
struct ImageDisplaySettings
{
    std::optional<std::string> m_displayName = std::nullopt;
    std::optional<double> m_opacity = std::nullopt;
    std::optional<double> m_window = std::nullopt;
    std::optional<double> m_level = std::nullopt;
    std::optional<double> m_thresholdLow = std::nullopt;
    std::optional<double> m_thresholdHigh = std::nullopt;
    std::optional< imageio::ImageSettings::InterpolationMode > m_interpolationMode = std::nullopt;

};


/// Serialized data for a reference image or a parcellation
/// The only field that is required when reading the JSON is the file name.
struct Image
{
    std::string m_fileName; //!< Image file name on disk (REQUIRED in JSON)
    ImageDisplaySettings m_displaySettings; //!< Display settings (OPTIONAL in JSON)

    /// Rigid-body transformation from the image Subject space to World space (OPTIONAL in JSON)
    CoordinateFrame m_world_T_subject;
};


/// Serialized data for a slide
/// The only field that is required when reading the JSON is the file name.
struct Slide
{
    std::string m_fileName; //!< Slide image file name on disk (REQUIRED in JSON)
    slideio::SlideProperties m_properties; //!< Slide display properties (OPTIONAL in JSON)

    /// Rigid-body transformation from local slide space to the slide stack space
    /// (OPTIONAL in JSON)
    slideio::SlideTransformation m_slideStack_T_slide;
};


/// Serialized data for a HistoloZee project.
/// The only field that is required when reading the JSON is the reference image.
struct HZeeProject
{
    std::string m_fileName; //!< Project file name

    std::vector<Image> m_refImages; //!< Reference images (REQUIRED in JSON)
    std::vector<Image> m_parcellations; //!< Parcellation images (OPTIONAL in JSON)
    std::vector<Slide> m_slides; //!< Slide images (OPTIONAL in JSON)

    /// Rigid-body transformation from the space of the slide stack to World space
    /// (OPTIONAL in JSON)
    CoordinateFrame m_world_T_slideStack;

    /// Index of the active reference image, if there is at least one (OPTIONAL in JSON)
    uint32_t m_activeRefImage;

    /// Index of the active parcellation, if there is at least one (OPTIONAL in JSON)
    std::optional<uint32_t> m_activeParcellation;
};


/// Open project file
void open( HZeeProject& project, const std::string& fileName );

/// Save project file
/// @param[in] project Project to save
/// @param[in] newFileName Optional new file name to override the existing file name in project
void save( const HZeeProject& project, const std::optional< std::string >& newFileName );

} // namespace serialize

#endif // PARSE_ARGUMENTS_H
