#ifndef GUI_VIEW_TYPE_H
#define GUI_VIEW_TYPE_H

#include <string>


namespace gui
{

/**
 * @brief Defines the available view types in the application.
 * Each view type gets a unique Camera object.
 */
enum class ViewType
{
    /// Initially aligned with axial plane of active image (orthographic projection)
    Image_Axial,

    /// Initially aligned with coronal plane of active image (orthographic projection)
    Image_Coronal,

    /// Initially aligned with sagittal plane of active image (orthographic projection)
    Image_Sagittal,

    /// Initially aligned with coronal plane of active image (perspective projection)
    Image_3D,

    /// Initially aligned with coronal plane of active image (perspective projection)
    Image_Big3D,

    /// Initially aligned parallel to the stack at the location of the active slide
    /// (orthographic projection)
    Stack_ActiveSlide,

    /// Initially aligned perpendicular to the stack (orthographic projection)
    Stack_StackSide1,

    /// Initially aligned perpendicular to the stack and also perpendicular to
    /// \c Stack_StackSide1 (orthographic projection)
    Stack_StackSide2,

    /// Initially aligned parallel to the stack (perspective projection)
    /// @note Not currently used in any layouts.
    Stack_3D,

    /// Initially aligned parallel to the stack at the location of the active slide
    /// (orthographic projection)
    Reg_ActiveSlide,

    /// Initially aligned parallel to the stack at the location of the active slide
    /// (orthographic projection)
    Reg_RefImageAtSlide
};


/**
 * @brief Return the view type as a string
 * @param[in] viewType
 */
std::string viewTypeString( const ViewType& viewType );

} // namespace gui

#endif // GUI_VIEW_TYPE_H
