#include "gui/docks/labels/LabelTableModel.h"

#include <glm/glm.hpp>

#include <QColor>

#include <string>


namespace
{

// Column heading names
static const std::vector< std::string > sk_columns = {
    "Value",    /* Column 0: Label value, color, and global visibility */
    "Color",    /* Column 1: Label RGB color  */
    "Opacity",  /* Column 2: Label alpha value (opacity) */
    "Mesh",     /* Column 3: Label mesh visibility */
    "Name"      /* Column 4: Label name */
};


/**
 * @brief Convert RGB color stored as \c glm::vec3 to QColor
 * @param color RGB color as \c glm::vec3
 * @return Color in Qt's QColor format
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

LabelTableModel::LabelTableModel( QObject* parent )
    :
      QAbstractTableModel( parent ),
      m_labelData(),
      m_blockDataEditedSignal( false )
{}


int LabelTableModel::rowCount( const QModelIndex& /*parent*/ ) const
{
    return static_cast<int>( m_labelData.size() );
}


int LabelTableModel::columnCount( const QModelIndex& /*parent*/ ) const
{
    return static_cast<int>( sk_columns.size() );
}


QVariant LabelTableModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( section < 0 )
    {
        return QVariant(); // Invalid section
    }

    if ( Qt::DisplayRole == role && Qt::Horizontal == orientation )
    {
        if ( columnCount() <= section )
        {
            return QVariant(); // Invalid section
        }

        return QString::fromStdString( sk_columns.at( static_cast<size_t>( section ) ) );
    }

    return QVariant();
}


QVariant LabelTableModel::data( const QModelIndex& index, int role ) const
{   
    if ( ! checkIndex( index ) )
    {
        return QVariant();
    }

    if ( index.row() < 0 || rowCount() <= index.row() ||
         index.column() < 0 || columnCount() <= index.column() )
    {
        return QVariant(); // Invalid index
    }

    const ParcellationLabel* label = getLabel( index.row() );
    if ( ! label )
    {
        return QVariant();
    }

    switch ( role )
    {
    case Qt::DisplayRole:
    {
        switch ( index.column() )
        {
        case LABEL_VALUE_COLUMN:
        {
            return QString::number( label->m_value );
        }
        case LABEL_NAME_COLUMN:
        {
            return QString::fromStdString( label->m_name );
        }
        case LABEL_COLOR_COLUMN:
        {
            return "";
        }
        case LABEL_ALPHA_COLUMN:
        {
            QString text( "%1" );
            return text.arg( label->m_alpha );
        }
        case LABEL_MESH_VISIBILITY_COLUMN:
        {
            return "";
        }
        }

        break;
    }
    case Qt::ToolTipRole:
    {
        switch ( index.column() )
        {
        /// @todo Set sensible Tool Tips
        case LABEL_VALUE_COLUMN:
        {
            return QString( "Label no. " ) + QString::number( label->m_value );
        }
        case LABEL_NAME_COLUMN:
        {
            return QString::fromStdString( label->m_name );
        }
        case LABEL_COLOR_COLUMN:
        {
            return rgbToQColor( label->m_color );
        }
        }

        break;
    }
    case Qt::EditRole:
    {
        switch ( index.column() )
        {
        case LABEL_VALUE_COLUMN: return QString::number( label->m_value );
        case LABEL_NAME_COLUMN: return QString::fromStdString( label->m_name );
        case LABEL_COLOR_COLUMN: return rgbToQColor( label->m_color );
        case LABEL_ALPHA_COLUMN: return label->m_alpha;
        case LABEL_MESH_VISIBILITY_COLUMN: return "";
        }

        break;
    }
    case Qt::BackgroundColorRole:
    {
        switch ( index.column() )
        {
        case LABEL_COLOR_COLUMN: return rgbToQColor( label->m_color );
        }

        break;
    }
    case Qt::DecorationRole:
    {
        switch ( index.column() )
        {
        // Decorate the label value section with the label color
        case LABEL_VALUE_COLUMN: return rgbToQColor( label->m_color );
        }

        break;
    }
    case Qt::CheckStateRole:
    {
        switch ( index.column() )
        {
        case LABEL_VALUE_COLUMN:
        {
            // Global label visibility in both 2D and 3D views:
            return ( label->m_visible ? Qt::Checked : Qt::Unchecked );
        }
        case LABEL_MESH_VISIBILITY_COLUMN:
        {
            // Label mesh visibility:
            return ( label->m_showMesh ? Qt::Checked : Qt::Unchecked );
        }
        }

        break;
    }
    case Qt::TextAlignmentRole:
    {
        switch ( index.column() )
        {
        case LABEL_MESH_VISIBILITY_COLUMN: return Qt::AlignCenter; // Show mesh
        }

        break;
    }
    }

    return QVariant();
}


