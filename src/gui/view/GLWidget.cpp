#include "gui/view/GLWidget.h"
#include "gui/view/BorderPainter.h"

#include "common/CoordinateFrame.h"
#include "common/ThrowAssert.hpp"
#include "common/HZeeException.hpp"

#include "logic/camera/Camera.h"
#include "logic/camera/CameraHelpers.h"
#include "logic/interfaces/IInteractionHandler.h"

#include "rendering/interfaces/IRenderer.h"
#include "rendering/utility/math/MathUtility.h"

#include <glm/glm.hpp>

#include <QApplication>
#include <QOpenGLContext>
#include <QtWidgets>

#include <iostream>

#define DEBUG_TIMING 0


namespace gui
{

GLWidget::GLWidget(
        std::string name,
        std::unique_ptr<IRenderer> renderer,
        GetterType<camera::Camera*> cameraProvider,
        GetterType<IInteractionHandler*> interactionHandlerProvider,
        GetterType<CoordinateFrame> crosshairsProvider,
        const QList< Qt::GestureType >& gesturesToGrab,
        QWidget* parent )
    :
      QOpenGLWidget( parent ),

      m_name( std::move( name ) ),
      m_renderer( std::move( renderer ) ),

      m_cameraProvider( cameraProvider ),
      m_activeInteractionHandlerProvider( interactionHandlerProvider ),
      m_crosshairsProvider( crosshairsProvider ),

