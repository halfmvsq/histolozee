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

    for ( auto& parcel : project.at( "parcellations" ) )
    {
        parcel.at( "fileName" ) = fs::canonical(
                    fs::path( parcel.at( "fileName" ).get<std::string>() ), basePath ).string();
    }

    for ( auto& slide : project.at( "slides" ) )
    {
        slide.at( "fileName" ) = fs::canonical(
                    fs::path( slide.at( "fileName" ).get<std::string>() ), basePath ).string();
    }
}

} // anonymous


namespace imageio
{

/// Define serialization of InterpolationMode to JSON as strings
NLOHMANN_JSON_SERIALIZE_ENUM(
        ImageSettings::InterpolationMode, {
            { ImageSettings::InterpolationMode::Linear, "Linear" },
            { ImageSettings::InterpolationMode::NearestNeighbor, "NearestNeighbor" }
        } );

} // namespace imageio


namespace serialize
{

void to_json( json& j, const ImageSettings& settings )
{
    j = json{
    { "level", settings.m_level },
    { "window", settings.m_window },
    { "slope", settings.m_slope },
    { "intercept", settings.m_intercept },
    { "thresholdLow", settings.m_thresholdLow },
    { "thresholdHigh", settings.m_thresholdHigh },
    { "opacity", settings.m_opacity },
    { "interpolationMode", settings.m_interpolationMode },
    { "colorMapName", settings.m_colorMapName } };
}

void from_json( const json& j, ImageSettings& settings )
{
    j.at( "level" ).get_to( settings.m_level );
    j.at( "window" ).get_to( settings.m_window );
    j.at( "slope" ).get_to( settings.m_slope );
    j.at( "intercept" ).get_to( settings.m_intercept );
    j.at( "thresholdLow" ).get_to( settings.m_thresholdLow );
    j.at( "thresholdHigh" ).get_to( settings.m_thresholdHigh );
    j.at( "opacity" ).get_to( settings.m_opacity );
    j.at( "interpolationMode" ).get_to( settings.m_interpolationMode );
    j.at( "colorMapName" ).get_to( settings.m_colorMapName );
}


void to_json( json& j, const Image& image )
{
    j = json{
    { "fileName", image.m_fileName },
    { "displayName", image.m_displayName },
    { "worldSubjectOrigin", image.m_worldSubjectOrigin },
    { "subjectToWorldRotation", image.m_subjectToWorldRotation },
    { "settings", image.m_settings } };
}

void from_json( const json& j, Image& image )
{
    j.at( "fileName" ).get_to( image.m_fileName );
    j.at( "displayName" ).get_to( image.m_displayName );
    j.at( "worldSubjectOrigin" ).get_to( image.m_worldSubjectOrigin );
    j.at( "subjectToWorldRotation" ).get_to( image.m_subjectToWorldRotation );
}


void to_json( json& j, const Slide& slide )
{
    j = json{
    { "fileName", slide.m_fileName } };
}

void from_json( const json& j, Slide& slide )
{
    j.at( "fileName" ).get_to( slide.m_fileName );
}


void to_json( json& j, const HZeeProject& project )
{
    j = json{
    { "referenceImages", project.m_refImages },
    { "parcellations", project.m_parcellations },
    { "slides", project.m_slides },
    { "activeImage", project.m_activeImage },
    { "activeParcellation", project.m_activeParcellation } };
}

void from_json( const json& j, HZeeProject& project )
{
    j.at( "referenceImages" ).get_to( project.m_refImages );
    j.at( "parcellations" ).get_to( project.m_parcellations );
    j.at( "slides" ).get_to( project.m_slides );
    j.at( "activeImage" ).get_to( project.m_activeImage );
    j.at( "activeParcellation" ).get_to( project.m_activeParcellation );
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
        std::cout << std::setw( 4 ) << j << std::endl << std::endl;

        project = j.get<HZeeProject>();
    }
    catch ( const std::exception& e )
    {
        std::ostringstream ss;
        ss << "Error parsing from JSON:\n" << e.what() << std::ends;
        throw_debug( ss.str() );
    }
}


void save( const HZeeProject& project, const std::string& fileName )
{
    try
    {
        std::ofstream outFile( fileName );

        const json j = project;
        outFile << std::setw( 4 ) << j;

        std::cout << "Saved project to " << fileName << ":" << std::endl << std::endl;
        std::cout << std::setw( 4 ) << j << std::endl << std::endl;
    }
    catch ( const std::exception& e )
    {
        std::ostringstream ss;
        ss << "Error saving to JSON:\n" << e.what() << std::ends;
        throw_debug( ss.str() );
    }
}

} // namespace serialize
