#ifndef BORDER_PAINTER_H
#define BORDER_PAINTER_H

#include <QPainter>

namespace gui
{

/**
 * @brief The BorderPainter class draws a rectangle of given size and color on a QPaintDevice.
 * The border width and color alpha are currently defined as fixed constants.
 */
class BorderPainter final
{
public:

    explicit BorderPainter( QPaintDevice* device );

    void setSize( int width, int height );

    void setColor( uint8_t r, uint8_t g, uint8_t b );

    void draw();


private:

    QPainter m_painter;
    QPainterPath m_path;
};

} // namespace gui

#endif // BORDER_PAINTER_H
