#include "gui/ActionsContainer.h"
#include "gui/MainWindow.h"

#include <boost/filesystem.hpp>
#include <QFileDialog>
#include <iostream>


namespace gui
{

ActionsContainer::ActionsContainer(
        SetterType<InteractionModeType> interactionModeSetter,
        CrosshairsAlignerType crosshairsToActiveSlideAligner,
        CrosshairsAlignerType crosshairsToSlideStackAligner,
        CrosshairsAlignerType crosshairsToAnatomicalPlanesAligner,
        SetterType<bool> slideStackViews3dModeSetter,
        AllViewsResetterType viewResetter,
        ProjectSaverType projectSaver,

        QAction* refImageDockTogglerAction,
        QAction* slideStackDockTogglerAction,
        QMainWindow* mainWindow,
        QObject* parent )
    :
      QObject( parent ),
      m_mainWindow( mainWindow ),

      m_interactionModeSetter( interactionModeSetter ),
      m_crosshairsToActiveSlideAligner( crosshairsToActiveSlideAligner ),
      m_crosshairsToSlideStackAligner( crosshairsToSlideStackAligner ),
      m_crosshairsToAnatomicalPlanesAligner( crosshairsToAnatomicalPlanesAligner ),
      m_slideStackViews3dModeSetter( slideStackViews3dModeSetter ),
      m_allViewsResetter( viewResetter ),
      m_projectSaver( projectSaver ),

      m_pointerModeSelectionGroup( new QActionGroup( this ) ),

      m_crosshairsAction( new QAction( "&Crosshairs" ) ),
      m_cameraTranslateAction( new QAction( "&Pan View" ) ),
      m_cameraRotateAction( new QAction( "&Rotate View" ) ),
      m_cameraZoomAction( new QAction( "&Zoom View" ) ),
      m_refImageWindowLevelAction( new QAction( "&Window/Level Reference Image" ) ),

      m_refImageRotateAction( new QAction( "Rotate Reference Image" ) ),
      m_refImageTranslateAction( new QAction( "Translate Reference Image" ) ),

      m_stackRotateAction( new QAction( "Rotate Slide Stack" ) ),
      m_stackTranslateAction( new QAction( "Translate Slide Stack" ) ),

      m_slideRotateAction( new QAction( "Rotate Slide" ) ),
      m_slideStretchAction( new QAction( "Stretch Slide" ) ),
      m_slideTranslateAction( new QAction( "Translate Slide" ) ),

      m_saveProjectAction( new QAction( "Save Project" ) ),
      m_saveProjectAsAction( new QAction( "Save Project As..." ) ),

      m_alignCrosshairsToActiveSlideAction( new QAction( "Align Crosshairs to Slide Stack" ) ),
      m_alignCrosshairsToAnatomicalPlanesAction( new QAction( "Align Crosshairs to Anatomy" ) ),
      m_resetViewsAction( new QAction( "Reset Views" ) ),
      m_slideStackViews3dModeAction( new QAction( "Slide Stack 3D" ) ),

