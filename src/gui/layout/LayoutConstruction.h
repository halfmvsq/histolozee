#ifndef LAYOUT_CONSTRUCTION_H
#define LAYOUT_CONSTRUCTION_H

#include "common/UID.h"
#include "gui/layout/LayoutSerialization.h"
#include "gui/layout/ViewType.h"

#include <QLayout>

#include <list>
#include <string>
#include <tuple>
#include <unordered_map>


namespace gui
{

/**
 * @brief Make all objects required for GUI's layout in Qt.
 * The layout is entirely described by a LayoutTabs object.
 *
 * @note This function is no longer used in HistoloZee. It has been replaced by makeLayouts().
 *
 * @return Tuple consisting of
 * 1) Hash map of all layout UIDs to their "central widget":
 * Each is a QWidget containing the generated QLayout tree
 * 2) List of layout UIDs in the order that they are to be displayed
 * 3) Hash map of all view UIDs to their type
 * 4) Hash map of all view UIDs to their containing QLayout
 */
std::tuple<
    std::unordered_map< UID, std::pair< QWidget*, std::string > >,
    std::list< UID >,
    std::unordered_map< UID, ViewType >,
    std::unordered_map< UID, QLayout* > >
makeTraditionalLayouts( const AllLayoutTabs& );


#if 0
// Used for programmatic definition of layouts
extern const LayoutTabs k_layoutTabs;
#endif

} // namespace gui

#endif // LAYOUT_CONSTRUCTION_H
