#include "gui/docks/slides/SlideSorterTableModel.h"
#include "common/HZeeException.hpp"

#include <glm/glm.hpp>

#include <QColor>
#include <QImage>
#include <QMimeData>

#include <sstream>


namespace
{

// Column heading names
static const std::vector< std::string > sk_columns = {
    "Slide",   /* Column 0: Slide thumbnail (with colored frame) */
    "Opacity", /* Column 1: Slide visibility check box and opacity spin box */
    "ID"       /* Column 2: Slide name text box */

// Column 3 is no longer in use:
//  "Annotation" /* Column 3: Slide annotation visibility check box and opacity spin box */
};


/**
 * @brief Convert RGB color from \c glm::vec3 to \c QColor object
 * @param[in] color RGB color as \c glm::vec3
 * @return Color in Qt's \c QColor format
 */
QColor rgbToQColor( const glm::vec3& color )
{
    const glm::dvec3 c{ color };
    QColor qc;
    qc.setRgbF( c.r, c.g, c.b );
    return qc;
}

} // anonymous


namespace gui
{

SlideSorterTableModel::SlideSorterTableModel( QObject* parent )
    :
      QAbstractTableModel( parent ),
      m_slideData()
{}


int SlideSorterTableModel::rowCount( const QModelIndex& /*parent*/ ) const
{
    return static_cast<int>( m_slideData.size() );
}


int SlideSorterTableModel::columnCount( const QModelIndex& /*parent*/ ) const
{
    return static_cast<int>( sk_columns.size() );
}


QVariant SlideSorterTableModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( section < 0 )
    {
        return QVariant(); // Invalid section
    }

    if ( Qt::DisplayRole != role )
    {
        return QVariant();
    }

    if ( Qt::Horizontal == orientation )
    {
        const int col = section;

        if ( col >= columnCount() )
        {
            return QVariant();
        }

        return QString::fromStdString( sk_columns.at( static_cast<size_t>( col ) ) );
    }
    else if ( Qt::Vertical == orientation )
    {
        // Display slide index in vertical header
        const int row = section;

        if ( row >= rowCount() )
        {
            return QVariant();
        }

        return QString::number( getModelRow( row ).first.m_index );
    }

    return QVariant();
}


Qt::DropActions SlideSorterTableModel::supportedDropActions() const
{
    return QAbstractItemModel::supportedDropActions() | Qt::MoveAction;
}


bool SlideSorterTableModel::insertSlide( int row, const std::pair<SlidePreview, QPixmap>& slideData )
{
    static const QVector<int> sk_rolesChanged{ Qt::DisplayRole };

    if ( row < 0 || rowCount() < row )
    {
        return false;
    }

    beginInsertRows( QModelIndex(), row, row );
    {
        // Move the slide to its new row
        auto it = std::next( std::begin( m_slideData ), row );
        m_slideData.insert( it, slideData );

        const QModelIndex startIndex = createIndex( row, 0 );
        const QModelIndex endIndex = createIndex( row, columnCount() - 1 );
        emit dataChanged( startIndex, endIndex, sk_rolesChanged );
    }
    endInsertRows();

    // Fix the slide indices
    reassignSlideIndices();

    return true;
}


bool SlideSorterTableModel::removeSlide( int row )
{
    if ( row < 0 || rowCount() <= row )
    {
        return false;
    }

//    beginRemoveRows( QModelIndex(), row, row );
    {
        auto beginIt = std::next( std::begin( m_slideData ), row );

        auto endIt = beginIt;
        std::advance( endIt, 1 );

        m_slideData.erase( beginIt, endIt );
    }
//    endRemoveRows();

    // Fix the slide indices
    reassignSlideIndices();

    return true;
}


void SlideSorterTableModel::reassignSlideIndices()
{
    static const QVector<int> sk_rolesChanged{ Qt::DisplayRole };

    size_t index = 0;

    for ( auto& slide : m_slideData )
    {
        if ( slide.first.m_index != index )
        {
            slide.first.m_index = index;

            const int row = static_cast<int>( index );
            QModelIndex startIndex = createIndex( row, 0 );
            QModelIndex endIndex = createIndex( row, columnCount() - 1 );
            emit dataChanged( startIndex, endIndex, sk_rolesChanged );
        }

        ++index;
    }
}


QStringList SlideSorterTableModel::mimeTypes() const
{
    static const QStringList types( "text/plain" );
    return types;
}


