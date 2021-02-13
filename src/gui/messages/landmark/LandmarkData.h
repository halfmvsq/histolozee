#ifndef GUI_LANDMARK_DATA_H
#define GUI_LANDMARK_DATA_H

#include "common/UID.h"

#include <glm/vec3.hpp>

#include <string>


namespace gui
{

/**
 * @brief A single landmark for either a reference image or a slide.
 * The landmark has both a unique ID and an index that indicates is position
 * within an ordered group of landmarks.
 */
struct Landmark
{
    UID m_uid; //!< Unique ID of the landmark
    uint32_t m_index; //!< Index of the landmark for ordering it in its group
    std::string m_name; //!< Landmark name
    glm::dvec3 m_position; //!< Landmark position
    bool m_visible; //!< Landmark visibility
};


/**
 * @brief Hash function of a landmark that is just the landmark UID.
 */
struct LandmarkHasher
{
    size_t operator() ( const Landmark& l ) const
    {
        return l.m_uid.hash();
    }
};

/**
 * @brief Simple comparator of two landmarks based on their UIDs.
 */
struct LandmarkComparator
{
    bool operator() ( const Landmark& l1, const Landmark& l2 ) const
    {
        return ( l1.m_uid == l2.m_uid );
    }
};

} // namespace gui


namespace std
{

/**
 * @brief Hash function in standard namespace for a Landmark.
 */
template<>
struct hash< gui::LandmarkComparator >
{
    size_t operator() ( const gui::Landmark& l ) const
    {
        return l.m_uid.hash();
    }
};

} // namespace std

#endif // GUI_LANDMARK_DATA_H
