#include "gui/view/VTKWidget.h"

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDataSetMapper.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleJoystickActor.h>
#include <vtkMatrix4x4.h>
#include <vtkSphereSource.h>
#include <vtkInteractorStyleSwitch.h>


namespace gui
{

VTKWidget::VTKWidget( QWidget* parent )
    : QVTKOpenGLWidget( parent )
{
    vtkNew< vtkGenericOpenGLRenderWindow > window;
    SetRenderWindow( window.Get() );

    m_renderer = vtkSmartPointer< vtkRenderer >::New();

    GetRenderWindow()->AddRenderer( m_renderer );

    m_renderer->SetBackground( 0.1, 0.2, 0.4 );

    vtkSmartPointer< vtkCamera > camera = vtkSmartPointer<vtkCamera>::New();
    camera->SetViewUp( 0, 1, 0 );
    camera->SetPosition( 0, 0, 10 );
    camera->SetFocalPoint( 0, 0, 0 );

    m_renderer->SetActiveCamera( camera );


    vtkSmartPointer< vtkCylinderSource > cylinder = vtkSmartPointer< vtkCylinderSource >::New();
    vtkSmartPointer< vtkPolyDataMapper > cylinderMapper = vtkSmartPointer< vtkPolyDataMapper >::New();
    vtkSmartPointer< vtkActor > cylinderActor = vtkSmartPointer< vtkActor >::New();

    cylinder->SetResolution( 8 );

    cylinderMapper->SetInputConnection( cylinder->GetOutputPort() );

    cylinderActor->SetMapper( cylinderMapper );

    cylinderActor->GetProperty()->SetColor( 1.0000, 0.3882, 0.2784 );
    cylinderActor->GetProperty()->SetOpacity( 1.0 );
    cylinderActor->RotateX( 30.0 );
    cylinderActor->RotateY( -45.0 );

    cylinderActor->GetProperty()->SetEdgeVisibility( 1 );
    cylinderActor->GetProperty()->SetEdgeColor( 0.9, 0.9, 0.4 );
    cylinderActor->GetProperty()->SetLineWidth( 6 );
    cylinderActor->GetProperty()->SetPointSize( 12 );
    cylinderActor->GetProperty()->SetRenderLinesAsTubes( 1 );
    cylinderActor->GetProperty()->SetRenderPointsAsSpheres( 1 );
    cylinderActor->GetProperty()->SetVertexVisibility( 1 );
    cylinderActor->GetProperty()->SetVertexColor( 0.5, 1.0, 0.8 );


    vtkNew< vtkSphereSource > sphere;
    vtkNew< vtkPolyDataMapper > sphereMapper;
    vtkNew< vtkActor > sphereActor;

    sphereMapper->SetInputConnection( sphere->GetOutputPort() );

    sphereActor->SetMapper( sphereMapper.Get() );
    sphereActor->GetProperty()->SetOpacity( 0.5 );

    m_renderer->AddActor( sphereActor.Get() );
    m_renderer->AddActor( cylinderActor );

    m_renderer->ResetCamera();
    m_renderer->GetActiveCamera()->Zoom( 1.5 );

    // An interactor
    //    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    //      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    //    renderWindowInteractor->SetRenderWindow(GetRenderWindow());

    vtkSmartPointer<vtkInteractorStyleTrackballCamera> tstyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    vtkSmartPointer<vtkInteractorStyleSwitch> sstyle = vtkSmartPointer<vtkInteractorStyleSwitch>::New();

    GetRenderWindow()->GetInteractor()->SetInteractorStyle( sstyle );

    GetRenderWindow()->SetAlphaBitPlanes( 1 );

    // Force to not pick a framebuffer with a multisample buffer ( as initial value is 8)
    GetRenderWindow()->SetMultiSamples( 0 );

    m_renderer->SetUseDepthPeeling( 1 );

    // Set depth peeling parameters.
    // Set the maximum number of rendering passes (initial value is 4)
    m_renderer->SetMaximumNumberOfPeels( 8 );

    // Set the occlusion ratio (initial value is 0.0, exact image)
    m_renderer->SetOcclusionRatio( 0.0 );

//    int depthPeelingWasUsed = m_renderer->GetLastRenderingUsedDepthPeeling();

    // Begin mouse interaction
    //    renderWindowInteractor->Start();
    //    GetRenderWindow()->SetInteractor( renderWindowInteractor );

//    renderVTK();
}


void VTKWidget::zoomToExtent()
{
    // Zoom to extent of last added actor
    vtkSmartPointer<vtkActor> actor = m_renderer->GetActors()->GetLastActor();

    if ( actor != nullptr )
    {
        m_renderer->ResetCamera( actor->GetBounds() );
    }

    renderVTK();
}


void VTKWidget::mouseDoubleClickEvent( QMouseEvent* event )
{
    QVTKOpenGLWidget::mouseDoubleClickEvent( event );
}

void VTKWidget::mouseMoveEvent( QMouseEvent* event )
{
    QVTKOpenGLWidget::mouseMoveEvent( event );

    //    std::cout << *(m_renderer->GetActiveCamera()->GetViewTransformMatrix()) << std::endl;
    //    m_renderer->GetActiveCamera()->GetViewTransformMatrix()->PrintSelf(std::cout, vtkIndent());
}

void VTKWidget::mousePressEvent( QMouseEvent* event )
{
    QVTKOpenGLWidget::mousePressEvent( event );
}

void VTKWidget::mouseReleaseEvent( QMouseEvent* event )
{
    QVTKOpenGLWidget::mouseReleaseEvent( event );
}

} // namespace gui
