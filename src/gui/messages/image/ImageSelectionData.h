#ifndef GUI_IMAGE_SELECTION_DATA_H
#define GUI_IMAGE_SELECTION_DATA_H

#include "common/UID.h"

#include <boost/optional.hpp>

#include <string>
#include <vector>


namespace gui
{

/**
 * @brief A single image selection item
 */
struct ImageSelectionItem
{
    UID m_imageUid; //!< Image UID
    std::string m_displayName; //!< Display name of image
};


/**
 * @brief Message of image selection data sent from app to UI.
 * When received by the UI, this replaces all existing image selection items in the UI.
 */
struct ImageSelections_msgToUi
{
    std::vector<ImageSelectionItem> m_selectionItems; //!< List of selectable images
    boost::optional<int> m_selectionIndex; //!< Index of currently selected (active) image
};


/**
 * @brief Message of image selection data sent from UI to app.
 */
struct ImageSelections_msgFromUi
{
    boost::optional<int> m_selectionIndex; //!< Index of currently selected (active) image in combo box
    boost::optional<UID> m_imageUid; //!< UID of currently selected (active) image
};

} // namespace gui

#endif // GUI_IMAGE_SELECTION_DATA_H
