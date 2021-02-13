#include "gui/docks/RefFrameEditorDock.h"
#include "gui/docks/Utility.h"
#include "gui/docks/labels/DoubleSpinBoxDelegate.h"
#include "gui/docks/labels/LabelColorDialog.h"
#include "gui/docks/labels/LabelColorDialogDelegate.h"
#include "gui/docks/labels/LabelTableModel.h"
#include "gui/docks/labels/OpacitySpinBox.h"
#include "gui/messages/parcellation/ParcellationLabelData.h"
//#include "gui/treemodel/TreeItem.h"
#include "gui/treemodel/TreeModel.h"

#include "common/HZeeException.hpp"

#include "externals/ctk/Widgets/ctkCollapsibleGroupBox.h"
#include "externals/ctk/Widgets/ctkDoubleRangeSlider.h"
#include "externals/ctk/Widgets/ctkDoubleSlider.h"
#include "externals/ctk/Widgets/ctkDoubleSpinBox.h"
#include "externals/ctk/Widgets/ctkMatrixWidget.h"
#include "externals/ctk/Widgets/ctkPathLineEdit.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemEditorFactory>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QTableView>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

#include <list>
#include <string>


namespace
{

/// Optionally accentuate group box labels with bold font
static constexpr bool sk_useBoldGroupBoxFont = false;

static const QString sk_scrollAreaStyleSheet(
            "QScrollArea { background: transparent; }"
            "QScrollArea > QWidget > QWidget { background: transparent; }"
            "QScrollArea > QWidget > QScrollBar { background: palette(base); }" );


/**
 * @brief Convert an image color map item's "icon buffer" into a \c QIcon object.
 * @param item Image color map item
 * @param iconSize Desired output \c QIcon size
 * @return Icon
 */
QIcon makeQIconFromColorMapItem( const gui::ImageColorMapItem& item, QSize iconSize )
{
    const size_t iconWidth = item.m_iconBuffer.size() / 4;

    QImage image( item.m_iconBuffer.data(), iconWidth, 1,
                  QImage::Format::Format_RGBA8888_Premultiplied );

    return QIcon( QPixmap::fromImage( image ).
                  scaled( iconSize, Qt::AspectRatioMode::IgnoreAspectRatio,
                          Qt::TransformationMode::SmoothTransformation ) );
}


void setTableHeader( QTableWidget* W, const std::vector< std::pair< std::string, std::string > >& items )
{
    W->setRowCount( static_cast<int>( items.size() ) );
    W->setColumnCount( 2 );

    for ( size_t i = 0; i < items.size(); ++i )
    {
        const auto& item = items.at( i );

        auto propText = QString::fromStdString( item.first );
        auto valueText = QString::fromStdString( item.second );

        auto* propItem = new QTableWidgetItem( propText );
        auto* valueItem = new QTableWidgetItem( valueText );

        propItem->setToolTip( propText );
        valueItem->setToolTip( valueText );

        W->setItem( static_cast<int>( i ), 0, propItem );
        W->setItem( static_cast<int>( i ), 1, valueItem );
    }

    W->resizeColumnsToContents();
}


/// @see https://stackoverflow.com/questions/42458735/how-do-i-adjust-a-qtableview-height-according-to-contents
void verticalResizeTableViewToContents( QTableView* tableView, int minRowCount = 2 )
{
    // Rows height
    const int count = std::min( tableView->verticalHeader()->count(), minRowCount );

    int rowTotalHeight = 0;

    for ( int i = 0; i < count; ++i )
    {
        // Only account for row if it is visible
        if ( ! tableView->verticalHeader()->isSectionHidden( i ) )
        {
            rowTotalHeight += tableView->verticalHeader()->sectionSize( i );
        }
    }

    // Check for scrollbar visibility
    if ( ! tableView->horizontalScrollBar()->isHidden() )
    {
        rowTotalHeight += tableView->horizontalScrollBar()->height();
    }

    // Check for header visibility
    if ( ! tableView->horizontalHeader()->isHidden() )
    {
        rowTotalHeight += tableView->horizontalHeader()->height();
    }

    tableView->setMinimumHeight( rowTotalHeight );
}

} // anonymous


namespace gui
{

RefFrameEditorDock::RefFrameEditorDock( QWidget* parent )
    :
      QDockWidget( parent ),

      m_imageSelectionsPublisher( nullptr ),
      m_parcelSelectionsPublisher( nullptr ),

      m_imagePropertiesPartialPublisher( nullptr ),
      m_imageTransformationPublisher( nullptr ),
      m_parcelPropertiesPartialPublisher( nullptr ),
      m_parcelLabelsPartialPublisher( nullptr ),

      m_imageSelectionsResponder( nullptr ),
      m_parcelSelectionsResponder( nullptr ),

      m_imagePropertiesCompleteResponder( nullptr ),
      m_imageHeaderResponder( nullptr ),
      m_imageTransformationResponder( nullptr ),
      m_parcelPropertiesCompleteResponder( nullptr ),
      m_parcelHeaderResponder( nullptr ),
      m_parcelLabelsCompleteResponder( nullptr ),

      m_imageColorMapsResponder( nullptr ),

      m_selectionWidgets(),
      m_imageWidgets(),
      m_parcelWidgets(),

      m_selectionWidgetsList(),
      m_imageWidgetsList(),
      m_parcelWidgetsList(),
      m_transformWidgetsList(),

      m_imageSelections(),
      m_parcelSelections(),
      m_imageColorMaps(),

      m_currentImageUid( std::nullopt ),
      m_currentParcelUid( std::nullopt ),
      m_currentLabelsUid( std::nullopt )
{
    // Note: In case no parent is provided on construction, this dock widget will get
    // parented when added as a dock to the QMainWindow object

    setWindowTitle( "Reference Frame Editor" );

    setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

    setFeatures( QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable |
                 QDockWidget::DockWidgetMovable );


    auto imageSelectorWidget = createImageSelectorWidget();
    if ( ! imageSelectorWidget )
    {
        throw_debug( "Unable to create Image/Parcellation Loader Widget" );
    }

    auto tabWidget = createTabWidget();
    if ( ! tabWidget )
    {
        throw_debug( "Unable to create Image/Parcellation Tab Widget" );
    }


    auto layout = new QVBoxLayout();
    setZeroContentsMargins( layout, true, true, true, true );
    layout->addWidget( imageSelectorWidget );
    layout->addWidget( tabWidget );

    auto widget = new QWidget();
    setZeroContentsMargins( widget, true, true, true, true );
    widget->setLayout( layout );

    setWidget( widget );

    connectImageWidgets();
    connectParcellationWidgets();
    connectTransformationWidgets();

    refresh();
}

RefFrameEditorDock::~RefFrameEditorDock()
{
    // ImageDockWidget owns the table model
    if ( m_labelTableModel )
    {
        delete m_labelTableModel;
        m_labelTableModel = nullptr;
    }

    // ImageDockWidget owns the color dialog delegate
    if ( m_labelColorDialogDelegate )
    {
        delete m_labelColorDialogDelegate;
        m_labelColorDialogDelegate = nullptr;
    }
}

void RefFrameEditorDock::setImageSelectionsPublisher( ImageSelections_msgFromUi_PublisherType publisher )
{
    m_imageSelectionsPublisher = publisher;
}

void RefFrameEditorDock::setImagePropertiesPartialPublisher( ImagePropertiesPartial_msgFromUi_PublisherType publisher )
{
    m_imagePropertiesPartialPublisher = publisher;
}

void RefFrameEditorDock::setImageTransformationPublisher( ImageTransformation_msgFromUi_PublisherType publisher )
{
    m_imageTransformationPublisher = publisher;
}

void RefFrameEditorDock::setParcellationSelectionsPublisher( ParcellationSelection_msgFromUi_PublisherType publisher )
{
    m_parcelSelectionsPublisher = publisher;
}

void RefFrameEditorDock::setParcellationPropertiesPartialPublisher( ParcellationPropertiesPartial_msgFromUi_PublisherType publisher )
{
    m_parcelPropertiesPartialPublisher = publisher;
}

void RefFrameEditorDock::setParcellationLabelsPartialPublisher( ParcellationLabelsPartial_msgFromUi_PublisherType publisher )
{
    m_parcelLabelsPartialPublisher = publisher;
}

void RefFrameEditorDock::setImageSelectionsResponder( ImageSelections_msgToUi_ResponderType responder )
{
    m_imageSelectionsResponder = responder;
}

void RefFrameEditorDock::setImagePropertiesCompleteResponder( ImagePropertiesComplete_msgToUi_ResponderType responder )
{
    m_imagePropertiesCompleteResponder = responder;
}

void RefFrameEditorDock::setImageTransformationResponder( ImageTransformation_msgToUi_ResponderType responder )
{
    m_imageTransformationResponder = responder;
}

void RefFrameEditorDock::setImageHeaderResponder( ImageHeader_msgToUi_ResponderType responder )
{
    m_imageHeaderResponder = responder;
}

void RefFrameEditorDock::setParcellationSelectionsResponder( ParcellationSelections_msgToUi_ResponderType responder )
{
    m_parcelSelectionsResponder = responder;
}

void RefFrameEditorDock::setParcellationPropertiesCompleteResponder( ParcellationPropertiesComplete_msgToUi_ResponderType responder )
{
    m_parcelPropertiesCompleteResponder = responder;
}

void RefFrameEditorDock::setParcellationHeaderResponder( ParcellationHeader_msgToUi_ResponderType responder )
{
    m_parcelHeaderResponder = responder;
}

void RefFrameEditorDock::setParcellationLabelsCompleteResponder( ParcellationLabelsComplete_msgToUi_ResponderType responder )
{
    m_parcelLabelsCompleteResponder = responder;
}

void RefFrameEditorDock::setImageColorMapsResponder( ImageColorMaps_msgToUi_ResponderType provider )
{
    m_imageColorMapsResponder = provider;
}


void RefFrameEditorDock::refresh()
{
    updateImageSelections();
    updateParcellationSelections();

    updateImageProperties();
    updateImageColorMaps();
    updateImageHeader();
    updateImageTransformation();

    updateParcellationProperties();
    updateParcellationHeader();
    updateParcellationLabels();
}


QWidget* RefFrameEditorDock::createImageSelectorWidget()
{
    // Combo boxes for selecting the current image and parcellation:
    // Selecting image or parcellation will update the dock widget.
    // Note: "None" is a valid option.
    m_selectionWidgets.m_imageSelectionComboBox = new QComboBox();
//    m_selectionWidgets.m_imageSelectionListView = new QListView();
//    m_selectionWidgets.m_imageSelectionItemModel = new QStandardItemModel();

//    m_selectionWidgets.m_imageSelectionListView->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
//    m_selectionWidgets.m_imageSelectionListView->setModel( m_selectionWidgets.m_imageSelectionItemModel );

    m_selectionWidgetsList.push_back( m_selectionWidgets.m_imageSelectionComboBox );
    m_selectionWidgets.m_imageSelectionComboBox->setToolTip( "Select image" );

//    m_selectionWidgetsList.push_back( m_selectionWidgets.m_imageSelectionListView );
//    m_selectionWidgets.m_imageSelectionListView->setToolTip( "Select images" );

    m_selectionWidgets.m_parcelSelectionComboBox = new QComboBox();
    m_selectionWidgetsList.push_back( m_selectionWidgets.m_parcelSelectionComboBox );
    m_selectionWidgets.m_parcelSelectionComboBox->setToolTip( "Select parcellation" );

    //    QIcon icon = QIcon::fromTheme("list-add");

    // Tool buttons for loading and removing images and labels:
    m_selectionWidgets.m_imageLoadButton = new QToolButton();
    m_selectionWidgetsList.push_back( m_selectionWidgets.m_imageLoadButton );
    m_selectionWidgets.m_imageLoadButton->setToolButtonStyle( Qt::ToolButtonStyle::ToolButtonIconOnly );
    m_selectionWidgets.m_imageLoadButton->setText( "+" ); // remove this
    m_selectionWidgets.m_imageLoadButton->setToolTip( "Load image" );
    //    loadImageButton->setIcon( icon );

    m_selectionWidgets.m_imageUnloadButton = new QToolButton;
    m_selectionWidgetsList.push_back( m_selectionWidgets.m_imageUnloadButton );
    m_selectionWidgets.m_imageUnloadButton->setToolButtonStyle( Qt::ToolButtonStyle::ToolButtonIconOnly );
    m_selectionWidgets.m_imageUnloadButton->setText( "-" ); // remove this
    m_selectionWidgets.m_imageUnloadButton->setToolTip( "Unload image" );


    m_selectionWidgets.m_parcelLoadButton = new QToolButton();
    m_selectionWidgetsList.push_back( m_selectionWidgets.m_parcelLoadButton );
    m_selectionWidgets.m_parcelLoadButton->setToolButtonStyle( Qt::ToolButtonStyle::ToolButtonIconOnly );
    m_selectionWidgets.m_parcelLoadButton->setText( "+" ); // remove this
    m_selectionWidgets.m_parcelLoadButton->setToolTip( "Load image parcellation" );

    m_selectionWidgets.m_parcelUnloadButton = new QToolButton;
    m_selectionWidgetsList.push_back( m_selectionWidgets.m_parcelUnloadButton );
    m_selectionWidgets.m_parcelUnloadButton->setToolButtonStyle( Qt::ToolButtonStyle::ToolButtonIconOnly );
    m_selectionWidgets.m_parcelUnloadButton->setText( "-" ); // remove this
    m_selectionWidgets.m_parcelUnloadButton->setToolTip( "Unload image parcellation" );


    // Layout for holding image combo box and buttons:
    auto imageLayout = new QHBoxLayout();
    imageLayout->setSpacing( 0 );
    imageLayout->addWidget( m_selectionWidgets.m_imageSelectionComboBox );
    imageLayout->addSpacing( 5 );
    imageLayout->addWidget( m_selectionWidgets.m_imageLoadButton );
    imageLayout->addWidget( m_selectionWidgets.m_imageUnloadButton );


    // Layout for holding image list widget
//    auto imageListLayout = new QHBoxLayout();
//    imageListLayout->setSpacing( 0 );
//    imageListLayout->addWidget( m_selectionWidgets.m_imageSelectionListView );


    // Layout for holding parcellation combo box and buttons:
    auto parcelLayout = new QHBoxLayout();
    parcelLayout->setSpacing( 0 );
    parcelLayout->addWidget( m_selectionWidgets.m_parcelSelectionComboBox );
    parcelLayout->addSpacing( 5 );
    parcelLayout->addWidget( m_selectionWidgets.m_parcelLoadButton );
    parcelLayout->addWidget( m_selectionWidgets.m_parcelUnloadButton );



    auto& IW = m_imageWidgets;

    IW.m_planesVisibleIn2dViewsCheckBox = new QCheckBox( "2D" );
    m_imageWidgetsList.push_back( IW.m_planesVisibleIn2dViewsCheckBox );
    IW.m_planesVisibleIn2dViewsCheckBox->setToolTip( "Set image plane visibility in 2D views" );

    IW.m_planesVisibleIn3dViewsCheckBox = new QCheckBox( "3D views" );
    m_imageWidgetsList.push_back( IW.m_planesVisibleIn3dViewsCheckBox );
    IW.m_planesVisibleIn3dViewsCheckBox->setToolTip( "Set image plane visibility in 3D views" );

    IW.m_planesAutoHideCheckBox = new QCheckBox( "Auto-hide in 3D" );
    m_imageWidgetsList.push_back( IW.m_planesAutoHideCheckBox );
    IW.m_planesAutoHideCheckBox->setToolTip( "Set image plane auto-hiding" );

    auto planesLayout = new QHBoxLayout();
    planesLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    planesLayout->setContentsMargins( 0, 0, 0, 0 );
    planesLayout->setMargin( 0 );
    planesLayout->addWidget( IW.m_planesVisibleIn2dViewsCheckBox );
    planesLayout->addWidget( IW.m_planesVisibleIn3dViewsCheckBox );
//    planesLayout->addSpacing( 5 );
//    planesLayout->addWidget( IW.m_planesAutoHideCheckBox );

    auto planesVLayout = new QVBoxLayout();
    planesVLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    planesVLayout->setContentsMargins( 0, 0, 0, 0 );
    planesVLayout->addLayout( planesLayout );
    planesVLayout->addWidget( IW.m_planesAutoHideCheckBox, Qt::AlignLeft );




    // Layout for holding both image and parecellation layouts:
    auto layout = new QFormLayout();
    layout->addRow( "Image:", imageLayout );
//    layout->addRow( "Images:", imageListLayout );
    layout->addRow( "Parcellation:", parcelLayout );
    layout->addRow( "Visibility:", planesVLayout );


    // Main widget:
    auto widget = new QWidget();
    widget->setLayout( layout );
    widget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );

