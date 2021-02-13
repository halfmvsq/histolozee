#include "gui/docks/SlideStackEditorDock.h"
#include "gui/docks/slides/PixmapDelegate.h"
#include "gui/docks/slides/SlideSorterTableModel.h"
#include "gui/docks/Utility.h"

#include "gui/messages/slide/MoveToSlide.h"
#include "gui/messages/slide/SlideCommonProperties.h"
#include "gui/messages/slide/SlideData.h"
#include "gui/messages/slide/SlideStackData.h"

#include "common/HZeeException.hpp"

#include "externals/ctk/Widgets/ctkCollapsibleGroupBox.h"
#include "externals/ctk/Widgets/ctkDoubleSlider.h"
#include "externals/ctk/Widgets/ctkDoubleSpinBox.h"
#include "externals/ctk/Widgets/ctkMatrixWidget.h"
#include "externals/ctk/Widgets/ctkRangeSlider.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QColorDialog>
#include <QDoubleValidator>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSlider>
#include <QSpacerItem>
#include <QSpinBox>
#include <QTableView>
#include <QTableWidget>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>

#include <iostream>


namespace
{

static const QString sk_scrollAreaStyleSheet(
        "QScrollArea { background: transparent; }"
        "QScrollArea > QWidget > QWidget { background: transparent; }"
        "QScrollArea > QWidget > QScrollBar { background: palette(base); }" );


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


void setDimsTable( QTableWidget* W, const std::vector< glm::i64vec2 >& layerDims, bool doResize )
{
    W->setRowCount( static_cast<int>( layerDims.size() ) );
    W->setColumnCount( 2 );

    for ( size_t i = 0; i < layerDims.size(); ++i )
    {
        const auto& dims = layerDims.at( i );

        auto xDimString = QString::number( dims.x );
        auto yDimString = QString::number( dims.y );

        auto* xDimItem = new QTableWidgetItem( xDimString );
        auto* yDimItem = new QTableWidgetItem( yDimString );

        xDimItem->setToolTip( xDimString );
        yDimItem->setToolTip( yDimString );

        xDimItem->setFlags( xDimItem->flags() & ~Qt::ItemIsEditable );
        yDimItem->setFlags( yDimItem->flags() & ~Qt::ItemIsEditable );

        W->setItem( static_cast<int>( i ), 0, xDimItem );
        W->setItem( static_cast<int>( i ), 1, yDimItem );
    }

    if ( doResize )
    {
        W->resizeColumnsToContents();
    }
}


glm::vec3 convertQColorToVec3( const QColor& qc )
{
    return { qc.redF(), qc.greenF(), qc.blueF() };
}

} // anonymous


namespace gui
{

SlideStackEditorDock::SlideStackEditorDock( QWidget* parent )
    :
      QDockWidget( parent ),

      //      m_slideStackCompletePublisher( nullptr ),
      m_slideStackPartialPublisher( nullptr ),
      m_slideStackOrderPublisher( nullptr ),
      m_activeSlidePublisher( nullptr ),
      m_slideStackRenderingPartialPublisher( nullptr ),
      m_slideHeaderPartialPublisher( nullptr ),
      m_slideViewDataPartialPublisher( nullptr ),
      m_slideTxDataPartialPublisher( nullptr ),
      m_moveToSlidePublisher( nullptr ),

      m_slideStackCompleteResponder( nullptr ),
      m_activeSlideResponder( nullptr ),
      m_slideStackRenderingCompleteResponder( nullptr ),
      m_slideHeaderCompleteResponder( nullptr ),
      m_slideViewDataCompleteResponder( nullptr ),
      m_slideTxDataCompleteResponder( nullptr ),

