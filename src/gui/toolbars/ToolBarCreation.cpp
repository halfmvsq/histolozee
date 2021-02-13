#include "gui/toolbars/ToolBarCreation.h"


namespace gui
{

QToolBar* createPointerToolBar( ActionsContainer& actions )
{
    QToolBar* toolbar = new QToolBar( "Pointer Toolbar" );

    toolbar->setMovable( false );
    toolbar->setFloatable( false );
    toolbar->setOrientation( Qt::Horizontal );
    toolbar->setIconSize( QSize( 16, 16 ) );

    toolbar->addAction( actions.pointerAction() );
    toolbar->addAction( actions.translateAction() );
    toolbar->addAction( actions.rotateAction() );
    toolbar->addAction( actions.zoomAction() );
    toolbar->addAction( actions.windowLevelAction() );
    toolbar->addSeparator();

    toolbar->addAction( actions.refImageTranslateAction() );
    toolbar->addAction( actions.refImageRotateAction() );
    toolbar->addSeparator();

    toolbar->addAction( actions.stackTranslateAction() );
    toolbar->addAction( actions.stackRotateAction() );
    toolbar->addSeparator();

    toolbar->addAction( actions.slideTranslateAction() );
    toolbar->addAction( actions.slideRotateAction() );
    toolbar->addAction( actions.slideStretchAction() );
    toolbar->addSeparator();

    toolbar->addAction( actions.alignCrosshairsToSlideAction() );
    toolbar->addAction( actions.alignCrosshairsToAnatomicalPlanes() );
    toolbar->addAction( actions.resetViewsAction() );


    QWidget* spacerWidget = new QWidget();
    spacerWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    spacerWidget->setVisible( true );

    toolbar->addWidget( spacerWidget );

    toolbar->addAction( actions.refImageDockTogglerAction() );
    toolbar->addAction( actions.slideStackDockTogglerAction() );

    return toolbar;
}

} // namespace gui
