#ifndef GUI_PARCELLATION_PROPERTIES_H
#define GUI_PARCELLATION_PROPERTIES_H

#include "common/Identity.h"
#include "common/UID.h"

#include <boost/optional.hpp>

#include <string>
#include <utility>


namespace gui
{

/**
 * @brief Properties of the widgets that control the parcellation in the UI.
 * Template type V can be either boost::optional (to denote all optional fields) or
 * Required (to denote all required fields).
 */
template< template<typename> class V >
struct ParcellationProperties
{
    /// Flag indicating whether the parcellation was loaded from a file (true)
    /// or created prorammatically in the app (false)
    V<bool> m_loadedFromFile;

    V<std::string> m_path; //!< Parcellation file path
    V<std::string> m_displayName; //!< Display name

    V<bool> m_visibleIn2dViewsChecked; //!< Checked state of parcellation visibility in 2D views check box
    V<bool> m_visibleIn3dViewsChecked; //!< Checked state of parcellation visibility in 3D views check box

    V< std::pair<int, int> > m_opacityRange; //!< Allowable range of opacity slider and spin box
    V<int> m_opacitySingleStep; //!< Single-step value of opacity slider and spin box
    V<int> m_opacitySliderPageStep; //!< Page-step value of opacity slider
    V<int> m_opacityValue; //!< Current value of opacity slider and spin box
};


/**
 * @brief Properties that apply to all parcellation label meshes.
 */
template< template<typename> class V >
struct LabelMeshCommonProperties
{
    V<bool> m_meshesVisibleIn2dViews; //!< Label mesh visibility in 2D views
    V<bool> m_meshesVisibleIn3dViews; //!< Label mesh visibility in 3D views

    V< std::pair<int, int> > m_meshOpacityRange; //!< Allowable range of mesh opacity slider and spin box
    V<int> m_meshOpacitySingleStep; //!< Single-step value of mesh opacity slider and spin box
    V<int> m_meshOpacitySliderPageStep; //!< Page-step value of mesh opacity slider
    V<int> m_meshOpacityValue; //!< Current value of mesh opacity slider and spin box

    V<bool> m_meshesXrayModeChecked; //!< Checked state of x-ray mode checked box

    V< std::pair<double, double> > m_meshXrayPowerRange; //!< Allowable range of mesh x-ray power spin box
    V<double> m_meshXrayPowerSingleStep; //!< Single-step value of mesh x-ray power and spin box
    V<int> m_meshXrayPowerSpinBoxDecimals; //!< No. decimals of precision for mesh x-ray power spin box
    V<double> m_meshXrayPowerValue; //!< Current value of mesh x-ray power spin box
};


/**
 * @brief Message of some widget properties for the currently active parcellation:
 * sent from app to UI. All property fields are optional. When received by the UI,
 * the defined properties are overwritten in the UI.
 *
 * @note Identical message structure to \c ParcellationPropertiesPartialMsg_fromUI
 */
struct ParcellationPropertiesPartial_msgToUi
{
    UID m_parcelUid; //!< Parcellation UID
    ParcellationProperties<boost::optional> m_properties; //!< Parcellation properties
    LabelMeshCommonProperties<boost::optional> m_meshProperties; //!< Label mesh common properties
};


/**
 * @brief Message of some widget properties for the currently active parcellation:
 * sent from UI to app. All property fields are optional.
 *
 * @note Identical message structure to \c ParcellationPropertiesPartial_msgToUi
 */
struct ParcellationPropertiesPartial_msgFromUi
{
    UID m_parcelUid; //!< Parcellation UID
    ParcellationProperties<boost::optional> m_properties; //!< Parcellation properties
    LabelMeshCommonProperties<boost::optional> m_meshProperties; //!< Label mesh common properties
};


/**
 * @brief Message of complete parcellation widget properties sent from app to UI.
 * All property fields are mandatory and must be set.
 *
 * @note Identical message structure to \c ParcellationPropertiesComplete_msgFromUi
 */
struct ParcellationPropertiesComplete_msgToUi
{
    UID m_parcelUid; //!< Parcellation UID
    ParcellationProperties<Required> m_properties; //!< Parcellation properties
    LabelMeshCommonProperties<Required> m_meshProperties; //!< Label mesh common properties
};


/**
 * @brief Message of complete parcellation widget properties for the current image:
 * sent from UI to app. All properties are mandatory.
 *
 * @note Identical message structure to \c ParcellationPropertiesComplete_msgToUi
 */
struct ParcellationPropertiesComplete_msgFromUi
{
    UID m_parcelUid; //!< Parcellation UID
    ParcellationProperties<Required> m_properties; //!< Parcellation properties
    LabelMeshCommonProperties<Required> m_meshProperties; //!< Label mesh common properties
};

} // namespace gui

#endif // GUI_PARCELLATION_PROPERTIES_H