    return widget;
}


QLayout* RefFrameEditorDock::createImagePropertiesLayout()
{
    auto& IW = m_imageWidgets;

    // Color map selection combo box:
    IW.m_colorMapComboBox = new QComboBox();
    m_imageWidgetsList.push_back( IW.m_colorMapComboBox );
    IW.m_colorMapComboBox->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    IW.m_colorMapComboBox->setToolTip( "Set color map" );

    QSize iconSize = IW.m_colorMapComboBox->iconSize();
    iconSize.setWidth( 3 * iconSize.width() );
    iconSize.setHeight( 3 * iconSize.height() / 4 );
    IW.m_colorMapComboBox->setIconSize( iconSize );
    //    IW.m_colorMapComboBox->setSizeAdjustPolicy( QComboBox::AdjustToContentsOnFirstShow );


    // Colormap description line edit:
    IW.m_colorMapDescriptionLineEdit = new QLineEdit();
    m_imageWidgetsList.push_back( IW.m_colorMapDescriptionLineEdit );
    IW.m_colorMapDescriptionLineEdit->setReadOnly( true );
    IW.m_colorMapDescriptionLineEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );

    // Colormap layout:
    auto cmapLayout = new QVBoxLayout();
    cmapLayout->addWidget( IW.m_colorMapComboBox );
    cmapLayout->addWidget( IW.m_colorMapDescriptionLineEdit );

    // Opacity slider:
    IW.m_opacitySlider = new QSlider( Qt::Orientation::Horizontal );
    m_imageWidgetsList.push_back( IW.m_opacitySlider );
    IW.m_opacitySlider->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    IW.m_opacitySlider->setToolTip( "Set opacity" );

    // Opacity spin box:
    IW.m_opacitySpinBox = new QSpinBox();
    m_imageWidgetsList.push_back( IW.m_opacitySpinBox );
    IW.m_opacitySpinBox->setToolTip( "Set opacity" );

    // Opacity layout:
    auto opacityLayout = new QHBoxLayout();
    opacityLayout->setContentsMargins( 0, 0, 0, 0 );
    opacityLayout->addWidget( IW.m_opacitySlider );
    opacityLayout->addWidget( IW.m_opacitySpinBox );


    // Windowing (and leveling) setting range slider:
    IW.m_windowRangeSlider = new ctkDoubleRangeSlider( Qt::Orientation::Horizontal );
    m_imageWidgetsList.push_back( IW.m_windowRangeSlider );
    IW.m_windowRangeSlider->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    IW.m_windowRangeSlider->setToolTip( "Set window/level" );

    // Windowing spin boxes:
    IW.m_windowMinSpinBox = new ctkDoubleSpinBox();
    m_imageWidgetsList.push_back( IW.m_windowMinSpinBox );
    IW.m_windowMinSpinBox->setToolTip( "Set window minimum" );

    IW.m_windowMaxSpinBox = new ctkDoubleSpinBox();
    m_imageWidgetsList.push_back( IW.m_windowMaxSpinBox );
    IW.m_windowMaxSpinBox->setToolTip( "Set window maximum" );


    // Windowing layout for spin boxes:
    auto windowSpinBoxLayout = new QHBoxLayout();
    windowSpinBoxLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    windowSpinBoxLayout->setContentsMargins( 0, 0, 0, 0 );

    windowSpinBoxLayout->addWidget( new QLabel( "Min:" ) );
    windowSpinBoxLayout->addWidget( IW.m_windowMinSpinBox, 0, Qt::AlignLeft );
    windowSpinBoxLayout->addWidget( new QLabel( "Max:" ) );
    windowSpinBoxLayout->addWidget( IW.m_windowMaxSpinBox, 0, Qt::AlignLeft );
    windowSpinBoxLayout->insertSpacing( 2, 10 );


    // Main windowing layout containing both range slider and spin boxes:
    auto windowMainLayout = new QVBoxLayout();
    windowMainLayout->setSpacing( 3 );
    windowMainLayout->setContentsMargins( 0, 0, 0, 0 );
    windowMainLayout->addWidget( IW.m_windowRangeSlider );
    windowMainLayout->addLayout( windowSpinBoxLayout, 0 );


    // Thresholding range slider:
    IW.m_threshRangeSlider = new ctkDoubleRangeSlider( Qt::Orientation::Horizontal );
    m_imageWidgetsList.push_back( IW.m_threshRangeSlider );
    IW.m_threshRangeSlider->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    IW.m_threshRangeSlider->setToolTip( "Set thresholds" );

    // Thresholding spin boxes:
    IW.m_threshLowSpinBox = new ctkDoubleSpinBox();
    m_imageWidgetsList.push_back( IW.m_threshLowSpinBox );
    IW.m_threshLowSpinBox->setToolTip( "Set low threshold" );

    IW.m_threshHighSpinBox = new ctkDoubleSpinBox();
    m_imageWidgetsList.push_back( IW.m_threshHighSpinBox );
    IW.m_threshHighSpinBox->setToolTip( "Set high threshold" );

    // Thresholding layout for spin boxes:
    auto threshSpinBoxLayout = new QHBoxLayout();
    threshSpinBoxLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    threshSpinBoxLayout->setContentsMargins( 0, 0, 0, 0 );

    threshSpinBoxLayout->addWidget( new QLabel( "Low:" ) );
    threshSpinBoxLayout->addWidget( IW.m_threshLowSpinBox, 0, Qt::AlignLeft | Qt::AlignVCenter );
    threshSpinBoxLayout->addWidget( new QLabel( "High:" ) );
    threshSpinBoxLayout->addWidget( IW.m_threshHighSpinBox, 0, Qt::AlignLeft | Qt::AlignVCenter );
    threshSpinBoxLayout->insertSpacing( 2, 10 );


    // Main thresholding layout containing both range slider and spin boxes:
    auto threshMainLayout = new QVBoxLayout();
    threshMainLayout->setSpacing( 3 );
    threshMainLayout->setContentsMargins( 0, 0, 0, 0 );
    threshMainLayout->addWidget( IW.m_threshRangeSlider );
    threshMainLayout->addLayout( threshSpinBoxLayout, 0 );


    // Radio buttons for setting interpolation (sampling):
    IW.m_samplingNNRadioButton = new QRadioButton( "Nearest" );
    m_imageWidgetsList.push_back( IW.m_samplingNNRadioButton );
    IW.m_samplingNNRadioButton->setToolTip( "Set nearest-neighbor interpolation" );

    IW.m_samplingLinearRadioButton = new QRadioButton( "Linear" );
    m_imageWidgetsList.push_back( IW.m_samplingLinearRadioButton );
    IW.m_samplingLinearRadioButton->setToolTip( "Set linear interpolation" );


    // Sampling setting layout:
    auto samplingLayout = new QHBoxLayout();
    samplingLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    samplingLayout->addWidget( IW.m_samplingNNRadioButton, 0, Qt::AlignLeft | Qt::AlignVCenter );
    samplingLayout->addWidget( IW.m_samplingLinearRadioButton, 0, Qt::AlignLeft | Qt::AlignVCenter );


    auto mainLayout = new QFormLayout();

//    setZeroContentsMargins( layout, true, false, true, false );

    auto layoutAlignment = mainLayout->labelAlignment();
    mainLayout->setLabelAlignment( layoutAlignment | Qt::AlignTop );
    mainLayout->setFieldGrowthPolicy( QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow );

    mainLayout->addRow( "Opacity:", opacityLayout );
    mainLayout->addRow( "Window:", windowMainLayout );
    mainLayout->addRow( "Threshold:", threshMainLayout );
    mainLayout->addRow( "Color Map:", cmapLayout );
    mainLayout->addRow( "Sampling:", samplingLayout );

    return mainLayout;
}


QGroupBox* RefFrameEditorDock::createImagePropertiesGroupBox()
{
    auto groupBox = new ctkCollapsibleGroupBox( "Properties" );

    QFont boldFont = groupBox->font();
    boldFont.setBold( sk_useBoldGroupBoxFont );
    boldFont.setUnderline( true );

    groupBox->setFont( boldFont );
    groupBox->setFlat( true );
    groupBox->setLayout( createImagePropertiesLayout() );
    groupBox->setCollapsed( false );

    return groupBox;
}


QWidget* RefFrameEditorDock::createImageHeaderTableWidget()
{
    auto& T = m_imageWidgets.m_headerTableWidget;

    T = new QTableWidget( 1, 2 );

    if ( ! T )
    {
        return nullptr;
    }

    if ( T->horizontalHeader() )
    {
        T->horizontalHeader()->setSectionsMovable( false );
        T->horizontalHeader()->setStretchLastSection( true );
        T->horizontalHeader()->setSectionResizeMode( QHeaderView::Interactive );
        T->horizontalHeader()->setVisible( true );
    }

    if ( T->verticalHeader() )
    {
        T->verticalHeader()->setVisible( false );
    }

    T->setEditTriggers( QAbstractItemView::NoEditTriggers );
    T->setHorizontalHeaderLabels( { "Property", "Value" } );
    T->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    T->setSelectionBehavior( QAbstractItemView::SelectRows );
    T->setSelectionMode( QAbstractItemView::SingleSelection );
    T->setShowGrid( false );
    T->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );
    T->setWordWrap( false );

    T->resizeColumnsToContents();

    m_imageWidgetsList.push_back( T );

    return T;
}


