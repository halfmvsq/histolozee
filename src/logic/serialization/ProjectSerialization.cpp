#include "logic/serialization/ProjectSerialization.h"

#include "common/HZeeException.hpp"
#include "common/JSONSerializers.hpp"

#include <boost/filesystem.hpp>

#include <iostream>
#include <fstream>
#include <sstream>


using json = nlohmann::json;


namespace
{

/// Make all image, parcellation, and slide file names canonical in a project
void makeFileNamesCanonical( json& project, const boost::filesystem::path& basePath )
{
    namespace fs = boost::filesystem;

    for ( auto& image : project.at( "referenceImages" ) )
    {
        image.at( "fileName" ) = fs::canonical(
                    fs::path( image.at( "fileName" ).get<std::string>() ), basePath ).string();
    }

    if ( project.contains( "parcellations" ) )
    {
        for ( auto& parcel : project.at( "parcellations" ) )
        {
            parcel.at( "fileName" ) = fs::canonical(
                        fs::path( parcel.at( "fileName" ).get<std::string>() ), basePath ).string();
        }
    }

    if ( project.contains( "slides" ) )
    {
        for ( auto& slide : project.at( "slides" ) )
        {
            slide.at( "fileName" ) = fs::canonical(
                        fs::path( slide.at( "fileName" ).get<std::string>() ), basePath ).string();
        }
    }
}

} // anonymous


// Write coordinate frame
void to_json( json& j, const CoordinateFrame& frame )
{
    j = json {
    { "worldOrigin", frame.worldOrigin() },
    { "subjectToWorldQuaternion", frame.world_O_frame_rotation() }
    };
}

// Write coordinate frame
void from_json( const json& j, CoordinateFrame& frame )
{
    static const glm::vec3 sk_origin{ 0.0f, 0.0f, 0.0f };
    static const glm::quat sk_ident{ 1.0f, 0.0f, 0.0f, 0.0f };

    // Initialize origin (translation) and quaternion rotation with identity,
    // in case they are no defined in the JSON:
    glm::vec3 worldOrigin{ sk_origin };
    glm::quat frameToWorldRotation{ sk_ident };

    // worldOrigin and subjectToWorldQuaternion are optional fields
    if ( j.contains( "worldOrigin" ) ) j.at( "worldOrigin" ).get_to( worldOrigin );
    if ( j.contains( "subjectToWorldQuaternion" ) ) j.at( "subjectToWorldQuaternion" ).get_to( frameToWorldRotation );

    frame.setWorldOrigin( worldOrigin );
    frame.setFrameToWorldRotation( frameToWorldRotation );
}


namespace imageio
{

/// Define serialization of InterpolationMode to JSON as strings
NLOHMANN_JSON_SERIALIZE_ENUM(
        ImageSettings::InterpolationMode, {
            { ImageSettings::InterpolationMode::Linear, "Linear" },
            { ImageSettings::InterpolationMode::NearestNeighbor, "NearestNeighbor" }
        } );

} // namespace imageio


