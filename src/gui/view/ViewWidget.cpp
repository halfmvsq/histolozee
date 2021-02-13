#include "gui/view/ViewWidget.h"
#include "gui/view/GLWidget.h"
#include "gui/controls/QRealScrollBar.h"
#include "gui/docks/Utility.h"

#include "externals/ctk/Widgets/ctkDoubleSlider.h"

#include "common/HZeeException.hpp"

#include <QGridLayout>

#include <cmath>


namespace
{

// Pixel margin around layouts
static constexpr int sk_margin = 6;

// Pixel spacing in layouts
static constexpr int sk_spacing = 3;

}


namespace gui
{

ViewWidget::ViewWidget(
        const UID& viewUid,
        GLWidget* glWidget,
        ScrollBarValuesBroadcasterType xyScrollBarValuesBroadcaster,
        SliceSliderValueBroadcasterType sliceSliderValueBroadcaster,
        QWidget* parent )
    :
      QWidget( parent ),

      m_viewUid( viewUid ),

      m_glWidget( glWidget ),

      m_xScrollBar( new QRealScrollBar( Qt::Orientation::Horizontal ) ),
      m_yScrollBar( new QRealScrollBar( Qt::Orientation::Vertical ) ),
      m_zSlider( new ctkDoubleSlider( Qt::Orientation::Vertical ) ),

      m_scrollBarsAndSliderParamsProvider( nullptr ),
      m_xyScrollBarValuesBroadcaster( xyScrollBarValuesBroadcaster ),
      m_sliceSliderValueBroadcaster( sliceSliderValueBroadcaster )
{
    if ( ! m_glWidget )
    {
        throw_debug( "Cannot construct ViewWidget with null GLWidget" );
    }

    if ( ! m_xScrollBar || ! m_xScrollBar || ! m_zSlider )
    {
        throw_debug( "Cannot construct ViewWidget with null scrollbars or slider" );
    }

    QGridLayout* layout = new QGridLayout;

    layout->setContentsMargins( sk_margin, sk_margin, sk_margin, sk_margin );
    layout->setSpacing( sk_spacing );

    layout->addWidget( m_zSlider, 0, 0, 2, 1 );
    layout->addWidget( m_glWidget, 0, 1, 1, 1 );
    layout->addWidget( m_yScrollBar, 0, 2, 1, 1 );
    layout->addWidget( m_xScrollBar, 1, 1, 1, 1 );

    // Prevent slider and scrollbars from getting focus,
    m_xScrollBar->setFocusPolicy( Qt::FocusPolicy::NoFocus );
    m_yScrollBar->setFocusPolicy( Qt::FocusPolicy::NoFocus );
    m_zSlider->setFocusPolicy( Qt::FocusPolicy::NoFocus );

    setLayout( layout );


    auto xScrollBarValueChangedBroadcaster = [this] ( double value )
    {
        if ( m_xyScrollBarValuesBroadcaster )
        {
            m_xyScrollBarValuesBroadcaster( m_viewUid, value, 0.0 );
        }
    };

    auto yScrollBarChangedValueBroadcaster = [this] ( double value )
    {
        if ( m_xyScrollBarValuesBroadcaster )
        {
            // Invert the value, since the vertical scroll bar's
            // coordinates are inverted w.r.t. view y coordinates
            m_xyScrollBarValuesBroadcaster( m_viewUid, 0.0, -value );
        }
    };

    auto sliceValueChangedBroadcaster = [this] ( double value )
    {
        if ( m_sliceSliderValueBroadcaster )
        {
            m_sliceSliderValueBroadcaster( m_viewUid, value );
        }
    };

    connect( m_xScrollBar, &QRealScrollBar::valueChangedF, this, xScrollBarValueChangedBroadcaster );
    connect( m_yScrollBar, &QRealScrollBar::valueChangedF, this, yScrollBarChangedValueBroadcaster );
    connect( m_zSlider, &ctkDoubleSlider::valueChanged, this, sliceValueChangedBroadcaster );
}


void ViewWidget::resizeEvent( QResizeEvent* event )
{
    QWidget::resizeEvent( event );

    if ( event )
    {
        updateScrollBarsAndSlider();
    }
}


void ViewWidget::setScrollBarsAndSliderParamsProvider( ScrollBarsAndSliderParamsProviderType provider )
{
    m_scrollBarsAndSliderParamsProvider = provider;
}

void ViewWidget::setScrollBarValuesBroadcaster( ScrollBarValuesBroadcasterType broadcaster )
{
    m_xyScrollBarValuesBroadcaster = broadcaster;
}

void ViewWidget::setSliceSliderValueBroadcaster( SliceSliderValueBroadcasterType broadcaster )
{
    m_sliceSliderValueBroadcaster = broadcaster;
}


void ViewWidget::setHorizontalScrollBarParams( const ViewSliderParams& params )
{
    if ( m_xScrollBar )
    {
        QSignalBlocker2<QRealScrollBar> blocker( m_xScrollBar );

        m_xScrollBar->setRangeF( params.m_minimum, params.m_maximum );
        m_xScrollBar->setSingleStepF( params.m_singleStep );
        m_xScrollBar->setPageStepF( params.m_pageStep );
        m_xScrollBar->setValueF( params.m_value );
        m_xScrollBar->setEnabled( params.m_enabled );
    }
}


void ViewWidget::setVerticalScrollBarParams( const ViewSliderParams& params )
{
    if ( m_yScrollBar )
    {
        QSignalBlocker2<QRealScrollBar> blocker( m_yScrollBar );

        // Invert and swap the min/max values, since the vertical scroll bar's
        // coordinates are inverted w.r.t. view y coordinates
        m_yScrollBar->setRangeF( -params.m_maximum, -params.m_minimum );
        m_yScrollBar->setSingleStepF( params.m_singleStep );
        m_yScrollBar->setPageStepF( params.m_pageStep );
        m_yScrollBar->setValueF( params.m_value );
        m_yScrollBar->setEnabled( params.m_enabled );
    }
}


void ViewWidget::setSliceSliderParams( const ViewSliderParams& params )
{
    if ( m_zSlider )
    {
        QSignalBlocker2<ctkDoubleSlider> blocker( m_zSlider );

        m_zSlider->setRange( params.m_minimum, params.m_maximum );
        m_zSlider->setPageStep( params.m_pageStep );
        m_zSlider->setTickInterval( params.m_pageStep );
        m_zSlider->setValue( params.m_value );

        if ( ! std::isnan( params.m_singleStep) )
        {
            /// @todo Find out why this value is nan. Cameras in off-screen, non-rendered
            /// views have bad values.
            m_zSlider->setSingleStep( params.m_singleStep );
        }

        m_zSlider->setEnabled( params.m_enabled );
    }
}


const UID& ViewWidget::getViewUid() const
{
    return m_viewUid;
}


IRenderer* ViewWidget::getRenderer()
{
    if ( m_glWidget )
    {
        return m_glWidget->getRenderer();
    }
    return nullptr;
}


void ViewWidget::renderUpdate()
{
    if ( m_glWidget )
    {
        m_glWidget->update();
        updateScrollBarsAndSlider();
    }
}


void ViewWidget::updateScrollBarsAndSlider()
{
    if ( m_scrollBarsAndSliderParamsProvider )
    {
        const auto params = m_scrollBarsAndSliderParamsProvider( m_viewUid );
        setHorizontalScrollBarParams( std::get<0>( params ) );
        setVerticalScrollBarParams( std::get<1>( params ) );
        setSliceSliderParams( std::get<2>( params ) );
    }
}
} // namespace gui