bool LabelTableModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    if ( ! checkIndex( index ) )
    {
        return false;
    }

    if ( index.row() < 0 || rowCount() <= index.row() ||
         index.column() < 0 || columnCount() <= index.column() )
    {
        return false; // Invalid index
    }

    ParcellationLabel* label = getModelRow( index.row() );
    if ( ! label )
    {
        return false;
    }


    QVector<int> rolesChanged;

    switch ( role )
    {
    case Qt::EditRole:
    {
        switch ( index.column() )
        {
        case LABEL_NAME_COLUMN:
        {
            label->m_name = value.toString().toStdString();
            rolesChanged << Qt::EditRole;
            emit dataChanged( index, index, rolesChanged );

            if ( ! m_blockDataEditedSignal )
            {
                emit dataEdited( { index.row() } );
            }

            return true;
        }
        case LABEL_COLOR_COLUMN:
        {
            const QColor color = value.value<QColor>();
            if ( ! color.isValid() )
            {
                // Invalid color if user clicks "cancel" in QColorDialog
                return false;
            }

            double r, g, b;
            color.getRgbF( &r, &g, &b, nullptr );
            label->m_color.r = static_cast<float>( r );
            label->m_color.g = static_cast<float>( g );
            label->m_color.b = static_cast<float>( b );

            rolesChanged << Qt::EditRole;
            emit dataChanged( index, index, rolesChanged );

            if ( ! m_blockDataEditedSignal )
            {
                emit dataEdited( { index.row() } );
            }

            return true;
        }
        case LABEL_ALPHA_COLUMN:
        {
            label->m_alpha = value.toInt();

            rolesChanged << Qt::EditRole;
            emit dataChanged( index, index, rolesChanged );

            if ( ! m_blockDataEditedSignal )
            {
                emit dataEdited( { index.row() } );
            }

            return true;
        }
        }

        break;
    }
    case Qt::DecorationRole:
    {
        switch ( index.column() )
        {
        case LABEL_VALUE_COLUMN:
        {
            const QColor color = value.value<QColor>();
            if ( ! color.isValid() )
            {
                // Invalid color if user clicks "cancel" in QColorDialog
                return false;
            }

            double r, g, b;
            color.getRgbF( &r, &g, &b, nullptr );
            label->m_color.r = static_cast<float>( r );
            label->m_color.g = static_cast<float>( g );
            label->m_color.b = static_cast<float>( b );

            rolesChanged << Qt::DecorationRole;
            emit dataChanged( index, index, rolesChanged );

            if ( ! m_blockDataEditedSignal )
            {
                emit dataEdited( { index.row() } );
            }

            return true;
        }
        }

        break;
    }
    case Qt::CheckStateRole:
    {
        switch ( index.column() )
        {
        case LABEL_VALUE_COLUMN:
        {
            // Set global label visibility:
            const bool visible = value.toBool();
            label->m_visible = visible;

            // Modulate visibility of label mesh by the global visibility:
            label->m_showMesh &= visible;

            rolesChanged << Qt::CheckStateRole;
            emit dataChanged( index, index, rolesChanged );

            // Signal that we also changed mesh visibility:
            const QModelIndex meshVisIndex = createIndex( index.row(), LABEL_MESH_VISIBILITY_COLUMN );
            emit dataChanged( meshVisIndex, meshVisIndex, rolesChanged );

            if ( ! m_blockDataEditedSignal )
            {
                emit dataEdited( { index.row() } );
            }

            return true;
        }
        case LABEL_MESH_VISIBILITY_COLUMN:
        {
            if ( 0 == label->m_value )
            {
                // Never allow the mesh at for label value 0 to be changed
                break;
            }

            label->m_showMesh = value.toBool();

            rolesChanged << Qt::CheckStateRole;
            emit dataChanged( index, index, rolesChanged );

            if ( ! m_blockDataEditedSignal )
            {
                emit dataEdited( { index.row() } );
            }

            return true;
        }
        }

        break;
    }
    }

    return false;
}