QMimeData* SlideSorterTableModel::mimeData( const QModelIndexList& indexes ) const
{
    if ( indexes.size() < 1 || ! indexes[0].isValid() )
    {
        return nullptr;
    }

    // Note: indexes should contain an element for each column of the row:
    // i.e. { (row, 0), (row, 1), (row, 2) }
    const int row = indexes[0].row();

    // Encode the row as text:
    QMimeData* mimeData = new QMimeData();
    mimeData->setText( QString::number( row ) );

    return mimeData;
}


bool SlideSorterTableModel::dropMimeData( const QMimeData* data, Qt::DropAction action,
                                          int destRow, int /*destColumn*/, const QModelIndex& parent )
{
    if ( Qt::IgnoreAction == action || ! data || ! data->hasText() )
    {
        return false;
    }

    // Decode the row from the MIME text
    const int srcRow = data->text().toInt();

    if ( srcRow < 0 || rowCount() <= srcRow )
    {
        return false;
    }

    if ( destRow < 0 || rowCount() < destRow )
    {
        destRow = rowCount();
    }

    if ( srcRow == destRow )
    {
        // Do nothing if source and destination row are the same
        return false;
    }

    insertSlide( destRow, getModelRow( srcRow ) );

    if ( srcRow < destRow )
    {
        // Moved slide upwards in stack
        removeSlide( srcRow );
//        emit dataMovedRows( destRow - 1 );
    }
    else if ( srcRow > destRow )
    {
        // Moved slide downwards in stack. Since a row was inserted below the source,
        // the row to delete is now srcRow + 1
        removeSlide( srcRow + 1 );
//        emit dataMovedRows( destRow );
    }

    emit dataOrderChanged( getSlideStackOrderedUids() );

    return QAbstractTableModel::dropMimeData( data, action, destRow, 0, parent );
}


QVariant SlideSorterTableModel::data( const QModelIndex& index, int role ) const
{
    if ( ! checkIndex( index ) )
    {
        return QVariant();
    }

    if ( index.row() >= rowCount() || index.column() >= columnCount() )
    {
        return QVariant(); // Invalid index
    }

    const auto& slideData = getModelRow( index.row() );
    const SlidePreview& slidePreview = slideData.first;
    const QPixmap& slideThumb = slideData.second;

    switch ( role )
    {
    case Qt::DisplayRole:
    {
        switch ( index.column() )
        {
        case SLIDE_IMAGE_COLUMN:
        {
            return "";
        }
        case SLIDE_OPACITY_COLUMN:
        {
            QString text( "%1" );
            return text.arg( slidePreview.m_opacity );
        }
        case SLIDE_ID_COLUMN:
        {
            return QString::fromStdString( slidePreview.m_name );
        }
//        case SLIDE_ANNOT_COLUMN:
//        {
//            QString text( "%1" );
//            return text.arg( slide.m_annotOpacity );
//        }
        }

        break;
    }
    case Qt::ToolTipRole:
    {
        switch ( index.column() )
        {
        case SLIDE_IMAGE_COLUMN:
        {
            return QString::fromStdString( slidePreview.m_name ) +
                    ( slidePreview.m_visible ? QString( " (visible)") : QString( " (hidden)" ) );
        }
        case SLIDE_OPACITY_COLUMN:
        {
            QString text( "Opacity: %1" );
            return text.arg( slidePreview.m_opacity );
        }
        case SLIDE_ID_COLUMN:
        {
            return QString::fromStdString( slidePreview.m_name ) +
                    ( slidePreview.m_visible ? QString( " (visible)") : QString( " (hidden)" ) );
        }
//        case SLIDE_ANNOT_COLUMN:
//        {
//            QString text( "%1" );
//            return text.arg( slide.m_annotOpacity );
//        }
        }

        break;
    }
    case Qt::EditRole:
    {
        switch ( index.column() )
        {
        case SLIDE_IMAGE_COLUMN:
        {
            return rgbToQColor( slidePreview.m_borderColor );
        }
        case SLIDE_OPACITY_COLUMN:
        {
            return slidePreview.m_opacity;
        }
        case SLIDE_ID_COLUMN:
        {
            return QString::fromStdString( slidePreview.m_name );
        }
//        case SLIDE_ANNOT_COLUMN:
//        {
//            return slide.m_annotOpacity;
//        }
        }

        break;
    }
    case Qt::BackgroundColorRole:
    {
        switch ( index.column() )
        {
        case SLIDE_IMAGE_COLUMN:
        {
            return rgbToQColor( slidePreview.m_borderColor );
        }
        }

        break;
    }
    case Qt::DecorationRole:
    {
        switch ( index.column() )
        {
        case SLIDE_IMAGE_COLUMN:
        {
            return slideThumb;
        }
        }

        break;
    }
    case Qt::CheckStateRole:
    {
        switch ( index.column() )
        {
        case SLIDE_OPACITY_COLUMN:
        {
            return ( slidePreview.m_visible ? Qt::Checked : Qt::Unchecked );
        }
//        case SLIDE_ANNOT_COLUMN:
//        {
//            return ( slide.m_annotVisible ? Qt::Checked : Qt::Unchecked );
//        }
        }

        break;
    }
    }

    return QVariant();
}


