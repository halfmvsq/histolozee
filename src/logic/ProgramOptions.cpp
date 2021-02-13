#include "logic/ProgramOptions.h"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include <iostream>


ProgramOptions::ProgramOptions( std::string appName )
    :
      m_appName( std::move( appName ) ),
      m_verbose( false ),
      m_projectFileName()
{}


ProgramOptions::ExitCode ProgramOptions::parseCommandLine( int argc, char *argv[] )
{
    try
    {
        namespace po = boost::program_options;

        const std::string appName = boost::filesystem::basename( argv[0] );

        po::options_description optionDescriptions( "Options" );

        optionDescriptions.add_options()
                ( "help,h",
                  boost::str( boost::format( "Display program help information for %s" ) % appName ).c_str() )

                ( "verbose,v",
                  po::bool_switch( &m_verbose )->default_value( false ),
                  "Enable verbose output mode" )

                ( "project",
                  po::value<std::string>( &m_projectFileName )->required()->value_name( "project_path" ),
                  "Path to project file (required)" )
                ;

        po::positional_options_description positionalOptions;
        positionalOptions.add( "project", 1 );

        if ( argc < 2 )
        {
            std::cout << optionDescriptions << std::endl;
            return ExitCode::Failure;
        }

        po::variables_map variablesMap;

        try
        {
            po::store( po::command_line_parser( argc, argv )
                       .options( optionDescriptions )
                       .positional( positionalOptions )
                       .run(),
                       variablesMap );

            po::notify( variablesMap );

            if ( variablesMap.count( "help" ) )
            {
                std::cout << optionDescriptions << std::endl;
                return ExitCode::Help;
            }

            if ( variablesMap.count( "verbose" ) )
            {
                m_verbose = variablesMap["verbose"].as<bool>();
            }

            if ( variablesMap.count( "project" ) )
            {
                boost::filesystem::path p( variablesMap["project"].as<std::string>() );
                m_projectFileName = boost::filesystem::canonical( p ).string();
            }
        }
        catch ( const po::required_option& e )
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return ExitCode::Failure;
        }
        catch ( const po::error& e )
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return ExitCode::Failure;
        }
        catch ( const std::exception& e )
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return ExitCode::Failure;
        }
        catch ( ... )
        {
            std::cerr << "Exception of unknown type!" << std::endl;
            return ExitCode::Failure;
        }
    }
    catch ( std::exception& e )
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return ExitCode::Failure;
    }
    catch ( ... )
    {
        std::cerr << "Exception of unknown type!" << std::endl;
        return ExitCode::Failure;
    }

    return ExitCode::Success;
}


const std::string& ProgramOptions::appName() const
{
    return m_appName;
}

const std::string& ProgramOptions::projectFileName() const
{
    return m_projectFileName;
}

bool ProgramOptions::useVerbose() const
{
    return m_verbose;
}
