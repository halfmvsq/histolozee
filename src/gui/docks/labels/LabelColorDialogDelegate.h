#ifndef GUI_LABEL_COLOR_DIALOG_DELEGATE_H
#define GUI_LABEL_COLOR_DIALOG_DELEGATE_H

#include <QStyledItemDelegate>


namespace gui
{

class LabelColorDialogDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    LabelColorDialogDelegate( QObject* parent = nullptr );

    QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const override;

    void setEditorData( QWidget* editor, const QModelIndex& index ) const override;

    void setModelData( QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const override;

    void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option,
                               const QModelIndex& index) const override;
};

} // namespace gui

#endif // GUI_LABEL_COLOR_DIALOG_DELEGATE_H
