#ifndef Q_REAL_SLIDER_H
#define Q_REAL_SLIDER_H

#include <QSlider>

#include <utility>

namespace gui
{

/**
 * @brief Subclass of QSlider that handles real, floating-point
 * ranges, step sizes, and values. Most of QRealSlider's setters and
 * getters mirror those of QSlider, except with the suffix "F" denoting
 * support of floating-point numbers.
 *
 * @note There is a CTK widget (ctkDoubleSlider) that does much the same thing.
 * We may migrate to the CTK widget at some point.
 */
class QRealSlider : public QSlider
{
    Q_OBJECT

public:

    /**
     * @brief QRealSlider
     * @param orientation Horizontal/vertical
     * @param precision Total number of discrete step steps for the slider.
     * More steps means higher precision.
     * @param parent Qt parent widget
    */
    explicit QRealSlider(
            Qt::Orientation orientation,
            int precision = 100000,
            QWidget* parent = nullptr );

    qreal minimumF() const;
    void setMinimumF( qreal min );

    qreal maximumF() const;
    void setMaximumF( qreal max );

    std::pair<qreal, qreal> rangeF() const;
    void setRangeF( qreal min, qreal max );

    qreal singleStepF() const;
    void setSingleStepF( qreal step );

    qreal pageStepF() const;
    void setPageStepF( qreal step );

    qreal tickIntervalF() const;
    void setTickIntervalF( qreal ti );

    qreal valueF() const;
    void setValueF( qreal val );

    int precision() const;
    void setPrecision( int N );


signals:

    /// Signal emitted when the slider's value changes by any means
    void valueChangedF( double value );

    /// Signal emitted when the slider's value changes by a user mouse
    /// movement, press, or release
    void sliderMovedF( double value );


private:

    qreal normRange() const;

    /// Total number of single slider steps. More steps equates to more
    /// floating point precision for the slider.
    int m_N;

    /// Minimum value
    qreal m_minF;

    /// Maximum value
    qreal m_maxF;
};

} // namespace gui

#endif // Q_REAL_SLIDER_H
