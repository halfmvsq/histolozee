#ifndef COORDFRAME_LINKING_TYPE_H
#define COORDFRAME_LINKING_TYPE_H

/// Type of CoordinateFrame to which a camera start frame parameter can be linked
enum class LinkedFrameType
{
    Crosshairs, //!< Linked to the crosshairs frame
    SlideStack, //!< Linked to the slide stack frame
    None        //!< Not linked to any frame
};

#endif // COORDFRAME_LINKING_TYPE_H
