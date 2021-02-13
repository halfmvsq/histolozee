#ifndef PROGRAM_OPTIONS_H
#define PROGRAM_OPTIONS_H

#include <string>


class ProgramOptions
{
public:

    /// Possible exit codes after parsing command line arguments
    enum class ExitCode
    {
        Success = 0,
        Failure = 1,
        Help = 2
    };


    ProgramOptions( std::string appName );

    ExitCode parseCommandLine( int argc, char *argv[] );

    const std::string& appName() const;

    const std::string& projectFileName() const;

    bool useVerbose() const;


private:

    /// Name of the calling application
    std::string m_appName;

    /// Flag to use verbose output
    bool m_verbose;

    /// Path to project file
    std::string m_projectFileName;
};

#endif // PROGRAM_OPTIONS_H