      m_slideSorterTableView( nullptr ),
      m_slideSorterTableModel( nullptr ),
      m_slideSorterPixmapDelegate( nullptr )
{
    // Note: In case no parent is provided on construction, this dock widget will get
    // parented when added as a dock to the QMainWindow object

    setWindowTitle( "Slide Stack Editor" );

    setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

    setFeatures( QDockWidget::DockWidgetClosable |
                 QDockWidget::DockWidgetFloatable |
                 QDockWidget::DockWidgetMovable );

    auto slideSorterWidget = createSlideSorderWidget();
    if ( ! slideSorterWidget )
    {
        throw_debug( "Unable to create Slide Sorter Widget" );
    }

    auto tabWidget = createTabWidget();
    if ( ! tabWidget )
    {
        throw_debug( "Unable to create Slide Stack Tab Widget" );
    }

    auto commonScrollArea = createSlideStackCommonScrollArea();
    if ( ! commonScrollArea )
    {
        throw_debug( "Unable to create common rendering properties widget" );
    }

    auto layout = new QVBoxLayout();
    setZeroContentsMargins( layout, true, true, true, true );
    layout->addWidget( slideSorterWidget );
    layout->addWidget( tabWidget );
    layout->addWidget( commonScrollArea );

    auto widget = new QWidget();
    setZeroContentsMargins( widget, true, true, true, true );
    widget->setLayout( layout );

    setWidget( widget );

    connectCommonWidgets();
    connectHeaderWidgets();
    connectViewWidgets();
    connectTxWidgets();

    refresh();
}

SlideStackEditorDock::~SlideStackEditorDock() = default;


void SlideStackEditorDock::refresh()
{
    if ( ! m_slideStackCompleteResponder || ! m_activeSlideResponder ||
         ! m_slideStackRenderingCompleteResponder )
    {
        return;
    }

    setSlideStackComplete( m_slideStackCompleteResponder() );
    setActiveSlide( m_activeSlideResponder() );
    setCommonSlidePropertiesComplete( m_slideStackRenderingCompleteResponder() );
}


void SlideStackEditorDock::setSlideStackPartialPublisher( SlideStackPartial_msgFromUi_PublisherType publisher )
{
    m_slideStackPartialPublisher = publisher;
}

void SlideStackEditorDock::setSlideStackOrderPublisher( SlideStackOrder_msgFromUi_PublisherType publisher )
{
    m_slideStackOrderPublisher = publisher;
}

void SlideStackEditorDock::setActiveSlidePublisher( ActiveSlide_msgFromUi_PublisherType publisher )
{
    m_activeSlidePublisher = publisher;
}

void SlideStackEditorDock::setSlideCommonPropertiesPartialPublisher(
        SlideCommonPropertiesPartial_msgFromUi_PublisherType publisher )
{
    m_slideStackRenderingPartialPublisher = publisher;
}

void SlideStackEditorDock::setSlideHeaderPartialPublisher( SlideHeaderPartial_msgFromUi_PublisherType publisher )
{
    m_slideHeaderPartialPublisher = publisher;
}

void SlideStackEditorDock::setSlideViewDataPartialPublisher( SlideViewDataPartial_msgFromUi_PublisherType publisher )
{
    m_slideViewDataPartialPublisher = publisher;
}

void SlideStackEditorDock::setSlideTxDataPartialPublisher( SlideTxDataPartial_msgFromUi_PublisherType publisher )
{
    m_slideTxDataPartialPublisher = publisher;
}

void SlideStackEditorDock::setMoveToSlidePublisher( MoveToSlide_msgFromUi_PublisherType publisher )
{
    m_moveToSlidePublisher = publisher;
}

void SlideStackEditorDock::setSlideStackCompleteResponder( SlideStackComplete_msgToUi_ResponderType responder )
{
    m_slideStackCompleteResponder = responder;
}

void SlideStackEditorDock::setActiveSlideResponder( ActiveSlide_msgToUi_ResponderType responder )
{
    m_activeSlideResponder = responder;
}

void SlideStackEditorDock::setSlideCommonPropertiesCompleteResponder(
        SlideCommonPropertiesComplete_msgToUi_ResponderType responder )
{
    m_slideStackRenderingCompleteResponder = responder;
}

void SlideStackEditorDock::setSlideHeaderCompleteResponder( SlideHeaderComplete_msgToUi_ResponderType responder )
{
    m_slideHeaderCompleteResponder = responder;
}

void SlideStackEditorDock::setSlideViewDataCompleteResponder( SlideViewDataComplete_msgToUi_ResponderType responder )
{
    m_slideViewDataCompleteResponder = responder;
}

void SlideStackEditorDock::setSlideTxDataCompleteResponder( SlideTxDataComplete_msgToUi_ResponderType responder )
{
    m_slideTxDataCompleteResponder = responder;
}


void SlideStackEditorDock::setSlideStackComplete( const SlideStackComplete_msgToUi& msg )
{
    if ( m_slideSorterTableModel && m_slideSorterTableView )
    {
        m_slideSorterTableModel->setSlideStack( msg.m_slides );
        m_slideSorterTableView->resizeColumnsToContents();

        /// @todo Consolidate all below into single function
        if ( auto* H = m_slideSorterTableView->horizontalHeader() )
        {
            H->stretchLastSection();
        }

        verticalResizeTableViewToContents( m_slideSorterTableView );

        if ( msg.m_activeSlideIndex && msg.m_activeSlideUid )
        {
            // There is a currently active slide. Set it in the dock and update all widgets
            // with the slide's data.
            m_activeSlideUid = *msg.m_activeSlideUid;
            selectSlideIndex( *msg.m_activeSlideIndex );

            updateSlideTabWidgets( *msg.m_activeSlideUid );
        }
        else
        {
            m_activeSlideUid = std::nullopt;
            m_slideSorterTableView->clearSelection();

            /// @todo Clear all widgets
        }
    }

    setMatrixWidgetValues( m_commonWidgets.m_world_O_stack_matrixWidget, msg.m_world_O_stack );
}


void SlideStackEditorDock::setSlideStackPartial( const SlideStackPartial_msgToUi& msg )
{
    QSignalBlocker2<SlideStackEditorDock>( this );

    if ( m_slideSorterTableModel && m_slideSorterTableView )
    {
        for ( const auto& slide : msg.m_slides )
        {
            if ( ! m_slideSorterTableModel->replaceSlide( slide ) )
            {
                std::cerr << "Unable to replace slide at index " << slide.m_index << std::endl;
                continue;
            }
        }

        m_slideSorterTableView->resizeColumnsToContents();

        if ( auto* H = m_slideSorterTableView->horizontalHeader() )
        {
            H->stretchLastSection();
        }

        verticalResizeTableViewToContents( m_slideSorterTableView );
    }

    if ( msg.m_world_O_stack )
    {
        setMatrixWidgetValues( m_commonWidgets.m_world_O_stack_matrixWidget, *msg.m_world_O_stack );
    }
}


void SlideStackEditorDock::setActiveSlide( const ActiveSlide_msgToUi& msg )
{
    QSignalBlocker2<SlideStackEditorDock>( this );

    /// @todo Consolidate all below into single function

    if ( msg.m_activeSlideIndex && msg.m_activeSlideUid )
    {
        m_activeSlideUid = *msg.m_activeSlideUid;
        selectSlideIndex( *msg.m_activeSlideIndex );

        updateSlideTabWidgets( *msg.m_activeSlideUid );
    }
    else if ( m_slideSorterTableView )
    {
        m_activeSlideUid = std::nullopt;
        m_slideSorterTableView->clearSelection();
    }

    m_slideSorterTableView->resizeColumnsToContents();

    if ( auto* H = m_slideSorterTableView->horizontalHeader() )
    {
        H->stretchLastSection();
    }
}


void SlideStackEditorDock::setSlideCommonPropertiesPartial(
        const SlideCommonPropertiesPartial_msgToUi& msg )
{
    QSignalBlocker2<SlideStackEditorDock>( this );

    auto& W = m_commonWidgets;
    auto& P = msg.m_properties;

    if ( P.m_masterOpacityRange )
    {
        W.m_masterOpacitySlider->setRange( P.m_masterOpacityRange->first, P.m_masterOpacityRange->second );
        W.m_masterOpacitySpinBox->setRange( P.m_masterOpacityRange->first, P.m_masterOpacityRange->second );
    }
    if ( P.m_masterOpacitySingleStep )
    {
        W.m_masterOpacitySlider->setSingleStep( *P.m_masterOpacitySingleStep );
        W.m_masterOpacitySpinBox->setSingleStep( *P.m_masterOpacitySingleStep );
    }
    if ( P.m_masterOpacitySliderPageStep )
    {
        W.m_masterOpacitySlider->setPageStep( *P.m_masterOpacitySliderPageStep );
    }
    if ( P.m_masterOpacityValue )
    {
        W.m_masterOpacitySlider->setValue( *P.m_masterOpacityValue );
        W.m_masterOpacitySpinBox->setValue( *P.m_masterOpacityValue );
    }

    if ( P.m_image3dOpacityRange )
    {
        W.m_image3dLayerOpacitySlider->setRange( P.m_image3dOpacityRange->first, P.m_image3dOpacityRange->second );
        W.m_image3dLayerOpacitySpinBox->setRange( P.m_image3dOpacityRange->first, P.m_image3dOpacityRange->second );
    }
    if ( P.m_image3dOpacitySingleStep )
    {
        W.m_image3dLayerOpacitySlider->setSingleStep( *P.m_image3dOpacitySingleStep );
        W.m_image3dLayerOpacitySpinBox->setSingleStep( *P.m_image3dOpacitySingleStep );
    }
    if ( P.m_image3dOpacitySliderPageStep )
    {
        W.m_image3dLayerOpacitySlider->setPageStep( *P.m_image3dOpacitySliderPageStep );
    }
    if ( P.m_image3dOpacityValue )
    {
        W.m_image3dLayerOpacitySlider->setValue( *P.m_image3dOpacityValue );
        W.m_image3dLayerOpacitySpinBox->setValue( *P.m_image3dOpacityValue );
    }

    if ( P.m_stackVisibleIn2dViewsChecked )
    {
        W.m_visibleIn2dViewsCheckBox->setChecked( *P.m_stackVisibleIn2dViewsChecked );
    }
    if ( P.m_stackVisibleIn3dViewsChecked )
    {
        W.m_visibleIn3dViewsCheckBox->setChecked( *P.m_stackVisibleIn3dViewsChecked );
    }
    if ( P.m_activeSlideViewShows2dSlidesChecked )
    {
        W.m_activeSlideViewShows2dSlidesRadioButton->setChecked( *P.m_activeSlideViewShows2dSlidesChecked );
    }
    if ( P.m_activeSlideViewDirectionTopToBottomChecked )
    {
        W.m_activeSlideViewDirectionButton->setChecked( *P.m_activeSlideViewDirectionTopToBottomChecked );
    }
}


void SlideStackEditorDock::setCommonSlidePropertiesComplete(
        const SlideCommonPropertiesComplete_msgToUi& msg )
{
    QSignalBlocker2<SlideStackEditorDock>( this );

    auto& W = m_commonWidgets;
    auto& P = msg.m_properties;

    W.m_masterOpacitySlider->setRange( P.m_masterOpacityRange().first, P.m_masterOpacityRange().second );
    W.m_masterOpacitySpinBox->setRange( P.m_masterOpacityRange().first, P.m_masterOpacityRange().second );

    W.m_masterOpacitySlider->setSingleStep( P.m_masterOpacitySingleStep() );
    W.m_masterOpacitySpinBox->setSingleStep( P.m_masterOpacitySingleStep() );

    W.m_masterOpacitySlider->setPageStep( P.m_masterOpacitySliderPageStep() );

    W.m_masterOpacitySlider->setValue( P.m_masterOpacityValue() );
    W.m_masterOpacitySpinBox->setValue( P.m_masterOpacityValue() );


    W.m_image3dLayerOpacitySlider->setRange( P.m_image3dOpacityRange().first, P.m_image3dOpacityRange().second );
    W.m_image3dLayerOpacitySpinBox->setRange( P.m_image3dOpacityRange().first, P.m_image3dOpacityRange().second );

    W.m_image3dLayerOpacitySlider->setSingleStep( P.m_image3dOpacitySingleStep() );
    W.m_image3dLayerOpacitySpinBox->setSingleStep( P.m_image3dOpacitySingleStep() );

    W.m_image3dLayerOpacitySlider->setPageStep( P.m_image3dOpacitySliderPageStep() );

    W.m_image3dLayerOpacitySlider->setValue( P.m_image3dOpacityValue() );
    W.m_image3dLayerOpacitySpinBox->setValue( P.m_image3dOpacityValue() );


    W.m_visibleIn2dViewsCheckBox->setChecked( P.m_stackVisibleIn2dViewsChecked() );
    W.m_visibleIn3dViewsCheckBox->setChecked( P.m_stackVisibleIn3dViewsChecked() );
    W.m_activeSlideViewShows2dSlidesRadioButton->setChecked( P.m_activeSlideViewShows2dSlidesChecked() );
    W.m_activeSlideViewDirectionButton->setChecked( P.m_activeSlideViewDirectionTopToBottomChecked() );
}


void SlideStackEditorDock::setSlideHeaderComplete( const SlideHeaderComplete_msgToUi& msg )
{
    if ( ! isActiveSlide( msg.m_uid ) )
    {
        // Ignore incoming slide data from inactive slide
        return;
    }

    const auto& HI = msg.m_headerImmutable;
    const auto& HM = msg.m_headerMutable;

    auto& W = m_headerWidgets;

    QSignalBlocker2<SlideStackEditorDock>( this );

    if ( W.m_pixelSizeHorizLineEdit )
    {
        W.m_pixelSizeHorizLineEditValidator->setRange(
                    HI.m_pixelSizeRange.first, HI.m_pixelSizeRange.second, 6 );
        W.m_pixelSizeHorizLineEdit->setText( QString::number( static_cast<double>( HM.m_pixelSizeX() ) ) );
    }

    if ( W.m_pixelSizeVertLineEdit )
    {
        W.m_pixelSizeVertLineEditValidator->setRange(
                    HI.m_pixelSizeRange.first, HI.m_pixelSizeRange.second, 6 );
        W.m_pixelSizeVertLineEdit->setText( QString::number( static_cast<double>( HM.m_pixelSizeY() ) ) );
    }

    if ( W.m_thicknessLineEdit )
    {
        W.m_thicknessLineEditValidator->setRange(
                    HI.m_thicknessRange.first, HI.m_thicknessRange.second, 6 );
        W.m_thicknessLineEdit->setText( QString::number( static_cast<double>( HM.m_thickness() ) ) );
    }

    if ( W.m_displayNameLineEdit )
    {
        W.m_displayNameLineEdit->setText( QString::fromStdString( HM.m_displayName() ) );
    }

    if ( W.m_fileNameLineEdit )
    {
        W.m_fileNameLineEdit->setText( QString::fromStdString( HI.m_filePath ) );
    }

    if ( W.m_vendorIdLineEdit )
    {
        W.m_vendorIdLineEdit->setText( QString::fromStdString( HI.m_slideType ) );
    }

    if ( W.m_layerDimsTableWidget )
    {
        static constexpr bool sk_doResize = false;
        setDimsTable( W.m_layerDimsTableWidget, HI.m_layerDims, sk_doResize );
    }

    if ( W.m_labelImageLabel )
    {
        if ( auto buffer = HI.m_labelImageBuffer.lock() )
        {
            const QImage image( reinterpret_cast<unsigned char*>( buffer->data() ),
                                static_cast<int>( HI.m_labelImageDims.x ),
                                static_cast<int>( HI.m_labelImageDims.y ),
                                QImage::Format_ARGB32_Premultiplied );

            W.m_labelImageLabel->setPixmap( QPixmap::fromImage( image ) );
            W.m_labelImageLabel->setText( "" );
        }
        else
        {
            W.m_labelImageLabel->setText( "N/A" );
        }
    }

    if ( W.m_macroImageLabel )
    {
        if ( auto buffer = HI.m_macroImageBuffer.lock() )
        {
            const QImage image( reinterpret_cast<unsigned char*>( buffer->data() ),
                                static_cast<int>( HI.m_macroImageDims.x ),
                                static_cast<int>( HI.m_macroImageDims.y ),
                                QImage::Format_ARGB32_Premultiplied );

            W.m_macroImageLabel->setPixmap( QPixmap::fromImage( image ) );
            W.m_macroImageLabel->setText( "" );
        }
        else
        {
            W.m_macroImageLabel->setText( "N/A" );
        }
    }
}


void SlideStackEditorDock::setSlideViewDataComplete( const SlideViewDataComplete_msgToUi& msg )
{
    if ( ! isActiveSlide( msg.m_uid ) )
    {
        // Ignore incoming slide data from inactive slide
        return;
    }

    QSignalBlocker2<SlideStackEditorDock>( this );

    const auto& VD = msg.m_viewData;
    auto& W = m_viewWidgets;

    if ( W.m_showSlideCheckBox )
    {
        W.m_showSlideCheckBox->setChecked( VD.m_slideVisibleChecked() );
    }

    if ( W.m_opacitySlider && W.m_opacitySpinBox )
    {
        W.m_opacitySlider->setRange( VD.m_opacityRange().first, VD.m_opacityRange().second );
        W.m_opacitySpinBox->setRange( VD.m_opacityRange().first, VD.m_opacityRange().second );

        W.m_opacitySlider->setSingleStep( VD.m_opacitySingleStep() );
        W.m_opacitySpinBox->setSingleStep( VD.m_opacitySingleStep() );

        W.m_opacitySlider->setPageStep( VD.m_opacitySliderPageStep() );

        W.m_opacitySlider->setValue( VD.m_opacityValue() );
        W.m_opacitySpinBox->setValue( VD.m_opacityValue() );
    }

    if ( W.m_threshRangeSlider && W.m_threshLowSpinBox && W.m_threshHighSpinBox )
    {
        W.m_threshRangeSlider->setRange( VD.m_threshRange().first, VD.m_threshRange().second );
        W.m_threshLowSpinBox->setRange( VD.m_threshRange().first, VD.m_threshRange().second );
        W.m_threshHighSpinBox->setRange( VD.m_threshRange().first, VD.m_threshRange().second );

        W.m_threshRangeSlider->setSingleStep( VD.m_threshSingleStep() );
        W.m_threshLowSpinBox->setSingleStep( VD.m_threshSingleStep() );
        W.m_threshHighSpinBox->setSingleStep( VD.m_threshSingleStep() );

        W.m_threshRangeSlider->setPageStep( VD.m_threshSliderPageStep() );

        W.m_threshRangeSlider->setValues( VD.m_threshValues().first, VD.m_threshValues().second );
        W.m_threshLowSpinBox->setValue( VD.m_threshValues().first );
        W.m_threshHighSpinBox->setValue( VD.m_threshValues().second );
    }

    if ( W.m_showEdgesCheckBox )
    {
        W.m_showEdgesCheckBox->setChecked( VD.m_edgesVisibleChecked() );
    }

    if ( W.m_edgesMagnitudeSlider && W.m_edgesMagnitudeSpinBox )
    {
        W.m_edgesMagnitudeSlider->setRange( VD.m_edgesMagnitudeRange().first, VD.m_edgesMagnitudeRange().second );
        W.m_edgesMagnitudeSpinBox->setRange( VD.m_edgesMagnitudeRange().first, VD.m_edgesMagnitudeRange().second );

        W.m_edgesMagnitudeSlider->setSingleStep( VD.m_edgesMagnitudeSingleStep() );
        W.m_edgesMagnitudeSpinBox->setSingleStep( VD.m_edgesMagnitudeSingleStep() );

        W.m_edgesMagnitudeSlider->setPageStep( VD.m_edgesMagnitudePageStep() );

        W.m_edgesMagnitudeSpinBox->setDecimals( VD.m_edgesMagnitudeDecimalPrecision() );

        W.m_edgesMagnitudeSlider->setValue( VD.m_edgesMagnitudeValue() );
        W.m_edgesMagnitudeSpinBox->setValue( VD.m_edgesMagnitudeValue() );
    }

    if ( W.m_edgesSmoothingSlider && W.m_edgesSmoothingSpinBox )
    {
        W.m_edgesSmoothingSlider->setRange( VD.m_edgesSmoothingRange().first, VD.m_edgesSmoothingRange().second );
        W.m_edgesSmoothingSpinBox->setRange( VD.m_edgesSmoothingRange().first, VD.m_edgesSmoothingRange().second );

        W.m_edgesSmoothingSlider->setSingleStep( VD.m_edgesSmoothingSingleStep() );
        W.m_edgesSmoothingSpinBox->setSingleStep( VD.m_edgesSmoothingSingleStep() );

        W.m_edgesSmoothingSlider->setPageStep( VD.m_edgesSmoothingPageStep() );

        W.m_edgesSmoothingSpinBox->setDecimals( VD.m_edgesSmoothingDecimalPrecision() );

        W.m_edgesSmoothingSlider->setValue( VD.m_edgesSmoothingValue() );
        W.m_edgesSmoothingSpinBox->setValue( VD.m_edgesSmoothingValue() );
    }

    if ( W.m_borderColorButton )
    {
        const auto c = glm::dvec3{ VD.m_borderColor() };

        QPixmap px( 32, 32 );
        px.fill( QColor::fromRgbF( c.r, c.g, c.b ) );
        W.m_borderColorButton->setIcon( px );
    }
}


void SlideStackEditorDock::setSlideViewDataPartial( const SlideViewDataPartial_msgToUi& msg )
{
    if ( ! isActiveSlide( msg.m_uid ) )
    {
        // Ignore incoming slide data from inactive slide
        return;
    }

    QSignalBlocker2<SlideStackEditorDock>( this );

    const auto& VD = msg.m_viewData;
    auto& W = m_viewWidgets;

    if ( VD.m_slideVisibleChecked && W.m_showSlideCheckBox )
    {
        W.m_showSlideCheckBox->setChecked( *VD.m_slideVisibleChecked );
    }

    if ( VD.m_opacityValue && W.m_opacitySlider && W.m_opacitySpinBox )
    {
        W.m_opacitySlider->setValue( *VD.m_opacityValue );
        W.m_opacitySpinBox->setValue( *VD.m_opacityValue );
    }

    if ( VD.m_threshValues && W.m_threshRangeSlider && W.m_threshLowSpinBox && W.m_threshHighSpinBox )
    {
        W.m_threshRangeSlider->setValues( VD.m_threshValues->first, VD.m_threshValues->second );
        W.m_threshLowSpinBox->setValue( VD.m_threshValues->first );
        W.m_threshHighSpinBox->setValue( VD.m_threshValues->second );
    }

    if ( VD.m_edgesVisibleChecked && W.m_showEdgesCheckBox )
    {
        W.m_showEdgesCheckBox->setChecked( *VD.m_edgesVisibleChecked );
    }

    if ( VD.m_edgesMagnitudeValue && W.m_edgesMagnitudeSlider && W.m_edgesMagnitudeSpinBox )
    {
        W.m_edgesMagnitudeSlider->setValue( *VD.m_edgesMagnitudeValue );
        W.m_edgesMagnitudeSpinBox->setValue( *VD.m_edgesMagnitudeValue );
    }

    if ( VD.m_edgesSmoothingValue && W.m_edgesSmoothingSlider && W.m_edgesSmoothingSpinBox )
    {
        W.m_edgesSmoothingSlider->setValue( *VD.m_edgesSmoothingValue );
        W.m_edgesSmoothingSpinBox->setValue( *VD.m_edgesSmoothingValue );
    }

    if ( VD.m_borderColor && W.m_borderColorButton )
    {
        const auto c = glm::dvec3{ *VD.m_borderColor };

        QPixmap px( 32, 32 );
        px.fill( QColor::fromRgbF( c.r, c.g, c.b ) );
        W.m_borderColorButton->setIcon( px );
    }
}


void SlideStackEditorDock::setSlideTxDataComplete( const SlideTxDataComplete_msgToUi& msg )
{
    if ( ! isActiveSlide( msg.m_uid ) )
    {
        // Ignore incoming slide data from inactive slide
        return;
    }

    QSignalBlocker2<SlideStackEditorDock>( this );

    const auto& TD = msg.m_txData;
    auto& W = m_transformWidgets;

    if ( W.m_xTranslationSpinBox )
    {
        W.m_xTranslationSpinBox->setRange( TD.m_translationRange().first, TD.m_translationRange().second );
        W.m_xTranslationSpinBox->setSingleStep( TD.m_translationSingleStep() );
        W.m_xTranslationSpinBox->setDecimals( TD.m_translationDecimalPrecision() );
        W.m_xTranslationSpinBox->setValue( TD.m_xTranslationValueInMm() );
    }

    if ( W.m_yTranslationSpinBox )
    {
        W.m_yTranslationSpinBox->setRange( TD.m_translationRange().first, TD.m_translationRange().second );
        W.m_yTranslationSpinBox->setSingleStep( TD.m_translationSingleStep() );
        W.m_yTranslationSpinBox->setDecimals( TD.m_translationDecimalPrecision() );
        W.m_yTranslationSpinBox->setValue( TD.m_yTranslationValueInMm() );
    }

    if ( W.m_zTranslationSpinBox )
    {
        W.m_zTranslationSpinBox->setRange( TD.m_translationRange().first, TD.m_translationRange().second );
        W.m_zTranslationSpinBox->setSingleStep( TD.m_translationSingleStep() );
        W.m_zTranslationSpinBox->setDecimals( TD.m_translationDecimalPrecision() );
        W.m_zTranslationSpinBox->setValue( TD.m_zTranslationValueInMm() );
    }

    if ( W.m_zRotationSpinBox )
    {
        W.m_zRotationSpinBox->setRange( TD.m_rotationRange().first, TD.m_rotationRange().second );
        W.m_zRotationSpinBox->setSingleStep( TD.m_rotationSingleStep() );
        W.m_zRotationSpinBox->setDecimals( TD.m_rotationDecimalPrecision() );
        W.m_zRotationSpinBox->setValue( TD.m_zRotationValueInDeg() );
    }

    if ( W.m_xScaleSpinBox )
    {
        W.m_xScaleSpinBox->setRange( TD.m_scaleRange().first, TD.m_scaleRange().second );
        W.m_xScaleSpinBox->setSingleStep( TD.m_scaleSingleStep() );
        W.m_xScaleSpinBox->setDecimals( TD.m_scaleDecimalPrecision() );
        W.m_xScaleSpinBox->setValue( TD.m_xScaleValue() );
    }

    if ( W.m_yScaleSpinBox )
    {
        W.m_yScaleSpinBox->setRange( TD.m_scaleRange().first, TD.m_scaleRange().second );
        W.m_yScaleSpinBox->setSingleStep( TD.m_scaleSingleStep() );
        W.m_yScaleSpinBox->setDecimals( TD.m_scaleDecimalPrecision() );
        W.m_yScaleSpinBox->setValue( TD.m_yScaleValue() );
    }

    if ( W.m_xShearSpinBox )
    {
        W.m_xShearSpinBox->setRange( TD.m_shearRange().first, TD.m_shearRange().second );
        W.m_xShearSpinBox->setSingleStep( TD.m_shearSingleStep() );
        W.m_xShearSpinBox->setDecimals( TD.m_shearDecimalPrecision() );
        W.m_xShearSpinBox->setValue( TD.m_xShearValueInDeg() );
    }

    if ( W.m_yShearSpinBox )
    {
        W.m_yShearSpinBox->setRange( TD.m_shearRange().first, TD.m_shearRange().second );
        W.m_yShearSpinBox->setSingleStep( TD.m_shearSingleStep() );
        W.m_yShearSpinBox->setDecimals( TD.m_shearDecimalPrecision() );
        W.m_yShearSpinBox->setValue( TD.m_yShearValueInDeg() );
    }

    if ( W.m_zScaleRotationSpinBox )
    {
        W.m_zScaleRotationSpinBox->setRange( TD.m_scaleRotationRange().first, TD.m_scaleRotationRange().second );
        W.m_zScaleRotationSpinBox->setSingleStep( TD.m_scaleRotationSingleStep() );
        W.m_zScaleRotationSpinBox->setDecimals( TD.m_scaleRotationDecimalPrecision() );
        W.m_zScaleRotationSpinBox->setValue( TD.m_zScaleRotationValueInDeg() );
    }

    if ( W.m_xOriginSpinBox )
    {
        W.m_xOriginSpinBox->setRange( TD.m_originRange().first, TD.m_originRange().second );
        W.m_xOriginSpinBox->setSingleStep( TD.m_originSingleStep() );
        W.m_xOriginSpinBox->setDecimals( TD.m_originDecimalPrecision() );
        W.m_xOriginSpinBox->setValue( TD.m_xOriginValueInMm() );
    }

    if ( W.m_yOriginSpinBox )
    {
        W.m_yOriginSpinBox->setRange( TD.m_originRange().first, TD.m_originRange().second );
        W.m_yOriginSpinBox->setSingleStep( TD.m_originSingleStep() );
        W.m_yOriginSpinBox->setDecimals( TD.m_originDecimalPrecision() );
        W.m_yOriginSpinBox->setValue( TD.m_yOriginValueInMm() );
    }

    if ( W.m_paramScaleRotationRadioButton )
    {
        const bool checked = TD.m_useScaleRotationParameterization();
        W.m_paramScaleRotationRadioButton->setChecked( checked );
        W.m_zScaleRotationSpinBox->setEnabled( checked );
        W.m_xShearSpinBox->setEnabled( ! checked );
        W.m_yShearSpinBox->setEnabled( ! checked );
    }

    if ( W.m_paramShearAnglesRadioButton )
    {
        const bool checked = ! TD.m_useScaleRotationParameterization();
        W.m_paramShearAnglesRadioButton->setChecked( checked );
        W.m_zScaleRotationSpinBox->setEnabled( ! checked );
        W.m_xShearSpinBox->setEnabled( checked );
        W.m_yShearSpinBox->setEnabled( checked );
    }

    if ( W.m_stack_O_slide_matrixWidget )
    {
        setMatrixWidgetValues( W.m_stack_O_slide_matrixWidget, TD.m_stack_O_slide_matrix() );
    }
}


void SlideStackEditorDock::setSlideTxDataPartial( const SlideTxDataPartial_msgToUi& msg )
{   
    if ( ! isActiveSlide( msg.m_uid ) )
    {
        // Ignore incoming slide data from inactive slide
        return;
    }

    QSignalBlocker2<SlideStackEditorDock>( this );

    const auto& TD = msg.m_txData;
    auto& W = m_transformWidgets;

    if ( TD.m_xTranslationValueInMm && W.m_xTranslationSpinBox )
    {
        W.m_xTranslationSpinBox->setValue( *TD.m_xTranslationValueInMm );
    }

    if ( TD.m_yTranslationValueInMm && W.m_yTranslationSpinBox )
    {
        W.m_yTranslationSpinBox->setValue( *TD.m_yTranslationValueInMm );
    }

    if ( TD.m_zTranslationValueInMm && W.m_zTranslationSpinBox )
    {
        W.m_zTranslationSpinBox->setValue( *TD.m_zTranslationValueInMm );
    }

    if ( TD.m_zRotationValueInDeg && W.m_zRotationSpinBox )
    {
        W.m_zRotationSpinBox->setValue( *TD.m_zRotationValueInDeg );
    }

    if ( TD.m_xScaleValue && W.m_xScaleSpinBox )
    {
        W.m_xScaleSpinBox->setValue( *TD.m_xScaleValue );
    }

    if ( TD.m_yScaleValue && W.m_yScaleSpinBox )
    {
        W.m_yScaleSpinBox->setValue( *TD.m_yScaleValue );
    }

    if ( TD.m_xShearValueInDeg && W.m_xShearSpinBox )
    {
        W.m_xShearSpinBox->setValue( *TD.m_xShearValueInDeg );
    }

    if ( TD.m_yShearValueInDeg && W.m_yShearSpinBox )
    {
        W.m_yShearSpinBox->setValue( *TD.m_yShearValueInDeg );
    }

    if ( TD.m_zScaleRotationValueInDeg && W.m_zScaleRotationSpinBox )
    {
        W.m_zScaleRotationSpinBox->setValue( *TD.m_zScaleRotationValueInDeg );
    }

    if (TD.m_xOriginValueInMm && W.m_xOriginSpinBox )
    {
        W.m_xOriginSpinBox->setValue( *TD.m_xOriginValueInMm );
    }

    if ( TD.m_yOriginValueInMm && W.m_yOriginSpinBox )
    {
        W.m_yOriginSpinBox->setValue( *TD.m_yOriginValueInMm );
    }

    if ( TD.m_useScaleRotationParameterization && W.m_paramScaleRotationRadioButton )
    {
        const bool checked = *TD.m_useScaleRotationParameterization;
        W.m_paramScaleRotationRadioButton->setChecked( checked );
        W.m_zScaleRotationSpinBox->setEnabled( checked );
        W.m_xShearSpinBox->setEnabled( ! checked );
        W.m_yShearSpinBox->setEnabled( ! checked );
    }

    if ( TD.m_useScaleRotationParameterization && W.m_paramShearAnglesRadioButton )
    {
        const bool checked = ! ( *TD.m_useScaleRotationParameterization );
        W.m_paramShearAnglesRadioButton->setChecked( checked );
        W.m_zScaleRotationSpinBox->setEnabled( ! checked );
        W.m_xShearSpinBox->setEnabled( checked );
        W.m_yShearSpinBox->setEnabled( checked );
    }

    if ( TD.m_stack_O_slide_matrix && W.m_stack_O_slide_matrixWidget )
    {
        setMatrixWidgetValues( W.m_stack_O_slide_matrixWidget, *TD.m_stack_O_slide_matrix );
    }
}


QWidget* SlideStackEditorDock::createSlideSorderWidget()
{
    auto layout = new QVBoxLayout();
    layout->addWidget( createSlideSorterTableView() );

    /// @todo Disable button if slide sorter table is empty
    m_moveToSlideButton = new QPushButton( "Go To Slide" );
    m_moveToSlideButton->setToolTip( "Center crosshairs on active slide" );
    m_moveToSlideButton->setStatusTip( "Center crosshairs on active slide" );
    m_moveToSlideButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    layout->addWidget( m_moveToSlideButton );


    // Handle change of data in row of the model:
    auto slideSorterDataEditedHandler = [this] ( int row )
    {
        if ( m_slideSorterTableModel && m_slideStackPartialPublisher )
        {
            if ( row < 0 || m_slideSorterTableModel->rowCount() <= row )
            {
                return;
            }

            SlideStackPartial_msgFromUi stackPartialMsg;
            stackPartialMsg.m_slides.emplace( m_slideSorterTableModel->getSlide( row ) );
            m_slideStackPartialPublisher( stackPartialMsg );
        }
    };


    // Handle moving a row in the model: Select the destination row.
    // Do not send message to app, since that is done by the handler of
    // SlideSorterTableModel::dataOrderChanged
    auto slideSorterDataMovedHandler = [this] ( int destRow )
    {
        if ( 0 <= destRow && m_slideSorterTableView )
        {
            m_slideSorterTableView->clearSelection();
            m_slideSorterTableView->selectRow( destRow );

            m_slideSorterTableView->resizeColumnsToContents();

            if ( auto* H = m_slideSorterTableView->horizontalHeader() )
            {
                H->stretchLastSection();
            }

            verticalResizeTableViewToContents( m_slideSorterTableView );
        }
    };


    // Handle reordering of slides in model
    auto slidesReorderedHandler = [this] ( const std::list<UID>& orderedSlideUids )
    {
        if ( m_slideStackOrderPublisher )
        {
            SlideStackOrder_msgFromUi stackOrderMsg;
            stackOrderMsg.m_orderedSlideUids = orderedSlideUids;
            m_slideStackOrderPublisher( stackOrderMsg );
        }
    };


    // Handle pressing of slide move button
    auto moveToSlideButtonHandler = [this] ()
    {
        if ( const auto index = getActiveSlideIndex() )
        {
            moveToSlide( *index );
        }
    };


    connect( m_slideSorterTableModel.get(), &SlideSorterTableModel::dataEdited, slideSorterDataEditedHandler );
    connect( m_slideSorterTableModel.get(), &SlideSorterTableModel::dataMovedRows, slideSorterDataMovedHandler );
    connect( m_slideSorterTableModel.get(), &SlideSorterTableModel::dataOrderChanged, slidesReorderedHandler );

    connect( m_moveToSlideButton, &QPushButton::clicked, moveToSlideButtonHandler );


    auto widget = new QWidget();
    widget->setLayout( layout );

    return widget;
}


QWidget* SlideStackEditorDock::createSlideSorterTableView()
{
    auto& T = m_slideSorterTableView;

    T = new QTableView();

    if ( ! T )
    {
        return nullptr;
    }

    m_widgetsList.push_back( T );

    // Note: QTableView does NOT take ownership of the model or delegate,
    // so the pointers are stored in this class
    m_slideSorterTableModel = std::make_unique<SlideSorterTableModel>();
    m_slideSorterPixmapDelegate = std::make_unique<PixmapDelegate>();

    T->setModel( m_slideSorterTableModel.get() );

    T->setItemDelegateForColumn( SlideSorterTableModel::SLIDE_IMAGE_COLUMN,
                                 m_slideSorterPixmapDelegate.get() );

    // Enable drag and drop to rearrange rows:
    T->setDragEnabled( true );
    T->setAcceptDrops( true );
    T->viewport()->setAcceptDrops( true );
    T->setDragDropOverwriteMode( false );
    T->setDropIndicatorShown( true );
    T->setDragDropMode( QAbstractItemView::InternalMove/*DragDrop*/ );
    T->setDefaultDropAction( Qt::MoveAction );

    // Enable selection of items:
    T->setSelectionBehavior( QAbstractItemView::SelectRows );
    T->setSelectionMode( QAbstractItemView::SingleSelection );

    // Other options:
    T->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding );
    T->setShowGrid( true );
    T->setSortingEnabled( false );
    T->setWordWrap( true );
    T->setCornerButtonEnabled( false );
    T->setEditTriggers( QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );
    T->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );


    if ( auto H = T->horizontalHeader() )
    {
        H->setSectionResizeMode( QHeaderView::Interactive );
        H->setSectionsMovable( false );
        H->setStretchLastSection( true );
    }

    if ( auto V = T->verticalHeader() )
    {
        V->setSectionResizeMode( QHeaderView::Interactive );
        V->setSectionsMovable( false );
    }

    // Resize columns after model has been set that defines headers:
    T->resizeColumnsToContents();

    verticalResizeTableViewToContents( m_slideSorterTableView );


    // Handle change of selection in table:
    auto selectionChangedHandler = [this, &T] ( const QItemSelection& selected, const QItemSelection& /*deselected*/ )
    {
        if ( selected.indexes().empty() )
        {
            return;
        }

        if ( m_activeSlidePublisher )
        {
            ActiveSlide_msgFromUi msg;
            msg.m_activeSlideUid = getActiveSlideUid();
            msg.m_activeSlideIndex = getActiveSlideIndex();
            m_activeSlidePublisher( msg );

            // Save the active slide
            m_activeSlideUid = msg.m_activeSlideUid;
        }

        T->horizontalHeader()->stretchLastSection();
    };


    // Handle double clicking in table:
    auto doubleClickedHandler = [this] ( const QModelIndex& index )
    {
        moveToSlide( index.row() );
    };


    connect( T->selectionModel(), &QItemSelectionModel::selectionChanged, selectionChangedHandler );
    connect( T, &QTableView::doubleClicked, doubleClickedHandler );

    return T;
}


