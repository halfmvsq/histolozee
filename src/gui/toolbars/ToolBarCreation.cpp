#include "gui/toolbars/ToolBarCreation.h"


namespace gui
{

QToolBar* createPointerToolBar( ActionsContainer& actions )
{
    QToolBar* toolbar = new QToolBar( "Toolbar" );

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
    toolbar->addSeparator();

    toolbar->addAction( actions.saveProject() );

    QWidget* spacerWidget1 = new QWidget();
    spacerWidget1->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    spacerWidget1->setVisible( true );
    toolbar->addWidget( spacerWidget1 );

    toolbar->addAction( actions.refImageDockTogglerAction() );
    toolbar->addAction( actions.slideStackDockTogglerAction() );

    return toolbar;
}

} // namespace gui
