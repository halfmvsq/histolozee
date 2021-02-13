#include "gui/docks/slides/PixmapDelegate.h"

#include <QColor>
#include <QColorDialog>
#include <QPainter>
#include <QPixmap>
#include <QStandardItemModel>


namespace gui
{

PixmapDelegate::PixmapDelegate( QObject* parent )
    :
    QStyledItemDelegate( parent )
{
}


void PixmapDelegate::paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index ) const
{
    QColor borderColor = index.data( Qt::BackgroundColorRole ).value<QColor>();

    if ( ! borderColor.isValid() )
    {
        borderColor.setRgbF( 0.0, 0.0, 0.0, 0.0 );
    }

    // Center and scale the pixmap inside the item's rectangle
    const QPixmap pixmap = index.data( Qt::DecorationRole ).value<QPixmap>();
    const QPixmap pixmapScaled = pixmap.scaled( option.rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation );

    const int left = option.rect.left() + ( option.rect.width() - pixmapScaled.width() ) / 2;
    const int top = option.rect.top() + ( option.rect.height() - pixmapScaled.height() ) / 2;

    static constexpr int PEN_WIDTH = 2;

    QPen pen;
    pen.setStyle( Qt::SolidLine );
    pen.setWidth( PEN_WIDTH );
    pen.setBrush( QBrush( borderColor ) );
    pen.setCapStyle( Qt::RoundCap );
    pen.setJoinStyle( Qt::RoundJoin );

    painter->save();
    {
        painter->setPen( pen );
        painter->drawPixmap( left, top, pixmapScaled );
        painter->drawRect( QRect( left, top, pixmapScaled.width(), pixmapScaled.height() ) );
    }
    painter->restore();
}


QWidget* PixmapDelegate::createEditor(
        QWidget* parent,
        const QStyleOptionViewItem& option,
        const QModelIndex& index ) const
{
    QColorDialog* editor = new QColorDialog( parent );

    if ( ! editor )
    {
        return QStyledItemDelegate::createEditor( parent, option, index );
    }

    editor->setWindowTitle( "Select Slide Border Color" );
    editor->setOption( QColorDialog::ShowAlphaChannel ) ;

    connect( editor, &QColorDialog::colorSelected,
             this, &PixmapDelegate::commitAndCloseEditor );

    return editor;
}


void PixmapDelegate::setEditorData(
        QWidget* editor,
        const QModelIndex& index ) const
{
    if ( ! index.model() )
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }

    QColor color = index.model()->data( index, Qt::BackgroundColorRole ).value<QColor>();

    if ( ! color.isValid() )
    {
        color = QColor::fromRgbF( 1.0, 1.0, 1.0, 1.0 );
    }

    if ( QColorDialog* colorEditor = qobject_cast<QColorDialog*>( editor ) )
    {
        colorEditor->setCurrentColor( color );
    }
}


void PixmapDelegate::setModelData(
        QWidget* editor,
        QAbstractItemModel* model,
        const QModelIndex& index ) const
{
    QColorDialog* colorEditor = qobject_cast<QColorDialog*>( editor );

    if ( ! colorEditor )
    {
        QStyledItemDelegate::setModelData( editor, model, index );
    }

    QColor color = colorEditor->selectedColor();

    if ( color.isValid() )
    {
        model->setData( index, QVariant( color ), Qt::BackgroundColorRole );
    }
}


void PixmapDelegate::updateEditorGeometry(
        QWidget* editor,
        const QStyleOptionViewItem& option,
        const QModelIndex& /*index*/ ) const
{
    editor->setGeometry( option.rect );
}


void PixmapDelegate::commitAndCloseEditor()
{
    if ( QColorDialog* editor = qobject_cast<QColorDialog*>( sender() ) )
    {
        // inform the view that there is edited data to replace existing data
        emit commitData( editor );

        // notify the view that this editor is no longer required,
        // at which point the model will delete it
        emit closeEditor( editor );
    }
}

} // namespace gui


//#include "PixmapDelegate.moc"
