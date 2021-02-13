#ifndef GUI_DOCKS_PUBLIC_TYPES_H
#define GUI_DOCKS_PUBLIC_TYPES_H

#include "common/UID.h"

#include <functional>
#include <optional>


namespace gui
{

struct ImageSelections_msgFromUi;
struct ImageSelections_msgToUi;

struct ImageColorMaps_msgToUi;

struct ImageHeader_msgToUi;

struct ImagePropertiesPartial_msgFromUi;
struct ImagePropertiesPartial_msgToUi;
struct ImagePropertiesComplete_msgToUi;

struct ImageTransformation_msgFromUi;
struct ImageTransformation_msgToUi;

struct ParcellationSelections_msgFromUi;
struct ParcellationSelections_msgToUi;

struct ParcellationPropertiesPartial_msgFromUi;
struct ParcellationPropertiesPartial_msgToUi;
struct ParcellationPropertiesComplete_msgToUi;

struct ParcellationLabelsPartial_msgFromUi;
struct ParcellationLabelsComplete_msgToUi;


/// @todo Remove all of these definitions and use the templated ones from common/PublicTypes

/*** Functions that publish data from UI to APP ***/

/// Functional to notify the app of the image selection changes from the UI
using ImageSelections_msgFromUi_PublisherType =
    std::function< void ( const ImageSelections_msgFromUi& ) >;

/// Functional to notify the app of some (partial) image property changes from the UI
using ImagePropertiesPartial_msgFromUi_PublisherType =
    std::function< void ( const ImagePropertiesPartial_msgFromUi& ) >;

/// Functional to notify the app of image transformation changes from the UI
using ImageTransformation_msgFromUi_PublisherType =
    std::function< void ( const ImageTransformation_msgFromUi& ) >;


/// Functional to notify the app of the parcellation selection changes from the UI
using ParcellationSelection_msgFromUi_PublisherType =
    std::function< void ( const ParcellationSelections_msgFromUi& ) >;

/// Functional to notify the app of some (partial) parcellation property changes from the UI
using ParcellationPropertiesPartial_msgFromUi_PublisherType =
    std::function< void ( const ParcellationPropertiesPartial_msgFromUi& ) >;

/// Functional to notify the app of some (partial) parcellation label changes from the UI
using ParcellationLabelsPartial_msgFromUi_PublisherType =
    std::function< void ( const ParcellationLabelsPartial_msgFromUi& ) >;



/*** Functions that publish data from APP to UI ***/

/// Functional for modifying image selection data in UI
using ImageSelections_msgToUi_PublisherType =
    std::function< void ( const ImageSelections_msgToUi& ) >;

/// Functional for modifying image color map data in UI
using ImageColorMaps_msgToUi_PublisherType =
    std::function< void ( const ImageColorMaps_msgToUi& ) >;

/// Functional for modifying some image property parameters in UI from app data
using ImagePropertiesPartial_msgToUi_PublisherType =
    std::function< void ( const ImagePropertiesPartial_msgToUi& ) >;

/// Functional for modifying all image property parameters in UI from app data
using ImagePropertiesComplete_msgToUi_PublisherType =
    std::function< void ( const ImagePropertiesComplete_msgToUi& ) >;

/// Functional for modifying image transformation data in UI from app data
using ImageTransformation_msgToUi_PublisherType =
    std::function< void ( const ImageTransformation_msgToUi& ) >;


/// Functional for modifying parcellation selection data in UI
using ParcellationSelections_msgToUi_PublisherType =
    std::function< void ( const ParcellationSelections_msgToUi& ) >;

/// Functional for modifying some parcellation property parameters in UI from app data
using ParcellationPropertiesPartial_msgToUi_PublisherType =
    std::function< void ( const ParcellationPropertiesPartial_msgToUi& ) >;

/// Functional for modifying all parcellation property parameters in UI from app data
using ParcellationPropertiesComplete_msgToUi_PublisherType =
    std::function< void ( const ParcellationPropertiesComplete_msgToUi& ) >;

/// Functional for modifying some parcellation labels in UI from app data
using ParcellationLabelsComplete_msgToUi_PublisherType =
    std::function< void ( const ParcellationLabelsComplete_msgToUi& ) >;


/*** Functions that respond with data from APP to requests from UI ***/

/// Functional for the app to respond to image selection request from UI
using ImageSelections_msgToUi_ResponderType =
    std::function< ImageSelections_msgToUi (void) >;

/// Functional for the app to respond to image color map request from UI
using ImageColorMaps_msgToUi_ResponderType =
    std::function< ImageColorMaps_msgToUi (void) >;

/// Functional for the app to respond to request from UI for all properties of a given image
using ImagePropertiesComplete_msgToUi_ResponderType =
    std::function< std::optional< ImagePropertiesComplete_msgToUi > ( const UID& imageUid ) >;

/// Functional for the app to respond to request from UI for the header of a given image
using ImageHeader_msgToUi_ResponderType =
    std::function< std::optional< ImageHeader_msgToUi > ( const UID& imageUid ) >;

/// Functional for the app to respond to request from UI for the transformation of a given image
using ImageTransformation_msgToUi_ResponderType =
    std::function< std::optional< ImageTransformation_msgToUi > ( const UID& imageUid ) >;


/// Functional for the app to respond to parcellation selection request from UI
using ParcellationSelections_msgToUi_ResponderType =
    std::function< ParcellationSelections_msgToUi (void) >;

/// Functional for the app to respond to request from UI for all properties of a given parcellation
using ParcellationPropertiesComplete_msgToUi_ResponderType =
    std::function< std::optional< ParcellationPropertiesComplete_msgToUi > ( const UID& parcelUid ) >;

/// Functional for the app to respond to request from UI for the header of a given parcellation
using ParcellationHeader_msgToUi_ResponderType =
    std::function< std::optional< ImageHeader_msgToUi > ( const UID& parcelUid ) >;

/// Functional for the app to respond to request from UI for all labels of a given parcellation
using ParcellationLabelsComplete_msgToUi_ResponderType =
    std::function< std::optional< ParcellationLabelsComplete_msgToUi > ( const UID& parcelUid ) >;

} // namespace gui

#endif // GUI_DOCKS_PUBLIC_TYPES_H
