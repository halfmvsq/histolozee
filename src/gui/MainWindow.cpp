#include "gui/MainWindow.h"

#include "common/HZeeException.hpp"

#include <boost/filesystem.hpp>

#include <QAction>
#include <QActionGroup>
#include <QFileDialog>
#include <QGradient>
#include <QIcon>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QStatusBar>
#include <QTabBar>
#include <QTabWidget>
#include <QToolBar>

#include <iostream>


//#ifndef QT_NO_CURSOR
//    switch (gesture->state()) {
//        case Qt::GestureStarted:
//        case Qt::GestureUpdated:
//            setCursor(Qt::SizeAllCursor);
//            break;
//        default:
//            setCursor(Qt::ArrowCursor);
//    }
//#endif

namespace
{

static const std::string sk_glContextErrorMsg(
        "The global shared OpenGL context could not be made current." );

} // anonymous


namespace gui
{

MainWindow::MainWindow( QWidget* parent )
    :
      QMainWindow( parent ),
      m_viewLayoutTabChangedPublisher( nullptr )
{
    createUI();
}

void MainWindow::setViewLayoutTabChangedPublisher( ViewLayoutTabChangedPublisher publisher )
{
    m_viewLayoutTabChangedPublisher = publisher;
}

void MainWindow::setImageLoader( ImageLoaderType loader )
{
    m_imageLoader = loader;
}

void MainWindow::setParcellationLoader( ImageLoaderType loader )
{
    m_parcellationLoader = loader;
}

void MainWindow::setSlideLoader( SlideLoaderType loader )
{
    m_slideLoader = loader;
}


void MainWindow::keyPressEvent( QKeyEvent* event )
{
    if ( Qt::Key_Escape == event->key() )
    {
        close();
        return;
    }

    QMainWindow::keyPressEvent( event );
}


void MainWindow::resizeEvent( QResizeEvent* event )
{
    QMainWindow::resizeEvent( event );

    if ( m_memoryUseProgressbar )
    {
        const int width = ( event ) ? event->size().width() / 8 : 256;
        m_memoryUseProgressbar->setFixedWidth( width );
    }
}


void MainWindow::createActions()
{
    QList< QKeySequence > keyList;

    m_importRefImageAction = new QAction( tr("Import Image..." ), this );
    m_importRefImageAction->setIconVisibleInMenu( false );
    m_importRefImageAction->setCheckable( false );
    m_importRefImageAction->setChecked( false );
    m_importRefImageAction->setStatusTip( "Import 3D reference image" );
    m_importRefImageAction->setToolTip( "Import 3D reference image" );
    m_importRefImageAction->setWhatsThis( "Import 3D reference image" );

    connect( m_importRefImageAction, &QAction::triggered,
             this, &MainWindow::importImage );


    m_importParcellationAction = new QAction( tr("Import Labels..." ), this );
    m_importParcellationAction->setIconVisibleInMenu( false );
    m_importParcellationAction->setCheckable( false );
    m_importParcellationAction->setChecked( false );
    m_importParcellationAction->setStatusTip( "Import 3D label image" );
    m_importParcellationAction->setToolTip( "Import 3D label image" );
    m_importParcellationAction->setWhatsThis( "Import 3D label image" );

    connect( m_importParcellationAction, &QAction::triggered,
             this, &MainWindow::importParcellation );


    m_insertSlidesAction = new QAction( tr( "&Insert Slide(s)..." ), this );
    m_insertSlidesAction->setIconVisibleInMenu( false );
    m_insertSlidesAction->setStatusTip( "Insert existing slide(s) into stack" );
    m_insertSlidesAction->setToolTip( "Insert existing slide(s) into stack" );
    m_insertSlidesAction->setWhatsThis( "Insert existing slide(s) into stack" );

    keyList.clear();
    keyList << QKeySequence( Qt::SHIFT + Qt::CTRL + Qt::Key_N );
    m_insertSlidesAction->setShortcuts( keyList );

    connect( m_insertSlidesAction, &QAction::triggered,
             this, &MainWindow::insertSlides );


    m_imagesActionGroup = new QActionGroup( this );
    m_imagesActionGroup->setExclusive( false );
    m_imagesActionGroup->addAction( m_importRefImageAction );
    m_imagesActionGroup->addAction( m_importParcellationAction );

    m_stackActionGroup = new QActionGroup( this );
    m_stackActionGroup->setExclusive( false );
    m_stackActionGroup->addAction( m_insertSlidesAction );
}

void MainWindow::createMenuBar()
{
    QMenuBar* menu = menuBar();

    if ( ! menu )
    {
        return;
    }

    m_fileMenu = menu->addMenu( tr("&File") );
    m_fileMenu->addAction( m_importRefImageAction );
    m_fileMenu->addAction( m_importParcellationAction );
    m_fileMenu->addSeparator();
    m_fileMenu->addAction( m_insertSlidesAction );
}

void MainWindow::clearViewLayoutTabs()
{
    if ( m_viewLayoutTabWidget )
    {
        m_viewLayoutTabWidget->clear();
    }
}

void MainWindow::insertViewLayoutTab( int index, QWidget* tab, const std::string& name )
{
    // Add the layout widgets to the tab widget in order
    if ( m_viewLayoutTabWidget && tab )
    {
        const QString qname( name.c_str() );
        m_viewLayoutTabWidget->insertTab( index, tab, qname );
        m_viewLayoutTabWidget->setTabToolTip( index, qname );
    }
}

void MainWindow::setWorldPositionStatusText( const std::string& status )
{
    m_worldPosStatus->setText( status.c_str() );
}

void MainWindow::setImageValueStatusText( const std::string& status )
{
    m_imageValueStatus->setText( status.c_str() );
}

void MainWindow::setLabelValueStatusText( const std::string& status )
{
    m_labelValueStatus->setText( status.c_str() );
}

void MainWindow::createViewLayoutTabWidget()
{
    // QMainWindow takes ownership of the widget and its children,
    // deleting them all at the appropriate time

    m_viewLayoutTabWidget = new QTabWidget( this );

//    m_viewLayoutTabWidget->tabBar()->setBackgroundRole( QPalette::Button );
//    m_viewLayoutTabWidget->tabBar()->setForegroundRole( QPalette::ButtonText );
    m_viewLayoutTabWidget->setMovable( true );
    m_viewLayoutTabWidget->setDocumentMode( false );
    m_viewLayoutTabWidget->setStyleSheet( "QTabWidget::pane { border: 0; }" );
    //m_viewLayoutTabWidget->setStyleSheet( "QTabWidget::pane { border: 0; }\n QTabWidget::tab-bar{alignment:left; }" );
    m_viewLayoutTabWidget->setTabPosition( QTabWidget::TabPosition::South );
    m_viewLayoutTabWidget->setTabsClosable( false );
    m_viewLayoutTabWidget->setContentsMargins( 0, 0, 0, 0 );


    // Give the view layout tab widget a nice background gradient.
    // The gradient preset is defined at webgradients.com
    QGradient gradient( QGradient::Preset::ViciousStance );
//    QGradient gradient( QGradient::Preset::MountainRock );
//    QGradient gradient( QGradient::Preset::EternalConstance );
//    gradient.setCoordinateMode( QGradient::ObjectMode );

    QBrush brush( gradient );
    QPalette palette;
    palette.setBrush( QPalette::Window, brush );

    // Note: if this palette is applied to MainWindow, then it will propagate
    // to all of its children (e.g. the dock wigets)
    m_viewLayoutTabWidget->setAutoFillBackground( true );
    m_viewLayoutTabWidget->setPalette( palette );


    // Publish that current tab changed. This is used to update the views in the layouts.
    auto layoutTabChanged = [this] ( int tabIndex )
    {
        if ( m_viewLayoutTabChangedPublisher )
        {
            m_viewLayoutTabChangedPublisher( tabIndex );
        }
    };

    connect( m_viewLayoutTabWidget, &QTabWidget::currentChanged, layoutTabChanged );
}

void MainWindow::createStatusBar()
{
    if ( statusBar() )
    {
        statusBar()->setSizeGripEnabled( true );

        m_memoryUseStatus = new QLabel;
        m_memoryUseStatus->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
        m_memoryUseStatus->setText( "Memory use:" );

        m_memoryUseProgressbar = new QProgressBar;
        m_memoryUseProgressbar->setRange( 0, 8196 );
        m_memoryUseProgressbar->setValue( 2048 );
        m_memoryUseProgressbar->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
        m_memoryUseProgressbar->setOrientation( Qt::Orientation::Horizontal );
        m_memoryUseProgressbar->setTextVisible( true );

        m_worldPosStatus = new QLabel;
        m_worldPosStatus->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

        m_imageValueStatus = new QLabel;
        m_imageValueStatus->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

        m_labelValueStatus = new QLabel;
        m_labelValueStatus->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

        statusBar()->addWidget( m_memoryUseStatus, 0 );
        statusBar()->addWidget( m_memoryUseProgressbar, 0 );

        statusBar()->addPermanentWidget( m_worldPosStatus, 0 );
        statusBar()->addPermanentWidget( m_imageValueStatus, 0 );
        statusBar()->addPermanentWidget( m_labelValueStatus, 0 );

        //    statusBar()->showMessage( "HELLO", 0 );
    }
}

void MainWindow::createUI()
{
    createActions();
    createMenuBar();
    createStatusBar();
    createViewLayoutTabWidget();

    setCentralWidget( m_viewLayoutTabWidget );
    setContentsMargins( 0, 0, 0, 0 );

    setWindowTitle( tr("HistoloZee") );
}


void MainWindow::importImage()
{
    QStringList filters;
    filters
            << "All files (*.*)"
            << "Analyze images (*.hdr *.img)"
            << "DICOM series (*.dcm)"
            << "MetaImage images (*.mhd *.mhd.gz)"
            << "NIfTI images (*.nii *.nii.gz)"
            << "NRRD images (*.nrdd *.nhdr)";

    QFileDialog dialog( this );
    dialog.setWindowTitle( "Import 3D Reference Image" );
    dialog.setFileMode( QFileDialog::Directory ); // ExistingFile
    dialog.setNameFilters( filters );
    dialog.selectNameFilter( "All files (*.*)" );
    dialog.setAcceptMode( QFileDialog::AcceptOpen );
    dialog.setViewMode( QFileDialog::Detail );

    std::string filename;

    if ( QDialog::Accepted != dialog.exec() )
    {
        return;
    }

    filename = dialog.selectedFiles().first().toStdString();

    if ( ! boost::filesystem::exists( filename ) )
    {
        std::cerr << "Invalid file '" << filename << "'" << std::endl;
        return;
    }

    /// @todo Need to query the DICOM series UID list, if the user has
    /// chosen a file/folder with multiple series in it
    const std::optional< std::string > dicomSeriesUID = std::nullopt;

    /// @todo remove from here. it's in AppController now, but doesn't belong there either.
//    loadImage( filename, dicomSeriesUID );

    //    createReferenceImageIsoSurfaceMeshes();

    //    createLabelImageTexture(); /// may be redundant
    //    createLabelMeshes(); /// may be redundant
}

void MainWindow::importParcellation()
{
}

void MainWindow::insertSlides()
{
}

} // namespace gui
