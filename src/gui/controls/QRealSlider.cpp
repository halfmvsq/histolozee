#include "gui/controls/QRealSlider.h"

namespace gui
{

QRealSlider::QRealSlider(
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
    setTickIntervalF( 10.0 );
    setValueF( 0.0 );


    // Forward slider event signals from QSlider to the sliderMovedF and
    // valueChangedF signals.
    auto emitSliderMovedF = [this] ()
    {
        emit sliderMovedF( valueF() );
    };

    // This signal is emitted when sliderDown is true and the slider moves.
    // This usually happens when the user is dragging the slider.
    connect( this, &QSlider::sliderMoved, emitSliderMovedF );

    // This signal is emitted when the user presses the slider with the mouse.
    connect( this, &QSlider::sliderPressed, emitSliderMovedF );

    // This signal is emitted when the user releases the slider with the mouse.
    connect( this, &QSlider::sliderReleased, emitSliderMovedF );


    // The signal valueChanged is emitted when ever the slider value changes.
    // Note: emitting this signal, even if never used in the app,
    // seems to sometimes break slice scrolling. No idea why.
    // Since it is not used, just comment out emission of the signal.

    //    auto emitValueChangedF = [this] ()
    //    {
    //        emit valueChangedF( valueF() );
    //    };

    // connect( this, &QSlider::valueChanged, emitValueChangedF );
}

qreal QRealSlider::minimumF() const
{
    return m_minF;
}

void QRealSlider::setMinimumF( qreal min )
{
    m_minF = min;
    setValueF( valueF() );
}

qreal QRealSlider::maximumF() const
{
    return m_maxF;
}

void QRealSlider::setMaximumF( qreal max )
{
    m_maxF = max;
    setValueF( valueF() );
}

std::pair< qreal, qreal > QRealSlider::rangeF() const
{
    return { m_minF, m_maxF };
}

void QRealSlider::setRangeF( qreal min, qreal max )
{
    m_minF = min;
    m_maxF = max;
    setValueF( valueF() );
}

qreal QRealSlider::singleStepF() const
{
    return singleStep() * normRange();
}

void QRealSlider::setSingleStepF( qreal step )
{
    setSingleStep( static_cast<int>( step / normRange() ) );
}

qreal QRealSlider::pageStepF() const
{
    return pageStep() * normRange();
}

void QRealSlider::setPageStepF( qreal step )
{
    setPageStep( static_cast<int>( step / normRange() ) );
}

qreal QRealSlider::tickIntervalF() const
{
    return tickInterval() * normRange();
}

void QRealSlider::setTickIntervalF( qreal ti )
{
    setTickInterval( static_cast<int>( ti / normRange() ) );
}

qreal QRealSlider::valueF() const
{
    return ( m_minF + normRange() * value() );
}

void QRealSlider::setValueF( qreal val )
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
        int newValue = static_cast<int>( ( val - m_minF ) / normRange() );

        if ( newValue != value() )
        {
            setValue( newValue );
        }
    }
    blockSignals( false );
}

int QRealSlider::precision() const
{
    return m_N;
}

void QRealSlider::setPrecision( int N )
{
    if ( N <= 0 && N == m_N )
    {
        return;
    }

    const qreal saveVal = valueF();

    m_N = N;
    setRange( 0, m_N );

    // Restore saved value
    setValueF( saveVal );
}

qreal QRealSlider::normRange() const
{
    return ( ( m_maxF - m_minF ) / static_cast<qreal>( m_N ) );
}

} // namespace gui
