#ifndef GLWIDGET_CONNECTOR_H
#define GLWIDGET_CONNECTOR_H

#include <boost/signals2/signal.hpp>

#include <vector>

class GLWidget;
class StateChangeSignaler;

class GLWidgetConnector
{
public:

    GLWidgetConnector( GLWidget* widget );
    ~GLWidgetConnector() = default;

    void connectToWidgetUpdate( std::weak_ptr<StateChangeSignaler> signaler );
    void connectToWidgetUpdate( StateChangeSignaler& signaler );


private:

    GLWidget* m_glWidget;

    /// @todo Store pair< StateChangeSignaler*, boost::signals2::connection >,
    /// so that we know what the connection is from. These connections can get
    /// stale...
    std::vector< boost::signals2::connection > m_connectionsToWidgetUpdate;
};

#endif // GLWIDGET_CONNECTOR_H
