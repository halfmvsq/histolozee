#ifndef GLERRORCHECKER_H
#define GLERRORCHECKER_H

#include <QOpenGLFunctions_3_3_Core>

#include <iostream>

#ifdef NDEBUG
#define CHECK_GL_ERROR( checker ) ((void)0)
#else
#define CHECK_GL_ERROR( checker ) checker( __FILE__, __PRETTY_FUNCTION__, __LINE__ );
#endif

#define PRINTLINE std::cout << __FILE__ << " : " << __PRETTY_FUNCTION__ << " : " << __LINE__ << std::endl;

class GLErrorChecker final : protected QOpenGLFunctions_3_3_Core
{
public:

    GLErrorChecker();
    ~GLErrorChecker() = default;

    void operator()( const char* file, const char* function, int line );
};

#endif // GLERRORCHECKER_H
