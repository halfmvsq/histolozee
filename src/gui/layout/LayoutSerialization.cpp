#include "gui/layout/LayoutSerialization.h"


using json = nlohmann::json;

namespace gui
{

void to_json( json& j, const ViewNode& v )
{
    j = json {
    { "viewType", v.m_viewType } };
}

void from_json( const json& j, ViewNode& v )
{
    j.at( "viewType" ).get_to( v.m_viewType );
}


void to_json( json& j, const LayoutNode& n )
{
    j = json{
    { "orientation", n.m_orientation },
    { "stretch", n.m_stretch },
    { "subLayouts", n.m_childLayouts },
    { "view", n.m_view } };
}

void from_json( const json& j, LayoutNode& n )
{
    j.at( "orientation" ).get_to( n.m_orientation );
    j.at( "stretch" ).get_to( n.m_stretch );
    j.at( "subLayouts" ).get_to( n.m_childLayouts );
    j.at( "view" ).get_to( n.m_view );
}


void to_json( json& j, const LayoutTab& n )
{
    j = json{
    { "name", n.m_name },
    { "description", n.m_description },
    { "layoutNode", n.m_layoutNode },
    { "centersCrosshairs", n.m_centersCrosshairs } };
}

void from_json( const json& j, LayoutTab& n )
{
    j.at( "name" ).get_to( n.m_name );
    j.at( "description" ).get_to( n.m_description );
    j.at( "layoutNode" ).get_to( n.m_layoutNode );
    j.at( "centersCrosshairs" ).get_to( n.m_centersCrosshairs );
}


void to_json( nlohmann::json& j, const AllLayoutTabs& t )
{
    j = json{
    { "layouts", t.m_layouts } };
}

void from_json( const nlohmann::json& j, AllLayoutTabs& t )
{
    j.at( "layouts" ).get_to( t.m_layouts );
}

} // namespace gui
