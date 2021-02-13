#include "gui/docks/labels/OpacityDoubleSpinBox.h"

OpacityDoubleSpinBox::OpacityDoubleSpinBox( QWidget* parent )
    : QDoubleSpinBox( parent )
{
    setFrame( false );
    setRange( 0.0, 1.0 );
    setSingleStep( 0.01 );
    setDecimals( 2 );
}