QGroupBox* RefFrameEditorDock::createImageHeaderGroupBox()
{
    auto groupBox = new ctkCollapsibleGroupBox( "Header" );

//    setZeroContentsMargins( groupBox, true, false, true, false );

    QFont boldFont = groupBox->font();
    boldFont.setBold( sk_useBoldGroupBoxFont );
    boldFont.setUnderline( true );

    auto& W = m_imageWidgets;

    // File path line editor:
    W.m_pathLineEdit = new ctkPathLineEdit();
    W.m_pathLineEdit->setLabel( "Image File Path" );
    W.m_pathLineEdit->setShowBrowseButton( true ); // "..." button that opens FileDialog
    W.m_pathLineEdit->setShowHistoryButton( true ); // use ComboBox instead of LineEdit
    W.m_pathLineEdit->setToolTip( "Set image file path" );

    auto pathLayout = new QHBoxLayout();
    pathLayout->addWidget( new QLabel( "File:" ) );
    pathLayout->addWidget( W.m_pathLineEdit );


    W.m_subject_O_pixels_matrixWidget = new ctkMatrixWidget( 4, 4 );
    m_imageWidgetsList.push_back( W.m_subject_O_pixels_matrixWidget );
    W.m_subject_O_pixels_matrixWidget->setDecimals( 3 );
    W.m_subject_O_pixels_matrixWidget->setDecimalsOption( ctkDoubleSpinBox::DecimalsByShortcuts );
    W.m_subject_O_pixels_matrixWidget->setToolTip( "Pixel to Subject (LPS) space transformation matrix" );
    W.m_subject_O_pixels_matrixWidget->setEditable( false );
    W.m_subject_O_pixels_matrixWidget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );

    auto mainLayout = new QVBoxLayout();
    mainLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    mainLayout->addSpacing( 5 );

//    setZeroContentsMargins( layout, true, false, true, false );
    mainLayout->addLayout( pathLayout );
    mainLayout->addWidget( createImageHeaderTableWidget() );
    mainLayout->addSpacing( 5 );

    mainLayout->addWidget( new QLabel( "Pixel to Subject (LPS) matrix:" ) );
    mainLayout->addWidget( W.m_subject_O_pixels_matrixWidget );


    groupBox->setLayout( mainLayout );
    groupBox->setFont( boldFont );
    groupBox->setFlat( true );
    groupBox->setCollapsed( true );

    return groupBox;
}


QGroupBox* RefFrameEditorDock::createImageTransformationGroupBox()
{
    auto groupBox = new ctkCollapsibleGroupBox( "Transformation" );

//    setZeroContentsMargins( groupBox, true, false, true, false );

    QFont boldFont = groupBox->font();
    boldFont.setBold( sk_useBoldGroupBoxFont );
    boldFont.setUnderline( true );

//    setZeroContentsMargins( layout, true, false, true, false );

    groupBox->setFont( boldFont );
    groupBox->setFlat( true );
    groupBox->setCollapsed( true );
    groupBox->setLayout( createImageTransformLayout() );

    return groupBox;
}


QLayout* RefFrameEditorDock::createImageTransformLayout()
{
    auto& TW = m_transformWidgets;

    TW.m_world_O_subject_matrixWidget = new ctkMatrixWidget( 4, 4 );
    m_transformWidgetsList.push_back( TW.m_world_O_subject_matrixWidget );
    TW.m_world_O_subject_matrixWidget->setDecimals( 3 );
    TW.m_world_O_subject_matrixWidget->setDecimalsOption( ctkDoubleSpinBox::DecimalsByShortcuts );
    TW.m_world_O_subject_matrixWidget->setToolTip( "Subject (LPS) to World Space transformation matrix" );
    TW.m_world_O_subject_matrixWidget->setEditable( false );
    TW.m_world_O_subject_matrixWidget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );

    TW.m_setIdentityButton = new QPushButton( "Set Identity" );
    m_transformWidgetsList.push_back( TW.m_setIdentityButton );
    TW.m_setIdentityButton->setToolTip( "Set slide stack transformation to identity" );
    TW.m_setIdentityButton->setStatusTip( "Set slide stack transformation to identity" );
    TW.m_setIdentityButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    auto mainLayout = new QVBoxLayout();
    mainLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    mainLayout->addSpacing( 5 );
    mainLayout->addWidget( new QLabel( "Subject (LPS) to World matrix:" ) );
    mainLayout->addWidget( TW.m_world_O_subject_matrixWidget );
    mainLayout->addWidget( TW.m_setIdentityButton );
    mainLayout->addSpacing( 5 );

    return mainLayout;
}


QLayout* RefFrameEditorDock::createImageLandmarksLayout()
{
    auto& W = m_landmarkWidgets;
    auto* T = W.m_landmarkTreeView;

    T = new QTreeView();
    m_landmarkWidgetsList.push_back( T );

    T->setAlternatingRowColors( true );
    T->setSelectionBehavior( QAbstractItemView::SelectItems );
    T->setSelectionMode( QAbstractItemView::SingleSelection );
    T->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
    T->setAnimated( false );
    T->setSortingEnabled( false );
    T->setAllColumnsShowFocus( true );
    T->setWordWrap( false );
//    T->setEditTriggers( QAbstractItemView::AllEditTriggers );
    T->setToolTip( "Landmark positions defined in Subject space (LPS)" );
    T->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
//    T->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

    T->header()->setSectionsMovable( false );
//    T->header()->setStretchLastSection( true );
    T->header()->setDefaultAlignment( Qt::AlignLeft );

    const QStringList headers( { tr("Landmarks"), "x", "y", "z" } );

    QString s = R"(
                Red		2	3	4
                    One		2.234234	3	4
                    Two		2	3	4
                Green	a	b	c
                    One		5	6	7
                    Two		2	3	4
                )";

    // Note: QTreeView does NOT take ownership of the model,
    // so the pointer to TreeModel is stored
    m_refImageLandmarkTreeModel = new TreeModel( headers, s );
    T->setModel( m_refImageLandmarkTreeModel );


    T->expandAll();
    T->header()->resizeSections( QHeaderView::ResizeToContents );
//    T->resizeColumnToContents( 0 );
//    T->resizeColumnToContents( 1 );
//    T->resizeColumnToContents( 2 );
//    T->resizeColumnToContents( 3 );

////    verticalResizeTableViewToContents( W.m_landmarkTreeView );

    auto mainLayout = new QVBoxLayout();
    mainLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    mainLayout->addSpacing( 5 );
    mainLayout->addWidget( new QLabel( "Positions in Subject space (LPS):" ) );
    mainLayout->addWidget( T );
    mainLayout->addSpacing( 5 );

    return mainLayout;
}


QGroupBox* RefFrameEditorDock::createImageLandmarksGroupBox()
{
    auto groupBox = new ctkCollapsibleGroupBox( "Landmarks" );

//    setZeroContentsMargins( groupBox, true, false, true, false );

    QFont boldFont = groupBox->font();
    boldFont.setBold( sk_useBoldGroupBoxFont );
    boldFont.setUnderline( true );

//    setZeroContentsMargins( layout, true, false, true, false );

    groupBox->setFont( boldFont );
    groupBox->setFlat( true );
    groupBox->setCollapsed( true );
    groupBox->setLayout( createImageLandmarksLayout() );

    return groupBox;
}


QScrollArea* RefFrameEditorDock::createImageScrollArea()
{
    // Scroll the properties, header, and transformation group boxes:
    auto layout = new QVBoxLayout();

    setZeroContentsMargins( layout, true, false, true, false );
    layout->setAlignment( Qt::AlignLeft | Qt::AlignTop );

    layout->addWidget( createImageHeaderGroupBox() );
    layout->addWidget( createImagePropertiesGroupBox() );
    layout->addWidget( createImageTransformationGroupBox() );
    layout->addWidget( createImageLandmarksGroupBox() );

    // Inner widget to scroll:
    auto innerWidget = new QWidget();
    innerWidget->setLayout( layout );

    auto scrollArea = new QScrollArea();
    scrollArea->setWidget( innerWidget );
    scrollArea->setWidgetResizable( true );
    scrollArea->setStyleSheet( sk_scrollAreaStyleSheet );

    return scrollArea;
}


QWidget* RefFrameEditorDock::createImageTab()
{
    auto& IW = m_imageWidgets;

    // Image name editor:
    IW.m_displayNameLineEdit = new QLineEdit();
    m_imageWidgetsList.push_back( IW.m_displayNameLineEdit );
    IW.m_displayNameLineEdit->setToolTip( "Set image name" );


    auto nameLayout = new QHBoxLayout();
    nameLayout->addWidget( new QLabel( "ID:" ) );
    nameLayout->addWidget( IW.m_displayNameLineEdit );


    // Scrollable area:
    auto scrollArea = createImageScrollArea();

    auto layout = new QVBoxLayout();
    layout->addLayout( nameLayout );
    layout->addWidget( scrollArea );

    auto widget = new QWidget();
    widget->setLayout( layout );

    return widget;
}


QWidget* RefFrameEditorDock::createParcellationTab()
{
    auto& W = m_parcelWidgets;

    // Parcellation name editor:
    W.m_displayNameLineEdit = new QLineEdit();
    m_parcelWidgetsList.push_back( W.m_displayNameLineEdit );
    W.m_displayNameLineEdit->setToolTip( "Set parcellation name" );

    auto nameLayout = new QHBoxLayout();
    nameLayout->addWidget( new QLabel( "ID:" ) );
    nameLayout->addWidget( W.m_displayNameLineEdit );


    // Scrollable area:
    auto scrollArea = createParcellationScrollArea();

    auto layout = new QVBoxLayout();
    layout->addLayout( nameLayout );
    layout->addWidget( scrollArea );

    auto widget = new QWidget();
    widget->setLayout( layout );

    return widget;
}


QLayout* RefFrameEditorDock::createParcelPropertiesLayout()
{
    // Visibility checkboxes:
    m_parcelWidgets.m_visibilityIn2dViewsCheckBox = new QCheckBox( "2D" );
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_visibilityIn2dViewsCheckBox );
    m_parcelWidgets.m_visibilityIn2dViewsCheckBox->setToolTip( "Set image plane visibility in 2D views" );

    m_parcelWidgets.m_visibilityIn3dViewsCheckBox = new QCheckBox( "3D views" );
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_visibilityIn3dViewsCheckBox );
    m_parcelWidgets.m_visibilityIn3dViewsCheckBox->setToolTip( "Set image plane visibility in 3D views" );

    // Opacity slider:
    m_parcelWidgets.m_opacitySlider = new QSlider( Qt::Orientation::Horizontal );
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_opacitySlider );
    m_parcelWidgets.m_opacitySlider->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    m_parcelWidgets.m_opacitySlider->setToolTip( "Set opacity" );

    // Opacity spin box:
    m_parcelWidgets.m_opacitySpinBox = new QSpinBox();
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_opacitySpinBox );
    m_parcelWidgets.m_opacitySpinBox->setToolTip( "Set opacity" );

    // Visibility layout:
    auto visibilityLayout = new QHBoxLayout();
    visibilityLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    visibilityLayout->setContentsMargins( 0, 0, 0, 0 );
    visibilityLayout->setMargin( 0 );
    visibilityLayout->addWidget( m_parcelWidgets.m_visibilityIn2dViewsCheckBox );
    visibilityLayout->addWidget( m_parcelWidgets.m_visibilityIn3dViewsCheckBox );

    // Opacity layout:
    auto opacityLayout = new QHBoxLayout();
    opacityLayout->setContentsMargins( 0, 0, 0, 0 );

    opacityLayout->addWidget( m_parcelWidgets.m_opacitySlider );
    opacityLayout->addWidget( m_parcelWidgets.m_opacitySpinBox );


    auto mainLayout = new QFormLayout();

//    setZeroContentsMargins( layout, true, false, true, false );

    auto layoutAlignment = mainLayout->labelAlignment();
    mainLayout->setLabelAlignment( layoutAlignment | Qt::AlignTop );
    mainLayout->setFieldGrowthPolicy( QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow );

    mainLayout->addRow( "Visibility:", visibilityLayout );
    mainLayout->addRow( "Opacity:", opacityLayout );

    return mainLayout;
}


