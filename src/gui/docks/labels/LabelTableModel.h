#ifndef GUI_LABEL_TABLE_MODEL_H
#define GUI_LABEL_TABLE_MODEL_H

#include "gui/messages/parcellation/ParcellationLabelData.h"

#include <QAbstractTableModel>

#include <vector>


namespace gui
{

class LabelTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    /**
     * @brief Construct a model with no label data
     */
    explicit LabelTableModel( QObject* parent = nullptr );

    ~LabelTableModel() override = default;

    int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
    int columnCount( const QModelIndex& parent = QModelIndex() ) const override;

    Qt::ItemFlags flags( const QModelIndex& index ) const override;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
    bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole ) override;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

    /**
     * @brief Set data at multiple indices to the same value and for the same role
     * @param indices Vector of indices in model to modifty
     * @param value New value
     * @param role Role to edit
     * @return True iff the data were set
     */
    bool setData( const std::vector<QModelIndex>& indices, const QVariant& value, int role = Qt::EditRole );

    /**
     * @brief Clear all labels in model
     */
    void clearAllLabels();

    /**
     * @brief Set all labels
     * @param[in] labels New labels
     */
    void setAllLabels( std::vector<ParcellationLabel> labels );

    /**
     * @brief Get label at specific row of model
     * @param[in] row Row of model
     * @return Const (non-owning) pointer to label at row; nullptr if model row is invalid.
     */
    const ParcellationLabel* getLabel( int row ) const;

    /**
     * @brief Get all labels in model
     * @return Vector of all labels
     */
    const std::vector<ParcellationLabel>& getAllLabels() const;


    // Column indices of the model:
    static constexpr int LABEL_VALUE_COLUMN = 0;
    static constexpr int LABEL_COLOR_COLUMN = 1;
    static constexpr int LABEL_ALPHA_COLUMN = 2;
    static constexpr int LABEL_MESH_VISIBILITY_COLUMN = 3;
    static constexpr int LABEL_NAME_COLUMN = 4;


signals:

    /**
     * @brief Signal that data was edited in rows by the user.
     * @note This is a simplified version of the signal dataChanged
     *
     * @param[in] rows Rows of data edited in model
     */
    void dataEdited( std::vector<int> rows ) const;


private:

    ParcellationLabel* getModelRow( int row );

    std::vector<ParcellationLabel> m_labelData;

    /// Flag to block "dataEdited" signal from being emitted
    bool m_blockDataEditedSignal = false;
};

} // namespace gui

#endif // GUI_LABEL_TABLE_MODEL_H
