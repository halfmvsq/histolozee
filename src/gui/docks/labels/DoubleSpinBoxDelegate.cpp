#include "gui/docks/labels/DoubleSpinBoxDelegate.h"

#include <QDoubleSpinBox>

#include <iostream>


namespace gui
{

DoubleSpinBoxDelegate::DoubleSpinBoxDelegate( QObject* parent )
    : QStyledItemDelegate( parent )
{}


QWidget* DoubleSpinBoxDelegate::createEditor(
        QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/ ) const
{
    QDoubleSpinBox* editor = new QDoubleSpinBox( parent );

    if ( ! editor )
    {
        return nullptr;
    }

    editor->setFrame( false );
    editor->setMinimum( 0.0 );
    editor->setMaximum( 1.0 );
    editor->setSingleStep( 0.01 );
    editor->setDecimals( 2 );

    // user presses Enter or moves the focus out of editor
    connect( editor, &QDoubleSpinBox::editingFinished,
             this, &DoubleSpinBoxDelegate::commitAndCloseEditor );

    return editor;
}


void DoubleSpinBoxDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
    if ( ! index.model() )
    {
        return;
    }

    const double value = index.model()->data( index, Qt::EditRole ).toDouble();

    if ( QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>( editor ) )
    {
        spinBox->setValue( value );
    }
}


void DoubleSpinBoxDelegate::setModelData(
        QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const
{
    if ( ! model )
    {
        return;
    }

    if ( QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>( editor ) )
    {
        spinBox->interpretText();
        const double value = spinBox->value();

        model->setData( index, value, Qt::EditRole );
    }
}


void DoubleSpinBoxDelegate::updateEditorGeometry(
        QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/ ) const
{
    if ( editor )
    {
        editor->setGeometry( option.rect );
    }
}


void DoubleSpinBoxDelegate::commitAndCloseEditor()
{
    if ( QDoubleSpinBox* editor = qobject_cast<QDoubleSpinBox*>( sender() ) )
    {
        // inform the view that there is edited data to replace existing data
        emit commitData( editor );

        // notify the view that this editor is no longer required,
        // at which point the model will delete it
        emit closeEditor( editor );
    }
}

} // namespace gui
