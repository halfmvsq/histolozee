#include "gui/layout/ViewType.h"

#include <sstream>


namespace gui
{

std::string viewTypeString( const ViewType& viewType )
{
    std::ostringstream os;

    switch ( viewType )
    {
    case ViewType::Image_Axial:         os << "Image_Axial"; break;
    case ViewType::Image_Coronal:       os << "Image_Coronal"; break;
    case ViewType::Image_Sagittal:      os << "Image_Sagittal"; break;
    case ViewType::Image_3D:            os << "Image_3D"; break;
    case ViewType::Image_Big3D:         os << "Image_Big3D"; break;
    case ViewType::Stack_ActiveSlide:   os << "Stack_ActiveSlide"; break;
    case ViewType::Stack_StackSide1:    os << "Stack_StackSide1"; break;
    case ViewType::Stack_StackSide2:    os << "Stack_StackSide2"; break;
    case ViewType::Stack_3D:            os << "Stack_3D"; break;
    case ViewType::Reg_ActiveSlide:     os << "Reg_ActiveSlide"; break;
    case ViewType::Reg_RefImageAtSlide: os << "Reg_RefImageAtSlide"; break;
    }

    return os.str();
}

} // namespace gui
