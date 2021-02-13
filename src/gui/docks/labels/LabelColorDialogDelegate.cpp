#include "gui/docks/labels/LabelColorDialogDelegate.h"
#include "gui/docks/labels/LabelColorDialog.h"


namespace gui
{

LabelColorDialogDelegate::LabelColorDialogDelegate( QObject* parent )
    : QStyledItemDelegate( parent )
{}


QWidget* LabelColorDialogDelegate::createEditor(
        QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/ ) const
{
    LabelColorDialog *editor = new LabelColorDialog( parent );
    return editor;
}


void LabelColorDialogDelegate::setEditorData( QWidget* editor, const QModelIndex&index ) const
{
    if ( ! index.model() || ! editor )
    {
        return;
    }

    QColor color = index.model()->data( index, Qt::DecorationRole ).value<QColor>();

    if ( ! color.isValid() )
    {
        // Invalid color if user clicks "cancel" in QColorDialog
        return;
    }

    if ( LabelColorDialog* dialog = static_cast<LabelColorDialog*>( editor ) )
    {
        dialog->setColor( color );
    }
}


void LabelColorDialogDelegate::setModelData(
        QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const
{
    if ( ! model || ! editor )
    {
        return;
    }

    if ( LabelColorDialog* dialog = static_cast<LabelColorDialog*>( editor ) )
    {
        QColor color = dialog->color();

        if ( color.isValid() )
        {
            model->setData( index, color, Qt::DecorationRole );
        }
    }
}


void LabelColorDialogDelegate::updateEditorGeometry(
        QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/ ) const
{
    if ( editor )
    {
        editor->setGeometry( option.rect );
    }
}

} // namespace gui