QTabWidget* SlideStackEditorDock::createTabWidget()
{
    auto tabWidget = new QTabWidget();

    tabWidget->addTab( createHeaderTab(), "Header" );
    tabWidget->addTab( createViewTab(), "View" );
    tabWidget->addTab( createTxTab(), "Transform" );
    tabWidget->addTab( createAnnotationTab(), "Annotation" );

    tabWidget->setDocumentMode( false );
    tabWidget->setMovable( false );
    tabWidget->setTabsClosable( false );
    tabWidget->setUsesScrollButtons( true );

    return tabWidget;
}


QLayout* SlideStackEditorDock::createCommonPropertiesLayout()
{
    auto& W = m_commonWidgets;

    // Global slide visibility in 2D/3D views:
    W.m_visibleIn2dViewsCheckBox = new QCheckBox( "2D" );
    m_widgetsList.push_back( W.m_visibleIn2dViewsCheckBox );
    W.m_visibleIn2dViewsCheckBox->setToolTip( "Slide visibility in 2D views" );

    W.m_visibleIn3dViewsCheckBox = new QCheckBox( "3D views" );
    m_widgetsList.push_back( W.m_visibleIn3dViewsCheckBox );
    W.m_visibleIn3dViewsCheckBox->setToolTip( "Slide visibility in 3D views" );

    auto* visibilityLayout = new QHBoxLayout();
    visibilityLayout->setContentsMargins( 0, 0, 0, 0 );
    visibilityLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    visibilityLayout->addWidget( W.m_visibleIn2dViewsCheckBox );
    visibilityLayout->addSpacing( 5 );
    visibilityLayout->addWidget( W.m_visibleIn3dViewsCheckBox );


    // Global slide opacity:
    W.m_masterOpacitySlider = new QSlider( Qt::Horizontal );
    m_widgetsList.push_back( W.m_masterOpacitySlider );
    W.m_masterOpacitySlider->setToolTip( "Slide master opacity" );

    W.m_masterOpacitySpinBox = new QSpinBox();
    m_widgetsList.push_back( W.m_masterOpacitySpinBox );
    W.m_masterOpacitySpinBox->setToolTip( "Slide master opacity" );

    auto* opacityLayout = new QHBoxLayout();
    opacityLayout->setContentsMargins( 0, 0, 0, 0 );
    opacityLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    opacityLayout->addWidget( W.m_masterOpacitySlider );
    opacityLayout->addWidget( W.m_masterOpacitySpinBox );


    // Image 3D layer opacity:
    W.m_image3dLayerOpacitySlider = new QSlider( Qt::Horizontal );
    m_widgetsList.push_back( W.m_image3dLayerOpacitySlider );
    W.m_image3dLayerOpacitySlider->setToolTip( "Image overlay opacity" );

    W.m_image3dLayerOpacitySpinBox = new QSpinBox();
    m_widgetsList.push_back( W.m_image3dLayerOpacitySpinBox );
    W.m_image3dLayerOpacitySpinBox->setToolTip( "Image overlay opacity" );

    auto* imageLayout = new QHBoxLayout();
    imageLayout->setContentsMargins( 0, 0, 0, 0 );
    imageLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    imageLayout->addWidget( W.m_image3dLayerOpacitySlider );
    imageLayout->addWidget( W.m_image3dLayerOpacitySpinBox );


    // Option for Slide Stack views to show slides in 2D/3D:
    W.m_activeSlideViewShows2dSlidesRadioButton = new QRadioButton( "2D" );
    m_widgetsList.push_back( W.m_activeSlideViewShows2dSlidesRadioButton );
    W.m_activeSlideViewShows2dSlidesRadioButton->setToolTip( "View slides as 2D" );

    W.m_activeSlideViewShows3dSlidesRadioButton = new QRadioButton( "3D" );
    m_widgetsList.push_back( W.m_activeSlideViewShows3dSlidesRadioButton );
    W.m_activeSlideViewShows3dSlidesRadioButton->setToolTip( "View slides as 3D" );

    W.m_activeSlideViewDirectionButton = new QPushButton( "Bottom to Top" );
    m_widgetsList.push_back( W.m_activeSlideViewDirectionButton );
    W.m_activeSlideViewDirectionButton->setToolTip( "Flip orientation of Active Slide view" );
    W.m_activeSlideViewDirectionButton->setCheckable( true );


    auto* stackStyleLayout = new QHBoxLayout();
    stackStyleLayout->setContentsMargins( 0, 0, 0, 0 );
    stackStyleLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
//    stackStyleLayout->addWidget( new QLabel( "Slides are " ) );
    stackStyleLayout->addWidget( W.m_activeSlideViewShows2dSlidesRadioButton );
    stackStyleLayout->addWidget( W.m_activeSlideViewShows3dSlidesRadioButton );


    auto* stackViewDirLayout = new QHBoxLayout();
    stackViewDirLayout->setContentsMargins( 0, 0, 0, 0 );
    stackViewDirLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
//    stackViewDirLayout->addWidget( new QLabel( "Look " ) );
    stackViewDirLayout->addWidget( W.m_activeSlideViewDirectionButton );


    QFormLayout* mainLayout = new QFormLayout();
    auto layoutAlignment = mainLayout->labelAlignment();
    mainLayout->setLabelAlignment( layoutAlignment | Qt::AlignTop );
    mainLayout->setFieldGrowthPolicy( QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow );

    mainLayout->addRow( "Visibility:", visibilityLayout );
    mainLayout->addRow( "Master Opacity:", opacityLayout );
    mainLayout->addRow( "Image Overlay:", imageLayout );
    mainLayout->addItem( new QSpacerItem( 0, 10, QSizePolicy::Fixed, QSizePolicy::Fixed ) );

    mainLayout->addRow( new QLabel( "Slide Stack View Options:" ) );
    mainLayout->addRow( "Slide Rendering:", stackStyleLayout );
    mainLayout->addRow( "View Direction:", stackViewDirLayout );

    return mainLayout;
}


