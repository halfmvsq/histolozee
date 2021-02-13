#ifndef GUI_IMAGE_HEADER_H
#define GUI_IMAGE_HEADER_H

#include "common/UID.h"

#include <glm/mat4x4.hpp>

#include <string>
#include <utility>
#include <vector>


namespace gui
{

struct ImageHeader_msgToUi
{
    UID m_imageUid; //!< Image UID
    std::vector< std::pair< std::string, std::string > > m_items; //!< Key-value pairs of header

    //!< Transformation matrix mapping image Pixel to Subject space
    glm::dmat4 m_subject_O_pixel;
};

}

#endif // GUI_IMAGE_HEADER_H
