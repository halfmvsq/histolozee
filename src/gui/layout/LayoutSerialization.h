#ifndef LAYOUT_SERIALIZATION_H
#define LAYOUT_SERIALIZATION_H

#include "common/JSONSerializers.hpp"
#include "gui/layout/ViewType.h"

#include <nlohmann/json.hpp>


namespace gui
{

/// Define serialization of ViewType to JSON as strings
NLOHMANN_JSON_SERIALIZE_ENUM(
        ViewType, {
            { ViewType::Image_Axial, "Image_Axial" },
            { ViewType::Image_Coronal, "Image_Coronal" },
            { ViewType::Image_Sagittal, "Image_Sagittal" },
            { ViewType::Image_3D, "Image_3D" },
            { ViewType::Image_Big3D, "Image_Big3D" },
            { ViewType::Stack_ActiveSlide, "Stack_ActiveSlide" },
            { ViewType::Stack_StackSide1, "Stack_StackSide1" },
            { ViewType::Stack_StackSide2, "Stack_StackSide2" },
            { ViewType::Stack_3D, "Stack_3D" },
            { ViewType::Reg_ActiveSlide, "Reg_ActiveSlide" },
            { ViewType::Reg_RefImageAtSlide, "Reg_RefImageAtSlide" }
        } );


/// Node specifying a view inside of a SubLayoutNode.
struct ViewNode
{
    ViewType m_viewType;
};


/// Node speciying a layout that may contain either other layout nodes or a ViewNode.
/// (A layout is defined by a hierarchical tree of layout nodes.)
struct LayoutNode
{
    enum class Orientation
    {
        Horizontal,
        Vertical
    };

    /// Orientation direction of child layout nodes of this node
    Orientation m_orientation;

    /// Optional stretch factor defining how to stretch this node relative to its siblings
    boost::optional<int> m_stretch;

    /// Child layout nodes of this node
    std::vector<LayoutNode> m_childLayouts;

    /// Optional ViewNode contained within this layout node
    boost::optional<ViewNode> m_view = boost::none;
};


/// Top-level container for a layout tab that is shown in the UI.
struct LayoutTab
{
    /// Name of the layout tab (must be unique)
    std::string m_name;

    /// Description of the layout tab
    std::string m_description;

    /// Parent of the tree of nodes that defines the layout tab
    LayoutNode m_layoutNode;

    /// Flag indicating whether switching to this layout causes the crosshairs to jump to the active slide
    /// @todo Generalize this behavior and give it a better name
    bool m_centersCrosshairs;
};


/// Ordered collection of all layouts, each of which is displayed in a separate tab within the application.
struct AllLayoutTabs
{
    /// The order of layouts in the application matches the order in this vector
    std::vector<LayoutTab> m_layouts;
};


/// Define serialization of SubLayoutNode::Orientation to JSON as string
NLOHMANN_JSON_SERIALIZE_ENUM(
        LayoutNode::Orientation, {
            { LayoutNode::Orientation::Horizontal, "horizontal" },
            { LayoutNode::Orientation::Vertical, "vertical" }
        } );


/// Serialization/de-serialization of ViewNode to/from JSON
void to_json( nlohmann::json&, const ViewNode& );
void from_json( const nlohmann::json&, ViewNode& );

/// Serialization/de-serialization of SubLayoutNode to/from JSON
void to_json( nlohmann::json&, const LayoutNode& );
void from_json( const nlohmann::json&, LayoutNode& );

/// Serialization/de-serialization of RootLayoutNode to/from JSON
void to_json( nlohmann::json&, const LayoutTab& );
void from_json( const nlohmann::json&, LayoutTab& );

/// Serialization/de-serialization of LayoutTabs to/from JSON
void to_json( nlohmann::json&, const AllLayoutTabs& );
void from_json( const nlohmann::json&, AllLayoutTabs& );

} // namespace gui

#endif // LAYOUT_SERIALIZATION_H