void SlideStackEditorDock::connectCommonWidgets()
{
    auto opacityChangedHandler = [this] ( int opacity )
    {
        if ( m_slideStackRenderingPartialPublisher )
        {
            SlideCommonPropertiesPartial_msgFromUi msg;
            msg.m_properties.m_masterOpacityValue = opacity;
            m_slideStackRenderingPartialPublisher( msg );
        }
    };

    auto imageLayerOpacityChangedHandler = [this] ( int opacity )
    {
        if ( m_slideStackRenderingPartialPublisher )
        {
            SlideCommonPropertiesPartial_msgFromUi msg;
            msg.m_properties.m_image3dOpacityValue = opacity;
            m_slideStackRenderingPartialPublisher( msg );
        }
    };

    auto visibility2dChangedHandler = [this] ( bool visible )
    {
        if ( m_slideStackRenderingPartialPublisher )
        {
            SlideCommonPropertiesPartial_msgFromUi msg;
            msg.m_properties.m_stackVisibleIn2dViewsChecked = visible;
            m_slideStackRenderingPartialPublisher( msg );
        }
    };

    auto visibility3dChangedHandler = [this] ( bool visible )
    {
        if ( m_slideStackRenderingPartialPublisher )
        {
            SlideCommonPropertiesPartial_msgFromUi msg;
            msg.m_properties.m_stackVisibleIn3dViewsChecked = visible;
            m_slideStackRenderingPartialPublisher( msg );
        }
    };

    auto stackScenesShow2dHandler = [this] ( bool show2d )
    {
        if ( m_slideStackRenderingPartialPublisher )
        {
            SlideCommonPropertiesPartial_msgFromUi msg;
            msg.m_properties.m_activeSlideViewShows2dSlidesChecked = show2d;
            m_slideStackRenderingPartialPublisher( msg );
        }
    };

    // Handler for toggling of button that controls view direction of Active Slide view
    auto activeSlideViewDirectionHandler = [this] ( bool showStackTopToBottom )
    {
        m_commonWidgets.m_activeSlideViewDirectionButton->setText(
                    ( showStackTopToBottom )
                    ? "Top to Bottom"
                    : "Bottom to Top" );

        if ( m_slideStackRenderingPartialPublisher )
        {
            SlideCommonPropertiesPartial_msgFromUi msg;
            msg.m_properties.m_activeSlideViewDirectionTopToBottomChecked = showStackTopToBottom;
            m_slideStackRenderingPartialPublisher( msg );
        }
    };

    // Handler to set stack transformation to identity
    auto setIdentityHandler = [this] ()
    {
        if ( m_slideStackPartialPublisher )
        {
            SlideStackPartial_msgFromUi msg;
            msg.m_set_world_O_stack_identity = true;
            m_slideStackPartialPublisher( msg );
        }
    };


    auto& W = m_commonWidgets;

    // Opacity:
    connect( W.m_masterOpacitySlider, &QSlider::valueChanged, W.m_masterOpacitySpinBox, &QSpinBox::setValue );
    connect( W.m_masterOpacitySlider, &QSlider::valueChanged, opacityChangedHandler );
    connect( W.m_masterOpacitySpinBox, qOverload<int>( &QSpinBox::valueChanged ),
             W.m_masterOpacitySlider, &QSlider::setValue );

    // Image 3D layering:
    connect( W.m_image3dLayerOpacitySlider, &QSlider::valueChanged, W.m_image3dLayerOpacitySpinBox, &QSpinBox::setValue );
    connect( W.m_image3dLayerOpacitySlider, &QSlider::valueChanged, imageLayerOpacityChangedHandler );
    connect( W.m_image3dLayerOpacitySpinBox, qOverload<int>( &QSpinBox::valueChanged ),
             W.m_image3dLayerOpacitySlider, &QSlider::setValue );

    // Visibility:
    connect( W.m_visibleIn2dViewsCheckBox, &QCheckBox::toggled, visibility2dChangedHandler );
    connect( W.m_visibleIn3dViewsCheckBox, &QCheckBox::toggled, visibility3dChangedHandler );

    // Show scenes as 2D/3D:
    connect( W.m_activeSlideViewShows2dSlidesRadioButton, &QRadioButton::toggled, stackScenesShow2dHandler );

    // Show slide stack top-to-bottom or bottom-to-top:
    connect( W.m_activeSlideViewDirectionButton, &QPushButton::toggled, activeSlideViewDirectionHandler );

    // Set identity button:
    connect( W.m_setIdentityButton, &QPushButton::pressed, setIdentityHandler );
}


