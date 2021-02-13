#include "gui/layout/LayoutConstruction.h"
#include "common/HZeeException.hpp"

#include <QWidget>


using json = nlohmann::json;

namespace gui
{

namespace detail
{

/**
 * @brief Construct the QLayout corresponding to a tree of SubLayoutNodes.
 * The root level SubLayoutNode and QLayout are passed in. This function
 * is called recursively to build up the tree of QLayouts beneath the input.
 *
 * @param[in] layoutNode Root node describing the layout
 * @param[in] layout QLayout to which all constructed QLayouts are parented
 * @param[out] viewTypes Hash map of view UID to ViewType for all views in the layout
 * @param[out] viewLayouts Hash map of view UID to the QLayout that will parent the view's QWidget
 */
void constructLayout(
        const LayoutNode& layoutNode,
        QLayout* const layout,
        std::unordered_map< UID, ViewType >& viewTypes,
        std::unordered_map< UID, QLayout* >& viewLayouts )
{
    if ( ! layout )
    {
        std::cerr << "Null parent layout: it is ignored." << std::endl;
        return;
    }

    if ( layoutNode.m_view )
    {
        // This node has a view, so create its record, consisting of
        // the view's type and its containing layout
        const UID viewUid;
        viewTypes.insert( { viewUid, layoutNode.m_view->m_viewType } );
        viewLayouts.insert( { viewUid, layout } );
    }

    // Recurse over all child nodes of this layout
    for ( const LayoutNode& childNode : layoutNode.m_childLayouts )
    {
        QLayout* childLayout;

        switch ( childNode.m_orientation )
        {
        case LayoutNode::Orientation::Horizontal:
        {
            childLayout = new QHBoxLayout();
            break;
        }
        case LayoutNode::Orientation::Vertical:
        {
            childLayout = new QVBoxLayout();
            break;
        }
        }

        // The default QLayout stretch factor is 0. Use this if none is provided.
        const int stretch = ( childNode.m_stretch )
                ? *( childNode.m_stretch ) : 0;

        switch ( layoutNode.m_orientation )
        {
        case LayoutNode::Orientation::Horizontal:
        {
            if ( auto l = dynamic_cast<QHBoxLayout*>( layout ) )
            {
                l->addLayout( childLayout, stretch );
            }
            else
            {
                throw_debug( "Null parent layout" );
            }
            break;
        }
        case LayoutNode::Orientation::Vertical:
        {
            if ( auto l = dynamic_cast<QVBoxLayout*>( layout ) )
            {
                l->addLayout( childLayout, stretch );
            }
            else
            {
                throw_debug( "Null parent layout" );
            }
        }
        }

        constructLayout( childNode, childLayout, viewTypes, viewLayouts );
    }
}


/**
 * @brief Make the "central widget" (a QWidget) containing the QLayout
 * tree of a SubLayoutNode tree
 * @param[in] rootNode Root node of layout description
 * @return Triple consisting of
 * 1) Central widget
 * 2) Hash map of all view UIDs to their type
 * 3) Hash map of all view UIDs to their containing QLayout
 */
std::tuple<
    QWidget*,
    std::unordered_map< UID, ViewType >,
    std::unordered_map< UID, QLayout* > >
makeCentralWidget( const LayoutNode& rootNode )
{
    // One layout may contain multiple views, each of which is represented
    // by a record (unique ID) in this map. Additionally, each view
    // has an associated ViewType and is contained in the GUI by a parent QLayout.
    std::unordered_map< UID, ViewType > viewTypes;
    std::unordered_map< UID, QLayout* > viewLayouts;

    QLayout* centralLayout = nullptr;

    switch ( rootNode.m_orientation )
    {
    case LayoutNode::Orientation::Horizontal:
    {
        centralLayout = new QHBoxLayout;
        break;
    }
    case LayoutNode::Orientation::Vertical:
    {
        centralLayout = new QVBoxLayout;
        break;
    }
    }

    constructLayout( rootNode, centralLayout, viewTypes, viewLayouts );

    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout( centralLayout );

    return std::make_tuple( centralWidget, viewTypes, viewLayouts );
}

} // namespace detail



std::tuple<
    std::unordered_map< UID, std::pair< QWidget*, std::string > >,
    std::list< UID >,
    std::unordered_map< UID, ViewType >,
    std::unordered_map< UID, QLayout* > >
makeTraditionalLayouts( const AllLayoutTabs& layoutTabs )
{
    std::unordered_map< UID, std::pair< QWidget*, std::string > > layoutWidgets;
    std::list< UID > orderedLayoutUIDs;
    std::unordered_map< UID, ViewType > allViewTypes;
    std::unordered_map< UID, QLayout* > allViewLayouts;

    for ( const auto& p : layoutTabs.m_layouts )
    {
        const auto& layoutName = p.m_name;
        const auto& subLayoutNode = p.m_layoutNode;

        QWidget* centralWidget = nullptr;
        std::unordered_map< UID, ViewType > viewTypes;
        std::unordered_map< UID, QLayout* > viewLayouts;

        std::tie( centralWidget, viewTypes, viewLayouts ) =
                detail::makeCentralWidget( subLayoutNode );

        if ( centralWidget )
        {
            UID layoutUid;
            layoutWidgets.emplace( layoutUid, std::make_pair( centralWidget, layoutName ) );
            orderedLayoutUIDs.push_back( layoutUid );

            allViewTypes.insert( std::begin( viewTypes ), std::end( viewTypes ) );
            allViewLayouts.insert( std::begin( viewLayouts ), std::end( viewLayouts ) );
        }
        else
        {
            std::cerr << "Null central widget created: it will be ignored" << std::endl;
        }
    }

    return std::make_tuple(
                layoutWidgets,
                orderedLayoutUIDs,
                allViewTypes,
                allViewLayouts );
}

} // namespace gui
