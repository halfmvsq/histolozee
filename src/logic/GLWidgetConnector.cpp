#include "logic/GLWidgetConnector.h"
#include "logic/StateChangeSignaler.h"
#include "gui/GLWidget.h"

#include "common/HZeeException.hpp"

#include <boost/bind.hpp>

GLWidgetConnector::GLWidgetConnector( GLWidget* widget )
    : m_glWidget( widget ),
      m_connectionsToWidgetUpdate()
{
    if ( ! m_glWidget )
    {
        throw_debug( "GLWidget is null: unable to construct GLWidgetConnector" );
    }
}

void GLWidgetConnector::connectToWidgetUpdate(
        std::weak_ptr<StateChangeSignaler> signaler )
{
    if ( auto s = signaler.lock() )
    {
        connectToWidgetUpdate( *s );
    }
}

void GLWidgetConnector::connectToWidgetUpdate(
        StateChangeSignaler& signaler )
{
    if ( ! m_glWidget )
    {
        return;
    }

    auto subscriberSlot = boost::bind( &GLWidget::update, m_glWidget );
    m_connectionsToWidgetUpdate.push_back( signaler.connectToSignal( subscriberSlot ) );
}