QGroupBox* SlideStackEditorDock::createCommonPropertiesGroupBox()
{
    auto groupBox = new ctkCollapsibleGroupBox( "Stack Properties" );

    //    setZeroContentsMargins( groupBox, true, false, true, false );

    QFont boldFont = groupBox->font();
    boldFont.setBold( false );
    boldFont.setUnderline( true );

    groupBox->setFont( boldFont );
    groupBox->setFlat( true );
    groupBox->setLayout( createCommonPropertiesLayout() );
    groupBox->setCollapsed( false );

    return groupBox;
}


QLayout* SlideStackEditorDock::createCommonStackTxLayout()
{
    auto& W = m_commonWidgets;

    W.m_world_O_stack_matrixWidget = new ctkMatrixWidget( 4, 4 );
    m_widgetsList.push_back( W.m_world_O_stack_matrixWidget );
    W.m_world_O_stack_matrixWidget->setDecimals( 3 );
    W.m_world_O_stack_matrixWidget->setDecimalsOption( ctkDoubleSpinBox::DecimalsByShortcuts );
    W.m_world_O_stack_matrixWidget->setRange( -1.0e9, 1.0e9 );
    W.m_world_O_stack_matrixWidget->setToolTip( "Stack to World space transformation matrix" );
    W.m_world_O_stack_matrixWidget->setEditable( false );
    W.m_world_O_stack_matrixWidget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );

    W.m_setIdentityButton = new QPushButton( "Set Identity" );
    m_widgetsList.push_back( W.m_setIdentityButton );
    W.m_setIdentityButton->setToolTip( "Set slide stack transformation to identity" );
    W.m_setIdentityButton->setStatusTip( "Set slide stack transformation to identity" );
    W.m_setIdentityButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    auto* layout = new QVBoxLayout();
    layout->setAlignment( Qt::AlignLeft | Qt::AlignTop );

    layout->addWidget( new QLabel( "Slide Stack to World matrix:" ) );
    layout->addWidget( W.m_world_O_stack_matrixWidget );
    layout->addWidget( W.m_setIdentityButton );

    return layout;
}


QGroupBox* SlideStackEditorDock::createCommonStackTxGroupBox()
{
    auto groupBox = new ctkCollapsibleGroupBox( "Stack Transformation" );

    //    setZeroContentsMargins( groupBox, true, false, true, false );

    QFont boldFont = groupBox->font();
    boldFont.setBold( false );
    boldFont.setUnderline( true );

    groupBox->setFont( boldFont );
    groupBox->setFlat( true );
    groupBox->setLayout( createCommonStackTxLayout() );
    groupBox->setCollapsed( true );

    return groupBox;
}


QScrollArea* SlideStackEditorDock::createSlideStackCommonScrollArea()
{
    auto layout = new QVBoxLayout();

    setZeroContentsMargins( layout, true, false, true, false );
    layout->setAlignment( Qt::AlignTop );

    layout->addWidget( createCommonPropertiesGroupBox() );
    layout->addWidget( createCommonStackTxGroupBox() );

    // Inner widget to scroll:
    auto innerWidget = new QWidget();
    innerWidget->setLayout( layout );

    auto scrollArea = new QScrollArea();
    scrollArea->setWidget( innerWidget );
    scrollArea->setWidgetResizable( true );
    scrollArea->setStyleSheet( sk_scrollAreaStyleSheet );

    return scrollArea;
}


QWidget* SlideStackEditorDock::createViewTab()
{
    // Scrollable area:
    auto scrollArea = createViewScrollArea();

    auto layout = new QVBoxLayout();
    //    layout->addLayout( pathLayout );
    layout->addWidget( scrollArea );

    auto widget = new QWidget();
    widget->setLayout( layout );

    return widget;
}


QWidget* SlideStackEditorDock::createTxTab()
{
    // Scrollable area:
    auto scrollArea = createTxScrollArea();

    auto layout = new QVBoxLayout();
    //    layout->addLayout( pathLayout );
    layout->addWidget( scrollArea );

    auto widget = new QWidget();
    widget->setLayout( layout );

    return widget;
}


QWidget* SlideStackEditorDock::createHeaderTab()
{
    // Scrollable area:
    auto scrollArea = createHeaderScrollArea();

    auto layout = new QVBoxLayout();
    //    layout->addLayout( pathLayout );
    layout->addWidget( scrollArea );

    auto widget = new QWidget();
    widget->setLayout( layout );

    return widget;
}


QWidget* SlideStackEditorDock::createAnnotationTab()
{
    // Scrollable area:
    auto scrollArea = createAnnotationScrollArea();

    auto layout = new QVBoxLayout();
    //    layout->addLayout( pathLayout );
    layout->addWidget( scrollArea );

    auto widget = new QWidget();
    widget->setLayout( layout );

    return widget;
}


QScrollArea* SlideStackEditorDock::createViewScrollArea()
{
    auto layout = new QVBoxLayout();

    setZeroContentsMargins( layout, true, false, true, false );
    layout->setAlignment( Qt::AlignTop );

    layout->addLayout( createViewTabLayout() );

    // Inner widget to scroll:
    auto innerWidget = new QWidget();
    innerWidget->setLayout( layout );

    auto scrollArea = new QScrollArea();
    scrollArea->setWidget( innerWidget );
    scrollArea->setWidgetResizable( true );
    scrollArea->setStyleSheet( sk_scrollAreaStyleSheet );

    return scrollArea;
}


QScrollArea* SlideStackEditorDock::createTxScrollArea()
{
    auto layout = new QVBoxLayout();

    setZeroContentsMargins( layout, true, false, true, false );
    layout->setAlignment( Qt::AlignTop );

    layout->addLayout( createTxTabLayout() );

    // Inner widget to scroll:
    auto innerWidget = new QWidget();
    innerWidget->setLayout( layout );

    auto scrollArea = new QScrollArea();
    scrollArea->setWidget( innerWidget );
    scrollArea->setWidgetResizable( true );
    scrollArea->setStyleSheet( sk_scrollAreaStyleSheet );

    return scrollArea;
}


QLayout* SlideStackEditorDock::createHeaderTabLayout()
{
    auto& W = m_headerWidgets;

    // Pixel horizontal size editor:
    W.m_pixelSizeHorizLineEdit = new QLineEdit();
    W.m_pixelSizeHorizLineEditValidator = new QDoubleValidator();
    m_widgetsList.push_back( W.m_pixelSizeHorizLineEdit );
    W.m_pixelSizeHorizLineEdit->setValidator( W.m_pixelSizeHorizLineEditValidator );
    W.m_pixelSizeHorizLineEdit->setToolTip( "Horizontal pixel size" );
    W.m_pixelSizeHorizLineEdit->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

    // Pixel vertical size editor:
    W.m_pixelSizeVertLineEdit = new QLineEdit();
    W.m_pixelSizeVertLineEditValidator = new QDoubleValidator();
    m_widgetsList.push_back( W.m_pixelSizeVertLineEdit );
    W.m_pixelSizeVertLineEdit->setValidator( W.m_pixelSizeVertLineEditValidator );
    W.m_pixelSizeVertLineEdit->setToolTip( "Vertical pixel size" );
    W.m_pixelSizeVertLineEdit->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );


    // Size layout:
    auto sizeXLayout = new QHBoxLayout();
    sizeXLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    sizeXLayout->addWidget( W.m_pixelSizeHorizLineEdit );
    sizeXLayout->addWidget( new QLabel( "mm" ) );

    // Size layout:
    auto sizeYLayout = new QHBoxLayout();
    sizeYLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    sizeYLayout->addWidget( W.m_pixelSizeVertLineEdit );
    sizeYLayout->addWidget( new QLabel( "mm" ) );

    // Thickness editor:
    W.m_thicknessLineEdit = new QLineEdit();
    W.m_thicknessLineEditValidator = new QDoubleValidator();
    m_widgetsList.push_back( W.m_thicknessLineEdit );
    W.m_thicknessLineEdit->setValidator( W.m_thicknessLineEditValidator );
    W.m_thicknessLineEdit->setToolTip( "Thickness" );
    W.m_thicknessLineEdit->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

    // Thickness layout:
    auto thicknessLayout = new QHBoxLayout();
    thicknessLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    thicknessLayout->addWidget( W.m_thicknessLineEdit );
    thicknessLayout->addWidget( new QLabel( "mm" ) );


    // File path editor (read only):
    W.m_fileNameLineEdit = new QLineEdit();
    m_widgetsList.push_back( W.m_fileNameLineEdit );
    W.m_fileNameLineEdit->setToolTip( "Slide file path" );
    W.m_fileNameLineEdit->setReadOnly( true );

    // Display name editor:
    W.m_displayNameLineEdit = new QLineEdit();
    m_widgetsList.push_back( W.m_displayNameLineEdit );
    W.m_displayNameLineEdit->setToolTip( "Set slide name" );

    // Vendor ID editor (read only):
    W.m_vendorIdLineEdit = new QLineEdit();
    m_widgetsList.push_back( W.m_vendorIdLineEdit );
    W.m_vendorIdLineEdit->setToolTip( "Slide vendor ID" );
    W.m_vendorIdLineEdit->setReadOnly( true );


    // Layer dimensions table:
    auto& T = W.m_layerDimsTableWidget;
    T = new QTableWidget( 1, 2 );
    m_widgetsList.push_back( T );

    T->setShowGrid( true );
    T->setSelectionMode( QAbstractItemView::SingleSelection );

    static const QStringList horizLabels{ "Horiz. (x)", "Vert. (y)" };
    T->setHorizontalHeaderLabels( horizLabels );

    T->horizontalHeader()->setStretchLastSection( true );
    T->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    T->horizontalHeader()->setSectionsClickable( false );

    T->verticalHeader()->setSectionsClickable( false );
    T->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
