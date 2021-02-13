#ifndef LAYOUT_DATA
#define LAYOUT_DATA

#include <string>

class QWidget;

namespace gui
{

/**
 * @brief Data for a layout tab
 */
struct LayoutTabData
{
    QWidget* m_containerWidget = nullptr;
    std::string m_displayName = "";
    bool m_centersCrosshairs = false;
};

} // namespace gui

#endif // LAYOUT_DATA
