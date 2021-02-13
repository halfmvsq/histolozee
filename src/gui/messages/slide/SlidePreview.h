#ifndef GUI_MSG_SLIDE_PREVIEW_H
#define GUI_MSG_SLIDE_PREVIEW_H

#include "common/UID.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_precision.hpp>

#include <memory>
#include <string>
#include <vector>


namespace gui
{

/**
 * @brief Preview of a slide that is shown in the Slide Stack Sorter table.
 * The preview shows a few properties, including slide index, name, border color,
 * visibility/opacity, annotation visibility/opacity, and thumbnail.
 */
struct SlidePreview
{
    UID m_uid; //!< Slide UID

    size_t m_index = 0; //!< Slide index in stack (corresponds to the row of Slide Sorter table)
    std::string m_name; //!< Slide display name (referred to as "Slide ID" in the UI)

    glm::vec3 m_borderColor = glm::vec3{ 1.0f }; //!< Slide border color (non-pre-multiplied RGB)

    bool m_visible = true; //!< Global slide visibility
    bool m_annotVisible = true; //!< Slide annotation visibility

    int m_opacity = 100; //! Slide opacity in range [0, 100]
    int m_annotOpacity = 100; //!< Slide annotation opacity in range [0, 100]

    /// Buffer for thumbnail image in pre-multiplied ARGB format
    /// (i.e. Qt's format \c QImage::Format_ARGB32_Premultiplied; 0xAARRGGBB)
    std::weak_ptr< std::vector<uint32_t> > m_thumbnailBuffer = {};

    glm::i64vec2 m_thumbnailDims = { 0, 0 }; //!< Thumbnail image dimensions
};


/**
 * @brief Hash function of a SlidePreview that is just the UID.
 */
struct SlidePreviewHasher
{
    size_t operator() ( const SlidePreview& s ) const
    {
        return s.m_uid.hash();
    }
};


/**
 * @brief Simple comparator of two SlidePreview objects based on their indices.
 */
struct SlidePreviewComparator
{
    bool operator() ( const SlidePreview& s1, const SlidePreview& s2 ) const
    {
        return ( s1.m_index == s2.m_index );
    }
};


} // namespace gui


namespace std
{

/**
 * @brief Hash function in std namespace for SlidePreview
 */
template<>
struct hash< gui::SlidePreview >
{
    size_t operator() ( const gui::SlidePreview& s ) const
    {
        return hash<UID>()( s.m_uid );
    }
};

} // namespace std

#endif // GUI_MSG_SLIDE_PREVIEW_H
