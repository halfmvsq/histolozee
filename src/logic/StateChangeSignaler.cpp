#include "logic/StateChangeSignaler.h"

boost::signals2::connection
StateChangeSignaler::connectToSignal( const Signal::slot_type& subscriberSlot )
{
    return m_signalChanged.connect( subscriberSlot );
}

void StateChangeSignaler::signalChanged()
{
    m_signalChanged();
}
