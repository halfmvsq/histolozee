#ifndef POINT_RECORD_H
#define POINT_RECORD_H

#include "common/UID.h"

#include <string>


/**
 * @brief Record for a point that represents a position in space.
 * The point has a unique ID.
 *
 * @tparam PositionType Position type
 */
template< class PositionType >
class PointRecord
{
public:

    /// Construct with a point from a position with an automatically generated unique ID
    explicit PointRecord( PositionType position )
        :
          m_name(),
          m_uid(),
          m_point( std::move( position ) ),
          m_visibility( true )
    {}

    ~PointRecord() = default;

    /// Set the point name
    void setName( std::string name )
    {
        m_name = std::move( name );
    }

    /// Get the point name
    const std::string& getName() const
    {
        return m_name;
    }

    /// Explicitly set the point's UID
    void setUid( UID uid ) const
    {
        m_uid = std::move( uid );
    }

    /// Get the point's UID
    UID uid() const
    {
        return m_uid;
    }

    /// Set position of the point
    void setPosition( PositionType position )
    {
        m_point = std::move( position );
    }

    /// Get the point's position
    PositionType getPosition() const
    {
        return m_point;
    }

    /// Set visibility of the point
    void setVisibility( bool visibility )
    {
        m_visibility = std::move( visibility );
    }

    /// Get the point's visibility
    PositionType getVisibility() const
    {
        return m_visibility;
    }


private:

    std::string m_name; //!< Name of point
    UID m_uid; //!< Unique ID of point
    PositionType m_point; //!< Point position
    bool m_visibility; //!< Point visibility
};

#endif // POINT_RECORD_H
