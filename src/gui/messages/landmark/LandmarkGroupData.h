#ifndef GUI_REF_IMAGE_LANDMARK_DATA_H
#define GUI_REF_IMAGE_LANDMARK_DATA_H

#include "common/Identity.h"
#include "common/UID.h"

#include "gui/messages/landmark/LandmarkData.h"

#include <boost/optional.hpp>

#include <glm/vec3.hpp>

#include <string>
#include <unordered_set>
#include <vector>


namespace gui
{

/**
 * @brief An ordered group of landmarks for either a reference image or a slide
 */
struct LandmarkGroup
{
    UID m_uid; //!< Unique ID of the group
    uint32_t m_index; //!< Index of the group
    std::string m_name; //!< Landmark group name
    bool m_visible; //!< Group visibility (applies to all of its landmarks)
    std::vector<Landmark> m_landmarks; //!< Ordered landmarks in the group
};


/**
 * @brief Message of the complete collection of landmark groups (for either a reference image
 * or a slide image), sent from app to UI. When received by the UI, this replaces all existing
 * landmarks in the UI for the image.
 */
struct LandmarkGroupsComplete_msgToUi
{
    /// Image UID to which landmarks belong
    UID m_imageUid;

    /// Ordered groups of landmarks
    std::vector<LandmarkGroup> m_landmarkGroups;
};


/**
 * @brief Message of the complete collection of landmark groups (for either a reference image
 * or a slide image), sent from UI to app. When received by the app, this replaces all existing
 * landmarks in the app for the image.
 */
using LandmarkGroupsComplete_msgFromUi = LandmarkGroupsComplete_msgToUi;


/**
 * @brief Message of some landmark groups (for either a reference image or slide image),
 * sent from UI to app. When received by the app, this replaces all landmarks in the set
 * for the image.
 */
struct LandmarksPartial_msgFromUi
{
    /// Image UID to which landmarks belong
    UID m_refImageUid;

    /// Un-ordered landmarks
    std::unordered_set< Landmark, LandmarkHasher, LandmarkComparator > m_landmarks;
};


/**
 * @brief Message of some landmark groups (for either a reference image or slide image),
 * sent from app to UI. When received by the app, this replaces all landmarks in the set
 * for the image.
 */
using LandmarksPartial_msgToUi = LandmarksPartial_msgFromUi;

} // namespace gui

#endif // GUI_REF_IMAGE_LANDMARK_DATA_H
