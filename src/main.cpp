#include "defines.h"

#include "common/HZeeException.hpp"
#include "logic/AppInitializer.h"
#include "logic/ProgramOptions.h"
#include "logic/serialization/ProjectSerialization.h"

#include <QApplication>
#include <QDebug>
#include <QDirIterator>
#include <QIcon>
#include <QSurfaceFormat>

#include <memory>
#include <vector>


#define USE_DARK_STYLE_SHEET 0

#if USE_DARK_STYLE_SHEET
#include <QStyleFactory>
#include <QTextStream>
#endif

/**
 * @note As of Qt 5.4, the QOpenGLWidget context is implicitly shared with other contexts
 * under the same window. You can also specify an application wide flag to make all
 * contexts shared or explicitly share selected contexts with a OpenGLContext method.
 *
 * @note When multiple QOpenGLWidgets are added as children to the same top-level
 * widget, their contexts will share with each other. This does not apply for QOpenGLWidget
 * instances that belong to different windows. This means that all QOpenGLWidgets
 * in the same window can access each other's sharable resources, like textures,
 * and there is no need for an extra "global share" context, as was the case with QGLWidget.
 *
 * @note To set up sharing between QOpenGLWidget instances belonging to different windows,
 * set the \c Qt::AA_ShareOpenGLContexts application attribute before instantiating
 * QApplication. This will trigger sharing between all QOpenGLWidget instances
 * without any further steps. Otherwise, the format will not be applied to the global share
 * context and therefore issues may arise with context sharing afterwards.
 *
 * @internal This enables resource sharing between the OpenGL contexts used by
 * classes like QOpenGLWidget and QQuickWidget. This allows sharing OpenGL resources,
 * like textures, between QOpenGLWidget instances that belong to different
 * top-level windows. This value has been added in Qt 5.4.
 */

int main( int argc, char *argv[] )
{
    QCoreApplication::setApplicationName( HZEE_APPNAME_FULL );
    QCoreApplication::setApplicationVersion( HZEE_VERSION_FULL );
    QCoreApplication::setOrganizationName( HZEE_ORGNAME_FULL );

    // Parse the command line arguments
    ProgramOptions options( QCoreApplication::applicationName().toStdString() );
    const ProgramOptions::ExitCode code = options.parseCommandLine( argc, argv);

    switch ( code )
    {
    case ProgramOptions::ExitCode::Success:
    {
        break;
    }
    case ProgramOptions::ExitCode::Failure:
    {
        return EXIT_FAILURE;
    }
    case ProgramOptions::ExitCode::Help:
    {
        return EXIT_SUCCESS;
    }
    }


    // Initialize resources that are stored in the application binary
    #if USE_DARK_STYLE_SHEET
    Q_INIT_RESOURCE( breeze ); // Breeze stylesheets
    #endif
    Q_INIT_RESOURCE( colormaps ); // Reference image color maps
    Q_INIT_RESOURCE( icons ); // Application icons
    Q_INIT_RESOURCE( letters ); // Anatomical direction letters
    Q_INIT_RESOURCE( toolbars ); // Toolbar icons


    QApplication::setAttribute( Qt::AA_ShareOpenGLContexts );
    QApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
    QGuiApplication::setAttribute( Qt::AA_UseHighDpiPixmaps );

    std::cout << HZEE_APPNAME_FULL << " (version " << HZEE_VERSION_FULL << ")" << std::endl;
    std::cout << HZEE_ORGNAME_FULL << std::endl << std::endl;


    // Select an OpenGL v3.3 context so that we get back-compatibility with older hardware.
    // This is the first so-called "Modern" OpenGL version.
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setRenderableType( QSurfaceFormat::OpenGL );
    surfaceFormat.setVersion( 3, 3 ); // version 3.3
    surfaceFormat.setProfile( QSurfaceFormat::CoreProfile );
    surfaceFormat.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
    surfaceFormat.setSwapInterval( 1 );
    surfaceFormat.setDepthBufferSize( 32 ); // 4-byte depth buffer
    surfaceFormat.setStencilBufferSize( 8 );
    surfaceFormat.setStereo( false );
    surfaceFormat.setSamples( 4 ); // 4x multi-sampling enabled

    /// @internal Sets the global default surface format: This format is used by default
    /// in QOpenGLContext, QWindow, QOpenGLWidget and similar classes.
    QSurfaceFormat::setDefaultFormat( surfaceFormat );
    qDebug() << "Surface format = " << surfaceFormat;


    QApplication app( argc, argv );
    app.setWindowIcon( QIcon( ":/HZeeIcon_noShadow.png" ) );


    #if USE_DARK_STYLE_SHEET
    // This style sheet can be used to set the application to "dark" mode. However, with native
    // support for dark mode on macOS, Linux, and Windows, this is not longer necessary. The user
    // has the ability to enable dark mode as they please at the O/S level.

    // Note: dark.ss requires view vertical scrollbars to be inverted.
    QFile file( ":/dark.qss" );
    file.open( QFile::ReadOnly | QFile::Text );
    QTextStream stream( &file );
    app.setStyleSheet( stream.readAll() );
    #endif


    auto appController = createAppController();
    if ( ! appController )
    {
        throw_debug( "Unable to construct AppController" )
    }


    // Create a list of the image color maps that are stored as Qt resources
    std::vector< std::string > colorMapFileNames;

    QDirIterator dirIt( ":/colormaps", QStringList() << "*.csv" << "*.CSV",
                        QDir::NoFilter, QDirIterator::Subdirectories );

    while ( dirIt.hasNext() )
    {
        colorMapFileNames.push_back( dirIt.next().toStdString() );
    }

    // Sort the files alphabetically
    std::sort( std::begin( colorMapFileNames ), std::end( colorMapFileNames ) );

    // Load the built-in color maps
    appController->loadBuiltInImageColorMaps( colorMapFileNames );


    // Open the project file and load images, parcellations, and slides
    serialize::HZeeProject project;
    serialize::open( project, options.projectFileName() );

    appController->loadProject( std::move( project ) );


    /*** START FEATURE EXPERIMENTATION ***/
//    appController->testAlignSlideStackToActiveImage();
//    appController->testCreateRefImageLandmark();
//    appController->testCreateSlideLandmark();
//    appController->testCreateSlideAnnotation();
    /*** END FEATURE EXPERIMENTATION ***/


    // Finalize the setup and show window!
    appController->setupCamerasAndCrosshairsForImage();
    appController->showMainWindow();

    return app.exec();
}
