#ifndef GUI_SLIDE_DOCK_PUBLIC_TYPES_H
#define GUI_SLIDE_DOCK_PUBLIC_TYPES_H

#include "common/UID.h"

#include <functional>
#include <optional>


namespace gui
{

struct SlideStackPartial_msgFromUi;
struct SlideStackComplete_msgToUi;
struct SlideStackPartial_msgToUi;

struct SlideStackOrder_msgFromUi;

struct ActiveSlide_msgFromUi;
struct ActiveSlide_msgToUi;

struct SlideCommonPropertiesPartial_msgFromUi;
struct SlideCommonPropertiesPartial_msgToUi;
struct SlideCommonPropertiesComplete_msgToUi;

struct SlideHeaderComplete_msgToUi;
struct SlideHeaderPartial_msgFromUi;
struct SlideViewDataComplete_msgToUi;
struct SlideViewDataPartial_msgToUi;
struct SlideViewDataPartial_msgFromUi;
struct SlideTxDataComplete_msgToUi;
struct SlideTxDataPartial_msgToUi;
struct SlideTxDataPartial_msgFromUi;

struct MoveToSlide_msgFromUi;



/*** Functions that publish data from UI to APP ***/

/// Functional to notify the app of some slide stack data from the UI
using SlideStackPartial_msgFromUi_PublisherType =
    std::function< void ( const SlideStackPartial_msgFromUi& ) >;

/// Functional to notify the app of the slide stack order from the UI
using SlideStackOrder_msgFromUi_PublisherType =
    std::function< void ( const SlideStackOrder_msgFromUi& ) >;

/// Functional to notify the app of the active slide from the UI
using ActiveSlide_msgFromUi_PublisherType =
    std::function< void ( const ActiveSlide_msgFromUi& ) >;

/// Functional to notify the app of some (partial) slide stack common properties from the UI
using SlideCommonPropertiesPartial_msgFromUi_PublisherType =
    std::function< void ( const SlideCommonPropertiesPartial_msgFromUi& ) >;

/// Functional to notify the app of a slide's partial header data from the UI
using SlideHeaderPartial_msgFromUi_PublisherType =
    std::function< void ( const SlideHeaderPartial_msgFromUi& ) >;

/// Functional to notify the app of a slide's partial view data from the UI
using SlideViewDataPartial_msgFromUi_PublisherType =
    std::function< void ( const SlideViewDataPartial_msgFromUi& ) >;

/// Functional to notify the app of a slide's partial transformation data from the UI
using SlideTxDataPartial_msgFromUi_PublisherType =
    std::function< void ( const SlideTxDataPartial_msgFromUi& ) >;

/// Function for UI to notify app of slide to move
using MoveToSlide_msgFromUi_PublisherType =
    std::function< void( const MoveToSlide_msgFromUi& ) >;


/*** Functions that publish data from APP to UI ***/

/// Functional for modifying all slide stack data in UI
using SlideStackComplete_msgToUi_PublisherType =
    std::function< void ( const SlideStackComplete_msgToUi& ) >;

/// Functional for modifying some slide stack data in UI
using SlideStackPartial_msgToUi_PublisherType =
    std::function< void ( const SlideStackPartial_msgToUi& ) >;

/// Functional to notify the UI of the active slide from the app
using ActiveSlide_msgToUi_PublisherType =
    std::function< void ( const ActiveSlide_msgToUi& ) >;

/// Functional to notify the UI of some (partial) slide stack common properties from the app
using SlideCommonPropertiesPartial_msgToUi_PublisherType =
    std::function< void ( const SlideCommonPropertiesPartial_msgToUi& ) >;

/// Functional to notify the UI of all (complete) slide stack common properties from the app
using SlideCommonPropertiesComplete_msgToUi_PublisherType =
    std::function< void ( const SlideCommonPropertiesComplete_msgToUi& ) >;

/// Functional to notify the UI of a slide's complete header data from the app
using SlideHeaderComplete_msgToUi_PublisherType =
    std::function< void ( const SlideHeaderComplete_msgToUi& ) >;

/// Functional to notify the UI of a slide's complete view data from the app
using SlideViewDataComplete_msgToUi_PublisherType =
    std::function< void ( const SlideViewDataComplete_msgToUi& ) >;

/// Functional to notify the UI of a slide's partial view data from the app
using SlideViewDataPartial_msgToUi_PublisherType =
    std::function< void ( const SlideViewDataPartial_msgToUi& ) >;

/// Functional to notify the UI of a slide's complete transformation data from the app
using SlideTxDataComplete_msgToUi_PublisherType =
    std::function< void ( const SlideTxDataComplete_msgToUi& ) >;

/// Functional to notify the UI of a slide's partial transformation data from the app
using SlideTxDataPartial_msgToUi_PublisherType =
    std::function< void ( const SlideTxDataPartial_msgToUi& ) >;


/*** Functions that respond with data from APP to requests from UI ***/

/// Functional for the app to respond to all slide stack data request from UI
using SlideStackComplete_msgToUi_ResponderType =
    std::function< SlideStackComplete_msgToUi (void) >;

/// Functional for the app to respond to active slide request from UI
using ActiveSlide_msgToUi_ResponderType =
    std::function< ActiveSlide_msgToUi (void) >;

/// Functional for the app to respond to slide stack common properties request from the UI
using SlideCommonPropertiesComplete_msgToUi_ResponderType =
    std::function< SlideCommonPropertiesComplete_msgToUi (void) >;

/// Functional for the app to respond to UI request for a slide's header
using SlideHeaderComplete_msgToUi_ResponderType =
    std::function< std::optional< SlideHeaderComplete_msgToUi > ( const UID& slideUid ) >;

/// Functional for the app to respond to UI request for a slide's view data
using SlideViewDataComplete_msgToUi_ResponderType =
    std::function< std::optional< SlideViewDataComplete_msgToUi > ( const UID& slideUid ) >;

/// Functional for the app to respond to UI request for a slide's transformation data
using SlideTxDataComplete_msgToUi_ResponderType =
    std::function< std::optional< SlideTxDataComplete_msgToUi > ( const UID& slideUid ) >;

} // namespace gui

#endif // GUI_DOCKS_PUBLIC_TYPES_H
