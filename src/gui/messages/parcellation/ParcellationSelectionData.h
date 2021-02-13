#ifndef GUI_PARCELLATION_SELECTION_DATA_H
#define GUI_PARCELLATION_SELECTION_DATA_H

#include "common/UID.h"

#include <optional>
#include <string>
#include <vector>


namespace gui
{

/**
 * @brief A single parcellation selection item
 */
struct ParcellationSelectionItem
{
    UID m_parcelUid; //!< Parcellation UID
    std::string m_displayName; //!< Name to display
};


/**
 * @brief Message of parcellation selections sent from app to UI.
 * When received by the UI, this replaces all existing parcellation selection items in the UI.
 */
struct ParcellationSelections_msgToUi
{
    std::vector< ParcellationSelectionItem > m_selectionItems; //!< List of selectable parcellations
    std::optional<int> m_selectionIndex; //!< Index of currently selected (active) parcellation
};


/**
 * @brief Message of parcellation selection data sent from UI to app
 */
struct ParcellationSelections_msgFromUi
{
    std::optional<int> m_selectionIndex; //!< Index of currently selected (active) parcellation in combo box
    std::optional<UID> m_parcelUid; //!< UID of currently selected (active) parcellation
};

} // namespace gui

#endif // GUI_PARCELLATION_SELECTION_DATA_H
