#ifndef VTKWIDGET_H
#define VTKWIDGET_H

#include <QVTKOpenGLWidget.h>

#include <vtkRenderer.h>


namespace gui
{

/**
 * @note This class was created for experimenting with VTK rendering.
 * It is not being used right now.
 */
class VTKWidget : public QVTKOpenGLWidget
{
    Q_OBJECT

public:

    explicit VTKWidget( QWidget* parent = nullptr );

    void zoomToExtent();


public slots:


protected:

    void mouseDoubleClickEvent( QMouseEvent* event ) override;
    void mouseMoveEvent( QMouseEvent* event ) override;
    void mousePressEvent( QMouseEvent* event ) override;
    void mouseReleaseEvent( QMouseEvent* event ) override;


private:

    vtkSmartPointer< vtkRenderer > m_renderer;

    void doRender();
};

} // namespace gui

#endif // VTKWIDGET_H
