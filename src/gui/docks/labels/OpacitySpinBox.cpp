#include "gui/docks/labels/OpacitySpinBox.h"

OpacitySpinBox::OpacitySpinBox( QWidget* parent )
    : QSpinBox( parent )
{
    setFrame( false );
    setRange( 0, 100 );
    setSingleStep( 1 );
}
