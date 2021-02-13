#ifndef GUI_PIXMAP_DELEGATE_H
#define GUI_PIXMAP_DELEGATE_H

#include <QStyledItemDelegate>


namespace gui
{

class PixmapDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    explicit PixmapDelegate( QObject* parent = nullptr );

    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const override;

    QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const override;

    void setEditorData( QWidget* editor, const QModelIndex& index ) const override;

    void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const override;

    void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const override;


private slots:

    void commitAndCloseEditor();
};

} // namespace gui

#endif // GUI_PIXMAP_DELEGATE_H
