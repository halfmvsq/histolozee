#include "gui/docks/labels/LabelColorDialog.h"

LabelColorDialog::LabelColorDialog( QWidget* widget )
    :
      QColorDialog( widget )
{
    // Do not show alpha (opacity), since it is set separately
    setOption( QColorDialog::ColorDialogOption::ShowAlphaChannel, false );
    setWindowTitle( "Choose Label Color" );

    connect( this, &QColorDialog::colorSelected,
             this, &LabelColorDialog::colorChanged );
}

QColor LabelColorDialog::color() const
{
    return QColorDialog::selectedColor();
}

void LabelColorDialog::setColor( QColor c )
{
    QColorDialog::setCurrentColor( c );
}
