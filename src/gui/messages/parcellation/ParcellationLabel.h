#ifndef GUI_PARCELLATION_LABEL_H
#define GUI_PARCELLATION_LABEL_H

#include <glm/vec3.hpp>

#include <string>


namespace gui
{

/**
 * @brief A parcellation label, defined by a numerical value and view properties
 */
struct ParcellationLabel
{
    uint32_t m_index;   //!< Label index
    int64_t m_value;    //!< Label value
    std::string m_name; //!< Label name (as displayed in UI)
    glm::vec3 m_color;  //!< Non-pre-multiplied RGB color (as displayed in UI and rendered views)
    int m_alpha;        //!< Label color alpha value (opacity) in [0, 100]
    bool m_visible;     //!< Global label visibility in all rendered views on image planes
    bool m_showMesh;    //!< Global label mesh visibility in all rendered views
};


/**
 * @brief Hash function of a label that is just the label index.
 */
struct ParcellationLabelHasher
{
    size_t operator() ( const ParcellationLabel& l ) const
    {
        return l.m_index;
    }
};

/**
 * @brief Simple comparator of two labels based on their indices.
 */
struct ParcellationLabelComparator
{
    bool operator() ( const ParcellationLabel& l1, const ParcellationLabel& l2 ) const
    {
        return ( l1.m_index == l2.m_index );
    }
};

} // namespace gui


namespace std
{

/**
 * @brief Hash function in standard namespace for ParcellationLabel
 */
template<>
struct hash< gui::ParcellationLabel >
{
    size_t operator() ( const gui::ParcellationLabel& l ) const
    {
        return l.m_index;
    }
};

} // namespace std

#endif // GUI_PARCELLATION_LABEL_H
