#include "gui/docks/Utility.h"

#include "externals/ctk/Widgets/ctkMatrixWidget.h"


namespace gui
{

void setZeroContentsMargins( QWidget* widget, bool zeroLeft, bool zeroTop, bool zeroRight, bool zeroBottom )
{
    if ( widget )
    {
        int l, t, r, b;
        widget->getContentsMargins( &l, &t, &r, &b );

        l = ( zeroLeft ) ? 0 : l;
        t = ( zeroTop ) ? 0 : t;
        r = ( zeroRight ) ? 0 : r;
        b = ( zeroBottom ) ? 0 : b;

        widget->setContentsMargins( l, t, r, b );
    }

    //    if ( QLayout* layout = widget->layout() )
    //    {
    //        int l, t, r, b;
    //        layout->getContentsMargins( &l, &t, &r, &b );

    //        l = ( zeroLeft ) ? 0 : l;
    //        t = ( zeroTop ) ? 0 : t;
    //        r = ( zeroRight ) ? 0 : r;
    //        b = ( zeroBottom ) ? 0 : b;

    //        layout->setContentsMargins( l, t, r, b );
    //    }
}


void setZeroContentsMargins( QLayout* layout, bool zeroLeft, bool zeroTop, bool zeroRight, bool zeroBottom )
{
    if ( layout )
    {
        int l, t, r, b;
        layout->getContentsMargins( &l, &t, &r, &b );

        l = ( zeroLeft ) ? 0 : l;
        t = ( zeroTop ) ? 0 : t;
        r = ( zeroRight ) ? 0 : r;
        b = ( zeroBottom ) ? 0 : b;

        layout->setContentsMargins( l, t, r, b );
    }
}


void expandContentsMargins( QLayout* layout, int addLeft, int addTop, int addRight, int addBottom )
{
    if ( layout )
    {
        int l, t, r, b;
        layout->getContentsMargins( &l, &t, &r, &b );
        layout->setContentsMargins( l + addLeft, t + addTop, r + addRight, b + addBottom );
    }
}


void setMatrixWidgetValues( ctkMatrixWidget* widget, const glm::dmat4& m )
{
    if ( ! widget || 4 != widget->rowCount() || 4 != widget->columnCount() )
    {
        return;
    }

    for ( int row = 0; row < 4; ++row )
    {
        for ( int col = 0; col < 4; ++col )
        {
            widget->setValue( row, col, m[col][row] );
        }
    }
}

} // namespace gui
