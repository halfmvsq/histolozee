#ifndef SPLITTER_LAYOUT_CONSTRUCTION_H
#define SPLITTER_LAYOUT_CONSTRUCTION_H

#include "common/UID.h"

#include "gui/layout/LayoutData.h"
#include "gui/layout/LayoutSerialization.h"
#include "gui/layout/ViewType.h"

#include <list>
#include <string>
#include <tuple>
#include <unordered_map>

class QSplitter;


namespace gui
{

/**
 * @brief Make all objects required for GUI's layout in Qt.
 * The layout is entirely described by a LayoutTabs object.
 *
 * @return Tuple consisting of
 * 1) Hash map of all layout UIDs to their "central widget":
 * Each is a QWidget containing the generated QSplitter tree
 * 2) List of layout UIDs in the order that they are to be displayed
 * 3) Hash map of all view UIDs to their type
 * 4) Hash map of all view UIDs to their containing QSplitter
 */
std::tuple<
    std::unordered_map< UID, LayoutTabData >,
    std::list< UID >,
    std::unordered_map< UID, ViewType >,
    std::unordered_map< UID, QSplitter* > >
makeLayouts( const AllLayoutTabs& );


#if 0
// Used for programmatic definition of layouts
extern const LayoutTabs k_layoutTabs;
#endif

} // namespace gui

#endif // SPLITTER_LAYOUT_CONSTRUCTION_H
