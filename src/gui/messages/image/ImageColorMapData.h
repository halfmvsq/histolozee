#ifndef GUI_IMAGE_COLOR_MAP_DATA_H
#define GUI_IMAGE_COLOR_MAP_DATA_H

#include "common/UID.h"

#include <string>
#include <vector>


namespace gui
{

/**
 * @brief A single image color map item
 */
struct ImageColorMapItem
{
    UID m_colorMapUid; //!< Color map UID
    std::string m_name; //!< Name of color map to display in UI
    std::string m_description; //!< Description to display in UI

    /// Preview icon buffer for the color map (in Qt5 pre-multiplied RGBA8888 format:
    /// The buffer is 1-dimensional (its height is 1), with one byte per color component;
    /// four bytes per color
    std::vector<uint8_t> m_iconBuffer;
};


/**
 * @brief Message of image color map items sent from app to UI.
 * When received by the UI, this replaces all existing color map items in the UI.
 */
struct ImageColorMaps_msgToUi
{
    /// Complete, sorted collection of color map items to display in UI
    std::vector<ImageColorMapItem> m_colorMapItems;
};

} // namespace gui

#endif // GUI_IMAGE_COLOR_MAP_DATA_H
