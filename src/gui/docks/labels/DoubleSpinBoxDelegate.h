#ifndef GUI_DOUBLE_SPINBOX_DELEGATE_H
#define GUI_DOUBLE_SPINBOX_DELEGATE_H

#include <QStyledItemDelegate>


namespace gui
{

class DoubleSpinBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    DoubleSpinBoxDelegate( QObject* parent = nullptr );
    ~DoubleSpinBoxDelegate() override = default;

    QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index ) const override;

    void setEditorData( QWidget* editor, const QModelIndex& index ) const override;

    void setModelData( QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index ) const override;

    void updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem& option,
                               const QModelIndex& index ) const override;


private slots:

    void commitAndCloseEditor();
};

} // namespace gui

#endif // GUI_DOUBLE_SPINBOX_DELEGATE_H