//    T->setFixedHeight( T->verticalHeader()->length() );


    // Label image:
    W.m_labelImageLabel = new QLabel( "label" );
    m_widgetsList.push_back( W.m_labelImageLabel );
    W.m_labelImageLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    W.m_labelImageLabel->setScaledContents( true );

    // Macro image:
    W.m_macroImageLabel = new QLabel( "macro" );
    m_widgetsList.push_back( W.m_macroImageLabel );
    W.m_macroImageLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    W.m_macroImageLabel->setScaledContents( true );


    auto layout = new QFormLayout();
//    setZeroContentsMargins( layout, true, false, true, false );
    expandContentsMargins( layout, 0, 0, 15, 0 );

    auto layoutAlignment = layout->labelAlignment();
    layout->setLabelAlignment( layoutAlignment | Qt::AlignTop );
    layout->setFieldGrowthPolicy( QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow );

    layout->addRow( "ID:", W.m_displayNameLineEdit );
    layout->addRow( "Pixel Size x:", sizeXLayout );
    layout->addRow( "Pixel Size y:", sizeYLayout );
    layout->addRow( "Thickness:", thicknessLayout );

    layout->addItem( new QSpacerItem( 0, 10, QSizePolicy::Fixed, QSizePolicy::Fixed ) );

    layout->addRow( "File Path:", W.m_fileNameLineEdit );
    layout->addRow( "Format:", W.m_vendorIdLineEdit );
    layout->addRow( "Layers:", T );

    layout->addItem( new QSpacerItem( 0, 10, QSizePolicy::Fixed, QSizePolicy::Fixed ) );

    layout->addRow( "Label Image:", W.m_labelImageLabel );
    layout->addRow( "Macro Image:", W.m_macroImageLabel );

    return layout;
}


void SlideStackEditorDock::connectHeaderWidgets()
{
    static QString s_cachedDisplayName;
    static QString s_cachedPixelSizeHoriz;
    static QString s_cachedPixelSizeVert;
    static QString s_cachedThickness;

    auto displayNameEditedHandler = [] ( const QString& value )
    {
        s_cachedDisplayName = value;
    };

    auto pixelSizeHorizEditedHandler = [] ( const QString& value )
    {
        s_cachedPixelSizeHoriz = value;
    };

    auto pixelSizeVertEditedHandler = [] ( const QString& value )
    {
        s_cachedPixelSizeVert = value;
    };

    auto thicknessEditedHandler = [] ( const QString& value )
    {
        s_cachedThickness = value;
    };


    auto displayNameEditingFinishedHandler = [this] ()
    {
        if ( ! s_cachedDisplayName.isNull() &&
             m_slideHeaderPartialPublisher && m_activeSlideUid )
        {
            SlideHeaderPartial_msgFromUi msg;
            msg.m_uid = *m_activeSlideUid;
            msg.m_headerMutable.m_displayName = s_cachedDisplayName.toStdString();
            m_slideHeaderPartialPublisher( msg );

            s_cachedDisplayName.clear(); // Clear cache
        }
    };

    auto pixelSizeHorizEditingFinishedHandler = [this] ()
    {
        if ( ! s_cachedPixelSizeHoriz.isNull() &&
             m_slideHeaderPartialPublisher && m_activeSlideUid )
        {
            bool ok;
            const float x = s_cachedPixelSizeHoriz.toFloat( &ok );

            if ( ok )
            {
                SlideHeaderPartial_msgFromUi msg;
                msg.m_uid = *m_activeSlideUid;
                msg.m_headerMutable.m_pixelSizeX = x;
                m_slideHeaderPartialPublisher( msg );

                s_cachedPixelSizeHoriz.clear(); // Clear cache
            }
        }
    };

    auto pixelSizeVertEditingFinishedHandler = [this] ()
    {
        if ( ! s_cachedPixelSizeVert.isNull() &&
             m_slideHeaderPartialPublisher && m_activeSlideUid )
        {
            bool ok;
            const float y = s_cachedPixelSizeVert.toFloat( &ok );

            if ( ok )
            {
                SlideHeaderPartial_msgFromUi msg;
                msg.m_uid = *m_activeSlideUid;
                msg.m_headerMutable.m_pixelSizeY = y;
                m_slideHeaderPartialPublisher( msg );

                s_cachedPixelSizeVert.clear(); // Clear cache
            }
        }
    };

    auto thicknessEditingFinishedHandler = [this] ()
    {
        if ( ! s_cachedThickness.isNull() &&
             m_slideHeaderPartialPublisher && m_activeSlideUid )
        {
            SlideHeaderPartial_msgFromUi msg;
            msg.m_uid = *m_activeSlideUid;
            msg.m_headerMutable.m_thickness = s_cachedThickness.toFloat();
            m_slideHeaderPartialPublisher( msg );

            s_cachedThickness.clear(); // Clear cache
        }
    };


    auto& W = m_headerWidgets;

    connect( W.m_displayNameLineEdit, &QLineEdit::textEdited, displayNameEditedHandler );
    connect( W.m_displayNameLineEdit, &QLineEdit::editingFinished, displayNameEditingFinishedHandler );

    connect( W.m_pixelSizeHorizLineEdit, &QLineEdit::textEdited, pixelSizeHorizEditedHandler );
    connect( W.m_pixelSizeHorizLineEdit, &QLineEdit::editingFinished, pixelSizeHorizEditingFinishedHandler );

    connect( W.m_pixelSizeVertLineEdit, &QLineEdit::textEdited, pixelSizeVertEditedHandler );
    connect( W.m_pixelSizeVertLineEdit, &QLineEdit::editingFinished, pixelSizeVertEditingFinishedHandler );

    connect( W.m_thicknessLineEdit, &QLineEdit::textEdited, thicknessEditedHandler );
    connect( W.m_thicknessLineEdit, &QLineEdit::editingFinished, thicknessEditingFinishedHandler );
}


QLayout* SlideStackEditorDock::createViewTabLayout()
{
    auto& W = m_viewWidgets;

    W.m_showSlideCheckBox = new QCheckBox( "Show Slide" );
    m_widgetsList.push_back( W.m_showSlideCheckBox );
    W.m_showSlideCheckBox->setToolTip( "Show slide" );

    W.m_showEdgesCheckBox = new QCheckBox( "Show Edges" );
    m_widgetsList.push_back( W.m_showEdgesCheckBox );
    W.m_showEdgesCheckBox->setToolTip( "Show slide edges" );

    W.m_opacitySlider = new QSlider( Qt::Horizontal );
    m_widgetsList.push_back( W.m_opacitySlider );
    W.m_opacitySlider->setToolTip( "Slide opacity" );

    W.m_opacitySpinBox = new QSpinBox();
    m_widgetsList.push_back( W.m_opacitySpinBox );
    W.m_opacitySpinBox->setToolTip( "Slide opacity" );


    auto* opacityLayout = new QHBoxLayout();
    opacityLayout->setContentsMargins( 0, 0, 0, 0 );
    opacityLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    opacityLayout->addWidget( W.m_opacitySlider );
    opacityLayout->addWidget( W.m_opacitySpinBox );


    // Thresholding slider:
    W.m_threshRangeSlider = new ctkRangeSlider( Qt::Orientation::Horizontal );
    m_widgetsList.push_back( W.m_threshRangeSlider );
    W.m_threshRangeSlider->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    W.m_threshRangeSlider->setToolTip( "Set thresholds" );

    // Thresholding spin boxes:
    W.m_threshLowSpinBox = new QSpinBox();
    m_widgetsList.push_back( W.m_threshLowSpinBox );
    W.m_threshLowSpinBox->setToolTip( "Set low threshold" );

    W.m_threshHighSpinBox = new QSpinBox();
    m_widgetsList.push_back( W.m_threshHighSpinBox );
    W.m_threshHighSpinBox->setToolTip( "Set high threshold" );


    // Thresholding layout for spin boxes:
    auto threshSpinBoxLayout = new QHBoxLayout();
    threshSpinBoxLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    threshSpinBoxLayout->setContentsMargins( 0, 0, 0, 0 );

    threshSpinBoxLayout->addWidget( new QLabel( "Low:" ) );
    threshSpinBoxLayout->addWidget( W.m_threshLowSpinBox, 0, Qt::AlignLeft | Qt::AlignVCenter );
    threshSpinBoxLayout->addWidget( new QLabel( "High:" ) );
    threshSpinBoxLayout->addWidget( W.m_threshHighSpinBox, 0, Qt::AlignLeft | Qt::AlignVCenter );
    threshSpinBoxLayout->insertSpacing( 2, 10 );


    // Main thresholding layout containing both range slider and spin boxes:
    auto threshMainLayout = new QVBoxLayout();
//    threshMainLayout->setSpacing( 0 );
    threshMainLayout->setContentsMargins( 0, 0, 0, 0 );
    threshMainLayout->addWidget( W.m_threshRangeSlider );
    threshMainLayout->addLayout( threshSpinBoxLayout, 0 );


    W.m_edgesMagnitudeSlider = new ctkDoubleSlider( Qt::Horizontal );
    m_widgetsList.push_back( W.m_edgesMagnitudeSlider );
    W.m_edgesMagnitudeSlider->setToolTip( "Edge magnitude" );

    W.m_edgesMagnitudeSpinBox = new ctkDoubleSpinBox();
    m_widgetsList.push_back( W.m_edgesMagnitudeSpinBox );
    W.m_edgesMagnitudeSpinBox->setToolTip( "Edge magnitude" );


    auto* edgeMagLayout = new QHBoxLayout();
    edgeMagLayout->setContentsMargins( 0, 0, 0, 0 );
    edgeMagLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    edgeMagLayout->addWidget( W.m_edgesMagnitudeSlider );
    edgeMagLayout->addWidget( W.m_edgesMagnitudeSpinBox );


    W.m_edgesSmoothingSlider = new ctkDoubleSlider( Qt::Horizontal );
    m_widgetsList.push_back( W.m_edgesSmoothingSlider );
    W.m_edgesSmoothingSlider->setToolTip( "Edge smoothing" );

    W.m_edgesSmoothingSpinBox = new ctkDoubleSpinBox();
    m_widgetsList.push_back( W.m_edgesSmoothingSpinBox );
    W.m_edgesSmoothingSpinBox->setToolTip( "Edge smoothing" );


    auto* edgeSmoothingLayout = new QHBoxLayout();
    edgeSmoothingLayout->setContentsMargins( 0, 0, 0, 0 );
    edgeSmoothingLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    edgeSmoothingLayout->addWidget( W.m_edgesSmoothingSlider );
    edgeSmoothingLayout->addWidget( W.m_edgesSmoothingSpinBox );


    W.m_borderColorButton = new QToolButton();
    W.m_borderColorButton->setToolButtonStyle( Qt::ToolButtonStyle::ToolButtonIconOnly );
//    W.m_borderColorButton->setAutoRaise( false );
    m_widgetsList.push_back( W.m_borderColorButton );


    auto mainSlideLayout = new QFormLayout();
    mainSlideLayout->setFieldGrowthPolicy( QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow );
    mainSlideLayout->addRow( "Opacity:", opacityLayout );
    mainSlideLayout->addRow( "Threshold:", threshMainLayout );
    mainSlideLayout->addRow( "Border:", W.m_borderColorButton );


    auto mainEdgeLayout = new QFormLayout();
    mainEdgeLayout->setFieldGrowthPolicy( QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow );
    mainEdgeLayout->addRow( "Magnitude:", edgeMagLayout );
    mainEdgeLayout->addRow( "Smoothing:", edgeSmoothingLayout );


    auto mainLayout = new QVBoxLayout();
    expandContentsMargins( mainLayout, 0, 0, 15, 0 );
    mainLayout->addWidget( W.m_showSlideCheckBox );
    mainLayout->addLayout( mainSlideLayout );
    mainLayout->addSpacing( 20 );
    mainLayout->addWidget( W.m_showEdgesCheckBox );
    mainLayout->addLayout( mainEdgeLayout );

    return mainLayout;
}