QWidget* RefFrameEditorDock::createLabelTableView()
{
    auto& T = m_parcelWidgets.m_labelTableView;

    // Create parcellation label table:
    T = new QTableView();

    if ( ! T )
    {
        return nullptr;
    }

    m_parcelWidgetsList.push_back( T );

    // Disable selection of items:
    T->setSelectionBehavior( QAbstractItemView::SelectRows );
    T->setSelectionMode( QAbstractItemView::NoSelection );

    // Other options:
    T->setShowGrid( true );
    T->setSortingEnabled( false );
    T->setWordWrap( true );
    T->setCornerButtonEnabled( false );
    T->setEditTriggers( QAbstractItemView::AllEditTriggers );
    T->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );


    // Note: QTableView does NOT take ownership of the model,
    // so the pointer to LabelTableModel is stored
    m_labelTableModel = new LabelTableModel();
    T->setModel( m_labelTableModel );

    // Hide the "label color" column, since the color is also displayed as the "decorator role"
    // of the "label value" column
    T->setColumnHidden( LabelTableModel::LABEL_COLOR_COLUMN, true );

    // Create custom delegate for editing colors of labels, which are presented as the
    // "decorator role" in the model's "label value" column
    m_labelColorDialogDelegate = new LabelColorDialogDelegate();
    T->setItemDelegateForColumn( LabelTableModel::LABEL_VALUE_COLUMN, m_labelColorDialogDelegate );

    // Resize to contents after model has been set that defines headers:
    T->resizeColumnsToContents();

    verticalResizeTableViewToContents( T );

    if ( auto H = T->horizontalHeader() )
    {
        // Auto-size these sections to their contenst and prevent user modification:
        H->setSectionResizeMode( LabelTableModel::LABEL_VALUE_COLUMN, QHeaderView::ResizeMode::ResizeToContents );
        H->setSectionResizeMode( LabelTableModel::LABEL_ALPHA_COLUMN, QHeaderView::ResizeMode::ResizeToContents );
        H->setSectionResizeMode( LabelTableModel::LABEL_COLOR_COLUMN, QHeaderView::ResizeMode::ResizeToContents );
        H->setSectionResizeMode( LabelTableModel::LABEL_MESH_VISIBILITY_COLUMN, QHeaderView::ResizeMode::ResizeToContents );

        // Stretch the "label name" section to fill the header width:
        H->setStretchLastSection( true );
    }

    if ( auto V = T->verticalHeader() )
    {
        // Hide row labels:
        V->hide();
    }

    // Factory that provides widgets for editing item data in TableView:
    QItemEditorFactory* factory = new QItemEditorFactory();

    // The standard factory implementation provides editors for some data types.
    // Register the custom OpacityDoubleSpinBox editor with the factor for doubles
    // and the custom LabelColorDialog for colors.
    QItemEditorCreatorBase* colorPickerCreator = new QStandardItemEditorCreator<LabelColorDialog>();
    QItemEditorCreatorBase* lineEditCreator = new QStandardItemEditorCreator<QLineEdit>();
    QItemEditorCreatorBase* spinBoxCreator = new QStandardItemEditorCreator<OpacitySpinBox>();

    factory->registerEditor( QVariant::Int, spinBoxCreator );
    factory->registerEditor( QVariant::String, lineEditCreator );
    factory->registerEditor( QVariant::Color, colorPickerCreator );

    QItemEditorFactory::setDefaultFactory( factory );

    return T;
}


QGroupBox* RefFrameEditorDock::createParcelPropertiesGroupBox()
{
    auto groupBox = new ctkCollapsibleGroupBox( "Properties" );

//    setZeroContentsMargins( groupBox, true, false, true, false );

    QFont boldFont = groupBox->font();
    boldFont.setBold( sk_useBoldGroupBoxFont );
    boldFont.setUnderline( true );

    groupBox->setFont( boldFont );
    groupBox->setFlat( true );
    groupBox->setLayout( createParcelPropertiesLayout() );
    groupBox->setCollapsed( false );

    return groupBox;
}


QGroupBox* RefFrameEditorDock::createParcelLabelMeshPropertiesGroupBox()
{
    auto groupBox = new ctkCollapsibleGroupBox( "Label Meshes" );

//    setZeroContentsMargins( groupBox, true, false, true, false );

    QFont boldFont = groupBox->font();
    boldFont.setBold( sk_useBoldGroupBoxFont );
    boldFont.setUnderline( true );

    groupBox->setFont( boldFont );
    groupBox->setFlat( true );
    groupBox->setLayout( createLabelMeshPropertiesLayout() );
    groupBox->setCollapsed( false );

    return groupBox;
}


QLayout* RefFrameEditorDock::createLabelMeshPropertiesLayout()
{
    // Mesh visibility in 2D views:
    m_parcelWidgets.m_meshesVisibleIn2dViewsCheckBox = new QCheckBox( "2D" );
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_meshesVisibleIn2dViewsCheckBox );
    m_parcelWidgets.m_meshesVisibleIn2dViewsCheckBox->setToolTip( "Show label mesh in 2D views" );

    // Mesh visibility in 3D views:
    m_parcelWidgets.m_meshesVisibleIn3dViewsCheckBox = new QCheckBox( "3D views" );
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_meshesVisibleIn3dViewsCheckBox );
    m_parcelWidgets.m_meshesVisibleIn3dViewsCheckBox->setToolTip( "Show label mesh in 3D views" );

    // Mesh x-ray mode:
    m_parcelWidgets.m_meshesXrayModeCheckBox = new QCheckBox( "Enabled" );
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_meshesXrayModeCheckBox );
    m_parcelWidgets.m_meshesXrayModeCheckBox->setToolTip( "Show label mesh as outline" );

    m_parcelWidgets.m_meshesXrayPowerSpinBox = new ctkDoubleSpinBox();
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_meshesXrayPowerSpinBox );
    m_parcelWidgets.m_meshesXrayPowerSpinBox->setToolTip( "Set label mesh outline intensity" );


    // Label mesh opacity slider:
    m_parcelWidgets.m_meshOpacitySlider = new QSlider( Qt::Orientation::Horizontal );
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_meshOpacitySlider );
    m_parcelWidgets.m_meshOpacitySlider->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    m_parcelWidgets.m_meshOpacitySlider->setToolTip( "Set overall mesh opacity" );

    // Label mesh opacity spin box:
    m_parcelWidgets.m_meshOpacitySpinBox = new QSpinBox();
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_meshOpacitySpinBox );
    m_parcelWidgets.m_meshOpacitySpinBox->setToolTip( "Set overall mesh opacity" );


    // Mesh view layout:
    auto meshesVisibilityLayout = new QHBoxLayout();
    meshesVisibilityLayout->setContentsMargins( 0, 0, 0, 0 );
    meshesVisibilityLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    meshesVisibilityLayout->addWidget( m_parcelWidgets.m_meshesVisibleIn2dViewsCheckBox );
    meshesVisibilityLayout->addSpacing( 5 );
    meshesVisibilityLayout->addWidget( m_parcelWidgets.m_meshesVisibleIn3dViewsCheckBox );


    // Mesh opacity layout:
    auto meshOpacityLayout = new QHBoxLayout();
    meshOpacityLayout->setContentsMargins( 0, 0, 0, 0 );
    meshOpacityLayout->addWidget( m_parcelWidgets.m_meshOpacitySlider );
    meshOpacityLayout->addWidget( m_parcelWidgets.m_meshOpacitySpinBox );


    // Mesh outline mode layout:
    auto meshesXrayLayout = new QHBoxLayout();
    meshesXrayLayout->setContentsMargins( 0, 0, 0, 0 );
    meshesXrayLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    meshesXrayLayout->addWidget( m_parcelWidgets.m_meshesXrayModeCheckBox );
    meshesXrayLayout->addSpacing( 5 );

    QLabel* powerLabel = new QLabel( "Power:" );
    powerLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

    meshesXrayLayout->addWidget( powerLabel );
    meshesXrayLayout->addWidget( m_parcelWidgets.m_meshesXrayPowerSpinBox, 0, Qt::AlignLeft );


    auto mainLayout = new QFormLayout();

//    setZeroContentsMargins( layout, true, false, true, false );

    auto layoutAlignment = mainLayout->labelAlignment();
    mainLayout->setLabelAlignment( layoutAlignment | Qt::AlignTop );
    mainLayout->setFieldGrowthPolicy( QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow );

    mainLayout->addRow( "Visibility:", meshesVisibilityLayout );
    mainLayout->addRow( "Opacity:", meshOpacityLayout );
    mainLayout->addRow( "Outline:", meshesXrayLayout );

    return mainLayout;
}


QGroupBox* RefFrameEditorDock::createParcelLabelTableGroupBox()
{   
    auto groupBox = new ctkCollapsibleGroupBox( "Labels" );

//    setZeroContentsMargins( groupBox, true, false, true, false );

    QFont boldFont = groupBox->font();
    boldFont.setBold( sk_useBoldGroupBoxFont );
    boldFont.setUnderline( true );


    m_parcelWidgets.m_showAllLabelsButton = new QPushButton( "Show All Labels" );
    m_parcelWidgets.m_showAllLabelsButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_showAllLabelsButton );

    m_parcelWidgets.m_hideAllLabelsButton = new QPushButton( "Hide All Labels" );
    m_parcelWidgets.m_hideAllLabelsButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_hideAllLabelsButton );

    m_parcelWidgets.m_showAllMeshesButton = new QPushButton( "Show All Meshes" );
    m_parcelWidgets.m_showAllMeshesButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_showAllMeshesButton );

    m_parcelWidgets.m_hideAllMeshesButton = new QPushButton( "Hide All Meshes" );
    m_parcelWidgets.m_hideAllMeshesButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
    m_parcelWidgetsList.push_back( m_parcelWidgets.m_hideAllMeshesButton );


    // Give all four buttons the same width
    const int w1 = m_parcelWidgets.m_showAllLabelsButton->sizeHint().width();
    const int w2 = m_parcelWidgets.m_hideAllLabelsButton->sizeHint().width();
    const int w3 = m_parcelWidgets.m_showAllMeshesButton->sizeHint().width();
    const int w4 = m_parcelWidgets.m_hideAllMeshesButton->sizeHint().width();
    const int maxWidth = std::max( w1, std::max( w2, std::max( w3, w4 ) )  );

    m_parcelWidgets.m_showAllLabelsButton->setMinimumWidth( maxWidth );
    m_parcelWidgets.m_hideAllLabelsButton->setMinimumWidth( maxWidth );
    m_parcelWidgets.m_showAllMeshesButton->setMinimumWidth( maxWidth );
    m_parcelWidgets.m_hideAllMeshesButton->setMinimumWidth( maxWidth );


    auto buttonLayout = new QGridLayout();
    buttonLayout->setColumnStretch( 0, 0 );
    buttonLayout->setColumnStretch( 1, 0 );
    buttonLayout->addWidget( m_parcelWidgets.m_showAllLabelsButton, 0, 0, Qt::AlignLeft | Qt::AlignVCenter );
    buttonLayout->addWidget( m_parcelWidgets.m_hideAllLabelsButton, 1, 0, Qt::AlignLeft | Qt::AlignVCenter );
    buttonLayout->addWidget( m_parcelWidgets.m_showAllMeshesButton, 0, 1, Qt::AlignLeft | Qt::AlignVCenter );
    buttonLayout->addWidget( m_parcelWidgets.m_hideAllMeshesButton, 1, 1, Qt::AlignLeft | Qt::AlignVCenter );


    auto layout = new QVBoxLayout();
    layout->setAlignment( Qt::AlignLeft | Qt::AlignTop );

//    setZeroContentsMargins( layout, true, false, true, false );
    layout->setSpacing( 0 );
    layout->addWidget( createLabelTableView() );
    layout->addSpacing( 5 );
    layout->addLayout( buttonLayout );
//    layout->addLayout( meshButtonLayout );

    groupBox->setLayout( layout );
    groupBox->setCollapsed( false );
    groupBox->setFont( boldFont );
    groupBox->setFlat( true );

    return groupBox;
}


QWidget* RefFrameEditorDock::createParcelHeaderTableWidget()
{
    auto& T = m_parcelWidgets.m_headerTableWidget;

    T = new QTableWidget( 1, 2 );

    if ( ! T )
    {
        return nullptr;
    }

    if ( T->horizontalHeader() )
    {
        T->horizontalHeader()->setSectionsMovable( false );
        T->horizontalHeader()->setStretchLastSection( true );
        T->horizontalHeader()->setSectionResizeMode( QHeaderView::Interactive );
        T->horizontalHeader()->setVisible( true );
    }

    if ( T->verticalHeader() )
    {
        T->verticalHeader()->setVisible( false );
    }

    T->setEditTriggers( QAbstractItemView::NoEditTriggers );
    T->setHorizontalHeaderLabels( { "Property", "Value" } );
    T->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    T->setSelectionBehavior( QAbstractItemView::SelectRows );
    T->setSelectionMode( QAbstractItemView::SingleSelection );
    T->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );
    T->setShowGrid( false );
    T->setWordWrap( false );

    T->resizeColumnsToContents();

    m_parcelWidgetsList.push_back( T );

    return T;
}


QGroupBox* RefFrameEditorDock::createParcelHeaderGroupBox()
{
    auto groupBox = new ctkCollapsibleGroupBox( "Header" );

//    setZeroContentsMargins( groupBox, true, false, true, false );

    QFont boldFont = groupBox->font();
    boldFont.setBold( sk_useBoldGroupBoxFont );
    boldFont.setUnderline( true );

    auto& W = m_parcelWidgets;

    // File path line editor:
    W.m_pathLineEdit = new ctkPathLineEdit();
    W.m_pathLineEdit->setLabel( "Parcellation File Path" );
    W.m_pathLineEdit->setShowBrowseButton( true ); // "..." button that opens FileDialog
    W.m_pathLineEdit->setShowHistoryButton( true ); // use ComboBox instead of LineEdit
    W.m_pathLineEdit->setToolTip( "Set image parcellation file path" );

    auto pathLayout = new QHBoxLayout();
    pathLayout->addWidget( new QLabel( "File:" ) );
    pathLayout->addWidget( m_parcelWidgets.m_pathLineEdit );


    W.m_subject_O_pixels_matrixWidget = new ctkMatrixWidget( 4, 4 );
    m_imageWidgetsList.push_back( W.m_subject_O_pixels_matrixWidget );
    W.m_subject_O_pixels_matrixWidget->setDecimals( 3 );
    W.m_subject_O_pixels_matrixWidget->setDecimalsOption( ctkDoubleSpinBox::DecimalsByShortcuts );
    W.m_subject_O_pixels_matrixWidget->setToolTip( "Pixel to Subject (LPS) space transformation matrix" );
    W.m_subject_O_pixels_matrixWidget->setEditable( false );
    W.m_subject_O_pixels_matrixWidget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );


    auto layout = new QVBoxLayout();
    layout->setAlignment( Qt::AlignLeft | Qt::AlignTop );

    layout->addSpacing( 5 );

