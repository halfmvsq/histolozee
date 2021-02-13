#ifndef STATE_CHANGE_SIGNALER_H
#define STATE_CHANGE_SIGNALER_H

#include <boost/signals2/signal.hpp>

class StateChangeSignaler
{
public:

    using Signal = boost::signals2::signal< void () >;

    StateChangeSignaler() = default;

    StateChangeSignaler( const StateChangeSignaler& ) = default;
    StateChangeSignaler& operator=( const StateChangeSignaler& ) = default;

    StateChangeSignaler( StateChangeSignaler&& ) = default;
    StateChangeSignaler& operator=( StateChangeSignaler&& ) = default;

    ~StateChangeSignaler() = default;

    /// Connect a subscriber's slot to the signal that is emitted this class changes
    boost::signals2::connection
    connectToSignal( const Signal::slot_type& subscriberSlot );

    void signalChanged();


private:

    Signal m_signalChanged;
};

#endif // STATE_CHANGE_SIGNALER_H