void SlideStackEditorDock::connectViewWidgets()
{
    auto& W = m_viewWidgets;


    auto borderColorHandler = [this, W] ()
    {
        // Create a color dialog just to get its flags
        QColorDialog* dialog = new QColorDialog( this );
        dialog->setOption( QColorDialog::ColorDialogOption::ShowAlphaChannel, false );

        const QColor currentColor( W.m_borderColorButton->icon().pixmap( 1, 1 )
                                   .toImage().pixelColor( 0, 0 ) );

        const QColor newColor = QColorDialog::getColor(
                    currentColor, this, "Slide Border Color", dialog->options() );

        if ( newColor.isValid() )
        {
            QPixmap px( 32, 32 );
            px.fill( newColor );
            SilentCall( W.m_borderColorButton )->setIcon( px );

            if ( m_slideViewDataPartialPublisher && m_activeSlideUid )
            {
                SlideViewDataPartial_msgFromUi msg;
                msg.m_uid = *m_activeSlideUid;
                msg.m_viewData.m_borderColor = convertQColorToVec3( newColor );
                m_slideViewDataPartialPublisher( msg );
            }
        }
    };

    auto slideVisibilityHandler = [this] ( bool checked )
    {
        if ( m_slideViewDataPartialPublisher && m_activeSlideUid )
        {
            SlideViewDataPartial_msgFromUi msg;
            msg.m_uid = *m_activeSlideUid;
            msg.m_viewData.m_slideVisibleChecked = checked;
            m_slideViewDataPartialPublisher( msg );
        }
    };

    auto edgeVisibilityHandler = [this] ( bool checked )
    {
        if ( m_slideViewDataPartialPublisher && m_activeSlideUid )
        {
            SlideViewDataPartial_msgFromUi msg;
            msg.m_uid = *m_activeSlideUid;
            msg.m_viewData.m_edgesVisibleChecked = checked;
            m_slideViewDataPartialPublisher( msg );
        }
    };

    auto opacityChangedHandler = [this] ( int value )
    {
        if ( m_slideViewDataPartialPublisher && m_activeSlideUid )
        {
            SlideViewDataPartial_msgFromUi msg;
            msg.m_uid = *m_activeSlideUid;
            msg.m_viewData.m_opacityValue = value;
            m_slideViewDataPartialPublisher( msg );
        }
    };

    auto thresholdChangedHandler = [this] ( int minValue, int maxValue )
    {
        if ( m_slideViewDataPartialPublisher && m_activeSlideUid )
        {
            SlideViewDataPartial_msgFromUi msg;
            msg.m_uid = *m_activeSlideUid;
            msg.m_viewData.m_threshValues = std::make_pair( minValue, maxValue );
            m_slideViewDataPartialPublisher( msg );
        }
    };

    auto edgeMagnitudeChangedHandler = [this] ( double value )
    {
        if ( m_slideViewDataPartialPublisher && m_activeSlideUid )
        {
            SlideViewDataPartial_msgFromUi msg;
            msg.m_uid = *m_activeSlideUid;
            msg.m_viewData.m_edgesMagnitudeValue = value;
            m_slideViewDataPartialPublisher( msg );
        }
    };

    auto edgeSmoothingChangedHandler = [this] ( double value )
    {
        if ( m_slideViewDataPartialPublisher && m_activeSlideUid )
        {
            SlideViewDataPartial_msgFromUi msg;
            msg.m_uid = *m_activeSlideUid;
            msg.m_viewData.m_edgesSmoothingValue = value;
            m_slideViewDataPartialPublisher( msg );
        }
    };


    // Slide visibility:
    connect( W.m_showSlideCheckBox, &QCheckBox::toggled, slideVisibilityHandler );

    // Border color:
    connect( W.m_borderColorButton, &QToolButton::clicked, borderColorHandler );

    // Opacity:
    connect( W.m_opacitySlider, &QSlider::valueChanged, W.m_opacitySpinBox, &QSpinBox::setValue );
    connect( W.m_opacitySlider, &QSlider::valueChanged, opacityChangedHandler );
    connect( W.m_opacitySpinBox, qOverload<int>( &QSpinBox::valueChanged ),
             W.m_opacitySlider, &QSlider::setValue );

    // Threshold:
    connect( W.m_threshRangeSlider, &ctkRangeSlider::minimumValueChanged,
             W.m_threshLowSpinBox, &QSpinBox::setValue );
    connect( W.m_threshRangeSlider, &ctkRangeSlider::maximumValueChanged,
             W.m_threshHighSpinBox, &QSpinBox::setValue );
    connect( W.m_threshRangeSlider, &ctkRangeSlider::valuesChanged, thresholdChangedHandler );

    connect( W.m_threshLowSpinBox, qOverload<int>( &QSpinBox::valueChanged ),
             W.m_threshRangeSlider, &ctkRangeSlider::setMinimumValue );
    connect( W.m_threshHighSpinBox, qOverload<int>( &QSpinBox::valueChanged ),
             W.m_threshRangeSlider, &ctkRangeSlider::setMaximumValue );

    // Edge visibility:
    connect( W.m_showEdgesCheckBox, &QCheckBox::toggled, edgeVisibilityHandler );

    // Edge magnitude:
    connect( W.m_edgesMagnitudeSlider, &ctkDoubleSlider::valueChanged,
             W.m_edgesMagnitudeSpinBox, &ctkDoubleSpinBox::setValue );
    connect( W.m_edgesMagnitudeSlider, &ctkDoubleSlider::valueChanged, edgeMagnitudeChangedHandler );
    connect( W.m_edgesMagnitudeSpinBox, &ctkDoubleSpinBox::valueChanged,
             W.m_edgesMagnitudeSlider, &ctkDoubleSlider::setValue );

    // Edge smoothing:
    connect( W.m_edgesSmoothingSlider, &ctkDoubleSlider::valueChanged,
             W.m_edgesSmoothingSpinBox, &ctkDoubleSpinBox::setValue );
    connect( W.m_edgesSmoothingSlider, &ctkDoubleSlider::valueChanged, edgeSmoothingChangedHandler );
    connect( W.m_edgesSmoothingSpinBox, &ctkDoubleSpinBox::valueChanged,
             W.m_edgesSmoothingSlider, &ctkDoubleSlider::setValue );
}


QLayout* SlideStackEditorDock::createTxTabLayout()
{
    auto& W = m_transformWidgets;

    W.m_xTranslationSpinBox = new QDoubleSpinBox();
    W.m_yTranslationSpinBox = new QDoubleSpinBox();
    W.m_zTranslationSpinBox = new QDoubleSpinBox();
    W.m_zRotationSpinBox = new QDoubleSpinBox();
    W.m_xScaleSpinBox = new QDoubleSpinBox();
    W.m_yScaleSpinBox = new QDoubleSpinBox();
    W.m_zScaleRotationSpinBox = new QDoubleSpinBox();
    W.m_xShearSpinBox = new QDoubleSpinBox();
    W.m_yShearSpinBox = new QDoubleSpinBox();
    W.m_xOriginSpinBox = new QDoubleSpinBox();
    W.m_yOriginSpinBox = new QDoubleSpinBox();

    W.m_stack_O_slide_matrixWidget = new ctkMatrixWidget( 4, 4 );
    W.m_stack_O_slide_matrixWidget->setDecimals( 3 );
    W.m_stack_O_slide_matrixWidget->setDecimalsOption( ctkDoubleSpinBox::DecimalsByShortcuts );
    W.m_stack_O_slide_matrixWidget->setRange( -1.0e9, 1.0e9 );
    W.m_stack_O_slide_matrixWidget->setToolTip( "Slide to Stack transformation matrix" );
    W.m_stack_O_slide_matrixWidget->setEditable( false );
    W.m_stack_O_slide_matrixWidget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );


    m_widgetsList.push_back( W.m_xTranslationSpinBox );
    m_widgetsList.push_back( W.m_yTranslationSpinBox );
    m_widgetsList.push_back( W.m_zTranslationSpinBox );
    m_widgetsList.push_back( W.m_zRotationSpinBox );
    m_widgetsList.push_back( W.m_xScaleSpinBox );
    m_widgetsList.push_back( W.m_yScaleSpinBox );
    m_widgetsList.push_back( W.m_zScaleRotationSpinBox );
    m_widgetsList.push_back( W.m_xShearSpinBox );
    m_widgetsList.push_back( W.m_yShearSpinBox );
    m_widgetsList.push_back( W.m_xOriginSpinBox );
    m_widgetsList.push_back( W.m_yOriginSpinBox );
    m_widgetsList.push_back( W.m_stack_O_slide_matrixWidget );


    W.m_xTranslationSpinBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    W.m_yTranslationSpinBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    W.m_zTranslationSpinBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

    W.m_zRotationSpinBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

    W.m_xScaleSpinBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    W.m_yScaleSpinBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

    W.m_zScaleRotationSpinBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

    W.m_xShearSpinBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    W.m_yShearSpinBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

    W.m_xOriginSpinBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    W.m_yOriginSpinBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

    W.m_xTranslationSpinBox->setToolTip( "Translation X" );
    W.m_yTranslationSpinBox->setToolTip( "Translation Y" );
    W.m_zTranslationSpinBox->setToolTip( "Translation Z" );
    W.m_zRotationSpinBox->setToolTip( "Rotation Z" );
    W.m_xScaleSpinBox->setToolTip( "Scale X" );
    W.m_yScaleSpinBox->setToolTip( "Scale Y" );
    W.m_zScaleRotationSpinBox->setToolTip( "Scale Rotation Z" );
    W.m_xShearSpinBox->setToolTip( "Shear X" );
    W.m_yShearSpinBox->setToolTip( "Shear Y" );
    W.m_xOriginSpinBox->setToolTip( "Origin X" );
    W.m_yOriginSpinBox->setToolTip( "Origin Y" );
    W.m_stack_O_slide_matrixWidget->setToolTip( "Slide to Stack transformation" );


    W.m_setIdentityButton = new QPushButton( "Reset All Parameters" );
    W.m_setIdentityButton->setToolTip( "Set slide transformation to identity" );
    W.m_setIdentityButton->setStatusTip( "Set slide transformation to identity" );
    W.m_setIdentityButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    m_widgetsList.push_back( W.m_setIdentityButton );


    W.m_paramScaleRotationRadioButton = new QRadioButton( "Scale Rotation:" );
    W.m_paramScaleRotationRadioButton->setToolTip( "Scale rotation (1 DOF)" );
    m_widgetsList.push_back( W.m_paramScaleRotationRadioButton );

    W.m_paramShearAnglesRadioButton = new QRadioButton( "Shear Angles (deg.):" );
    W.m_paramShearAnglesRadioButton->setToolTip( "Shear angles (2 DOF)" );
    m_widgetsList.push_back( W.m_paramShearAnglesRadioButton );

    W.m_paramButtonGroup = new QButtonGroup();
    W.m_paramButtonGroup->addButton( W.m_paramScaleRotationRadioButton );
    W.m_paramButtonGroup->addButton( W.m_paramShearAnglesRadioButton );


    auto translationLayout = new QGridLayout();
    translationLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    translationLayout->setVerticalSpacing( 3 );
    translationLayout->addWidget( new QLabel( " x:" ), 0, 0 );
    translationLayout->addWidget( W.m_xTranslationSpinBox, 0, 1 );
    translationLayout->addWidget( new QLabel( " y:" ), 1, 0 );
    translationLayout->addWidget( W.m_yTranslationSpinBox, 1, 1 );
    translationLayout->addWidget( new QLabel( " z:" ), 2, 0 );
    translationLayout->addWidget( W.m_zTranslationSpinBox, 2, 1 );

    auto rotationLayout = new QGridLayout();
    rotationLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    auto indentLabel = new QLabel( "z: " );
    indentLabel->setVisible( false );
    rotationLayout->addWidget( indentLabel, 0, 0 );
    rotationLayout->addWidget( W.m_zRotationSpinBox, 0, 1 );

    auto scaleLayout = new QGridLayout();
    scaleLayout->setVerticalSpacing( 3 );
    scaleLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    scaleLayout->addWidget( new QLabel( " x:" ), 0, 0 );
    scaleLayout->addWidget( W.m_xScaleSpinBox, 0, 1 );
    scaleLayout->addWidget( new QLabel( " y:" ), 1, 0 );
    scaleLayout->addWidget( W.m_yScaleSpinBox, 1, 1 );

    auto shearLayout = new QGridLayout();
    shearLayout->setVerticalSpacing( 3 );
    shearLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    shearLayout->addWidget( W.m_paramShearAnglesRadioButton, 0, 0, 1, 2 );
    shearLayout->addWidget( new QLabel( " x:" ), 1, 0 );
    shearLayout->addWidget( W.m_xShearSpinBox, 1, 1 );
    shearLayout->addWidget( new QLabel( " y:" ), 2, 0 );
    shearLayout->addWidget( W.m_yShearSpinBox, 2, 1 );

    shearLayout->addWidget( W.m_paramScaleRotationRadioButton, 0, 2, 1, 2 );
    shearLayout->addWidget( new QLabel( " " ), 1, 2 );
    shearLayout->addWidget( W.m_zScaleRotationSpinBox, 1, 3 );

    auto originLayout = new QGridLayout();
    originLayout->setVerticalSpacing( 3 );
    originLayout->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    originLayout->addWidget( new QLabel( " x:" ), 0, 0 );
    originLayout->addWidget( W.m_xOriginSpinBox, 0, 1 );
    originLayout->addWidget( new QLabel( " y:" ), 1, 0 );
    originLayout->addWidget( W.m_yOriginSpinBox, 1, 1 );


    auto mainLayout = new QVBoxLayout();
    expandContentsMargins( mainLayout, 0, 0, 15, 0 );
    mainLayout->setAlignment( Qt::AlignLeft );

    mainLayout->addWidget( new QLabel( "Translation (mm):" ) );
    mainLayout->addLayout( translationLayout );
    mainLayout->addSpacing( 5 );

    mainLayout->addWidget( new QLabel( "Rotation (deg.):" ) );
    mainLayout->addLayout( rotationLayout );
    mainLayout->addSpacing( 5 );

    mainLayout->addWidget( new QLabel( "Scale:" ) );
    mainLayout->addLayout( scaleLayout );
    mainLayout->addSpacing( 5 );

    mainLayout->addLayout( shearLayout );
    mainLayout->addSpacing( 5 );

    mainLayout->addWidget( new QLabel( "Center of Rotation (mm):" ) );
    mainLayout->addLayout( originLayout );
    mainLayout->addSpacing( 10 );

    mainLayout->addWidget( new QLabel( "Slide to Stack transformation matrix:" ) );
    mainLayout->addWidget( W.m_stack_O_slide_matrixWidget );
    mainLayout->addSpacing( 10 );

    mainLayout->addWidget( W.m_setIdentityButton );

    return mainLayout;
}


