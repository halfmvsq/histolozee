#include "gui/layout/SplitterLayoutConstruction.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <QWidget>


using json = nlohmann::json;

namespace
{

// Whether to render views dynamically as the splitters between the views move
static constexpr bool sk_dynamicViewResize = false;

}

namespace gui
{

namespace splitter_detail
{

/**
 * @brief Construct the QSplitter corresponding to a tree of SubLayoutNodes.
 * The root level SubLayoutNode and QSplitter are passed in. This function
 * is called recursively to build up the tree of QSplitters beneath the input.
 *
 * @param[in] layoutNode Root node describing the layout
 * @param[in] layout QSplitter to which all constructed QSplitter are parented
 * @param[out] viewTypes Hash map of view UID to ViewType for all views in the layout
 * @param[out] viewLayouts Hash map of view UID to the QLayout that will parent the
 * view's QWidget
 */
void constructSplitterLayout(
        const LayoutNode& parentNode,
        QSplitter* const parentSplitter,
        std::unordered_map< UID, ViewType >& viewTypes,
        std::unordered_map< UID, QSplitter* >& viewSplitters )
{
    if ( ! parentSplitter )
    {
        std::cerr << "Null parent QSplitter: it is being ignored." << std::endl;
        return;
    }

    if ( parentNode.m_view )
    {
        // This node has a view, so create its record, consisting of
        // the view's type and its containing QSplitter
        const UID viewUid;
        viewTypes.insert( { viewUid, parentNode.m_view->m_viewType } );
        viewSplitters.insert( { viewUid, parentSplitter } );
    }

    // Recurse over all child nodes of this node
    int widgetCount = 0;
    for ( const LayoutNode& childNode : parentNode.m_childLayouts )
    {
        const Qt::Orientation orientation =
                ( LayoutNode::Orientation::Horizontal == childNode.m_orientation )
                ? Qt::Horizontal : Qt::Vertical;

        QSplitter* childSplitter = new QSplitter( orientation );
        childSplitter->setOpaqueResize( sk_dynamicViewResize );

        // The default QSplitter stretch factor is 1.
        // Use this if none is provided.
        const int stretch = ( childNode.m_stretch )
                ? *( childNode.m_stretch ) : 0;

        parentSplitter->addWidget( childSplitter );
        parentSplitter->setStretchFactor( widgetCount++, stretch );

        constructSplitterLayout( childNode, childSplitter, viewTypes, viewSplitters );
    }
}


/**
 * @brief Make the "central widget" (a QWidget) containing the QSplitter
 * tree of a SubLayoutNode tree
 * @param[in] rootNode Root node of layout description
 * @return Triple consisting of
 * 1) Central widget
 * 2) Hash map of all view UIDs to their type
 * 3) Hash map of all view UIDs to their containing QSplitter
 */
std::tuple<
    QWidget*,
    std::unordered_map< UID, ViewType >,
    std::unordered_map< UID, QSplitter* > >
makeCentralWidget( const LayoutNode& rootNode )
{
    // One layout may contain multiple views, each of which is represented
    // by a record (with unique ID) in this map. Additionally, each view
    // has an associated ViewType and is contained in the GUI by a parent QSplitter.
    std::unordered_map< UID, ViewType > viewTypes;
    std::unordered_map< UID, QSplitter* > viewSplitters;

    Qt::Orientation orientation =
            ( LayoutNode::Orientation::Horizontal == rootNode.m_orientation )
            ? Qt::Horizontal : Qt::Vertical;

    QSplitter* centralSplitter = new QSplitter( orientation );
    centralSplitter->setOpaqueResize( sk_dynamicViewResize );

    QLayout* centralLayout = new QHBoxLayout;
    centralLayout->addWidget( centralSplitter );

    constructSplitterLayout( rootNode, centralSplitter, viewTypes, viewSplitters );

    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout( centralLayout );

    return std::make_tuple( centralWidget, viewTypes, viewSplitters );
}

} // namespace splitter_detail


std::tuple<
    std::unordered_map< UID, LayoutTabData >,
    std::list< UID >,
    std::unordered_map< UID, ViewType >,
    std::unordered_map< UID, QSplitter* > >
makeLayouts( const AllLayoutTabs& layoutTabs )
{
#if 0
    nlohmann::json j = layoutTabs;
    std::cout << j.dump( 2 ) << std::endl;
#endif


    std::unordered_map< UID, LayoutTabData > allLayoutData;
    std::list< UID > orderedLayoutUids;
    std::unordered_map< UID, ViewType > allViewTypes;
    std::unordered_map< UID, QSplitter* > allViewSplitters;

    for ( const auto& l : layoutTabs.m_layouts )
    {
        QWidget* centralWidget = nullptr;
        std::unordered_map< UID, ViewType > viewTypes;
        std::unordered_map< UID, QSplitter* > viewSplitters;

        std::tie( centralWidget, viewTypes, viewSplitters ) =
                splitter_detail::makeCentralWidget( l.m_layoutNode );

        centralWidget->setContentsMargins( 0, 0, 0, 0 );

        if ( centralWidget )
        {
            const UID newLayoutUid; // Create new layout UID

            LayoutTabData data;
            data.m_containerWidget = centralWidget;
            data.m_displayName = l.m_name;
            data.m_centersCrosshairs = l.m_centersCrosshairs;

            allLayoutData.emplace( newLayoutUid, std::move( data ) );
            orderedLayoutUids.push_back( newLayoutUid );

            allViewTypes.insert( std::begin( viewTypes ), std::end( viewTypes ) );
            allViewSplitters.insert( std::begin( viewSplitters ), std::end( viewSplitters ) );
        }
        else
        {
            std::cerr << "Null central widget created: it will be ignored" << std::endl;
        }
    }

    return std::make_tuple(
                allLayoutData,
                orderedLayoutUids,
                allViewTypes,
                allViewSplitters );
}


/// @internal The following code programmatically generates the layouts

#if 0

/// Layout with four views (axial, coronal, sagittal, and 3D)
/// that are by default aligned with the base reference image.
namespace main_layout
{
static const SubLayoutNode l1{ SubLayoutNode::Orientation::Horizontal, 1, {}, ViewNode{ ViewType::Image_Coronal } };
static const SubLayoutNode l2{ SubLayoutNode::Orientation::Horizontal, 1, {}, ViewNode{ ViewType::Image_Sagittal } };
static const SubLayoutNode l3{ SubLayoutNode::Orientation::Horizontal, 1, {}, ViewNode{ ViewType::Image_Axial } };
static const SubLayoutNode l4{ SubLayoutNode::Orientation::Horizontal, 1, {}, ViewNode{ ViewType::Image_3D } };

static const SubLayoutNode topLayout{ SubLayoutNode::Orientation::Horizontal, 1, { l1, l2 } };
static const SubLayoutNode bottomLayout{ SubLayoutNode::Orientation::Horizontal, 1, { l3, l4 } };

static const SubLayoutNode layout{ SubLayoutNode::Orientation::Vertical, 1, { topLayout, bottomLayout } };

static const RootLayoutNode root{ "Main", "Main image views (ax/cor/sag/3D)", layout };
}


/// Layout with a single 3D view that is by default aligned with the
/// base reference image.
namespace threeD_layout
{
// Note: this won't work until we create GLWidgets on demand
static const SubLayoutNode l{ SubLayoutNode::Orientation::Horizontal, 1, {}, ViewNode{ ViewType::Image_Big3D } };
static const SubLayoutNode layout{ SubLayoutNode::Orientation::Vertical, 1, { l} };

static const RootLayoutNode root{ "3D", "3D view", layout };
}


/// Layout with four views (three planar and one 3D) that by default
/// are aligned with the slide stack.
namespace stack_layout
{
static const SubLayoutNode l1{ SubLayoutNode::Orientation::Horizontal, 1, {}, ViewNode{ ViewType::Stack_3D } };
static const SubLayoutNode l3{ SubLayoutNode::Orientation::Horizontal, 1, {}, ViewNode{ ViewType::Stack_StackSide1 } };
static const SubLayoutNode l4{ SubLayoutNode::Orientation::Horizontal, 1, {}, ViewNode{ ViewType::Stack_StackSide2 } };

static const SubLayoutNode leftLayout{ SubLayoutNode::Orientation::Vertical, 2, { l1 } };
static const SubLayoutNode rightLayout{ SubLayoutNode::Orientation::Vertical, 1, { l2, l3, l4 } };

static const SubLayoutNode layout{ SubLayoutNode::Orientation::Horizontal, 1, { leftLayout, rightLayout } };

static const RootLayoutNode root{ "Slide Stack", "Views aligned with slide stack", layout };
}


/// Layout with two views that are by default aligned parallel to the
/// slide stack. This layout shows slides and corresponding reference
/// image sections.
namespace reg_layout
{
static const SubLayoutNode l1{ SubLayoutNode::Orientation::Horizontal, 1, {}, ViewNode{ ViewType::Reg_ActiveSlide } };
static const SubLayoutNode l2{ SubLayoutNode::Orientation::Horizontal, 1, {}, ViewNode{ ViewType::Reg_RefImageAtSlide } };

static const SubLayoutNode layout{ SubLayoutNode::Orientation::Horizontal, 1, { l1, l2 } };

static const RootLayoutNode root{ "Registration", "Views for slide-image registration", layout };
}


const LayoutTabs k_layoutTabs{
    { main_layout::root, threeD_layout::root, stack_layout::root, reg_layout::root } };

#endif

} // namespace gui
