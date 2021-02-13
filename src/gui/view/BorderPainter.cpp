#include "gui/view/BorderPainter.h"

namespace
{
static const int sk_alpha = 128;
static const double sk_width = 5.0;
}

namespace gui
{

BorderPainter::BorderPainter( QPaintDevice* device )
    : m_painter( device )
{
    setSize( 0, 0 );
    setColor( 0, 0, 0 );
}

void BorderPainter::setSize( int width, int height )
{
    QRectF rect( 0, 0, width, height );
    m_path.addRect( rect );
}

void BorderPainter::setColor( uint8_t r, uint8_t g, uint8_t b )
{
    QBrush brush( QColor( r, g, b, sk_alpha ) );
    QPen pen( brush, sk_width, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin );
    m_painter.setPen( pen );
}

void BorderPainter::draw()
{
    m_painter.drawPath( m_path );
}

} // namespace gui