bool SlideSorterTableModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    if ( ! checkIndex( index ) )
    {
        return false;
    }

    if ( index.row() >= rowCount() || index.column() >= columnCount() )
    {
        return false; // Invalid index
    }


    QVector<int> rolesChanged;

    switch ( role )
    {
    case Qt::EditRole:
    {
        switch ( index.column() )
        {
        case SLIDE_OPACITY_COLUMN:
        {
            auto& slide = getModelRow( index.row() ).first;
            slide.m_opacity = value.toInt();

            rolesChanged << Qt::EditRole;
            emit dataChanged( index, index, rolesChanged );
            emit dataEdited( index.row() );

            return true;
        }
        case SLIDE_ID_COLUMN:
        {
            auto& slide = getModelRow( index.row() ).first;
            slide.m_name = value.toString().toStdString();

            rolesChanged << Qt::EditRole;
            emit dataChanged( index, index, rolesChanged );
            emit dataEdited( index.row() );

            return true;
        }
//        case SLIDE_ANNOT_COLUMN:
//        {
//            m_slideData.at( index.row() ).m_annotOpacity = value.toInt();

//            rolesChanged << Qt::EditRole;
//            emit dataChanged( index, index, rolesChanged );
//            emit dataEdited( index.row() );

//            return true;
//        }
        default:
        {
            return false;
        }
        }
    }
    case Qt::CheckStateRole:
    {
        switch ( index.column() )
        {
        case SLIDE_OPACITY_COLUMN: // Global slide visibility
        {
            const bool visible = value.toBool();

            auto& slide = getModelRow( index.row() ).first;
            slide.m_visible = visible;

            rolesChanged << Qt::CheckStateRole;
            emit dataChanged( index, index, rolesChanged );
            emit dataEdited( index.row() );

            return true;
        }
//        case SLIDE_ANNOT_COLUMN: // Slide annotation visibility
//        {
//            m_slideData.at( index.row() ).m_annotVisible = value.toBool();

//            rolesChanged << Qt::CheckStateRole;
//            emit dataChanged( index, index, rolesChanged );
//            emit dataEdited( index.row() );

//            return true;
//        }
        }

        break;
    }
    case Qt::BackgroundColorRole:
    {
        switch ( index.column() )
        {
        case SLIDE_IMAGE_COLUMN:
        {
            const QColor color = value.value<QColor>();
            if ( ! color.isValid() )
            {
                // Invalid color if user clicks "cancel" in QColorDialog
                return false;
            }

            double r, g, b;
            color.getRgbF( &r, &g, &b, nullptr );

            auto& slide = getModelRow( index.row() ).first;
            slide.m_borderColor = glm::vec3{ r, g, b };

            rolesChanged << Qt::BackgroundColorRole;
            emit dataChanged( index, index, rolesChanged );
            emit dataEdited( index.row() );

            return true;
        }
        }

        break;
    }
    }

    return false;
}


Qt::ItemFlags SlideSorterTableModel::flags( const QModelIndex& index ) const
{
    Qt::ItemFlags defaultFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable |
                                Qt::ItemIsEditable | Qt::ItemIsDragEnabled );

    if ( ! index.isValid() )
    {
        // Dropping is only enabled at non-valid indices.
        // (i.e. One cannot drop on valid indices of the model.)
        return defaultFlags | Qt::ItemIsDropEnabled;
    }

    if ( ! checkIndex( index ) )
    {
        return defaultFlags;
    }

    switch ( index.column() )
    {
    case SLIDE_IMAGE_COLUMN:
    {
        return defaultFlags;
    }
    case SLIDE_OPACITY_COLUMN:
    {
        return ( defaultFlags | Qt::ItemIsUserCheckable );
    }
    case SLIDE_ID_COLUMN:
    {
        return defaultFlags;
    }
//    case SLIDE_ANNOT_COLUMN:
//    {
//        // Annotation opacity
//        return defaultFlags;
//    }
    }

    return defaultFlags;
}