//    setZeroContentsMargins( layout, true, false, true, false );
    layout->addLayout( pathLayout );
    layout->addWidget( createParcelHeaderTableWidget() );
    layout->addSpacing( 5 );

    layout->addWidget( new QLabel( "Pixel to Subject (LPS) matrix:" ) );
    layout->addWidget( W.m_subject_O_pixels_matrixWidget );

    groupBox->setLayout( layout );
    groupBox->setFont( boldFont );
    groupBox->setFlat( true );
    groupBox->setCollapsed( true );

    // Make rest of headear appear in pop-up window (e.g. Metadata)

    return groupBox;
}


QScrollArea* RefFrameEditorDock::createParcellationScrollArea()
{
    // Scroll the properties and header group boxes:
    auto layout = new QVBoxLayout();

    setZeroContentsMargins( layout, true, false, true, false );

    layout->setAlignment( Qt::AlignLeft | Qt::AlignTop );

    layout->addWidget( createParcelHeaderGroupBox() );
    layout->addWidget( createParcelPropertiesGroupBox() );
    layout->addWidget( createParcelLabelTableGroupBox() );
    layout->addWidget( createParcelLabelMeshPropertiesGroupBox() );

    // Inner widget to scroll:
    auto innerWidget = new QWidget();
    innerWidget->setLayout( layout );

    auto scrollArea = new QScrollArea();
    scrollArea->setWidget( innerWidget );
    scrollArea->setWidgetResizable( true );
    scrollArea->setStyleSheet( sk_scrollAreaStyleSheet );

    return scrollArea;
}


QGroupBox* RefFrameEditorDock::createImageSurfacesWidget()
{
    auto imageSurfacesGroupBox = new ctkCollapsibleGroupBox( "Surfaces" );

    QFont boldGroupBoxFont = imageSurfacesGroupBox->font();
    boldGroupBoxFont.setBold( sk_useBoldGroupBoxFont );
    boldGroupBoxFont.setUnderline( true );

    imageSurfacesGroupBox->setFont( boldGroupBoxFont );
    imageSurfacesGroupBox->setFlat( true );

    /// Rows: each isosurface, sorted by iso value
    /// Columns: Name, Isovalue, Material, Opacity, Visible, Delete Button,
    ///
    /// x-ray mode
    ///

    return imageSurfacesGroupBox;
}


QTabWidget* RefFrameEditorDock::createTabWidget()
{
    auto tabWidget = new QTabWidget();

    tabWidget->addTab( createImageTab(), "Image" );
    tabWidget->addTab( createParcellationTab(), "Parcellation" );
    //    tabWidget->addTab( labelScrollArea, "Surfaces" );

    tabWidget->setDocumentMode( false );
    tabWidget->setMovable( false );
    tabWidget->setTabsClosable( false );

    return tabWidget;
}


void RefFrameEditorDock::connectImageWidgets()
{
    auto imageSelectionChangedHandler = [this] ( int comboBoxIndex )
    {
        if ( comboBoxIndex < 0 )
        {
            return;
        }

        const size_t i = static_cast<size_t>( comboBoxIndex );

        if ( i < m_imageSelections.size() )
        {
            if ( m_currentImageUid && *m_currentImageUid == m_imageSelections[i].m_imageUid )
            {
                // Do nothing if the same image is being set
                return;
            }

            m_currentImageUid = m_imageSelections[i].m_imageUid;

            if ( m_currentImageUid && m_imageSelectionsPublisher )
            {
                ImageSelections_msgFromUi msg;
                msg.m_imageUid = *m_currentImageUid;
                msg.m_selectionIndex = comboBoxIndex;
                m_imageSelectionsPublisher( msg );
            }

            // Update the color maps, properties, and header for the new selected image:
            updateImageColorMaps();
            updateImageProperties();
            updateImageHeader();
            updateImageTransformation();
        }
        else
        {
            // error
        }
    };

    auto colorMapChangedHandler = [this] ( int comboBoxIndex )
    {
        updateImageColorMapDescription( comboBoxIndex );

        if ( m_currentImageUid && m_imagePropertiesPartialPublisher )
        {
            ImagePropertiesPartial_msgFromUi msg;
            msg.m_imageUid = *m_currentImageUid;
            msg.m_properties.m_colorMapIndex = comboBoxIndex;
            m_imagePropertiesPartialPublisher( msg );
        }
    };

    auto opacityChangedHandler = [this] ( int opacity )
    {
        if ( m_currentImageUid && m_imagePropertiesPartialPublisher )
        {
            ImagePropertiesPartial_msgFromUi msg;
            msg.m_imageUid = *m_currentImageUid;
            msg.m_properties.m_opacityValue = opacity;
            m_imagePropertiesPartialPublisher( msg );
        }
    };

    auto windowChangedHandler = [this] ( double minValue, double maxValue )
    {
        if ( m_currentImageUid && m_imagePropertiesPartialPublisher )
        {
            ImagePropertiesPartial_msgFromUi msg;
            msg.m_imageUid = *m_currentImageUid;
            msg.m_properties.m_windowValues = std::make_pair( minValue, maxValue );
            m_imagePropertiesPartialPublisher( msg );
        }
    };

    auto thresholdChangedHandler = [this] ( double minValue, double maxValue )
    {
        if ( m_currentImageUid && m_imagePropertiesPartialPublisher )
        {
            ImagePropertiesPartial_msgFromUi msg;
            msg.m_imageUid = *m_currentImageUid;
            msg.m_properties.m_threshValues = std::make_pair( minValue, maxValue );
            m_imagePropertiesPartialPublisher( msg );
        }
    };

    auto samplingNnChangedHandler = [this] ( bool useNearestNeighbor )
    {
        if ( m_currentImageUid && m_imagePropertiesPartialPublisher )
        {
            ImagePropertiesPartial_msgFromUi msg;
            msg.m_imageUid = *m_currentImageUid;
            msg.m_properties.m_samplingNnChecked = useNearestNeighbor;
            msg.m_properties.m_samplingLinearChecked = ! useNearestNeighbor;
            m_imagePropertiesPartialPublisher( msg );
        }
    };

    auto samplingLinearChangedHandler = [this] ( bool useLinear )
    {
        if ( m_currentImageUid && m_imagePropertiesPartialPublisher )
        {
            ImagePropertiesPartial_msgFromUi msg;
            msg.m_imageUid = *m_currentImageUid;
            msg.m_properties.m_samplingNnChecked = ! useLinear;
            msg.m_properties.m_samplingLinearChecked = useLinear;
            m_imagePropertiesPartialPublisher( msg );
        }
    };

    auto planesVisibilityIn2dViewsChangedHandler = [this] ( bool visible )
    {
        if ( m_currentImageUid && m_imagePropertiesPartialPublisher )
        {
            ImagePropertiesPartial_msgFromUi msg;
            msg.m_imageUid = *m_currentImageUid;
            msg.m_commonProperties.m_planesVisibleIn2dViewsChecked = visible;
            m_imagePropertiesPartialPublisher( msg );
        }
    };

    auto planesVisibilityIn3dViewsChangedHandler = [this] ( bool visible )
    {
        if ( m_currentImageUid && m_imagePropertiesPartialPublisher )
        {
            ImagePropertiesPartial_msgFromUi msg;
            msg.m_imageUid = *m_currentImageUid;
            msg.m_commonProperties.m_planesVisibleIn3dViewsChecked = visible;
            m_imagePropertiesPartialPublisher( msg );
        }
    };

    auto planesAutoHidingChangedHandler = [this] ( bool useAutoHiding )
    {
        if ( m_currentImageUid && m_imagePropertiesPartialPublisher )
        {
            ImagePropertiesPartial_msgFromUi msg;
            msg.m_imageUid = *m_currentImageUid;
            msg.m_commonProperties.m_planesAutoHidingChecked = useAutoHiding;
            m_imagePropertiesPartialPublisher( msg );
        }
    };


    auto& SW = m_selectionWidgets;
    auto& IW = m_imageWidgets;

    auto comboBoxIndexChanged = static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged );

    // Image selection:
    connect( SW.m_imageSelectionComboBox, comboBoxIndexChanged, imageSelectionChangedHandler );

    // Color map selection:
    connect( IW.m_colorMapComboBox, comboBoxIndexChanged, colorMapChangedHandler );

    // Opacity:
    connect( IW.m_opacitySlider, &QSlider::valueChanged, IW.m_opacitySpinBox, &QSpinBox::setValue );
    connect( IW.m_opacitySlider, &QSlider::valueChanged, opacityChangedHandler );
    connect( IW.m_opacitySpinBox, qOverload<int>( &QSpinBox::valueChanged ),
             IW.m_opacitySlider, &QSlider::setValue );

    // Window:
    connect( IW.m_windowRangeSlider, &ctkDoubleRangeSlider::minimumValueChanged, IW.m_windowMinSpinBox, &ctkDoubleSpinBox::setValue );
    connect( IW.m_windowRangeSlider, &ctkDoubleRangeSlider::maximumValueChanged, IW.m_windowMaxSpinBox, &ctkDoubleSpinBox::setValue );
    connect( IW.m_windowRangeSlider, &ctkDoubleRangeSlider::valuesChanged, windowChangedHandler );

    connect( IW.m_windowMinSpinBox, &ctkDoubleSpinBox::valueChanged, IW.m_windowRangeSlider, &ctkDoubleRangeSlider::setMinimumValue );
    connect( IW.m_windowMaxSpinBox, &ctkDoubleSpinBox::valueChanged, IW.m_windowRangeSlider, &ctkDoubleRangeSlider::setMaximumValue );

    // Threshold:
    connect( IW.m_threshRangeSlider, &ctkDoubleRangeSlider::minimumValueChanged, IW.m_threshLowSpinBox, &ctkDoubleSpinBox::setValue );
    connect( IW.m_threshRangeSlider, &ctkDoubleRangeSlider::maximumValueChanged, IW.m_threshHighSpinBox, &ctkDoubleSpinBox::setValue );
    connect( IW.m_threshRangeSlider, &ctkDoubleRangeSlider::valuesChanged, thresholdChangedHandler );

    connect( IW.m_threshLowSpinBox, &ctkDoubleSpinBox::valueChanged, IW.m_threshRangeSlider, &ctkDoubleRangeSlider::setMinimumValue );
    connect( IW.m_threshHighSpinBox, &ctkDoubleSpinBox::valueChanged, IW.m_threshRangeSlider, &ctkDoubleRangeSlider::setMaximumValue );

    // Sampling:
    connect( IW.m_samplingNNRadioButton, &QRadioButton::toggled, samplingNnChangedHandler );
    connect( IW.m_samplingLinearRadioButton, &QRadioButton::toggled, samplingLinearChangedHandler );

    // Planes:
    connect( IW.m_planesVisibleIn2dViewsCheckBox, &QCheckBox::toggled, planesVisibilityIn2dViewsChangedHandler );
    connect( IW.m_planesVisibleIn3dViewsCheckBox, &QCheckBox::toggled, planesVisibilityIn3dViewsChangedHandler );
    connect( IW.m_planesAutoHideCheckBox, &QCheckBox::toggled, planesAutoHidingChangedHandler );
}


