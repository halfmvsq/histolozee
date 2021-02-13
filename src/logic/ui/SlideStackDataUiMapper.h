#ifndef SLIDE_STACK_DATA_UI_MAPPER_H
#define SLIDE_STACK_DATA_UI_MAPPER_H

#include "gui/docks/PublicSlideTypes.h"
#include "gui/messages/slide/MoveToSlide.h"
#include "gui/messages/slide/SlideData.h"
#include "gui/messages/slide/SlideStackData.h"
#include "gui/messages/slide/SlideCommonProperties.h"

#include "common/PublicTypes.h"

#include <boost/optional.hpp>

#include <functional>
#include <memory>


class ActionManager;
class AssemblyManager;
class CoordinateFrame;
class DataManager;
class InteractionManager;


/**
 * @brief Class for connecting slide stack data between the Application and the UI
 */
class SlideStackDataUiMapper
{
    using CrosshairsToSlideCenterMover =  std::function< void ( const UID& slideUid ) >;


public:

    SlideStackDataUiMapper(
            ActionManager&,
            AssemblyManager&,
            DataManager&,
            InteractionManager&,
            AllViewsUpdaterType );

    ~SlideStackDataUiMapper();


    /// Set function that provides the slide stack frame
    void setSlideStackFrameProvider( GetterType<CoordinateFrame> );

    /// Set function that broadcasts committed change to the slide stack frame
    void setSlideStackFrameChangeDoneBroadcaster( SetterType<const CoordinateFrame&> );

    /// Set function that centers the crosshairs on a given slide
    void setCrosshairsToSlideCenterMover( CrosshairsToSlideCenterMover );


    /// Update some slide stack data from UI
    void setSlideStackPartial_fromUi( const gui::SlideStackPartial_msgFromUi& );

    /// Update the slide stack order from UI
    void setSlideStackOrder_fromUi( const gui::SlideStackOrder_msgFromUi& );

    /// Update the active slide selection from UI
    void setActiveSlide_fromUi( const gui::ActiveSlide_msgFromUi& );

    /// Update some slide stack common properties from UI
    void setSlideCommonPropertiesPartial_fromUi( const gui::SlideCommonPropertiesPartial_msgFromUi& );

    /// Update some slide header data from UI
    void setSlideHeaderPartial_fromUi( const gui::SlideHeaderPartial_msgFromUi& );

    /// Update some slide view data from UI
    void setSlideViewDataPartial_fromUi( const gui::SlideViewDataPartial_msgFromUi& );

    /// Update some slide transformation data from UI
    void setSlideTxDataPartial_fromUi( const gui::SlideTxDataPartial_msgFromUi& );

    /// Move to slide from UI action
    void setMoveToSlide_fromUi( const gui::MoveToSlide_msgFromUi& );


    /// Set the functional that updates the UI with all slide stack changes from the app
    void setSlideStackCompletePublisher_msgToUi( gui::SlideStackComplete_msgToUi_PublisherType );

    /// Set the functional that updates the UI with some slide stack changes from the app
    void setSlideStackPartialPublisher_msgToUi( gui::SlideStackPartial_msgToUi_PublisherType );

    /// Set the functional that updates the UI with the active slide from the app
    void setActiveSlidePublisher_msgToUi( gui::ActiveSlide_msgToUi_PublisherType );

    /// Set the functional that updates the UI with some slide stack common properties from the app
    void setSlideCommonPropertiesPartialPublisher_msgToUi(
            gui::SlideCommonPropertiesPartial_msgToUi_PublisherType );

    /// Set the functional that updates the UI with all slide stack common properties from the app
    void setSlideCommonPropertiesCompletePublisher_msgToUi(
            gui::SlideCommonPropertiesComplete_msgToUi_PublisherType );

    /// Set the function that updates the UI with all slide header data
    void setSlideHeaderCompletePublisher_msgToUi( gui::SlideHeaderComplete_msgToUi_PublisherType );

    /// Set the function that updates the UI with all slide view data
    void setSlideViewDataCompletePublisher_msgToUi( gui::SlideViewDataComplete_msgToUi_PublisherType );

    /// Set the function that updates the UI with some slide view data
    void setSlideViewDataPartialPublisher_msgToUi( gui::SlideViewDataPartial_msgToUi_PublisherType );

    /// Set the function that updates the UI with all slide transformation data
    void setSlideTxDataCompletePublisher_msgToUi( gui::SlideTxDataComplete_msgToUi_PublisherType );

    /// Set the function that updates the UI with some slide transformation data
    void setSlideTxDataPartialPublisher_msgToUi( gui::SlideTxDataPartial_msgToUi_PublisherType );



    /// Respond to UI request for all slide stack data
    gui::SlideStackComplete_msgToUi getSlideStackComplete_msgToUi() const;

    /// Respond to UI request for active slide
    gui::ActiveSlide_msgToUi getActiveSlide_msgToUi() const;

    /// Respond to UI request from all slide stack common properties
    gui::SlideCommonPropertiesComplete_msgToUi getSlideCommonPropertiesComplete_msgToUi() const;

    /// Respond to UI request for all slide header data
    boost::optional< gui::SlideHeaderComplete_msgToUi >
    getSlideHeaderComplete_msgToUi( const UID& slideUid ) const;

    /// Respond to UI request for all slide view data
    boost::optional< gui::SlideViewDataComplete_msgToUi >
    getSlideViewDataComplete_msgToUi( const UID& slideUid ) const;

    /// Respond to UI request for all slide transformation data
    boost::optional< gui::SlideTxDataComplete_msgToUi >
    getSlideTxDataComplete_msgToUi( const UID& slideUid ) const;


    /// Slot to update the UI due to change of the slide stack transformation
    void updateUiFromSlideStackFrameChange();


private:

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

#endif // SLIDE_STACK_DATA_UI_MAPPER_H
