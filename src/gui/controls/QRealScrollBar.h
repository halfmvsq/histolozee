#ifndef Q_REAL_SCROLLBAR_H
#define Q_REAL_SCROLLBAR_H

#include <QScrollBar>

#include <utility>


namespace gui
{

/**
 * @brief Subclass of QScrollBar that handles real, floating-point
 * ranges, step sizes, and values. Most of QRealScrollBar's setters and
 * getters mirror those of QScrollBar, except with the suffix "F" denoting
 * support of floating-point numbers.
 */
class QRealScrollBar : public QScrollBar
{
    Q_OBJECT

public:

    /**
     * @brief QRealScrollBar
     * @param orientation Horizontal/vertical
     * @param precision Total number of discrete step steps for the scroll bar.
     * More steps means higher precision.
     * @param parent Qt parent widget
     */
    explicit QRealScrollBar(
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

    /// Total number of single slider steps. More steps means more
    /// floating point precision for the scroll bar.
    int m_N;

    /// Minimum scroll value
    qreal m_minF;

    /// Maximum scroll value
    qreal m_maxF;
};

} // namespace gui

#endif // Q_REAL_SCROLLBAR_H