void RefFrameEditorDock::connectParcellationWidgets()
{
    auto parcelSelectionChangedHandler = [this] ( int comboBoxIndex )
    {
        if ( comboBoxIndex < 0 )
        {
            return;
        }

        const size_t i = static_cast<size_t>( comboBoxIndex );

        if ( i < m_parcelSelections.size() )
        {
            if ( m_currentParcelUid && *m_currentParcelUid == m_parcelSelections[i].m_parcelUid )
            {
                // Do nothing if the same parcellation is being set
                return;
            }

            m_currentParcelUid = m_parcelSelections[i].m_parcelUid;

            if ( m_currentParcelUid && m_parcelSelectionsPublisher )
            {
                ParcellationSelections_msgFromUi msg;
                msg.m_parcelUid = *m_currentParcelUid;
                msg.m_selectionIndex = comboBoxIndex;
                m_parcelSelectionsPublisher( msg );
            }

            // Update the properties, header, and labels for the new selected parcellation:
            updateParcellationProperties();
            updateParcellationHeader();
            updateParcellationLabels();
        }
        else
        {
            // error
        }
    };

    auto parcelVisibilityIn2dViewsChangedHandler = [this] ( bool visible )
    {
        if ( m_currentParcelUid && m_parcelPropertiesPartialPublisher )
        {
            ParcellationPropertiesPartial_msgFromUi msg;
            msg.m_parcelUid = *m_currentParcelUid;
            msg.m_properties.m_visibleIn2dViewsChecked = visible;
            m_parcelPropertiesPartialPublisher( msg );
        }
    };

    auto parcelVisibilityIn3dViewsChangedHandler = [this] ( bool visible )
    {
        if ( m_currentParcelUid && m_parcelPropertiesPartialPublisher )
        {
            ParcellationPropertiesPartial_msgFromUi msg;
            msg.m_parcelUid = *m_currentParcelUid;
            msg.m_properties.m_visibleIn3dViewsChecked = visible;
            m_parcelPropertiesPartialPublisher( msg );
        }
    };

    auto opacityChangedHandler = [this] ( int opacity )
    {
        if ( m_currentParcelUid && m_parcelPropertiesPartialPublisher )
        {
            ParcellationPropertiesPartial_msgFromUi msg;
            msg.m_parcelUid = *m_currentParcelUid;
            msg.m_properties.m_opacityValue = opacity;
            m_parcelPropertiesPartialPublisher( msg );
        }
    };

    auto meshVisibilityIn2dChangedHandler = [this] ( bool checked )
    {
        if ( m_currentParcelUid && m_parcelPropertiesPartialPublisher )
        {
            ParcellationPropertiesPartial_msgFromUi msg;
            msg.m_parcelUid = *m_currentParcelUid;
            msg.m_meshProperties.m_meshesVisibleIn2dViews = checked;
            m_parcelPropertiesPartialPublisher( msg );
        }
    };

    auto meshVisibilityIn3dChangedHandler = [this] ( bool checked )
    {
        if ( m_currentParcelUid && m_parcelPropertiesPartialPublisher )
        {
            ParcellationPropertiesPartial_msgFromUi msg;
            msg.m_parcelUid = *m_currentParcelUid;
            msg.m_meshProperties.m_meshesVisibleIn3dViews = checked;
            m_parcelPropertiesPartialPublisher( msg );
        }
    };

    auto meshOpacityChangedHandler = [this] ( int opacity )
    {
        if ( m_currentParcelUid && m_parcelPropertiesPartialPublisher )
        {
            ParcellationPropertiesPartial_msgFromUi msg;
            msg.m_parcelUid = *m_currentParcelUid;
            msg.m_meshProperties.m_meshOpacityValue = opacity;
            m_parcelPropertiesPartialPublisher( msg );
        }
    };

    auto meshesXrayModeChangedHandler = [this] ( bool useXrayMode )
    {
        if ( m_currentParcelUid && m_parcelPropertiesPartialPublisher )
        {
            ParcellationPropertiesPartial_msgFromUi msg;
            msg.m_parcelUid = *m_currentParcelUid;
            msg.m_meshProperties.m_meshesXrayModeChecked = useXrayMode;
            m_parcelPropertiesPartialPublisher( msg );
        }
    };

    auto meshesXrayPowerChangedHandler = [this] ( double power )
    {
        if ( m_currentParcelUid && m_parcelPropertiesPartialPublisher )
        {
            ParcellationPropertiesPartial_msgFromUi msg;
            msg.m_parcelUid = *m_currentParcelUid;
            msg.m_meshProperties.m_meshXrayPowerValue = power;
            m_parcelPropertiesPartialPublisher( msg );
        }
    };

    auto labelsChangedHandler = [this] ( std::vector<int> rows )
    {
        if ( m_labelTableModel && m_currentLabelsUid && m_parcelLabelsPartialPublisher )
        {
            ParcellationLabelsPartial_msgFromUi msg;
            msg.m_labelTableUid = *m_currentLabelsUid;

            for ( int row : rows )
            {
                if ( const ParcellationLabel* label = m_labelTableModel->getLabel( row ) )
                {
                    msg.m_labels.insert( *label );
                }
            }

            m_parcelLabelsPartialPublisher( msg );
        }
    };

    auto showAllLabelsHandler = [this] ( bool show )
    {
        if ( m_labelTableModel )
        {
            std::vector<QModelIndex> indices;

            for ( int row = 0; row < m_labelTableModel->rowCount(); ++row )
            {
                const ParcellationLabel* label = m_labelTableModel->getLabel( row );
                if ( label && ( 0 == label->m_value ) )
                {
                    // Do not affect the visibility of label with value 0
                    continue;
                }

                indices.emplace_back( m_labelTableModel->index( row, LabelTableModel::LABEL_VALUE_COLUMN ) );
            }

            // Set all data in one call, so that a new message isn't sent for each row of the model:
            m_labelTableModel->setData( indices, show, Qt::CheckStateRole );
        }
    };

    auto showAllLabelMeshesHandler = [this] ( bool show )
    {
        if ( m_labelTableModel )
        {
            std::vector<QModelIndex> indices;

            for ( int row = 0; row < m_labelTableModel->rowCount(); ++row )
            {
                indices.emplace_back( m_labelTableModel->index( row, LabelTableModel::LABEL_MESH_VISIBILITY_COLUMN ) );
            }

            // Set all data in one call, so that a new message isn't sent for each row of the model:
            m_labelTableModel->setData( indices, show, Qt::CheckStateRole );
        }
    };


    auto& SW = m_selectionWidgets;
    auto& PW = m_parcelWidgets;

    auto comboBoxIndexChanged = static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged );

    // Parcellation selection:
    connect( SW.m_parcelSelectionComboBox, comboBoxIndexChanged, parcelSelectionChangedHandler );

    // Visibility:
    connect( PW.m_visibilityIn2dViewsCheckBox, &QCheckBox::toggled, parcelVisibilityIn2dViewsChangedHandler );
    connect( PW.m_visibilityIn3dViewsCheckBox, &QCheckBox::toggled, parcelVisibilityIn3dViewsChangedHandler );

    // Opacity:
    connect( PW.m_opacitySlider, &QSlider::valueChanged, PW.m_opacitySpinBox, &QSpinBox::setValue );
    connect( PW.m_opacitySlider, &QSlider::valueChanged, opacityChangedHandler );
    connect( PW.m_opacitySpinBox, qOverload<int>( &QSpinBox::valueChanged ),
             PW.m_opacitySlider, &QSlider::setValue );

    // Mesh rendering:
    connect( PW.m_meshesVisibleIn2dViewsCheckBox, &QCheckBox::toggled, meshVisibilityIn2dChangedHandler );
    connect( PW.m_meshesVisibleIn3dViewsCheckBox, &QCheckBox::toggled, meshVisibilityIn3dChangedHandler );

    connect( PW.m_meshesXrayModeCheckBox, &QCheckBox::toggled, meshesXrayModeChangedHandler );
    connect( PW.m_meshesXrayPowerSpinBox, &ctkDoubleSpinBox::valueChanged, meshesXrayPowerChangedHandler );

    // Mesh opacity:
    connect( PW.m_meshOpacitySlider, &QSlider::valueChanged, PW.m_meshOpacitySpinBox, &QSpinBox::setValue );
    connect( PW.m_meshOpacitySlider, &QSlider::valueChanged, meshOpacityChangedHandler );
    connect( PW.m_meshOpacitySpinBox, qOverload<int>( &QSpinBox::valueChanged ),
             PW.m_meshOpacitySlider, &QSlider::setValue );

    // Label table model:
    connect( m_labelTableModel, &LabelTableModel::dataEdited, labelsChangedHandler );

    // Show/hide all labels/meshes:
    connect( PW.m_showAllLabelsButton, &QPushButton::pressed, std::bind( showAllLabelsHandler, true ) );
    connect( PW.m_hideAllLabelsButton, &QPushButton::pressed, std::bind( showAllLabelsHandler, false ) );
    connect( PW.m_showAllMeshesButton, &QPushButton::pressed, std::bind( showAllLabelMeshesHandler, true ) );
    connect( PW.m_hideAllMeshesButton, &QPushButton::pressed, std::bind( showAllLabelMeshesHandler, false ) );
}


void RefFrameEditorDock::connectTransformationWidgets()
{
    auto& W = m_transformWidgets;

    // Set Subject-to-World transformation to identity:
    auto setIdentityHandler = [this] ()
    {
        if ( m_imageTransformationPublisher && m_currentImageUid )
        {
            ImageTransformation_msgFromUi msg;
            msg.m_imageUid = *m_currentImageUid;
            msg.m_set_world_O_subject_identity = true;
            m_imageTransformationPublisher( msg );
        }
    };

    connect( W.m_setIdentityButton, &QPushButton::pressed, setIdentityHandler );
}


void RefFrameEditorDock::updateImageSelections()
{
    if ( m_imageSelectionsResponder )
    {
        setImageSelections( m_imageSelectionsResponder() );
        // enable props
    }
    else
    {
        // disable props
    }
}

void RefFrameEditorDock::updateImageColorMaps()
{
    if ( m_imageColorMapsResponder )
    {
        setImageColorMaps( m_imageColorMapsResponder() );
    }
    else
    {
        // clear color maps
    }
}

void RefFrameEditorDock::updateImageProperties()
{
    if ( m_currentImageUid && m_imagePropertiesCompleteResponder )
    {
//        setImageWidgetsEnabled( true );

        if ( const auto allProps = m_imagePropertiesCompleteResponder( *m_currentImageUid ) )
        {
            setImagePropertiesComplete( *allProps );
        }
    }
    else
    {
//        setImageWidgetsEnabled( false );
    }
}


void RefFrameEditorDock::updateImageHeader()
{
    if ( m_currentImageUid && m_imageHeaderResponder )
    {
//        setImageWidgetsEnabled( true );

        if ( const auto header = m_imageHeaderResponder( *m_currentImageUid ) )
        {
            setImageHeader( *header );
        }
    }
    else
    {
//        setImageWidgetsEnabled( false );
    }
}


void RefFrameEditorDock::updateImageTransformation()
{
    if ( m_currentImageUid && m_imageHeaderResponder )
    {
//        setImageWidgetsEnabled( true );

        if ( const auto tx = m_imageTransformationResponder( *m_currentImageUid ) )
        {
            setImageTransformation( *tx );
        }
    }
    else
    {
//        setImageWidgetsEnabled( false );
    }
}


void RefFrameEditorDock::updateImageColorMapDescription( int cmapSelectionIndex )
{
    if ( cmapSelectionIndex < 0 )
    {
        return;
    }

    const uint32_t i = static_cast<uint32_t>( cmapSelectionIndex );

    // Set text of color map description field based on selected index
    if ( i < m_imageColorMaps.size() )
    {
        m_imageWidgets.m_colorMapDescriptionLineEdit->setText(
                    QString( m_imageColorMaps[i].m_description.c_str() ) );
    }
    else
    {
        // error
        m_imageWidgets.m_colorMapDescriptionLineEdit->setText( "Invalid image color map" );
    }

    m_imageWidgets.m_colorMapDescriptionLineEdit->setCursorPosition( 0 );
}


void RefFrameEditorDock::updateParcellationSelections()
{
    if ( m_parcelSelectionsResponder )
    {
        setParcellationSelections( m_parcelSelectionsResponder() );
        // enable props
    }
    else
    {
        // disable props
    }
}


void RefFrameEditorDock::updateParcellationProperties()
{
    if ( m_currentParcelUid && m_parcelPropertiesCompleteResponder )
    {
//        setImageWidgetsEnabled( true );

        if ( const auto allProps = m_parcelPropertiesCompleteResponder( *m_currentParcelUid ) )
        {
            setParcellationPropertiesComplete( *allProps );
        }
    }
    else
    {
//        setImageWidgetsEnabled( false );
    }
}


void RefFrameEditorDock::updateParcellationHeader()
{
    if ( m_currentParcelUid && m_parcelHeaderResponder )
    {
//        setImageWidgetsEnabled( true );

        if ( const auto header = m_parcelHeaderResponder( *m_currentParcelUid ) )
        {
            setParcellationHeader( *header );
        }
    }
    else
    {
//        setImageWidgetsEnabled( false );
    }
}


void RefFrameEditorDock::updateParcellationLabels()
{
    if ( m_currentParcelUid && m_parcelLabelsCompleteResponder )
    {
//        setImageWidgetsEnabled( true );

        if ( const auto allLabels = m_parcelLabelsCompleteResponder( *m_currentParcelUid ) )
        {
            setParcellationLabelsComplete( *allLabels );
        }
        else
        {
            std::cerr << "Null labels for pacellation " << *m_currentParcelUid << std::endl;
        }
    }
    else
    {
//        setImageWidgetsEnabled( false );
    }
}


void RefFrameEditorDock::setImageSelections( const ImageSelections_msgToUi& data )
{
    if ( ! data.m_selectionIndex || *data.m_selectionIndex < 0 )
    {
        return;
    }

    const uint32_t i = static_cast<uint32_t>( *data.m_selectionIndex );
    if ( data.m_selectionItems.size() <= i )
    {
        return;
    }

    blockWidgetSignals( true );
    {
        // Copy the items:
        m_imageSelections = data.m_selectionItems;

        // Clear existing combo box items and set the new ones:
        auto& SW = m_selectionWidgets;
        SW.m_imageSelectionComboBox->clear();
//        SW.m_imageSelectionItemModel->clear();

        for ( const auto& item : data.m_selectionItems )
        {
            SW.m_imageSelectionComboBox->addItem( QString( item.m_displayName.c_str() ) );

//            QStandardItem* i = new QStandardItem();
//            i->setCheckable( true );
//            i->setData( QString( item.m_displayName.c_str() ), Qt::DisplayRole );
//            i->setData( Qt::Unchecked, Qt::CheckStateRole );
//            SW.m_imageSelectionItemModel->appendRow( i );
        }

        SW.m_imageSelectionComboBox->setCurrentIndex( *data.m_selectionIndex );
//        SW.m_imageSelectionListView->setCurrentIndex( *data.m_selectionIndex );
        m_currentImageUid = data.m_selectionItems.at( i ).m_imageUid;
    }
    blockWidgetSignals( false );

    updateImageProperties();
    updateImageHeader();
    updateImageTransformation();
    updateImageColorMaps();
}


