#ifndef GL_SHADER_H
#define GL_SHADER_H

#include "rendering/utility/containers/Uniforms.h"
#include "rendering/utility/gl/GLShaderType.h"
#include "rendering/utility/gl/GLErrorChecker.h"
#include "rendering/utility/gl/GLUniformTypes.h"

#include <QOpenGLFunctions_3_3_Core>

#include <glm/fwd.hpp>

#include <istream>
#include <string>
#include <unordered_map>
#include <vector>


/**
 * @brief Encapsulates an OpenGL shader program.
 */
class GLShader final : protected QOpenGLFunctions_3_3_Core
{
public:

    GLShader( std::string name, const ShaderType& type, const char* source );
    GLShader( std::string name, const ShaderType& type, std::istream& source );
//    GLShader( std::string name, const ShaderType& type, const std::vector< const char* >& sources );

    GLShader( const GLShader& ) = delete;
    GLShader& operator=( const GLShader& ) = delete;

    ~GLShader();

    const std::string& name() const;
    ShaderType type() const;
    GLuint handle() const;
    bool isValid();

    void setRegisteredUniforms( Uniforms uniforms );
    const Uniforms& getRegisteredUniforms() const;

    static const std::string& shaderTypeString( const ShaderType& type );


private:

    const std::string m_name;
    ShaderType m_type;
    GLuint m_handle;

    GLErrorChecker m_errorChecker;

    Uniforms m_uniforms;

    GLShader( std::string name, const ShaderType& type );

    void compileFromString( const char* source );
//    void compileFromStrings( const std::vector< const char* >& sources );

    bool checkShaderStatus( GLuint handle );
};

#endif // GL_SHADER_H
