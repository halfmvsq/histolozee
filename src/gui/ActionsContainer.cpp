#include "gui/ActionsContainer.h"


namespace gui
{

ActionsContainer::ActionsContainer(
        SetterType<InteractionModeType> interactionModeSetter,
        CrosshairsAlignerType crosshairsToActiveSlideAligner,
        CrosshairsAlignerType crosshairsToSlideStackAligner,
        CrosshairsAlignerType crosshairsToAnatomicalPlanesAligner,
        SetterType<bool> slideStackViews3dModeSetter,
        AllViewsResetterType viewResetter,
        QAction* refImageDockTogglerAction,
        QAction* slideStackDockTogglerAction,
        QObject* parent )
    :
      QObject( parent ),

      m_interactionModeSetter( interactionModeSetter ),
      m_crosshairsToActiveSlideAligner( crosshairsToActiveSlideAligner ),
      m_crosshairsToSlideStackAligner( crosshairsToSlideStackAligner ),
      m_crosshairsToAnatomicalPlanesAligner( crosshairsToAnatomicalPlanesAligner ),
      m_slideStackViews3dModeSetter( slideStackViews3dModeSetter ),
      m_allViewsResetter( viewResetter ),

      m_pointerModeSelectionGroup( new QActionGroup( this ) ),

      m_crosshairsAction( new QAction( "&Pointer" ) ),
      m_cameraTranslateAction( new QAction( "&Translate" ) ),
      m_cameraRotateAction( new QAction( "&Rotate" ) ),
      m_cameraZoomAction( new QAction( "&Zoom" ) ),
      m_refImageWindowLevelAction( new QAction( "&W/L" ) ),

      m_refImageRotateAction( new QAction( "Ref Image Rotate" ) ),
      m_refImageTranslateAction( new QAction( "Ref Image Translate" ) ),

      m_stackRotateAction( new QAction( "Stack Rotate" ) ),
      m_stackTranslateAction( new QAction( "Stack Translate" ) ),

      m_slideRotateAction( new QAction( "Slide Rotate" ) ),
      m_slideStretchAction( new QAction( "Slide Stretch" ) ),
      m_slideTranslateAction( new QAction( "Slide Translate" ) ),

      m_alignCrosshairsToActiveSlideAction( new QAction( "Align to Slide" ) ),
      m_alignCrosshairsToAnatomicalPlanesAction( new QAction( "Align to Anatomy" ) ),
      m_resetViewsAction( new QAction( "Reset Views" ) ),
      m_slideStackViews3dModeAction( new QAction( "Slide Stack 3D" ) ),

      m_refImageDockTogglerAction( refImageDockTogglerAction ),
      m_slideStackDockTogglerAction( slideStackDockTogglerAction )
{
    m_pointerModeSelectionGroup->setExclusive( true );

    m_crosshairsAction->setStatusTip( "Pointer" );
    m_crosshairsAction->setActionGroup( m_pointerModeSelectionGroup );
    m_crosshairsAction->setCheckable( true );
    m_crosshairsAction->setIcon( QIcon( ":/toolbars/icons8-center-of-gravity-100.png" ) );

    m_cameraTranslateAction->setStatusTip( "Translate" );
    m_cameraTranslateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_cameraTranslateAction->setCheckable( true );
    m_cameraTranslateAction->setIcon( QIcon( ":/toolbars/icons8-hand-100.png" ) );

    m_cameraRotateAction->setStatusTip( "Rotate" );
    m_cameraRotateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_cameraRotateAction->setCheckable( true );
    m_cameraRotateAction->setIcon( QIcon( ":/toolbars/icons8-3d-rotate-96.png" ) );

    m_cameraZoomAction->setStatusTip( "Zoom" );
    m_cameraZoomAction->setActionGroup( m_pointerModeSelectionGroup );
    m_cameraZoomAction->setCheckable( true );
    m_cameraZoomAction->setIcon( QIcon( ":/toolbars/icons8-focal-length-100.png" ) );

    m_refImageWindowLevelAction->setStatusTip( "W/L" );
    m_refImageWindowLevelAction->setActionGroup( m_pointerModeSelectionGroup );
    m_refImageWindowLevelAction->setCheckable( true );
    m_refImageWindowLevelAction->setIcon( QIcon( ":/toolbars/icons8-automatic-contrast-100.png" ) );


    m_refImageRotateAction->setStatusTip( "Ref Image Rotate" );
    m_refImageRotateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_refImageRotateAction->setCheckable( true );
    m_refImageRotateAction->setIcon( QIcon( ":/toolbars/icons8-rotate-96.png" ) );

    m_refImageTranslateAction->setStatusTip( "Ref Image Translate" );
    m_refImageTranslateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_refImageTranslateAction->setCheckable( true );
    m_refImageTranslateAction->setIcon( QIcon( ":/toolbars/icons8-move-96.png" ) );


    m_stackRotateAction->setStatusTip( "Stack Rotate" );
    m_stackRotateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_stackRotateAction->setCheckable( true );
    m_stackRotateAction->setIcon( QIcon( ":/toolbars/icons8-rotate-96.png" ) );

    m_stackTranslateAction->setStatusTip( "Stack Translate" );
    m_stackTranslateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_stackTranslateAction->setCheckable( true );
    m_stackTranslateAction->setIcon( QIcon( ":/toolbars/icons8-move-96.png" ) );


    m_slideRotateAction->setStatusTip( "Slide Rotate" );
    m_slideRotateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_slideRotateAction->setCheckable( true );

    m_slideStretchAction->setStatusTip( "Slide Stretch" );
    m_slideStretchAction->setActionGroup( m_pointerModeSelectionGroup );
    m_slideStretchAction->setCheckable( true );

    m_slideTranslateAction->setStatusTip( "Slide Translate" );
    m_slideTranslateAction->setActionGroup( m_pointerModeSelectionGroup );
    m_slideTranslateAction->setCheckable( true );



    m_alignCrosshairsToActiveSlideAction->setStatusTip( "Align Crosshairs to Slide" );
    m_alignCrosshairsToAnatomicalPlanesAction->setStatusTip( "Align Crosshairs to Anatomical Planes" );
    m_resetViewsAction->setStatusTip( "Reset Views" );

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

} // namespace gui