      m_viewport(),
      m_currentContext( nullptr ),
      m_enableColorBorder( true )
{
    if ( ! m_renderer || ! m_crosshairsProvider )
    {
        throw_debug( "Cannot construct GLWidget with null IRenderer or CrosshairsFrameProvider" );
    }

    setAttribute( Qt::WA_AcceptTouchEvents, true );
    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking( false );

    grabGestures( gesturesToGrab );
}

GLWidget::~GLWidget() = default;


void GLWidget::setEnableColorBorder( bool enable )
{
    m_enableColorBorder = enable;
}

IRenderer* GLWidget::getRenderer()
{
    return m_renderer.get();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize( 16, 16 );
}

QSize GLWidget::sizeHint() const
{
    return QSize( 256, 256 );
}


void GLWidget::initializeGL()
{
    //    QOpenGLContext *ctx = const_cast<QOpenGLContext *>(QOpenGLContext::currentContext());
    m_currentContext = context();
    throw_assert( m_currentContext, "Context is null." );

    std::cout << "Initializing view with OpenGL context: "
              << m_currentContext->nativeHandle().typeName() << std::endl;

    // This is how we can query GL extensions:
    // bool a = m_currentContext->hasExtension( "GL_ARB_clip_control" );

    //    GLint nrAttributes;
    //    glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &nrAttributes );
    //    std::cout << "Maximum number of vertex attributes supported: " << nrAttributes << std::endl;

    // The commented-out code below shows how to do teardown operations
    // when the application is about to quit. It was causing crashes and was so removed.
#if 0
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.

    auto teardown = [this] ()
    {
        QOpenGLContext* ctxt = this->context();
        QSurface* surface = ctxt ? ctxt->surface() : nullptr;
        if ( surface ) ctxt->makeCurrent( surface );

        m_renderer->teardown();
    };

    connect( m_currentContext, &QOpenGLContext::aboutToBeDestroyed, teardown );
    connect( QApplication::instance(), &QCoreApplication::aboutToQuit, teardown );
#endif

    if ( m_renderer )
    {
        m_renderer->initialize();
    }

    // Common values for the device-to-pixel ratio are 1 for normal-dpi displays and
    // 2 for high-dpi Apple "retina" displays.
    m_viewport.setDevicePixelRatio( devicePixelRatio() );
}


void GLWidget::paintGL()
{
#if DEBUG_TIMING
    std::chrono::high_resolution_clock::time_point startTime =
            std::chrono::high_resolution_clock::now();
#endif

    /// @note Qt calls glViewport prior to this paintGL, so there is no need
    /// for us to call it ourselves in either resize() or render() of the Drawables

    if ( ! m_renderer || ! m_cameraProvider || ! m_crosshairsProvider )
    {
        return;
    }

    if ( auto camera = m_cameraProvider() )
    {
        // Update the scene state variables that depend on camera and/or crosshairs.
        m_renderer->update( *camera, m_crosshairsProvider() );

        // Render the scene. HistoloZee uses the Dual Depth Peeling algorithm,
        // which performs multiple render passes over the scene in order to achieve
        // object order-independent transparence (OIT).
        m_renderer->render();
    }

#if DEBUG_TIMING
    m_frameTime = std::chrono::high_resolution_clock::now() - startTime;
    std::cout << m_name << ": " << m_frameTime.count() << " msec" << std::endl;
#endif
}


void GLWidget::resizeGL( int width, int height )
{
    throw_assert( width > 0 && height > 0 , "Width and height must be positive." );

    if ( ! m_cameraProvider )
    {
        return;
    }

    m_viewport.setWidth( static_cast<float>( width ) );
    m_viewport.setHeight( static_cast<float>( height ) );

    // Set device-to-pixel ratio on resize, in case the window is dragged between monitors
    m_viewport.setDevicePixelRatio( static_cast<float>( devicePixelRatio() ) );

    if ( auto camera = m_cameraProvider() )
    {
        // The only thing that the camera needs to know is the view aspect ratio
        camera->setAspectRatio( m_viewport.aspectRatio() );
    }

    if ( m_renderer )
    {
        m_renderer->resize( m_viewport );
    }
}


void GLWidget::paintEvent( QPaintEvent* event )
{
    QOpenGLWidget::paintEvent( event );

    if ( ! m_cameraProvider )
    {
        return;
    }

    auto camera = m_cameraProvider();

    if ( m_enableColorBorder && camera )
    {
        const glm::u8vec3 color = math::convertVecToRGB_uint8(
                    worldDirection( *camera, Directions::View::Back ) );

        BorderPainter painter( this );
        painter.setColor( color.r, color.g, color.b );
        painter.setSize( width(), height() );
        painter.draw();
    }
}


void GLWidget::grabGestures( const QList< Qt::GestureType >& gestures )
{
    foreach ( Qt::GestureType gesture, gestures )
    {
        grabGesture( gesture, Qt::GestureFlags() );
    }
}


bool GLWidget::event( QEvent* event )
{
    if ( event && QEvent::Gesture == event->type() &&
         m_activeInteractionHandlerProvider && m_cameraProvider )
    {
        QGestureEvent* gesture = dynamic_cast<QGestureEvent*>( event );
        auto handler = m_activeInteractionHandlerProvider();
        auto camera = m_cameraProvider();

        if ( gesture && handler && camera )
        {
            if ( handler->dispatchGestureEvent( gesture, m_viewport, *camera ) )
            {
                // It is possible to enqueue a re-render with update()
                // at this point if the event was handled. However, we opt
                // to re-render based on explicit function connections.
                return true;
            }
        }
    }

    return QWidget::event( event );
}


void GLWidget::mouseDoubleClickEvent( QMouseEvent* event )
{
    if ( event && m_activeInteractionHandlerProvider && m_cameraProvider )
    {
        auto handler = m_activeInteractionHandlerProvider();
        auto camera = m_cameraProvider();

        if ( handler && camera )
        {
            if ( handler->handleMouseDoubleClickEvent( event, m_viewport, *camera ) )
            {
                // Possible to update() here, but we do so with connections instead
                return;
            }
        }
    }

    QWidget::mouseDoubleClickEvent( event );
}


void GLWidget::mouseMoveEvent( QMouseEvent* event )
{
    if ( event && m_activeInteractionHandlerProvider && m_cameraProvider )
    {
        auto handler = m_activeInteractionHandlerProvider();
        auto camera = m_cameraProvider();

        if ( handler && camera )
        {
            if ( handler->handleMouseMoveEvent( event, m_viewport, *camera ) )
            {
                // Possible to update() here, but we do so with connections instead
                return;
            }
        }
    }

    QWidget::mouseMoveEvent( event );
}


void GLWidget::mousePressEvent( QMouseEvent* event )
{
    if ( event && m_activeInteractionHandlerProvider && m_cameraProvider && m_renderer )
    {
        auto handler = m_activeInteractionHandlerProvider();
        auto camera = m_cameraProvider();

        if ( ! handler || ! camera )
        {
            return;
        }

        if ( InteractionHandlerType::Crosshairs == handler->type() )
        {
            // Enqueue a re-render so that we have valid object ID and
            // depth buffers to pick on when the handler is called below
            update();

            // Enable point picking when interacting with the crosshairs
            m_renderer->setEnablePointPicking( true );
        }
        else
        {
            m_renderer->setEnablePointPicking( false );
        }

        if ( handler->handleMousePressEvent( event, m_viewport, *camera ) )
        {
            return;
        }
    }

    QWidget::mousePressEvent( event );
}


void GLWidget::mouseReleaseEvent( QMouseEvent* event )
{
    if ( event && m_activeInteractionHandlerProvider && m_cameraProvider )
    {
        auto handler = m_activeInteractionHandlerProvider();
        auto camera = m_cameraProvider();

        if ( handler && camera )
        {
            if ( handler->handleMouseReleaseEvent( event, m_viewport, *camera ) )
            {
                return;
            }
        }
    }

    QWidget::mouseReleaseEvent( event );
}


void GLWidget::tabletEvent( QTabletEvent* event )
{
    if ( event && m_activeInteractionHandlerProvider && m_cameraProvider )
    {
        auto handler = m_activeInteractionHandlerProvider();
        auto camera = m_cameraProvider();

        if ( handler && camera )
        {
            if ( handler->handleTabletEvent( event, m_viewport, *camera ) )
            {
                return;
            }
        }
    }

    QWidget::tabletEvent( event );
}


void GLWidget::wheelEvent( QWheelEvent* event )
{
    if ( event && m_activeInteractionHandlerProvider && m_cameraProvider )
    {
        auto handler = m_activeInteractionHandlerProvider();
        auto camera = m_cameraProvider();

        if ( handler && camera )
        {
            if ( handler->handleWheelEvent( event, m_viewport, *camera ) )
            {
                return;
            }
        }
    }

    QWidget::wheelEvent( event );
}


void GLWidget::timerEvent( QTimerEvent* )
{
    update();
}

} // namespace gui
