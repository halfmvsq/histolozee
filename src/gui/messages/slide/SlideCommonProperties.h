#ifndef GUI_MSG_SLIDE_COMMON_PROPERTY_DATA_H
#define GUI_MSG_SLIDE_COMMON_PROPERTY_DATA_H

#include "common/Identity.h"

#include <boost/optional.hpp>

#include <utility>


namespace gui
{

/**
* @brief Rendering properties common to all slides of the slide stack.
 * Template type V can be either boost::optional (to denote all optional fields) or
 * Required (to denote all required fields).
 */
template< template<typename> class V >
struct SlideCommonProperties
{
    V< std::pair<int, int> > m_masterOpacityRange; //!< Range of master opacity slider and spin box
    V<int> m_masterOpacitySingleStep; //!< Single-step of master opacity slider and spin box
    V<int> m_masterOpacitySliderPageStep; //!< Page-step of master opacity slider
    V<int> m_masterOpacityValue; //!< Value of master opacity slider and spin box

    V< std::pair<int, int> > m_image3dOpacityRange; //!< Range of image 3D opacity slider and spin box
    V<int> m_image3dOpacitySingleStep; //!< Single-step of image 3D opacity slider and spin box
    V<int> m_image3dOpacitySliderPageStep; //!< Page-step of image 3D opacity slider
    V<int> m_image3dOpacityValue; //!< Value of image 3D opacity slider and spin box

    V<bool> m_stackVisibleIn2dViewsChecked; //!< Checked state of stack visibility in 2D views
    V<bool> m_stackVisibleIn3dViewsChecked; //!< Checked state of stack visibility in 3D views

    /// Checked state of checkbox controlling whether the Slide Stack view shows slides as 2D or 3D
    V<bool> m_activeSlideViewShows2dSlidesChecked;

    /// Checked state of toggling pushbutton controlling whether the Slide Stack view shows the
    /// slide stack from top to bottom or vice-versa
    V<bool> m_activeSlideViewDirectionTopToBottomChecked;
};


/**
 * @brief Message of some slide stack rendering properties: sent from app to UI.
 * All property fields are optional. When received by the UI, the defined (non-boost::none)
 * properties are overwritten in the UI.
 *
 * @note Identical message structure to \c GlobalSlidePropertiesPartial_msgFromUi
 */
struct SlideCommonPropertiesPartial_msgToUi
{
    SlideCommonProperties<boost::optional> m_properties; //!< Rendering properties (all optional)
};


/**
 * @brief Message of some slide stack rendering properties: sent from UI to app.
 * All property fields are optional.
 *
 * @note Identical message structure to \c GlobalSlidePropertiesPartial_msgToUi
 */
struct SlideCommonPropertiesPartial_msgFromUi
{
    SlideCommonProperties<boost::optional> m_properties; //!< Rendering properties (all optional)
};


/**
 * @brief Message of all slide stack rendering properties: sent from app to UI.
 * All property fields are mandatory and must be set.
 */
struct SlideCommonPropertiesComplete_msgToUi
{
    SlideCommonProperties<Required> m_properties; //!< Rendering properties (all mandatory)
};

} // namespace gui

#endif // GUI_MSG_SLIDE_COMMON_PROPERTY_DATA_H
