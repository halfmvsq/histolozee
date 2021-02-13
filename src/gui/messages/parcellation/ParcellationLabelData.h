#ifndef GUI_PARCELLATION_LABEL_DATA_H
#define GUI_PARCELLATION_LABEL_DATA_H

#include "common/UID.h"
#include "gui/messages/parcellation/ParcellationLabel.h"

#include <unordered_set>
#include <vector>


namespace gui
{

/**
 * @brief Message of complete labels for the currently active parcellation,
 * sent from app to UI. The labels in the vector are ordered by label index.
 */
struct ParcellationLabelsComplete_msgToUi
{
    UID m_labelTableUid; //!< Label table UID
    std::vector<ParcellationLabel> m_labels; //!< All parcellation labels, ordered by index
};


/**
 * @brief Message of some labels for the label table of the currently active parcellation,
 * sent from UI to app.
 *
 * @note In the UI, the user can only set properties for a single label at one time,
 * so the list of container should contain only one element. The container is not ordered.
 */
struct ParcellationLabelsPartial_msgFromUi
{
    UID m_labelTableUid; //!< Label table UID

    /// Labels changed in UI, not ordered
    std::unordered_set< ParcellationLabel, ParcellationLabelHasher, ParcellationLabelComparator > m_labels;
};

} // namespace gui

#endif // GUI_PARCELLATION_LABEL_DATA_H