namespace slideio
{

// Write slide properties
void to_json( json& j, const SlideProperties& properties )
{
    j = json{
    { "displayName", properties.displayName() },
    { "borderColor", properties.borderColor() },
    { "visible", properties.visible() },
    { "opacity", properties.opacity() },
    { "thresholdLow", properties.intensityThresholds().first },
    { "thresholdHigh", properties.intensityThresholds().second }
    };
}

// Read slide properties
void from_json( const json& j, SlideProperties& properties )
{
    // All fields are optional

    if ( j.contains( "displayName" ) ) properties.setDisplayName( j["displayName"] );
    if ( j.contains( "borderColor" ) ) properties.setBorderColor( j["borderColor"] );
    if ( j.contains( "visible" ) ) properties.setVisible( j["visible"] );
    if ( j.contains( "opacity" ) ) properties.setOpacity( j["opacity"] );
    if ( j.contains( "thresholdLow" ) ) properties.setIntensityThresholdLow( j["thresholdLow"] );
    if ( j.contains( "thresholdHigh" ) ) properties.setIntensityThresholdHigh( j["thresholdHigh"] );
}


// Write slide-to-stack transformation
void to_json( json& j, const SlideTransformation& tx )
{
    j = json{
    { "normalizedTranslationXY", tx.normalizedTranslationXY() },
    { "stackTranslationZ", tx.stackTranslationZ() },
    { "rotationAngleZ", tx.rotationAngleZ() },
    { "scaleFactorsXY", tx.scaleFactorsXY() },
    { "normalizedRotationCenterXY", tx.normalizedRotationCenterXY() },
    };

    switch ( tx.shearParamMode() )
    {
    case SlideTransformation::ShearParamMode::ShearAngles:
    {
        j["shearAnglesXY"] = tx.shearAnglesXY();
        break;
    }
    case SlideTransformation::ShearParamMode::ScaleRotation:
    {
        j["scaleRotationAngle"] = tx.scaleRotationAngle();
        break;
    }
    }
}

// Read slide-to-stack transformation
void from_json( const json& j, SlideTransformation& tx )
{
    // All fields are optional

    if ( j.contains( "normalizedTranslationXY" ) ) tx.setNormalizedTranslationXY( j["normalizedTranslationXY"] );
    if ( j.contains( "stackTranslationZ" ) ) tx.setStackTranslationZ( j["stackTranslationZ"] );
    if ( j.contains( "rotationAngleZ" ) ) tx.setRotationAngleZ( j["rotationAngleZ"] );
    if ( j.contains( "scaleFactorsXY" ) ) tx.setScaleFactorsXY( j["scaleFactorsXY"] );
    if ( j.contains( "normalizedRotationCenterXY" ) ) tx.setNormalizedRotationCenterXY( j["normalizedRotationCenterXY"] );

    if ( j.contains( "shearAnglesXY" ) )
    {
        tx.setShearParamMode( SlideTransformation::ShearParamMode::ShearAngles );
        tx.setShearAnglesXY( j["shearAnglesXY"] );
    }
    else if ( j.contains( "scaleRotationAngle" ) )
    {
        tx.setShearParamMode( SlideTransformation::ShearParamMode::ScaleRotation );
        tx.setScaleRotationAngle( j["scaleRotationAngle"] );
    }
}

} // namespace slideio



