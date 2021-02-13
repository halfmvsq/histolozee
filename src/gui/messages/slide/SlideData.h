#ifndef GUI_MSG_SLIDE_DATA_H
#define GUI_MSG_SLIDE_DATA_H

#include "common/Identity.h"
#include "common/UID.h"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_precision.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>


namespace gui
{

/**
 * @brief Read-only slide header data for display in the UI.
 */
struct SlideHeaderImmutable
{
    std::string m_filePath; //!< Slide image file path
    std::string m_slideType; //!< Slide type (may be the vendor name)

    /// Slide layer dimensions in pixels, where 0 is the highest resolution layer.
    std::vector< glm::i64vec2 > m_layerDims;

    /// Buffer for slide label image in pre-multiplied ARGB format
    /// (i.e. Qt's format \c QImage::Format_ARGB32_Premultiplied; 0xAARRGGBB)
    std::weak_ptr< std::vector<uint32_t> > m_labelImageBuffer = {};
    glm::i64vec2 m_labelImageDims = { 0, 0 }; //!< Label image dimensions

    /// Buffer for slide macro image in pre-multiplied ARGB format
    std::weak_ptr< std::vector<uint32_t> > m_macroImageBuffer = {};
    glm::i64vec2 m_macroImageDims = { 0, 0 }; //!< Macro image dimensions

    std::pair<double, double> m_pixelSizeRange; //!< Allowable range of slide pixel sizes
    std::pair<double, double> m_thicknessRange; //!< Allowable range of slide thickness
};


/**
 * @brief Read/write slide header data for the UI.
 * Template type V can be either std::optional (to denote all optional fields) or
 * Required (to denote all required fields).
 */
template< template<typename> class V >
struct SlideHeaderMutable
{
    V<std::string> m_displayName; //!< Display name (referred to as "Name/ID" in the UI)

    V<float> m_pixelSizeX; //!< Horizontal (x) pixel size of highest resolution layer (in mm)
    V<float> m_pixelSizeY; //!< Vertical (y) pixel size of highest resolution layer (in mm)
    V<float> m_thickness; //!< Slide thickness (in mm)
};


/**
 * @brief Slide view data for display/editing in the UI.
 * Template type V can be either std::optional (to denote all optional fields) or
 * Required (to denote all required fields).
 */
template< template<typename> class V >
struct SlideViewData
{
    V<bool> m_slideVisibleChecked; //!< Slide visibility

    V<glm::vec3> m_borderColor; //!< Slide border color (non-pre-multiplied RGB)

    V< std::pair<int, int> > m_opacityRange; //!< Range of slide opacity slider and spin box
    V<int> m_opacitySingleStep; //!< Single-step of slide opacity slider and spin box
    V<int> m_opacitySliderPageStep; //!< Page-step of slide opacity slider
    V<int> m_opacityValue; //!< Value of slide opacity slider and spin box

    V< std::pair<int, int> > m_threshRange; //!< Range of thresholding slider and spin boxes
    V<int> m_threshSingleStep; //!< Single-step value of thresholding slider and spin boxes
    V<int> m_threshSliderPageStep; //!< Page-step value of thresholding slider
    V< std::pair<int, int> > m_threshValues; //!< Low/high values of thresholding range slider and spin boxes

    V<bool> m_edgesVisibleChecked = true; //!< Edge visibility

    V< std::pair<double, double> > m_edgesMagnitudeRange; //!< Range of edges magnitude slider and spin boxes
    V<double> m_edgesMagnitudeSingleStep; //!< Single-step value of edges magnitude slider and spin boxes
    V<double> m_edgesMagnitudePageStep; //!< Page-step value of edges magnitude slider
    V<int> m_edgesMagnitudeDecimalPrecision; //!< No. decimals of edges magnitude spin boxes
    V<double> m_edgesMagnitudeValue; //!< Value of edges magnitude slider and spin boxes

    V< std::pair<double, double> > m_edgesSmoothingRange; //!< Range of edges smoothing slider and spin boxes
    V<double> m_edgesSmoothingSingleStep; //!< Single-step value of edges smoothing slider and spin boxes
    V<double> m_edgesSmoothingPageStep; //!< Page-step value of edges smoothing slider
    V<int> m_edgesSmoothingDecimalPrecision; //!< No. decimals of edges smoothing spin boxes
    V<double> m_edgesSmoothingValue; //!< Value of edges smoothing slider and spin boxes
};


/**
 * @brief Slide transformation data for display/editing in the UI.
 * Template type V can be either std::optional (to denote all optional fields) or
 * Required (to denote all required fields).
 */
template< template<typename> class V >
struct SlideTransformationData
{
    V< std::pair<double, double> > m_translationRange; //!< Range of x,y translation spin boxes
    V<double> m_translationSingleStep; //!< Single-step value of translation spin boxes
    V<int> m_translationDecimalPrecision; //!< No. decimals of translation spin boxes

