#include "rendering/utility/gl/GLShader.h"
#include "rendering/utility/UnderlyingEnumType.h"

#include "common/HZeeException.hpp"

#include <glm/glm.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>


namespace
{

static const std::unordered_map< std::string, ShaderType > sk_shaderFileExtensionTypes =
{
    { ".vs",   ShaderType::Vertex },
    { ".vert", ShaderType::Vertex },
    { ".gs",   ShaderType::Geometry },
    { ".geom", ShaderType::Geometry },
    { ".tcs",  ShaderType::TessControl },
    { ".tes",  ShaderType::TessEvaluation },
    { ".fs",   ShaderType::Fragment },
    { ".frag", ShaderType::Fragment }

    /// @note Compute shaders are not supported in OpenGL 3.3
    //  { ".cs",   ShaderType::Compute }
};

static const std::unordered_map< ShaderType, std::string > sk_shaderTypeStrings =
{
    { ShaderType::Vertex, "vertex" },
    { ShaderType::Geometry, "geometry" },
    { ShaderType::TessControl, "tessControl" },
    { ShaderType::TessEvaluation, "tessEval" },
    { ShaderType::Fragment, "fragment" }
};

} // anonymous


GLShader::GLShader( std::string name, const ShaderType& type )
    :
      m_name( std::move( name ) ),
      m_type( type ),
      m_handle( 0u )
{
    initializeOpenGLFunctions();
}

GLShader::GLShader( std::string name, const ShaderType& type, const char* source )
    :
      GLShader( std::move( name ), type )
{
    compileFromString( source );
}

GLShader::GLShader( std::string name, const ShaderType& type, std::istream& source )
    :
      GLShader( std::move( name ), type )
{
    const std::string sourceString( std::istreambuf_iterator<char>( source ), {} );
    compileFromString( sourceString.c_str() );
}

//GLShader::GLShader( std::string name, const ShaderType& type,
//                    const std::vector< const char* >& sources )
//    : GLShader( std::move( name ), type )
//{
//    compileFromStrings( sources );
//}


GLShader::~GLShader()
{
    if ( ! m_handle )
    {
        return;
    }

    if ( glIsShader(m_handle) )
    {
        glDeleteShader( m_handle );
    }
}

const std::string& GLShader::name() const
{
    return m_name;
}

ShaderType GLShader::type() const
{
    return m_type;
}

GLuint GLShader::handle() const
{
    return m_handle;
}

bool GLShader::isValid()
{
    return ( m_handle && glIsShader( m_handle ) );
}


#if 0
void GLShader::compileFromFile( const char* fileName, const boost::optional<ShaderType>& type )
{
    if ( ! boost::filesystem::exists( fileName ) )
    {
        const std::string msg = std::string( "File: '" ) + fileName + "' not found.";
        throw_debug( msg );
    }

    std::ifstream inFile( fileName, std::ios::in );

    if ( ! inFile )
    {
        const std::string msg = std::string( "Unable to open file '") + fileName + "'";
        throw_debug( msg );
    }

    std::ostringstream source;
    source << inFile.rdbuf() << std::ends;
    inFile.close();

    if ( type )
    {
        m_type = *type;
    }
    else
    {
        const std::string ext = boost::to_lower_copy(
                    boost::filesystem::extension( fileName ) );

        const auto itr = s_shaderFileExtensionTypes.find( ext );

        if ( std::end( s_shaderFileExtensionTypes ) == itr )
        {
            const std::string msg = "Unrecognized shader extension: " + ext;
            throw msg;
        }

        m_type = itr->second;
    }

    compileFromString( source.str().c_str(), m_type );
}
#endif


void GLShader::compileFromString( const char* source )
{
    const GLuint handle = glCreateShader( underlyingType(m_type) );

    glShaderSource( handle, 1, &source, nullptr );
    glCompileShader( handle );

    if ( ! checkShaderStatus( handle ) )
    {
        throw_debug( "Cannot compile shader due to failed status check" );
    }

    m_handle = handle;

    CHECK_GL_ERROR( m_errorChecker );
}

//void GLShader::compileFromStrings( const std::vector< const char* >& sources )
//{
//    const GLuint handle = glCreateShader( underlyingType(m_type) );

//    glShaderSource( handle, static_cast<GLsizei>( sources.size() ), &sources[0], nullptr );
//    glCompileShader( handle );

//    if ( ! checkShaderStatus( handle ) )
//    {
//        throw_debug( "Cannot compile shader due to failed status check" );
//    }

//    m_handle = handle;

//    CHECK_GL_ERROR( m_errorChecker );
//}


void GLShader::setRegisteredUniforms( Uniforms uniforms )
{
    m_uniforms = std::move( uniforms );
}

const Uniforms& GLShader::getRegisteredUniforms() const
{
    return m_uniforms;
}


const std::string& GLShader::shaderTypeString( const ShaderType& type )
{
    return sk_shaderTypeStrings.at( type );
}


bool GLShader::checkShaderStatus( GLuint handle )
{
    GLint status;
    glGetShaderiv( handle, GL_COMPILE_STATUS, &status );

    if ( GL_FALSE == status )
    {
        GLint logLength = 0;
        glGetShaderiv( handle, GL_INFO_LOG_LENGTH, &logLength );

        std::string logString;

        if ( logLength > 0 )
        {
            std::vector<GLchar> cLog( static_cast<size_t>( logLength ) );
            GLsizei actualLength = 0;
            glGetShaderInfoLog( handle, logLength, &actualLength, &cLog[0] );
            logString = &cLog[0];
        }

        std::ostringstream msg;
        msg << "Shader compilation from source failed. OpenGL log:\n" << logString << std::ends;
        std::cerr << msg.str() << std::endl;

        return false;
    }

    return true;
}
