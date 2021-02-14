#include "gui/toolbars/ToolBarCreation.h"

#include <QLabel>


namespace gui
{

QToolBar* createPointerToolBar( ActionsContainer& actions )
{
    QToolBar* toolbar = new QToolBar( "Toolbar" );

    toolbar->setMovable( false );
    toolbar->setFloatable( false );
    toolbar->setOrientation( Qt::Horizontal );
    toolbar->setIconSize( QSize( 20, 20 ) );

    toolbar->addWidget( new QLabel( " View:" ) );
    toolbar->addAction( actions.pointerAction() );
    toolbar->addAction( actions.translateAction() );
    toolbar->addAction( actions.rotateAction() );
    toolbar->addAction( actions.zoomAction() );

    toolbar->addSeparator();
    toolbar->addAction( actions.alignCrosshairsToSlideAction() );
    toolbar->addAction( actions.alignCrosshairsToAnatomicalPlanes() );
    toolbar->addAction( actions.resetViewsAction() );

    toolbar->addSeparator();
    toolbar->addWidget( new QLabel( "Image:" ) );
    toolbar->addAction( actions.refImageTranslateAction() );
    toolbar->addAction( actions.refImageRotateAction() );
    toolbar->addAction( actions.windowLevelAction() );

    toolbar->addSeparator();
    toolbar->addWidget( new QLabel( "Stack:" ) );
    toolbar->addAction( actions.stackTranslateAction() );
    toolbar->addAction( actions.stackRotateAction() );

    toolbar->addSeparator();
    toolbar->addWidget( new QLabel( "Slide:" ) );
    toolbar->addAction( actions.slideTranslateAction() );
    toolbar->addAction( actions.slideRotateAction() );
    toolbar->addAction( actions.slideStretchAction() );

    toolbar->addSeparator();
    toolbar->addWidget( new QLabel( "Project:" ) );
    toolbar->addAction( actions.saveProjectAction() );
    toolbar->addAction( actions.saveProjectAsAction() );

    QWidget* spacerWidget = new QWidget();
    spacerWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    spacerWidget->setVisible( true );
    toolbar->addWidget( spacerWidget );

    toolbar->addAction( actions.refImageDockTogglerAction() );
    toolbar->addAction( actions.slideStackDockTogglerAction() );

    return toolbar;
}

} // namespace gui
