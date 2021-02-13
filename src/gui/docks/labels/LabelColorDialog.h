#ifndef GUI_LABEL_COLOR_DIALOG_H
#define GUI_LABEL_COLOR_DIALOG_H

#include <QColorDialog>


class LabelColorDialog : public QColorDialog
{
    Q_OBJECT

    Q_PROPERTY( QColor color
                READ color
                WRITE setColor
                NOTIFY colorChanged
                USER true )

public:

    LabelColorDialog( QWidget* widget = nullptr );
    ~LabelColorDialog() = default;

    QColor color() const;
    void setColor( QColor c );


signals:

    void colorChanged( const QColor& );
};

#endif // GUI_LABEL_COLOR_DIALOG_H
