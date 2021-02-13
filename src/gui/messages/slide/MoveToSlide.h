#ifndef GUI_MOVE_TO_SLIDE_H
#define GUI_MOVE_TO_SLIDE_H

#include "common/UID.h"


namespace gui
{

/**
 * @brief Message to move crosshairs to a slide in the stack, sent from UI to app.
 *
 * @note Both the slide UID and index are sent for redundancy. It should be checked
 * that the slide UID and index match up on the message receiver's side.
 */
struct MoveToSlide_msgFromUi
{
    UID m_slideUid; //!< UID of slide
    int m_slideIndex; //!< Index of slide
};

} // namespace gui

#endif // GUI_MOVE_TO_SLIDE_H
