#ifndef GUI_IMAGE_PROPERTY_DATA_H
#define GUI_IMAGE_PROPERTY_DATA_H

#include "common/Identity.h"
#include "common/UID.h"

#include <optional>
#include <string>
#include <utility>


namespace gui
{

/**
 * @brief Per-image property data for the UI.
 * Template type V can be either std::optional (to denote all optional fields) or
 * Required (to denote all required fields).
 */
template< template<typename> class V >
struct ImageProperties
{
    /// Flag indicating whether the image was loaded from a file (true)
    /// or created prorammatically in the app (false)
    V<bool> m_loadedFromFile;

    V<std::string> m_path; //!< File path
    V<std::string> m_displayName; //!< Display name
    V<int> m_colorMapIndex; //!< Color map selection index (into color map combo box)

    V< std::pair<int, int> > m_opacityRange; //!< Range of opacity slider and spin box
    V<int> m_opacitySingleStep; //!< Single-step value of opacity slider and spin box
    V<int> m_opacitySliderPageStep; //!< Page-step value of opacity slider

    V< std::pair<double, double> > m_windowRange; //!< Range of windowing slider and spin boxes
    V<double> m_windowSingleStep; //!< Single-step value of windowing slider and spin boxes
    V<int> m_windowSpinBoxesDecimals; //!< No. decimals of precision for windowing spin boxes

    V< std::pair<double, double> > m_threshRange; //!< Range of thresholding slider and spin boxes
    V<double> m_threshSingleStep; //!< Single-step value of thresholding slider and spin boxes
    V<int> m_threshSpinBoxesDecimals; //!< No. decimals of thresholding spin boxes

    V<int> m_opacityValue; //!< Value of opacity slider and spin box
    V< std::pair<double, double> > m_windowValues; //!< Min/max values of window range slider and spin boxes
    V< std::pair<double, double> > m_threshValues; //!< Low/high values of thresholding range slider and spin boxes

    V<bool> m_samplingNnChecked; //!< Checked state of NN sampling radio button
    V<bool> m_samplingLinearChecked; //!< Checked state of linear sampling radio button
};


/**
 * @brief Properties that apply to all image slices.
 */
template< template<typename> class V >
struct ImageSliceCommonProperties
{
    V<bool> m_planesVisibleIn2dViewsChecked; //!< Checked state of image slices visibility in 2D views check box
    V<bool> m_planesVisibleIn3dViewsChecked; //!< Checked state of image slices visibility in 3D views check box
    V<bool> m_planesAutoHidingChecked; //!< Checked state of image slices auto-hiding check box
};


/**
 * @brief Message of some widget properties of currently active image: sent from app to UI.
 * All property fields are optional. When received by the UI, the defined (non-std::nullopt)
 * properties are overwritten in the UI.
 *
 * @note Identical message structure to \c ImagePropertiesPartial_msgFromUi
 */
struct ImagePropertiesPartial_msgToUi
{
    UID m_imageUid; //!< Image UID
    ImageProperties<std::optional> m_properties; //!< Image properties (all optional)
    ImageSliceCommonProperties<std::optional> m_commonProperties; //!< Common properties (all optional)
};


/**
 * @brief Message of some widget properties of currently active image: sent from UI to app.
 * All property fields are optional.
 *
 * @note Identical message structure to \c ImagePropertiesPartial_msgToUi
 */
struct ImagePropertiesPartial_msgFromUi
{
    UID m_imageUid; //!< Image UID
    ImageProperties<std::optional> m_properties; //!< Image properties (all optional)
    ImageSliceCommonProperties<std::optional> m_commonProperties; //!< Common properties (all optional)
};


/**
 * @brief Message of all widget properties of currently active image: sent from app to UI.
 * All property fields are mandatory and must be set.
 *
 * @note Identical message structure to \c ImagePropertiesFullMsg_fromUI
 */
struct ImagePropertiesComplete_msgToUi
{
    UID m_imageUid; //!< Image UID
    ImageProperties<Required> m_properties; //!< Image properties (all mandatory)
    ImageSliceCommonProperties<Required> m_commonProperties; //!< Common properties (all mandatory)
};


/**
 * @brief Message of all widget properties of current image: sent from UI to app.
 * All property fields are mandatory and must be set.
 *
 * @note Identical message structure to \c ImageProperImagePropertiesFull_msgToUitiesFullMsg_toUI
 */
struct ImagePropertiesComplete_msgFromUi
{
    UID m_imageUid; //!< Image UID
    ImageProperties<Required> m_properties; //!< Image properties (all mandatory)
    ImageSliceCommonProperties<Required> m_commonProperties; //!< Common properties (all mandatory)
};

} // namespace gui

#endif // GUI_IMAGE_PROPERTY_DATA_H