Qt::ItemFlags LabelTableModel::flags( const QModelIndex& index ) const
{
    static const Qt::ItemFlags defaultFlags = Qt::ItemFlags( Qt::ItemIsEnabled ) & ( ~Qt::ItemIsEditable );

    switch ( index.column() )
    {
    case LABEL_VALUE_COLUMN: return ( defaultFlags | Qt::ItemIsUserCheckable | Qt::ItemIsEditable );
    case LABEL_NAME_COLUMN: return ( defaultFlags | Qt::ItemIsEditable );
    case LABEL_COLOR_COLUMN: return ( defaultFlags | Qt::ItemIsEditable );
    case LABEL_ALPHA_COLUMN: return ( defaultFlags | Qt::ItemIsEditable );
    case LABEL_MESH_VISIBILITY_COLUMN:
    {
        if ( 0 == index.row() )
        {
            // Row 0 corresponds to the the background label index, which is not meshable
            return ( defaultFlags & ( ~Qt::ItemIsEnabled ) );
        }
        else
        {
            return ( defaultFlags | Qt::ItemIsUserCheckable );
        }
    }
    }

    return defaultFlags;
}


bool LabelTableModel::setData( const std::vector<QModelIndex>& indices, const QVariant& value, int role )
{
    std::vector<int> rowsEdited;

    m_blockDataEditedSignal = true;

    for ( const QModelIndex& index : indices )
    {
        if ( setData( index, value, role ) )
        {
            rowsEdited.push_back( index.row() );
        }
    }

    m_blockDataEditedSignal = false;

    if ( ! rowsEdited.empty() )
    {
        emit dataEdited( rowsEdited );
        return true;
    }
    else
    {
        return false;
    }
}


void LabelTableModel::setAllLabels( std::vector<ParcellationLabel> labels )
{
    static const QVector<int> rolesChanged{
        Qt::EditRole, Qt::DisplayRole, Qt::CheckStateRole,
                Qt::BackgroundColorRole, Qt::DecorationRole };

    // Reset old data and replace with new data
    clearAllLabels();
    m_labelData = std::move( labels );

    // Emit signal that all rows and columns were changed
    const QModelIndex topLeft = createIndex( 0, 0 );
    const QModelIndex bottomRight = createIndex( rowCount() - 1, columnCount() - 1 );

    emit dataChanged( topLeft, bottomRight, rolesChanged );
}


void LabelTableModel::clearAllLabels()
{
    beginResetModel();
    {
        m_labelData.clear();
    }
    endResetModel();
}


const ParcellationLabel* LabelTableModel::getLabel( int row ) const
{
    if ( row < 0 || rowCount() <= row )
    {
        return nullptr; // Invalid row
    }

    return &( m_labelData.at( static_cast<size_t>( row ) ) );
}


ParcellationLabel* LabelTableModel::getModelRow( int row )
{
    if ( row < 0 || rowCount() <= row )
    {
        return nullptr; // Invalid row
    }

    return &( m_labelData.at( static_cast<size_t>( row ) ) );
}


const std::vector<ParcellationLabel>& LabelTableModel::getAllLabels() const
{
    return m_labelData;
}

} // namespace gui