void SlideSorterTableModel::clearSlideStack()
{
    beginResetModel();
    {
        m_slideData.clear();
    }
    endResetModel();
}


void SlideSorterTableModel::setSlideStack( const std::list<SlidePreview>& slideStack )
{
    static const QVector<int> sk_rolesChanged {
        Qt::EditRole, Qt::DisplayRole, Qt::CheckStateRole, Qt::DecorationRole };

    // Clear old model data
    clearSlideStack();

    // Add new data to model. Each row of model consists of a SlidePreview object
    // and a QPixmap decorator image.
    for ( const auto& slidePreview : slideStack )
    {
        auto dims = slidePreview.m_thumbnailDims;
        auto buffer = slidePreview.m_thumbnailBuffer.lock();

        if ( ! buffer )
        {
            continue;
        }

        const QImage image( reinterpret_cast<unsigned char*>( buffer->data() ),
                      static_cast<int>( dims.x ), static_cast<int>( dims.y ),
                      QImage::Format_ARGB32_Premultiplied );

        m_slideData.emplace_back( std::make_pair( slidePreview, QPixmap::fromImage( image ) ) );
    }

    // Emit signal that all rows and columns were changed
    const QModelIndex topLeft = createIndex( 0, 0 );
    const QModelIndex bottomRight = createIndex( rowCount() - 1, columnCount() - 1 );

    emit dataChanged( topLeft, bottomRight, sk_rolesChanged );
}


bool SlideSorterTableModel::replaceSlide( SlidePreview slidePreview )
{
    static const QVector<int> sk_rolesChanged {
        Qt::EditRole, Qt::DisplayRole, Qt::CheckStateRole, Qt::DecorationRole };

    if ( m_slideData.size() <= slidePreview.m_index )
    {
        return false; // Invalid index
    }

    // Replace old slide with new slide at the row
    const int row = static_cast<int>( slidePreview.m_index );

    try
    {
        getModelRow( row ).first = std::move( slidePreview );
    }
    catch ( const std::exception& e )
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    // Emit signal that data was changed in row
    const QModelIndex startIndex = createIndex( row, 0 );
    const QModelIndex endIndex = createIndex( row, columnCount() - 1 );
    emit dataChanged( startIndex, endIndex, sk_rolesChanged );

    return true;
}


const SlidePreview& SlideSorterTableModel::getSlide( int row ) const
{
    if ( row < 0 || rowCount() <= row )
    {
        std::ostringstream ss;
        ss << "Invalid slide row " << row << std::ends;
        throw_debug( ss.str() );
    }

    return getModelRow( row ).first;
}


std::list<SlidePreview> SlideSorterTableModel::getSlideStack() const
{
    std::list<SlidePreview> slides;

    for ( const auto& slide : m_slideData )
    {
        slides.push_back( slide.first );
    }

    return slides;
}


std::list<UID> SlideSorterTableModel::getSlideStackOrderedUids() const
{
    std::list<UID> uids;

    for ( const auto& slide : m_slideData )
    {
        uids.push_back( slide.first.m_uid );
    }

    return uids;
}


const std::pair<SlidePreview, QPixmap>& SlideSorterTableModel::getModelRow( int row ) const
{
    if ( row < 0 || rowCount() <= row )
    {
        throw_debug( "Invalid row requested of SlideSorterTableModel" );
    }

    return *( std::next( std::begin( m_slideData ), row ) );
}


std::pair<SlidePreview, QPixmap>& SlideSorterTableModel::getModelRow( int row )
{
    if ( row < 0 || rowCount() <= row )
    {
        throw_debug( "Invalid row requested of SlideSorterTableModel" );
    }

    return *( std::next( std::begin( m_slideData ), row ) );
}


void SlideSorterTableModel::printOrderedSlideUids()
{
    std::cout << "Slide data: " << std::endl;
    for ( const auto& s : m_slideData )
    {
        std::cout << '\t' << s.first.m_uid << std::endl;
    }
    std::cout << std::endl;
}

} // namespace gui
