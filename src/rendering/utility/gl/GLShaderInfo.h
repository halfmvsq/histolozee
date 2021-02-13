#ifndef SHADERINFO_H
#define SHADERINFO_H

#include <QOpenGLFunctions_3_3_Core>


class ShaderInfo : protected QOpenGLFunctions_3_3_Core
{
public:

    ShaderInfo();
    ~ShaderInfo() = default;

    bool checkForOpenGLError(const char *, int);

    void dumpGLInfo(bool dumpExtensions = false);

#ifdef USING_GL_VERSION_4_3
    void debugCallback( GLenum source, GLenum type, GLuint id,
                        GLenum severity, GLsizei length,
                        const GLchar * msg, const void * param );
#endif
};

#endif // SHADERINFO_H
