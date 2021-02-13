#include "gui/controls/QRealScrollBar.h"

namespace gui
{

QRealScrollBar::QRealScrollBar(
        Qt::Orientation orientation,
        int precision,
        QWidget* parent )
{
    setParent( parent );
    setOrientation( orientation );
    setPrecision( precision );

    setRangeF( 0.0, 100.0 );
    setSingleStepF( 1.0 );
    setPageStepF( 10.0 );
    setValueF( 0.0 );


    // Forward slider event signals from QSlider to the sliderMovedF and
    // valueChangedF signals.
    auto emitSliderMovedF = [this] ()
    {
        emit sliderMovedF( valueF() );
    };

    auto emitValueChangedF = [this] ()
    {
        emit valueChangedF( valueF() );
    };

    // This signal is emitted when sliderDown is true and the slider moves.
    // This usually happens when the user is dragging the slider.
    connect( this, &QScrollBar::sliderMoved, emitSliderMovedF );

    // This signal is emitted when the user presses the slider with the mouse.
    connect( this, &QScrollBar::sliderPressed, emitSliderMovedF );

    // This signal is emitted when the user releases the slider with the mouse.
    connect( this, &QScrollBar::sliderReleased, emitSliderMovedF );

    // This signal is emitted when ever the slider value changes.
    connect( this, &QScrollBar::valueChanged, emitValueChangedF );
}

qreal QRealScrollBar::minimumF() const
{
    return m_minF;
}

void QRealScrollBar::setMinimumF( qreal min )
{
    m_minF = min;
    setValueF( valueF() );
}

qreal QRealScrollBar::maximumF() const
{
    return m_maxF;
}

void QRealScrollBar::setMaximumF( qreal max )
{
    m_maxF = max;
    setValueF( valueF() );
}

std::pair< qreal, qreal > QRealScrollBar::rangeF() const
{
    return { m_minF, m_maxF };
}

void QRealScrollBar::setRangeF( qreal min, qreal max )
{
    m_minF = min;
    m_maxF = max;
    setValueF( valueF() );
}

qreal QRealScrollBar::singleStepF() const
{
    return singleStep() * normRange();
}

void QRealScrollBar::setSingleStepF( qreal step )
{
    setSingleStep( static_cast<int>( step / normRange() ) );
}

qreal QRealScrollBar::pageStepF() const
{
    return pageStep() * normRange();
}

void QRealScrollBar::setPageStepF( qreal step )
{
    setPageStep( static_cast<int>( step / normRange() ) );
}

qreal QRealScrollBar::valueF() const
{
    return ( m_minF + normRange() * value() );
}

void QRealScrollBar::setValueF( qreal val )
{
    if ( isSliderDown() )
    {
        // Prevent setting value when the slider is held down.
        // Without this check, while attempting to drag the slider,
        // it would be locked in place due to the App forcing its value.
        return;
    }

    // Block signals when the slider value is changed in order to prevent
    // ringing of signals caused by a signal-slot feedback loop.

    blockSignals( true );
    {
        setValue( static_cast<int>( ( val - m_minF ) / normRange() ) );
    }
    blockSignals( false );
}

int QRealScrollBar::precision() const
{
    return m_N;
}

void QRealScrollBar::setPrecision( int N )
{
    if ( N <= 0 )
    {
        return;
    }

    const qreal saveVal = valueF();

    m_N = N;
    setRange( 0, m_N );

    // Restore saved value
    setValueF( saveVal );
}

qreal QRealScrollBar::normRange() const
{
    return ( ( m_maxF - m_minF ) / static_cast<qreal>( m_N ) );
}

} // namespace gui
