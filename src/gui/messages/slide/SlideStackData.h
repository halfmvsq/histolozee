#ifndef GUI_MSG_SLIDE_STACK_DATA_H
#define GUI_MSG_SLIDE_STACK_DATA_H

#include "common/UID.h"

#include "gui/messages/slide/SlidePreview.h"

#include <glm/mat4x4.hpp>

#include <boost/optional.hpp>

#include <list>
#include <unordered_set>


namespace gui
{

/// A set of SlidePreview objects, with custom hash and equality comparator functions.
using SlidePreviewSet = std::unordered_set< SlidePreview, SlidePreviewHasher, SlidePreviewComparator >;


/**
 * @brief Message of previews of all ordered slides in the stack, sent from app to UI.
 * Slides are represented by the SlidePreview class.
 */
struct SlideStackComplete_msgToUi
{
    std::list<SlidePreview> m_slides; //!< All ordered slides in stack

    boost::optional<UID> m_activeSlideUid; //!< UID of active slide
    boost::optional<int> m_activeSlideIndex; //!< Index of active (selected) slide

    /// Affine transformation matrix mapping Slide Stack to World space.
    glm::dmat4 m_world_O_stack;
};

/**
 * @brief Message of previews of slides in the stack that have changed, sent from app to UI.
 * Slides are represented by the SlidePreview class.
 *
 * @note These are slides that have changed in the app. The container is not ordered.
 */
struct SlideStackPartial_msgToUi
{
    SlidePreviewSet m_slides; //!< Set of slides that were changed in the app

    /// Affine transformation matrix mapping Slide Stack to World space.
    boost::optional< glm::dmat4 > m_world_O_stack;
};

/**
 * @brief Message of previews of slides in the stack that have changed, sent from UI to app.
 * Slides are represented by the SlidePreview class.
 *
 * @note In the UI, the user can only set properties for a single slide at one time,
 * so the container of slides should contain only one element. The container is not ordered.
 */
struct SlideStackPartial_msgFromUi
{
    SlidePreviewSet m_slides; //!< Set of slides that were changed in the UI

    /// Flag to set the world_O_stack transformation to identity.
    /// (For use by the UI only.)
    boost::optional<bool> m_set_world_O_stack_identity;
};


/**
 * @brief Message of the active slide in the stack, sent from UI to app.
 * Need: The active slide can change based on the selected row of the slide sorter table.
 *
 * @note Both the slide UID and index are sent for redundancy. It should be checked
 * that the slide UID and index match up on the message receiver's side.
 */
struct ActiveSlide_msgFromUi
{
    boost::optional<UID> m_activeSlideUid; //!< UID of active slide
    boost::optional<int> m_activeSlideIndex = 0; //!< Index of active slide
};

/**
 * @brief Message of the active slide in the stack, sent from app to UI.
 * Need: The active slide can change in the application.
 *
 * @note Both the slide UID and index are sent for redundancy. It should be checked
 * that the slide UID and index match up on the message receiver's side.
 */
struct ActiveSlide_msgToUi
{
    boost::optional<UID> m_activeSlideUid; //!< UID of active slide
    boost::optional<int> m_activeSlideIndex = 0; //!< Index of active slide
};


/**
 * @brief Message of the slide stack order, sent from UI to app.
 * Need: The slide sorter table allows the user to change the order of slides.
 */
struct SlideStackOrder_msgFromUi
{
    std::list<UID> m_orderedSlideUids; //!< All ordered slide UIDs
};

} // namespace gui

#endif // GUI_MSG_SLIDE_STACK_DATA_H