void RefFrameEditorDock::setParcellationSelections( const ParcellationSelections_msgToUi& data )
{
    if ( ! data.m_selectionIndex || *data.m_selectionIndex < 0 )
    {
        return;
    }

    const uint32_t i = static_cast<uint32_t>( *data.m_selectionIndex );
    if ( data.m_selectionItems.size() <= i )
    {
        return;
    }

    blockWidgetSignals( true );
    {
        // Copy the items:
        m_parcelSelections = data.m_selectionItems;

        // Clear existing combo box items and set the new ones:
        auto& SW = m_selectionWidgets;
        SW.m_parcelSelectionComboBox->clear();

        for ( const auto& item : data.m_selectionItems )
        {
            SW.m_parcelSelectionComboBox->addItem( QString( item.m_displayName.c_str() ) );
        }

        SW.m_parcelSelectionComboBox->setCurrentIndex( *data.m_selectionIndex );
        m_currentParcelUid = data.m_selectionItems.at( i ).m_parcelUid;
    }
    blockWidgetSignals( false );

    updateParcellationProperties();
    updateParcellationHeader();
    updateParcellationLabels();
}


void RefFrameEditorDock::setImageColorMaps( const ImageColorMaps_msgToUi& msg )
{
    blockWidgetSignals( true );

    // Store the color map items. These are needed for the color map descriptions.
    m_imageColorMaps = msg.m_colorMapItems;

    // Set the color map combo box items:
    m_imageWidgets.m_colorMapComboBox->clear();

    for ( const auto& item : msg.m_colorMapItems )
    {
        m_imageWidgets.m_colorMapComboBox->addItem(
                    makeQIconFromColorMapItem( item, m_imageWidgets.m_colorMapComboBox->iconSize() ),
                    QString( item.m_name.c_str() ) );
    }

    blockWidgetSignals( false );
}


void RefFrameEditorDock::setImagePropertiesPartial( const ImagePropertiesPartial_msgToUi& data )
{
    blockWidgetSignals( true );

    auto& W = m_imageWidgets;
    auto& P = data.m_properties;
    auto& CP = data.m_commonProperties;

    if ( P.m_path )
    {
        W.m_pathLineEdit->setCurrentPath( QString( P.m_path->c_str() ) );
    }
    if ( P.m_loadedFromFile )
    {
        // Enable the path widget based on whether the image is from a file
        W.m_pathLineEdit->setEnabled( *P.m_loadedFromFile );
    }
    if ( P.m_displayName )
    {
        W.m_displayNameLineEdit->setText( QString( P.m_displayName->c_str() ) );
    }

    if ( P.m_colorMapIndex )
    {
        W.m_colorMapComboBox->setCurrentIndex( *P.m_colorMapIndex );
        updateImageColorMapDescription( *P.m_colorMapIndex );
    }

    if ( P.m_opacityRange )
    {
        W.m_opacitySlider->setRange( P.m_opacityRange->first, P.m_opacityRange->second );
        W.m_opacitySpinBox->setRange( P.m_opacityRange->first, P.m_opacityRange->second );
    }
    if ( P.m_opacitySingleStep )
    {
        W.m_opacitySlider->setSingleStep( *P.m_opacitySingleStep );
        W.m_opacitySpinBox->setSingleStep( *P.m_opacitySingleStep );
    }
    if ( P.m_opacitySliderPageStep )
    {
        W.m_opacitySlider->setPageStep( *P.m_opacitySliderPageStep );
    }

    if ( P.m_opacityValue )
    {
        W.m_opacitySlider->setValue( *P.m_opacityValue );
        W.m_opacitySpinBox->setValue( *P.m_opacityValue );
    }

    if ( P.m_windowRange )
    {
        W.m_windowRangeSlider->setRange( P.m_windowRange->first, P.m_windowRange->second );
        W.m_windowMinSpinBox->setRange( P.m_windowRange->first, P.m_windowRange->second );
        W.m_windowMaxSpinBox->setRange( P.m_windowRange->first, P.m_windowRange->second );
    }
    if ( P.m_windowSingleStep )
    {
        W.m_windowRangeSlider->setSingleStep( *P.m_windowSingleStep );
        W.m_windowMinSpinBox->setSingleStep( *P.m_windowSingleStep );
        W.m_windowMaxSpinBox->setSingleStep( *P.m_windowSingleStep );
    }
    if ( P.m_windowSpinBoxesDecimals )
    {
        W.m_windowMinSpinBox->setDecimals( *P.m_windowSpinBoxesDecimals );
        W.m_windowMaxSpinBox->setDecimals( *P.m_windowSpinBoxesDecimals );
    }
    if ( P.m_windowValues )
    {
        W.m_windowRangeSlider->setValues( P.m_windowValues->first, P.m_windowValues->second );
        W.m_windowMinSpinBox->setValue( P.m_windowValues->first );
        W.m_windowMaxSpinBox->setValue( P.m_windowValues->second );
    }

    if ( P.m_threshRange )
    {
        W.m_threshRangeSlider->setRange( P.m_threshRange->first, P.m_threshRange->second );
        W.m_threshLowSpinBox->setRange( P.m_threshRange->first, P.m_threshRange->second );
        W.m_threshHighSpinBox->setRange( P.m_threshRange->first, P.m_threshRange->second );
    }
    if ( P.m_threshSingleStep )
    {
        W.m_threshRangeSlider->setSingleStep( *P.m_threshSingleStep );
        W.m_threshLowSpinBox->setSingleStep( *P.m_threshSingleStep );
        W.m_threshHighSpinBox->setSingleStep( *P.m_threshSingleStep );
    }
    if ( P.m_threshSpinBoxesDecimals )
    {
        W.m_threshLowSpinBox->setDecimals( *P.m_threshSpinBoxesDecimals );
        W.m_threshHighSpinBox->setDecimals( *P.m_threshSpinBoxesDecimals );
    }
    if ( P.m_threshValues )
    {
        W.m_threshRangeSlider->setValues( P.m_threshValues->first, P.m_threshValues->second );
        W.m_threshLowSpinBox->setValue( P.m_threshValues->first );
        W.m_threshHighSpinBox->setValue( P.m_threshValues->second );
    }

    if ( P.m_samplingNnChecked )
    {
        W.m_samplingNNRadioButton->setChecked( *P.m_samplingNnChecked );
        W.m_samplingLinearRadioButton->setChecked( ! ( *P.m_samplingNnChecked ) );
    }
    if ( P.m_samplingLinearChecked )
    {
        W.m_samplingNNRadioButton->setChecked( ! ( *P.m_samplingLinearChecked ) );
        W.m_samplingLinearRadioButton->setChecked( *P.m_samplingLinearChecked );
    }

    if ( CP.m_planesVisibleIn2dViewsChecked )
    {
        W.m_planesVisibleIn2dViewsCheckBox->setChecked( *CP.m_planesVisibleIn2dViewsChecked );
    }
    if ( CP.m_planesVisibleIn3dViewsChecked )
    {
        W.m_planesVisibleIn3dViewsCheckBox->setChecked( *CP.m_planesVisibleIn3dViewsChecked );
    }
    if ( CP.m_planesAutoHidingChecked )
    {
        W.m_planesAutoHideCheckBox->setChecked( *CP.m_planesAutoHidingChecked );
    }

    blockWidgetSignals( false );
}


void RefFrameEditorDock::setImagePropertiesComplete( const ImagePropertiesComplete_msgToUi& data )
{
    blockWidgetSignals( true );

    auto& IW = m_imageWidgets;
    auto& P = data.m_properties;
    auto& CP = data.m_commonProperties;

    IW.m_pathLineEdit->setCurrentPath( QString( P.m_path().c_str() ) );

    // Enable the path widget based on whether the image is from a file
    IW.m_pathLineEdit->setEnabled( P.m_loadedFromFile() );

    IW.m_displayNameLineEdit->setText( QString( P.m_displayName().c_str() ) );

    IW.m_colorMapComboBox->setCurrentIndex( P.m_colorMapIndex() );
    updateImageColorMapDescription( P.m_colorMapIndex() );

    IW.m_opacitySlider->setRange( P.m_opacityRange().first, P.m_opacityRange().second );
    IW.m_opacitySpinBox->setRange( P.m_opacityRange().first, P.m_opacityRange().second );

    IW.m_opacitySlider->setSingleStep( P.m_opacitySingleStep() );
    IW.m_opacitySpinBox->setSingleStep( P.m_opacitySingleStep() );
    IW.m_opacitySlider->setPageStep( P.m_opacitySliderPageStep() );

    IW.m_opacitySlider->setValue( P.m_opacityValue() );
    IW.m_opacitySpinBox->setValue( P.m_opacityValue() );

    IW.m_windowRangeSlider->setRange( P.m_windowRange().first, P.m_windowRange().second );
    IW.m_windowMinSpinBox->setRange( P.m_windowRange().first, P.m_windowRange().second );
    IW.m_windowMaxSpinBox->setRange( P.m_windowRange().first, P.m_windowRange().second );

    IW.m_windowRangeSlider->setSingleStep( P.m_windowSingleStep() );
    IW.m_windowMinSpinBox->setSingleStep( P.m_windowSingleStep() );
    IW.m_windowMaxSpinBox->setSingleStep( P.m_windowSingleStep() );

    IW.m_windowMinSpinBox->setDecimals( P.m_windowSpinBoxesDecimals() );
    IW.m_windowMaxSpinBox->setDecimals( P.m_windowSpinBoxesDecimals() );

    IW.m_windowRangeSlider->setValues( P.m_windowValues().first, P.m_windowValues().second );
    IW.m_windowMinSpinBox->setValue( P.m_windowValues().first );
    IW.m_windowMaxSpinBox->setValue( P.m_windowValues().second );

    IW.m_threshRangeSlider->setRange( P.m_threshRange().first, P.m_threshRange().second );
    IW.m_threshLowSpinBox->setRange( P.m_threshRange().first, P.m_threshRange().second );
    IW.m_threshHighSpinBox->setRange( P.m_threshRange().first, P.m_threshRange().second );

    IW.m_threshRangeSlider->setSingleStep( P.m_threshSingleStep() );
    IW.m_threshLowSpinBox->setSingleStep( P.m_threshSingleStep() );
    IW.m_threshHighSpinBox->setSingleStep( P.m_threshSingleStep() );

    IW.m_threshLowSpinBox->setDecimals( P.m_threshSpinBoxesDecimals() );
    IW.m_threshHighSpinBox->setDecimals( P.m_threshSpinBoxesDecimals() );

    IW.m_threshRangeSlider->setValues( P.m_threshValues().first, P.m_threshValues().second );
    IW.m_threshLowSpinBox->setValue( P.m_threshValues().first );
    IW.m_threshHighSpinBox->setValue( P.m_threshValues().second );

    IW.m_samplingNNRadioButton->setChecked( P.m_samplingNnChecked() );
    IW.m_samplingLinearRadioButton->setChecked( ! ( P.m_samplingNnChecked() ) );

    IW.m_samplingNNRadioButton->setChecked( ! ( P.m_samplingLinearChecked() ) );
    IW.m_samplingLinearRadioButton->setChecked( P.m_samplingLinearChecked() );

    IW.m_planesVisibleIn2dViewsCheckBox->setChecked( CP.m_planesVisibleIn2dViewsChecked() );
    IW.m_planesVisibleIn3dViewsCheckBox->setChecked( CP.m_planesVisibleIn3dViewsChecked() );
    IW.m_planesAutoHideCheckBox->setChecked( CP.m_planesAutoHidingChecked() );

    blockWidgetSignals( false );
}


void RefFrameEditorDock::setImageHeader( const ImageHeader_msgToUi& msg )
{
    if ( ! m_currentImageUid || ( msg.m_imageUid != *m_currentImageUid ) )
    {
        return;
    }

    const int N = static_cast<int>( msg.m_items.size() );
    if ( N <= 0 )
    {
        return;
    }

    blockWidgetSignals( true );
    {
        setTableHeader( m_imageWidgets.m_headerTableWidget, msg.m_items );

        for ( int row = 0; row < 4; ++row )
        {
            for ( int col = 0; col < 4; ++col )
            {
                m_imageWidgets.m_subject_O_pixels_matrixWidget->setValue(
                            row, col, msg.m_subject_O_pixel[col][row] );
            }
        }
    }
    blockWidgetSignals( false );
}


void RefFrameEditorDock::setImageTransformation( const ImageTransformation_msgToUi& msg )
{
    if ( ! m_currentImageUid || ( msg.m_imageUid != *m_currentImageUid ) )
    {
        return;
    }

    blockWidgetSignals( true );
    {
        for ( int row = 0; row < 4; ++row )
        {
            for ( int col = 0; col < 4; ++col )
            {
                m_transformWidgets.m_world_O_subject_matrixWidget->setValue(
                            row, col, msg.m_world_O_subject[col][row] );
            }
        }
    }
    blockWidgetSignals( false );
}


