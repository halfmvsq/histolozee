#include "rendering/utility/gl/GLVersionChecker.h"

#include "common/HZeeException.hpp"

#include <iostream>
#include <sstream>


GLVersionChecker::GLVersionChecker()
{
    initializeOpenGLFunctions();

    // Major version number of the OpenGL API supported by the current context:
    GLint majorVersion;
    glGetIntegerv( GL_MAJOR_VERSION, &majorVersion );

    // Minor version number of the OpenGL API supported by the current context:
    GLint minorVersion;
    glGetIntegerv( GL_MINOR_VERSION, &minorVersion );

    if ( majorVersion < 3 || ( majorVersion == 3 && minorVersion < 3 ) )
    {
        std::ostringstream ss;
        ss << "OpenGL version " << majorVersion << "." << minorVersion
           << " is too low and not supported by HistoloZee.\n"
           << "The minimum required OpenGL version is 3.3" << std::ends;

        throw_debug( ss.str() );
    }

    // Profile mask used to create the context:
    GLint contextProfileMask;
    glGetIntegerv( GL_CONTEXT_PROFILE_MASK, &contextProfileMask );

    std::ostringstream ss;

    ss << "OpenGL context information:" << std::endl;
    ss << "\tVersion: " << glGetString( GL_VERSION );

    if ( contextProfileMask & GL_CONTEXT_CORE_PROFILE_BIT )
    {
        ss << " (core profile)";
    }
    else if ( contextProfileMask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT )
    {
        ss << " (compatibility profile)";
    }

    ss << std::endl;
    ss << "\tVendor: " << glGetString( GL_VENDOR ) << std::endl;
    ss << "\tRenderer: " << glGetString( GL_RENDERER ) << std::endl;

    /// @todo Logging:
    std::cout << ss.str() << std::endl;

    CHECK_GL_ERROR( m_errorChecker );
}
