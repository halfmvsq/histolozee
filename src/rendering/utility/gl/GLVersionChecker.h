#ifndef GLVERSIONCHECKER_H
#define GLVERSIONCHECKER_H

#include "rendering/utility/gl/GLErrorChecker.h"

#include <QOpenGLFunctions_3_3_Core>

class GLVersionChecker final : protected QOpenGLFunctions_3_3_Core
{
public:

    GLVersionChecker();

    GLVersionChecker( const GLVersionChecker& ) = delete;
    GLVersionChecker& operator=( const GLVersionChecker& ) = delete;

    GLVersionChecker( GLVersionChecker&& ) = default;
    GLVersionChecker& operator=( GLVersionChecker&& ) = default;

    ~GLVersionChecker() = default;


private:

    GLErrorChecker m_errorChecker;
};

#endif // GLVERSIONCHECKER_H
