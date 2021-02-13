#ifndef GUI_SLIDE_SORTER_TABLE_MODEL_H
#define GUI_SLIDE_SORTER_TABLE_MODEL_H

#include "gui/messages/slide/SlideStackData.h"

#include <QAbstractTableModel>
#include <QPixmap>

#include <list>
#include <utility>


namespace gui
{

/**
 * @brief Model for the Slide Stack Sorter Table
 */
class SlideSorterTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    /**
     * @brief Construct an empty model that contains no slides
     */
    explicit SlideSorterTableModel( QObject* parent = nullptr );

    ~SlideSorterTableModel() override = default;


    int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
    int columnCount( const QModelIndex& parent = QModelIndex() ) const override;

    Qt::ItemFlags flags( const QModelIndex& index ) const override;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
    bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole ) override;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

    Qt::DropActions supportedDropActions() const override;

    QStringList mimeTypes() const override;
    QMimeData* mimeData( const QModelIndexList& indexes ) const override;

    bool dropMimeData( const QMimeData* data, Qt::DropAction action,
                       int row, int column, const QModelIndex& parent ) override;


    /**
     * @brief Clear all model data
     */
    void clearSlideStack();

    /**
     * @brief Set all model data. All existing model data is replaced with new data.
     * @param[in] slideStack New slide stack data
     */
    void setSlideStack( const std::list<SlidePreview>& slideStack );

    /**
     * @brief Replace one row of model data. The data at the row corresponding to the new slide
     * is replaced. If no existing row matches the new slide, then nothing happens.
     *
     * @param[in] slide New slide data for the row
     * @return True iff the slide data was set in the model; false otherwise.
     */
    bool replaceSlide( SlidePreview slide );

    /**
     * @brief Get slide data at a row of the model
     * @param[in] row Row queried of model
     * @return Slide data at row
     */
    const SlidePreview& getSlide( int row ) const;

    /**
     * @brief Get all ordered slide stack data
     */
    std::list<SlidePreview> getSlideStack() const;

    /**
     * @brief Get all ordered slide UIDs in stack
     */
    std::list<UID> getSlideStackOrderedUids() const;


    // Column indices of the model:
    static constexpr int SLIDE_IMAGE_COLUMN = 0;
    static constexpr int SLIDE_OPACITY_COLUMN = 1;
    static constexpr int SLIDE_ID_COLUMN = 2;
//    static constexpr int SLIDE_ANNOT_COLUMN = 3;


signals:

    /**
     * @brief Signal that data was edited in a row of the model
     * @param[in] row Edited row
     */
    void dataEdited( int row ) const;

    /**
     * @brief Signal that data was moved to a new row in the model
     * @param[in] newRow Destination row
     */
    void dataMovedRows( int newRow ) const;

    /**
     * @brief Signal that the order of rows in the model changed
     * @param[in] List of all ordered slide UIDs
     */
    void dataOrderChanged( const std::list<UID>& orderedSlideUids ) const;


private:

    /// Get const reference to data at row of model
    const std::pair<SlidePreview, QPixmap>& getModelRow( int row ) const;

    /// Get non-const reference to data at row of model
    std::pair<SlidePreview, QPixmap>& getModelRow( int row );

    /// Insert slide data at row of model
    bool insertSlide( int row, const std::pair<SlidePreview, QPixmap>& slideData );

    /// Remove slide data at row of model
    bool removeSlide( int row );

    /// Reassign the index property of all slides based on their row in the model list
    void reassignSlideIndices();

    /// Print out the ordered UIDs of all slides in the model
    void printOrderedSlideUids();

    /// List of slide stack data, which consists of the SlidePreview object
    /// and the QPixmap that is displayed as the Qt::DecorationRole of the model
    std::list< std::pair<SlidePreview, QPixmap> > m_slideData;
};

} // namespace gui

#endif // GUI_SLIDE_SORTER_TABLE_MODEL_H