    V< std::pair<double, double> > m_rotationRange; //!< Range of z rotation spin box
    V<double> m_rotationSingleStep; //!< Single-step value of rotation spin box
    V<int> m_rotationDecimalPrecision; //!< No. decimals of rotation spin box

    V< std::pair<double, double> > m_scaleRange; //!< Range of x,y scale spin boxes
    V<double> m_scaleSingleStep; //!< Single-step value of scale spin boxes
    V<int> m_scaleDecimalPrecision; //!< No. decimals of scale spin boxes

    V< std::pair<double, double> > m_scaleRotationRange; //!< Range of z scale rotation spin box
    V<double> m_scaleRotationSingleStep; //!< Single-step value of scale rotation spin box
    V<int> m_scaleRotationDecimalPrecision; //!< No. decimals of scale rotation spin box

    V< std::pair<double, double> > m_shearRange; //!< Range of x,y shear spin boxes
    V<double> m_shearSingleStep; //!< Single-step value of shear spin boxes
    V<int> m_shearDecimalPrecision; //!< No. decimals of shear spin boxes

    V< std::pair<double, double> > m_originRange; //!< Range of x,y origin spin boxes
    V<double> m_originSingleStep; //!< Single-step value of origin spin boxes
    V<int> m_originDecimalPrecision; //!< No. decimals of origin spin boxes

    V<double> m_xTranslationValueInMm; //!< x axis translation value
    V<double> m_yTranslationValueInMm; //!< y axis translation value
    V<double> m_zTranslationValueInMm; //!< z axis translation value

    V<double> m_zRotationValueInDeg; //!< z axis rotation value

    V<double> m_xScaleValue; //!< x axis scale value
    V<double> m_yScaleValue; //!< y axis scale value

    V<double> m_xShearValueInDeg; //!< x axis shear value
    V<double> m_yShearValueInDeg; //!< y axis shear value

    V<double> m_zScaleRotationValueInDeg; //!< z axis scale rotation value

    V<double> m_xOriginValueInMm; //!< x axis origin value
    V<double> m_yOriginValueInMm; //!< y axis origin value

    /// Option for whether to parameterize shear using scale rotation (true)
    /// or separate shear angles (false)
    V<bool> m_useScaleRotationParameterization;

    /// Affine transformation matrix mapping this slide to the whole stack space.
    /// The UI cannot modify this.
    V<glm::dmat4> m_stack_O_slide_matrix;
};


/**
 * @brief Message of all slide header data, sent from app to UI.
 */
struct SlideHeaderComplete_msgToUi
{
    UID m_uid; //!< Slide UID
    SlideHeaderImmutable m_headerImmutable; //!< Non-editable header data
    SlideHeaderMutable<Required> m_headerMutable; //!< Editable header data
};


/**
 * @brief Message of partial header data, sent from UI to app.
 */
struct SlideHeaderPartial_msgFromUi
{
    UID m_uid; //!< Slide UID
    SlideHeaderMutable<std::optional> m_headerMutable; //!< Header data
};


/**
 * @brief Message of all slide view data, sent from app to UI.
 */
struct SlideViewDataComplete_msgToUi
{
    UID m_uid; //!< Slide UID
    SlideViewData<Required> m_viewData; //!< View data
};


/**
 * @brief Message of some slide view data, sent from app to UI.
 */
struct SlideViewDataPartial_msgToUi
{
    UID m_uid; //!< Slide UID
    SlideViewData<std::optional> m_viewData; //!< View data
};


/**
 * @brief Message of some slide view data, sent from UI to app.
 */
struct SlideViewDataPartial_msgFromUi
{
    UID m_uid; //!< Slide UID
    SlideViewData<std::optional> m_viewData; //!< View data
};


/**
 * @brief Message of all slide transformation data, sent from app to UI.
 */
struct SlideTxDataComplete_msgToUi
{
    UID m_uid; //!< Slide UID
    SlideTransformationData<Required> m_txData; //!< Transformation data
};


/**
 * @brief Message of some slide transformation data, sent from app to UI.
 */
struct SlideTxDataPartial_msgToUi
{
    UID m_uid; //!< Slide UID
    SlideTransformationData<std::optional> m_txData; //!< Transformation data
};


/**
 * @brief Message of some slide transformation data, sent from UI to app.
 */
struct SlideTxDataPartial_msgFromUi
{
    UID m_uid; //!< Slide UID
    SlideTransformationData<std::optional> m_txData; //!< Transformation data

    /// Flag to set the world_O_stack transformation to identity.
    std::optional<bool> m_set_stack_O_slide_identity;
};


} // namespace gui

#endif // GUI_MSG_SLIDE_DATA_H