void SlideStackEditorDock::connectTxWidgets()
{
    enum class Param
    {
        TranslationX,
        TranslationY,
        TranslationZ,
        RotationZ,
        ScaleX,
        ScaleY,
        ShearX,
        ShearY,
        ScaleRotationZ,
        OriginX,
        OriginY
    };


    auto& W = m_transformWidgets;

    auto paramScaleRotationToggledHandler = [this, W] ( bool checked )
    {
        W.m_xShearSpinBox->setEnabled( ! checked );
        W.m_yShearSpinBox->setEnabled( ! checked );
        W.m_zScaleRotationSpinBox->setEnabled( checked );

        if ( m_slideTxDataPartialPublisher && m_activeSlideUid )
        {
            SlideTxDataPartial_msgFromUi msg;
            msg.m_uid = *m_activeSlideUid;
            msg.m_txData.m_useScaleRotationParameterization = checked;
            m_slideTxDataPartialPublisher( msg );
        }
    };


    auto paramChangedHandler = [this] ( const Param& param, double value )
    {
        if ( m_slideTxDataPartialPublisher && m_activeSlideUid )
        {
            SlideTxDataPartial_msgFromUi msg;
            msg.m_uid = *m_activeSlideUid;

            switch (param)
            {
            case Param::TranslationX:   msg.m_txData.m_xTranslationValueInMm = value; break;
            case Param::TranslationY:   msg.m_txData.m_yTranslationValueInMm = value; break;
            case Param::TranslationZ:   msg.m_txData.m_zTranslationValueInMm = value; break;
            case Param::RotationZ:      msg.m_txData.m_zRotationValueInDeg = value; break;
            case Param::ScaleX:         msg.m_txData.m_xScaleValue = value; break;
            case Param::ScaleY:         msg.m_txData.m_yScaleValue = value; break;
            case Param::ShearX:         msg.m_txData.m_xShearValueInDeg = value; break;
            case Param::ShearY:         msg.m_txData.m_yShearValueInDeg = value; break;
            case Param::ScaleRotationZ: msg.m_txData.m_zScaleRotationValueInDeg = value; break;
            case Param::OriginX:        msg.m_txData.m_xOriginValueInMm = value; break;
            case Param::OriginY:        msg.m_txData.m_yOriginValueInMm = value; break;
            }

            m_slideTxDataPartialPublisher( msg );
        }
    };

    // Set slide-to-stack transformation to identity:
    auto setIdentityHandler = [this] ()
    {
        if ( m_slideTxDataPartialPublisher && m_activeSlideUid )
        {
            SlideTxDataPartial_msgFromUi msg;
            msg.m_uid = *m_activeSlideUid;
            msg.m_set_stack_O_slide_identity = true;
            m_slideTxDataPartialPublisher( msg );
        }
    };


    using std::placeholders::_1;

    connect( W.m_paramScaleRotationRadioButton, &QRadioButton::toggled, paramScaleRotationToggledHandler );
//    connect( W.m_paramShearAnglesRadioButton, &QRadioButton::toggled, paramShearAnglesToggledHandler );

    connect( W.m_xTranslationSpinBox, qOverload<double>( &QDoubleSpinBox::valueChanged ), std::bind( paramChangedHandler, Param::TranslationX, _1 ) );
    connect( W.m_yTranslationSpinBox, qOverload<double>( &QDoubleSpinBox::valueChanged ), std::bind( paramChangedHandler, Param::TranslationY, _1 ) );
    connect( W.m_zTranslationSpinBox, qOverload<double>( &QDoubleSpinBox::valueChanged ), std::bind( paramChangedHandler, Param::TranslationZ, _1 ) );

    connect( W.m_zRotationSpinBox, qOverload<double>( &QDoubleSpinBox::valueChanged ), std::bind( paramChangedHandler, Param::RotationZ, _1 ) );

    connect( W.m_xScaleSpinBox, qOverload<double>( &QDoubleSpinBox::valueChanged ), std::bind( paramChangedHandler, Param::ScaleX, _1 ) );
    connect( W.m_yScaleSpinBox, qOverload<double>( &QDoubleSpinBox::valueChanged ), std::bind( paramChangedHandler, Param::ScaleY, _1 ) );

    connect( W.m_zScaleRotationSpinBox, qOverload<double>( &QDoubleSpinBox::valueChanged ), std::bind( paramChangedHandler, Param::ScaleRotationZ, _1 ) );

    connect( W.m_xShearSpinBox, qOverload<double>( &QDoubleSpinBox::valueChanged ), std::bind( paramChangedHandler, Param::ShearX, _1 ) );
    connect( W.m_yShearSpinBox, qOverload<double>( &QDoubleSpinBox::valueChanged ), std::bind( paramChangedHandler, Param::ShearY, _1 ) );

    connect( W.m_xOriginSpinBox, qOverload<double>( &QDoubleSpinBox::valueChanged ), std::bind( paramChangedHandler, Param::OriginX, _1 ) );
    connect( W.m_yOriginSpinBox, qOverload<double>( &QDoubleSpinBox::valueChanged ), std::bind( paramChangedHandler, Param::OriginY, _1 ) );

    connect( W.m_setIdentityButton, &QPushButton::pressed, setIdentityHandler );
}


QScrollArea* SlideStackEditorDock::createHeaderScrollArea()
{
    auto layout = new QVBoxLayout();

    setZeroContentsMargins( layout, false, true, false, false );
    layout->setAlignment( Qt::AlignTop );

    layout->addLayout( createHeaderTabLayout() );

    // Inner widget to scroll:
    auto innerWidget = new QWidget();
    innerWidget->setLayout( layout );

    auto scrollArea = new QScrollArea();
    scrollArea->setWidget( innerWidget );
    scrollArea->setWidgetResizable( true );
    scrollArea->setStyleSheet( sk_scrollAreaStyleSheet );

    return scrollArea;
}


QScrollArea* SlideStackEditorDock::createAnnotationScrollArea()
{
    auto layout = new QVBoxLayout();

    setZeroContentsMargins( layout, true, false, true, false );
    layout->setAlignment( Qt::AlignTop );

    //    imageLayout->addLayout( fileEditLayout );
    //    layout->addWidget( createImagePropertiesGroupBox() );
    //    layout->addWidget( createImageHeaderGroupBox() );
    //    layout->addWidget( createImageTransformationGroupBox() );

    // Inner widget to scroll:
    auto innerWidget = new QWidget();
    innerWidget->setLayout( layout );

    auto scrollArea = new QScrollArea();
    scrollArea->setWidget( innerWidget );
    scrollArea->setWidgetResizable( true );
    scrollArea->setStyleSheet( sk_scrollAreaStyleSheet );

    return scrollArea;
}


std::optional<int> SlideStackEditorDock::getActiveSlideIndex() const
{
    if ( ! m_slideSorterTableModel || 0 == m_slideSorterTableModel->rowCount() ||
         ! m_slideSorterTableView || ! m_slideSorterTableView->selectionModel() )
    {
        return std::nullopt;
    }

    const auto selectedRows = m_slideSorterTableView->selectionModel()->selectedRows();
    if ( selectedRows.empty() )
    {
        return std::nullopt;
    }

    return selectedRows[0].row();
}


std::optional<UID> SlideStackEditorDock::getActiveSlideUid() const
{
    if ( const auto row = getActiveSlideIndex() )
    {
        return m_slideSorterTableModel->getSlide( *row ).m_uid;
    }
    else
    {
        return std::nullopt;
    }
}


void SlideStackEditorDock::selectSlideIndex( int row )
{
    if ( m_slideSorterTableModel &&
         0 <= row && row < m_slideSorterTableModel->rowCount() )
    {
        m_slideSorterTableView->selectRow( row );
    }
}


void SlideStackEditorDock::updateSlideTabWidgets( const UID& activeSlideUid )
{
    if ( ! m_slideHeaderCompleteResponder ||
         ! m_slideViewDataCompleteResponder ||
         ! m_slideTxDataCompleteResponder )
    {
        return;
    }

    if ( const auto header = m_slideHeaderCompleteResponder( activeSlideUid ) )
    {
        setSlideHeaderComplete( *header );
    }

    if ( const auto viewData = m_slideViewDataCompleteResponder( activeSlideUid ) )
    {
        setSlideViewDataComplete( *viewData );
    }

    if ( const auto txData = m_slideTxDataCompleteResponder( activeSlideUid ) )
    {
        setSlideTxDataComplete( *txData );
    }
}


bool SlideStackEditorDock::isActiveSlide( const UID& slideUid )
{
    if ( ! m_activeSlideResponder )
    {
        return false;
    }

    const auto activeSlide = m_activeSlideResponder();

    if ( m_activeSlideUid != activeSlide.m_activeSlideUid )
    {
        // Something has gone wrong, because these values should match!
        /// @todo Throw exception?
        return false;
    }

    if ( m_activeSlideUid && ( slideUid != *m_activeSlideUid ) )
    {
        return false;
    }

    return true;
}


void SlideStackEditorDock::moveToSlide( int slideIndex )
{
    if ( m_moveToSlidePublisher && m_slideSorterTableModel )
    {
        MoveToSlide_msgFromUi msg;
        msg.m_slideIndex = slideIndex;
        msg.m_slideUid = m_slideSorterTableModel->getSlide( slideIndex ).m_uid;
        m_moveToSlidePublisher( msg );
    }
}


void SlideStackEditorDock::blockWidgetSignals( bool block )
{
    if ( m_slideSorterTableView )
    {
        m_slideSorterTableView->blockSignals( block );

        if ( auto* M = m_slideSorterTableView->selectionModel() )
        {
            M->blockSignals( block );
        }
    }
}


void SlideStackEditorDock::setWidgetsEnabled( bool enabled )
{
    for ( auto widget : m_widgetsList )
    {
        widget->setEnabled( enabled );
    }
}


void SlideStackEditorDock::clearAllWidgetValues()
{
//    SilentCall;

//    macroPixmapLabel->clear();

//    for (int i = 0; i < levelsTable->rowCount(); i++)
//        levelsTable->removeRow(i);

//    levelsTable->setRowCount(0);
//    levelsTable->clearContents();


}

} // namespace gui