namespace serialize
{

// Write image settings
void to_json( json& j, const ImageDisplaySettings& settings )
{
    /// @note Don't do it this way, because it results in "null" being placed in the JSON
//    j = json{
//    { "displayName", settings.m_displayName },
//    ...
//    };

    if ( settings.m_displayName ) j[ "displayName" ] = *settings.m_displayName;
    if ( settings.m_window ) j[ "window" ] = *settings.m_window;
    if ( settings.m_level ) j[ "level" ] = *settings.m_level;
    if ( settings.m_thresholdLow ) j[ "thresholdLow" ] = *settings.m_thresholdLow;
    if ( settings.m_thresholdHigh ) j[ "thresholdHigh" ] = *settings.m_thresholdHigh;
    if ( settings.m_opacity ) j[ "opacity" ] = *settings.m_opacity;
    if ( settings.m_interpolationMode ) j[ "interpolation" ] = *settings.m_interpolationMode;
}

// Read image settings
void from_json( const json& j, ImageDisplaySettings& settings )
{
    if ( j.contains( "displayName" ) ) settings.m_displayName = j["displayName"];
    if ( j.contains( "window" ) ) settings.m_window = j["window"];
    if ( j.contains( "level" ) ) settings.m_level = j["level"];
    if ( j.contains( "thresholdLow" ) ) settings.m_thresholdLow = j["thresholdLow"];
    if ( j.contains( "thresholdHigh" ) ) settings.m_thresholdHigh = j["thresholdHigh"];
    if ( j.contains( "opacity" ) ) settings.m_opacity = j["opacity"];
    if ( j.contains( "interpolation" ) ) settings.m_interpolationMode = j["interpolation"];
}


// Write image
void to_json( json& j, const Image& image )
{
    j = json{
    { "fileName", image.m_fileName },
    { "world_T_subject", image.m_world_T_subject },
    { "displaySettings", image.m_displaySettings }
    };
}

// Read image
void from_json( const json& j, Image& image )
{
    // fileName is a required field
    j.at( "fileName" ).get_to( image.m_fileName );

    // world_T_subject is an optional field
    image.m_world_T_subject = ( j.contains( "world_T_subject" ) )
            ? j.at( "world_T_subject" ).get<CoordinateFrame>()
            : CoordinateFrame();

    // displaySettings is an optional field
    image.m_displaySettings = ( j.contains( "displaySettings" ) )
            ? j.at( "displaySettings" ).get<ImageDisplaySettings>()
            : ImageDisplaySettings();
}


// Write slide
void to_json( json& j, const Slide& slide )
{
    j = json{
    { "fileName", slide.m_fileName },
    { "slideStack_T_slide", slide.m_slideStack_T_slide },
    { "displaySettings", slide.m_properties }
    };
}

// Read slide
void from_json( const json& j, Slide& slide )
{
    // fileName is a required field
    j.at( "fileName" ).get_to( slide.m_fileName );

    // slideStack_T_slide is an optional field
    slide.m_slideStack_T_slide = ( j.contains( "slideStack_T_slide" ) )
            ? j.at( "slideStack_T_slide" ).get< slideio::SlideTransformation >()
            : slideio::SlideTransformation();

    // displaySettings is an optional field
    slide.m_properties = ( j.contains( "displaySettings" ) )
            ? j.at( "displaySettings" ).get< slideio::SlideProperties >()
            : slideio::SlideProperties();
}


// Write project
void to_json( json& j, const HZeeProject& project )
{
    /// @note Don't bother writing the active parcellation index.
    /// There might be a blank parcellation that will mess up the indexing,
    /// since blank parcellations are not written to the project file.

    j = json{
    { "referenceImages", project.m_refImages },
    { "parcellations", project.m_parcellations },
    { "slides", project.m_slides },
    { "activeImage", project.m_activeImage },
//    { "activeParcellation", project.m_activeParcellation },
    { "world_T_slideStack", project.m_world_T_slideStack }
    };
}

// Read project
void from_json( const json& j, HZeeProject& project )
{
    // referenceImages is a required field
    j.at( "referenceImages" ).get_to( project.m_refImages );

    // parcellations is an optional field
    project.m_parcellations = ( j.contains( "parcellations" ) )
            ? j.at( "parcellations" ).get< std::vector<Image> >()
            : std::vector<Image>{};

    // slides is an optional field
    project.m_slides = ( j.contains( "slides" ) )
            ? j.at( "slides" ).get< std::vector<Slide> >()
            : std::vector<Slide>{};

    // activeImage is an optional field
    project.m_activeImage = ( j.contains( "activeImage" ) )
            ? j.at( "activeImage" ).get<uint32_t>() : 0;

    // activeParcellation is an optional field
    project.m_activeParcellation = ( j.contains( "activeParcellation" ) )
            ? std::optional<uint32_t>( j.at( "activeParcellation" ).get<uint32_t>() )
            : std::nullopt;

    // world_T_slideStack is an optional field
    project.m_world_T_slideStack = ( j.contains( "world_T_slideStack" ) )
            ? j.at( "world_T_slideStack" ).get<CoordinateFrame>()
            : CoordinateFrame();
}


void open( HZeeProject& project, const std::string& fileName )
{
    try
    {
        std::ifstream inFile( fileName );

        json j;
        inFile >> j;

        // Get the parent directory:
        boost::filesystem::path basePath( fileName );
        basePath.remove_filename();

        makeFileNamesCanonical( j, basePath );

        std::cout << "\nLoaded project from " << fileName << ":" << std::endl << std::endl;
        std::cout << std::setw( 2 ) << j << std::endl << std::endl;

        project = j.get<HZeeProject>();
        project.m_fileName = fileName;
    }
    catch ( const std::exception& e )
    {
        std::ostringstream ss;
        ss << "Error parsing from JSON:\n" << e.what() << std::ends;
        throw_debug( ss.str() )
    }
}


void save( const HZeeProject& project, const std::optional< std::string >& newFileName )
{
    try
    {
        std::ofstream outFile( newFileName ? *newFileName : project.m_fileName );

        const json j = project;
        outFile << std::setw( 2 ) << j;

        std::cout << "Saved project to " << project.m_fileName << ":" << std::endl << std::endl;
        std::cout << std::setw( 2 ) << j << std::endl << std::endl;
    }
    catch ( const std::exception& e )
    {
        std::ostringstream ss;
        ss << "Error saving to JSON:\n" << e.what() << std::ends;
        throw_debug( ss.str() )
    }
}

} // namespace serialize