      m_refImageDockTogglerAction( refImageDockTogglerAction ),
      m_slideStackDockTogglerAction( slideStackDockTogglerAction )
{
    m_pointerModeSelectionGroup->setExclusive( true );

    m_crosshairsAction->setStatusTip( "Crosshairs" );
    m_crosshairsAction->setActionGroup( m_pointerModeSelectionGroup );
    m_crosshairsAction->setCheckable( true );
    m_crosshairsAction->setIcon( QIcon( ":/toolbars/icons8-cursor-96.png" ) );

    m_cameraTranslateAction->setStatusTip( "Pan View" );
    m_cameraTranslateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_cameraTranslateAction->setCheckable( true );
    m_cameraTranslateAction->setIcon( QIcon( ":/toolbars/icons8-hand-96.png" ) );

    m_cameraRotateAction->setStatusTip( "Rotate View" );
    m_cameraRotateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_cameraRotateAction->setCheckable( true );
    m_cameraRotateAction->setIcon( QIcon( ":/toolbars/icons8-rotate-camera-96.png" ) );

    m_cameraZoomAction->setStatusTip( "Zoom View" );
    m_cameraZoomAction->setActionGroup( m_pointerModeSelectionGroup );
    m_cameraZoomAction->setCheckable( true );
    m_cameraZoomAction->setIcon( QIcon( ":/toolbars/icons8-zoom-in-96.png" ) );


    m_refImageRotateAction->setStatusTip( "Rotate Reference Image" );
    m_refImageRotateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_refImageRotateAction->setCheckable( true );
    m_refImageRotateAction->setIcon( QIcon( ":/toolbars/icons8-rotate-96.png" ) );

    m_refImageTranslateAction->setStatusTip( "Translate Reference Image" );
    m_refImageTranslateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_refImageTranslateAction->setCheckable( true );
    m_refImageTranslateAction->setIcon( QIcon( ":/toolbars/icons8-move-96.png" ) );

    m_refImageWindowLevelAction->setStatusTip( "Window/Level" );
    m_refImageWindowLevelAction->setActionGroup( m_pointerModeSelectionGroup );
    m_refImageWindowLevelAction->setCheckable( true );
    m_refImageWindowLevelAction->setIcon( QIcon( ":/toolbars/icons8-automatic-contrast-96.png" ) );


    m_stackRotateAction->setStatusTip( "Rotate Slide Stack" );
    m_stackRotateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_stackRotateAction->setCheckable( true );
    m_stackRotateAction->setIcon( QIcon( ":/toolbars/icons8-3d-rotate-96.png" ) );

    m_stackTranslateAction->setStatusTip( "Translate Slide Stack" );
    m_stackTranslateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_stackTranslateAction->setCheckable( true );
    m_stackTranslateAction->setIcon( QIcon( ":/toolbars/icons8-portraits-96.png" ) );


    m_slideRotateAction->setStatusTip( "Rotate Slide" );
    m_slideRotateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_slideRotateAction->setCheckable( true );
    m_slideRotateAction->setIcon( QIcon( ":/toolbars/icons8-manual-page-rotation-96.png" ) );

    m_slideStretchAction->setStatusTip( "Stretch Slide" );
    m_slideStretchAction->setActionGroup( m_pointerModeSelectionGroup );
    m_slideStretchAction->setCheckable( true );
    m_slideStretchAction->setIcon( QIcon( ":/toolbars/icons8-resize-96.png" ) );

    m_slideTranslateAction->setStatusTip( "Translate Slide" );
    m_slideTranslateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_slideTranslateAction->setCheckable( true );
    m_slideTranslateAction->setIcon( QIcon( ":/toolbars/icons8-fit-to-page-96.png" ) );

    m_alignCrosshairsToActiveSlideAction->setStatusTip( "Align Crosshairs to Slide Stack" );
    m_alignCrosshairsToActiveSlideAction->setIcon( QIcon( ":/toolbars/icons8-ruler-combined-96.png" ) );

    m_alignCrosshairsToAnatomicalPlanesAction->setStatusTip( "Align Crosshairs to Anatomical Planes" );
    m_alignCrosshairsToAnatomicalPlanesAction->setIcon( QIcon( ":/toolbars/icons8-head-profile-96.png" ) );

    m_resetViewsAction->setStatusTip( "Reset Views" );
    m_resetViewsAction->setIcon( QIcon( ":/toolbars/icons8-target-96.png" ) );

    //    icons8-opened-folder-96.png

    m_saveProjectAction->setStatusTip( "Save Project" );
    m_saveProjectAction->setIcon( QIcon( ":/toolbars/icons8-save-96.png") );

    m_saveProjectAsAction->setStatusTip( "Save Project As..." );
    m_saveProjectAsAction->setIcon( QIcon( ":/toolbars/icons8-save-as-96.png") );

    m_slideStackViews3dModeAction->setStatusTip( "Slide Stack 3D" );
    m_slideStackViews3dModeAction->setCheckable( true );


    createConnections();
}


QActionGroup* ActionsContainer::pointerModeActionGroup()
{
    return m_pointerModeSelectionGroup;
}

QAction* ActionsContainer::pointerAction()
{
    return m_crosshairsAction;
}

QAction* ActionsContainer::translateAction()
{
    return m_cameraTranslateAction;
}

QAction* ActionsContainer::rotateAction()
{
    return m_cameraRotateAction;
}

QAction* ActionsContainer::zoomAction()
{
    return m_cameraZoomAction;
}

QAction* ActionsContainer::windowLevelAction()
{
    return m_refImageWindowLevelAction;
}

QAction* ActionsContainer::refImageRotateAction()
{
    return m_refImageRotateAction;
}

QAction* ActionsContainer::refImageTranslateAction()
{
    return m_refImageTranslateAction;
}

QAction* ActionsContainer::stackRotateAction()
{
    return m_stackRotateAction;
}

QAction* ActionsContainer::stackTranslateAction()
{
    return m_stackTranslateAction;
}

QAction* ActionsContainer::slideRotateAction()
{
    return m_slideRotateAction;
}

QAction* ActionsContainer::slideStretchAction()
{
    return m_slideStretchAction;
}

QAction* ActionsContainer::slideTranslateAction()
{
    return m_slideTranslateAction;
}

QAction* ActionsContainer::saveProjectAction()
{
    return m_saveProjectAction;
}

QAction* ActionsContainer::saveProjectAsAction()
{
    return m_saveProjectAsAction;
}

QAction* ActionsContainer::alignCrosshairsToSlideAction()
{
    return m_alignCrosshairsToActiveSlideAction;
}

QAction* ActionsContainer::alignCrosshairsToAnatomicalPlanes()
{
    return m_alignCrosshairsToAnatomicalPlanesAction;
}

QAction* ActionsContainer::resetViewsAction()
{
    return m_resetViewsAction;
}

QAction* ActionsContainer::slideStackViews3dModeAction()
{
    return m_slideStackViews3dModeAction;
}

QAction* ActionsContainer::refImageDockTogglerAction()
{
    return m_refImageDockTogglerAction;
}

QAction* ActionsContainer::slideStackDockTogglerAction()
{
    return m_slideStackDockTogglerAction;
}


void ActionsContainer::createConnections()
{
    auto& S = m_interactionModeSetter;

    connect( m_crosshairsAction, &QAction::triggered, [&S]() { if ( S ) S( InteractionModeType::CrosshairsPointer ); } );
    connect( m_cameraTranslateAction, &QAction::triggered, [&S]() { if ( S ) S( InteractionModeType::CameraTranslate ); } );
    connect( m_cameraRotateAction, &QAction::triggered, [&S]() { if ( S ) S( InteractionModeType::CameraRotate ); } );
    connect( m_cameraZoomAction, &QAction::triggered, [&S]() { if ( S ) S( InteractionModeType::CameraZoom ); } );

    connect( m_refImageRotateAction, &QAction::triggered, [&S]() { if ( S ) S( InteractionModeType::RefImageRotate ); } );
    connect( m_refImageTranslateAction, &QAction::triggered, [&S]() { if ( S ) S( InteractionModeType::RefImageTranslate ); } );
    connect( m_refImageWindowLevelAction, &QAction::triggered, [&S]() { if ( S ) S( InteractionModeType::RefImageWindowLevel ); } );

    connect( m_stackRotateAction, &QAction::triggered, [&S]() { if ( S ) S( InteractionModeType::StackRotate ); } );
    connect( m_stackTranslateAction, &QAction::triggered, [&S]() { if ( S ) S( InteractionModeType::StackTranslate ); } );

    connect( m_slideRotateAction, &QAction::triggered, [&S]() { if ( S ) S( InteractionModeType::SlideRotate ); } );
    connect( m_slideStretchAction, &QAction::triggered, [&S]() { if ( S ) S( InteractionModeType::SlideStretch ); } );
    connect( m_slideTranslateAction, &QAction::triggered, [&S]() { if ( S ) S( InteractionModeType::SlideTranslate ); } );

    connect( m_saveProjectAction, &QAction::triggered, [this] () { if ( m_projectSaver ) m_projectSaver( std::nullopt ); } );
    connect( m_saveProjectAsAction, &QAction::triggered, [this] () { saveProjectAs(); } );

    connect( m_alignCrosshairsToActiveSlideAction, &QAction::triggered,
             [this]() { if ( m_crosshairsToSlideStackAligner ) {
            m_crosshairsToSlideStackAligner(); } } );

    connect( m_alignCrosshairsToAnatomicalPlanesAction, &QAction::triggered,
             [this]() { if ( m_crosshairsToAnatomicalPlanesAligner ) {
            m_crosshairsToAnatomicalPlanesAligner(); } } );

    connect( m_resetViewsAction, &QAction::triggered,
             [this]() { if ( m_allViewsResetter ) m_allViewsResetter(); } );

    connect( m_slideStackViews3dModeAction, &QAction::triggered,
             [this]() { if ( m_slideStackViews3dModeSetter ) {
            m_slideStackViews3dModeSetter( m_slideStackViews3dModeAction->isChecked() ); } } );

    // Set default checked states:

    m_crosshairsAction->setChecked( true ); // default selection
    m_refImageWindowLevelAction->setChecked( false );
    m_cameraTranslateAction->setChecked( false );
    m_cameraRotateAction->setChecked( false );
    m_cameraZoomAction->setChecked( false );

    m_refImageRotateAction->setChecked( false );
    m_refImageTranslateAction->setChecked( false );

    m_stackRotateAction->setChecked( false );
    m_stackTranslateAction->setChecked( false );

    m_slideRotateAction->setChecked( false );
    m_slideStretchAction->setChecked( false );
    m_slideTranslateAction->setChecked( false );

    m_slideStackViews3dModeAction->setChecked( false );
}


void ActionsContainer::saveProjectAs()
{
    QStringList filters;

    filters << "All files (*.*)"
            << "JSON files (*.json)";

    QFileDialog dialog( m_mainWindow, "Save HistoloZee Project" );
    dialog.setFileMode( QFileDialog::AnyFile );
    dialog.setNameFilters( filters );
    dialog.selectNameFilter( "JSON files (*.json)" );
    dialog.setAcceptMode( QFileDialog::AcceptSave );
    dialog.setViewMode( QFileDialog::Detail );

    if ( QDialog::Accepted != dialog.exec() ||
         dialog.selectedFiles().empty() )
    {
        return;
    }

    if ( m_projectSaver )
    {
        const std::string filename = dialog.selectedFiles().first().toStdString();
        std::cout << "Saving project to " << filename << std::endl;
        m_projectSaver( filename );
    }
}

} // namespace gui
