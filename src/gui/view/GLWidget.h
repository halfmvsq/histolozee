#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include "common/PublicTypes.h"
#include "common/Viewport.h"

#include <glm/glm.hpp>

#include <QList>
#include <QOpenGLWidget>

#include <chrono>
#include <memory>


class CoordinateFrame;
class IInteractionHandler;
class IRenderer;

namespace camera
{
class Camera;
}


/**
 * @note Geometry is now specified in device independent pixels.
 * This includes widget and item geometry, event geometry, desktop, window,
 * and screen geometry, and animation velocities. Rendered output is in
 * device pixels, which corresponds to the display resolution.
 *
 * The ratio between the device independent (Pixels used by application
 * (user space), subject to scaling by the operating system or Qt)
 * and device pixel coordinate (Pixels of the display device) systems
 * is the devicePixelRatio.
 *
 * Applications mostly work with device independent pixels.
 * Notable exceptions are OpenGL and code that works with raster graphics.
 */

namespace gui
{

/**
 * @brief Widget that encapsulates the application's rendering within
 * the Qt5 scene graph. This class owns the IRenderer that performs
 * the actual OpenGL render calls. However, it holds weak pointers to
 * its assigned camera, crosshairs, and current interaction handler.
 *
 * @note paintGL() may be executed in the GUI's rendering thread
 */
class GLWidget : public QOpenGLWidget
{
    Q_OBJECT


public:

    GLWidget( std::string name,
              std::unique_ptr<IRenderer> renderer,
              GetterType<camera::Camera*> cameraProvider,
              GetterType<IInteractionHandler*> activeInteractionHandlerProvider,
              GetterType<CoordinateFrame> crosshairsProvider,
              const QList< Qt::GestureType >& gesturesToGrab,
              QWidget* parent = nullptr );

    GLWidget( const GLWidget& ) = delete;
    GLWidget& operator=( const GLWidget& ) = delete;

    GLWidget( GLWidget&& ) = delete;
    GLWidget& operator=( GLWidget&& ) = delete;

    ~GLWidget() override;


    /// Enable/disable the color border that indicates the view direction
    void setEnableColorBorder( bool );

    IRenderer* getRenderer();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    /// Subscribe the widget to given gestures with specific flags
    void grabGestures( const QList< Qt::GestureType >& gestures );


protected:

    /// OpenGL initialization happens here
    void initializeGL() override;

    /// OpenGL rendering happens here. This function may be executed from the Qt GUI
    /// rendering thread.
    void paintGL() override;

    /// OpenGL and rendering logic upon view resizing happens here
    /// @note Width and height are specified in device independent pixels
    void resizeGL( int width, int height ) override;

    void paintEvent( QPaintEvent* ) override;

    /// Override the widget's event handling functions using our application's
    /// custom interaction handlers. If we do not handle the events, then call
    /// the superclass handler.

    /// @note All event coordinates are in device-independent units
    bool event( QEvent* ) override;
    void mouseDoubleClickEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void tabletEvent( QTabletEvent* ) override;
    void wheelEvent( QWheelEvent* ) override;


private:

    /// @note Timer can be started with 'startTimer( interval, Qt::PreciseTimer )'
    void timerEvent( QTimerEvent* ) override;

    const std::string m_name;

    std::unique_ptr<IRenderer> m_renderer;

    GetterType<camera::Camera*> m_cameraProvider;
    GetterType<IInteractionHandler*> m_activeInteractionHandlerProvider;
    GetterType<CoordinateFrame> m_crosshairsProvider;

    /// Viewport of the view being rendered
    Viewport m_viewport;

    /// Timer for render profiling
    std::chrono::high_resolution_clock::time_point m_previousTime;
    std::chrono::duration<double> m_deltaTime;
    std::chrono::duration<double, std::milli> m_frameTime;

    QOpenGLContext* m_currentContext;

    /// Flag to enable the color border around the view
    bool m_enableColorBorder;
};

} // namespace gui

#endif // GL_WIDGET_H