std::optional< ImagePropertiesComplete_msgFromUi >
RefFrameEditorDock::getImagePropertiesComplete() const
{
    if ( ! m_currentImageUid )
    {
        return std::nullopt;
    }

    ImagePropertiesComplete_msgFromUi msg;
    msg.m_imageUid = *m_currentImageUid;

    auto& W = m_imageWidgets;
    auto& P = msg.m_properties;
    auto& CP = msg.m_commonProperties;

    P.m_displayName = W.m_displayNameLineEdit->text().toStdString();
    P.m_colorMapIndex = W.m_colorMapComboBox->currentIndex();
    P.m_opacityValue = W.m_opacitySlider->value();

    P.m_windowValues = std::make_pair(
                W.m_windowRangeSlider->minimumValue(),
                W.m_windowRangeSlider->maximumValue() );

    P.m_threshValues = std::make_pair(
                W.m_threshRangeSlider->minimumValue(),
                W.m_threshRangeSlider->maximumValue() );

    P.m_samplingNnChecked = W.m_samplingNNRadioButton->isChecked();
    P.m_samplingLinearChecked = W.m_samplingLinearRadioButton->isChecked();

    CP.m_planesVisibleIn2dViewsChecked = W.m_planesVisibleIn2dViewsCheckBox->isChecked();
    CP.m_planesVisibleIn3dViewsChecked = W.m_planesVisibleIn3dViewsCheckBox->isChecked();
    CP.m_planesAutoHidingChecked = W.m_planesAutoHideCheckBox->isChecked();

    return msg;
}


void RefFrameEditorDock::setParcellationPropertiesPartial( const ParcellationPropertiesPartial_msgToUi& data )
{
    blockWidgetSignals( true );

    auto& W = m_parcelWidgets;
    auto& P = data.m_properties;
    auto& MP = data.m_meshProperties;

    if ( P.m_path )
    {
        W.m_pathLineEdit->setCurrentPath( QString( P.m_path->c_str() ) );
    }
    if ( P.m_loadedFromFile )
    {
        // Enable the path widget based on whether the parcellation is from a file
        W.m_pathLineEdit->setEnabled( *P.m_loadedFromFile );
    }
    if ( P.m_displayName )
    {
        W.m_displayNameLineEdit->setText( QString( P.m_displayName->c_str() ) );
    }

    if ( P.m_visibleIn2dViewsChecked )
    {
        W.m_visibilityIn2dViewsCheckBox->setChecked( *P.m_visibleIn2dViewsChecked );
    }
    if ( P.m_visibleIn3dViewsChecked )
    {
        W.m_visibilityIn3dViewsCheckBox->setChecked( *P.m_visibleIn3dViewsChecked );
    }
    if ( P.m_opacityRange )
    {
        W.m_opacitySlider->setRange( P.m_opacityRange->first, P.m_opacityRange->second );
        W.m_opacitySpinBox->setRange( P.m_opacityRange->first, P.m_opacityRange->second );
    }
    if ( P.m_opacitySingleStep )
    {
        W.m_opacitySlider->setSingleStep( *P.m_opacitySingleStep );
        W.m_opacitySpinBox->setSingleStep( *P.m_opacitySingleStep );
    }
    if ( P.m_opacitySliderPageStep )
    {
        W.m_opacitySlider->setPageStep( *P.m_opacitySliderPageStep );
    }
    if ( P.m_opacityValue )
    {
        W.m_opacitySlider->setValue( *P.m_opacityValue );
        W.m_opacitySpinBox->setValue( *P.m_opacityValue );
    }

    if ( MP.m_meshesVisibleIn2dViews )
    {
        W.m_meshesVisibleIn2dViewsCheckBox->setChecked( *MP.m_meshesVisibleIn2dViews );
    }
    if ( MP.m_meshesVisibleIn3dViews )
    {
        W.m_meshesVisibleIn3dViewsCheckBox->setChecked( *MP.m_meshesVisibleIn3dViews );
    }

    if ( MP.m_meshesXrayModeChecked )
    {
        W.m_meshesXrayModeCheckBox->setChecked( *MP.m_meshesXrayModeChecked );
    }

    if ( MP.m_meshXrayPowerRange )
    {
        W.m_meshesXrayPowerSpinBox->setRange( MP.m_meshXrayPowerRange->first,
                                              MP.m_meshXrayPowerRange->second );
    }
    if ( MP.m_meshXrayPowerSingleStep )
    {
        W.m_meshesXrayPowerSpinBox->setSingleStep( *MP.m_meshXrayPowerSingleStep );
    }
    if ( MP.m_meshXrayPowerSpinBoxDecimals )
    {
        W.m_meshesXrayPowerSpinBox->setDecimals( *MP.m_meshXrayPowerSpinBoxDecimals );
    }
    if ( MP.m_meshXrayPowerValue )
    {
        W.m_meshesXrayPowerSpinBox->setValue( *MP.m_meshXrayPowerValue );
    }

    if ( MP.m_meshOpacityRange )
    {
        W.m_meshOpacitySlider->setRange( MP.m_meshOpacityRange->first, MP.m_meshOpacityRange->second );
        W.m_meshOpacitySpinBox->setRange( MP.m_meshOpacityRange->first, MP.m_meshOpacityRange->second );
    }
    if ( MP.m_meshOpacitySingleStep )
    {
        W.m_meshOpacitySlider->setSingleStep( *MP.m_meshOpacitySingleStep );
        W.m_meshOpacitySpinBox->setSingleStep( *MP.m_meshOpacitySingleStep );
    }
    if ( MP.m_meshOpacitySliderPageStep )
    {
        W.m_meshOpacitySlider->setPageStep( *MP.m_meshOpacitySliderPageStep );
    }
    if ( MP.m_meshOpacityValue )
    {
        W.m_meshOpacitySlider->setValue( *MP.m_meshOpacityValue );
        W.m_meshOpacitySpinBox->setValue( *MP.m_meshOpacityValue );
    }

    blockWidgetSignals( false );
}


void RefFrameEditorDock::setParcellationPropertiesComplete( const ParcellationPropertiesComplete_msgToUi& data )
{
    blockWidgetSignals( true );

    auto& W = m_parcelWidgets;
    auto& P = data.m_properties;
    auto& MP = data.m_meshProperties;

    W.m_pathLineEdit->setCurrentPath( QString( P.m_path().c_str() ) );

    // Enable the path widget based on whether the parcellation is from a file
    W.m_pathLineEdit->setEnabled( P.m_loadedFromFile() );

    W.m_displayNameLineEdit->setText( QString( P.m_displayName().c_str() ) );

    W.m_visibilityIn2dViewsCheckBox->setChecked( P.m_visibleIn2dViewsChecked() );
    W.m_visibilityIn3dViewsCheckBox->setChecked( P.m_visibleIn3dViewsChecked() );

    W.m_opacitySlider->setRange( P.m_opacityRange().first, P.m_opacityRange().second );
    W.m_opacitySpinBox->setRange( P.m_opacityRange().first, P.m_opacityRange().second );

    W.m_opacitySlider->setSingleStep( P.m_opacitySingleStep() );
    W.m_opacitySpinBox->setSingleStep( P.m_opacitySingleStep() );
    W.m_opacitySlider->setPageStep( P.m_opacitySliderPageStep() );

    W.m_opacitySlider->setValue( P.m_opacityValue() );
    W.m_opacitySpinBox->setValue( P.m_opacityValue() );


    W.m_meshesVisibleIn2dViewsCheckBox->setChecked( MP.m_meshesVisibleIn2dViews() );
    W.m_meshesVisibleIn3dViewsCheckBox->setChecked( MP.m_meshesVisibleIn3dViews() );


    W.m_meshesXrayModeCheckBox->setChecked( MP.m_meshesXrayModeChecked() );

    W.m_meshesXrayPowerSpinBox->setRange( MP.m_meshXrayPowerRange().first,
                                          MP.m_meshXrayPowerRange().second );
    W.m_meshesXrayPowerSpinBox->setSingleStep( MP.m_meshXrayPowerSingleStep() );
    W.m_meshesXrayPowerSpinBox->setDecimals( MP.m_meshXrayPowerSpinBoxDecimals() );
    W.m_meshesXrayPowerSpinBox->setValue( MP.m_meshXrayPowerValue() );


    W.m_meshOpacitySlider->setRange( MP.m_meshOpacityRange().first, MP.m_meshOpacityRange().second );
    W.m_meshOpacitySpinBox->setRange( MP.m_meshOpacityRange().first, MP.m_meshOpacityRange().second );

    W.m_meshOpacitySlider->setSingleStep( MP.m_meshOpacitySingleStep() );
    W.m_meshOpacitySpinBox->setSingleStep( MP.m_meshOpacitySingleStep() );
    W.m_meshOpacitySlider->setPageStep( MP.m_meshOpacitySliderPageStep() );

    W.m_meshOpacitySlider->setValue( MP.m_meshOpacityValue() );
    W.m_meshOpacitySpinBox->setValue( MP.m_meshOpacityValue() );

    blockWidgetSignals( false );
}


void RefFrameEditorDock::setParcellationLabelsComplete( const ParcellationLabelsComplete_msgToUi& msg )
{
    blockWidgetSignals( true );

    if ( m_labelTableModel )
    {
        m_currentLabelsUid = msg.m_labelTableUid;
        m_labelTableModel->setAllLabels( msg.m_labels );

        // Resize to contents after model has been set that defines headers:
        if ( m_parcelWidgets.m_labelTableView )
        {
            m_parcelWidgets.m_labelTableView->resizeColumnsToContents();
            m_parcelWidgets.m_labelTableView->resizeRowsToContents();

            verticalResizeTableViewToContents( m_parcelWidgets.m_labelTableView );
        }
    }
    else
    {
        std::cerr << "Error! Cannot display labels due to null model." << std::endl;
    }

    blockWidgetSignals( false );
}


void RefFrameEditorDock::setParcellationHeader( const ImageHeader_msgToUi& msg )
{
    if ( ! m_currentParcelUid || ( msg.m_imageUid != *m_currentParcelUid ) )
    {
        return;
    }

    const int N = static_cast<int>( msg.m_items.size() );
    if ( N <= 0 )
    {
        return;
    }

    blockWidgetSignals( true );
    {
        setTableHeader( m_parcelWidgets.m_headerTableWidget, msg.m_items );

        for ( int row = 0; row < 4; ++row )
        {
            for ( int col = 0; col < 4; ++col )
            {
                m_parcelWidgets.m_subject_O_pixels_matrixWidget->setValue(
                            row, col, msg.m_subject_O_pixel[col][row] );
            }
        }
    }
    blockWidgetSignals( false );
}


std::optional< ParcellationPropertiesComplete_msgFromUi >
RefFrameEditorDock::getParcellationPropertiesComplete() const
{
    if ( ! m_currentParcelUid )
    {
        return std::nullopt;
    }

    ParcellationPropertiesComplete_msgFromUi msg;
    msg.m_parcelUid = *m_currentParcelUid;

    auto& W = m_parcelWidgets;
    auto& P = msg.m_properties;
    auto& MP = msg.m_meshProperties;

    P.m_displayName = W.m_displayNameLineEdit->text().toStdString();
    P.m_visibleIn2dViewsChecked = W.m_visibilityIn2dViewsCheckBox->isChecked();
    P.m_visibleIn3dViewsChecked = W.m_visibilityIn3dViewsCheckBox->isChecked();
    P.m_opacityValue = W.m_opacitySlider->value();
    MP.m_meshesVisibleIn2dViews = W.m_meshesVisibleIn2dViewsCheckBox->isChecked();
    MP.m_meshesVisibleIn3dViews = W.m_meshesVisibleIn3dViewsCheckBox->isChecked();
    MP.m_meshesXrayModeChecked = W.m_meshesXrayModeCheckBox->isChecked();
    MP.m_meshXrayPowerValue = W.m_meshesXrayPowerSpinBox->value();
    MP.m_meshOpacityValue = W.m_meshOpacitySlider->value();

    return msg;
}


std::optional< ParcellationLabelsComplete_msgToUi >
RefFrameEditorDock::getParcellationLabelsComplete() const
{
    if ( ! m_currentLabelsUid || ! m_labelTableModel )
    {
        return std::nullopt;
    }

    ParcellationLabelsComplete_msgToUi msg;
    msg.m_labelTableUid = *m_currentLabelsUid;
    msg.m_labels = m_labelTableModel->getAllLabels();

    return msg;
}


void RefFrameEditorDock::blockWidgetSignals( bool block )
{
    for ( auto widget : m_selectionWidgetsList )
    {
        widget->blockSignals( block );
    }

    for ( auto widget : m_imageWidgetsList )
    {
        widget->blockSignals( block );
    }

    for ( auto widget : m_parcelWidgetsList )
    {
        widget->blockSignals( block );
    }

//    m_labelTableModel->blockSignals( block );
}


void RefFrameEditorDock::setWidgetsEnabled( bool enabled )
{
    for ( auto widget : m_selectionWidgetsList )
    {
        widget->setEnabled( enabled );
    }

    for ( auto widget : m_imageWidgetsList )
    {
        widget->setEnabled( enabled );
    }

    for ( auto widget : m_parcelWidgetsList )
    {
        widget->setEnabled( enabled );
    }
}

} // namespace gui
