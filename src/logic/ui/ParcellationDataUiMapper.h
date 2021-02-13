#ifndef PARCELLATION_DATA_UI_MAPPER_H
#define PARCELLATION_DATA_UI_MAPPER_H

#include "gui/docks/PublicTypes.h"
#include "gui/messages/image/ImageHeaderData.h"
#include "gui/messages/parcellation/ParcellationLabelData.h"
#include "gui/messages/parcellation/ParcellationPropertyData.h"
#include "gui/messages/parcellation/ParcellationSelectionData.h"

#include "common/PublicTypes.h"

#include <functional>
#include <memory>
#include <optional>


class ActionManager;
class AssemblyManager;
class DataManager;


/**
 * @brief Class for connecting parcellation data between the Application and the UI
 */
class ParcellationDataUiMapper
{
public:

    ParcellationDataUiMapper( ActionManager&, AssemblyManager&, DataManager&, AllViewsUpdaterType );
    ~ParcellationDataUiMapper();


    /// Update parcellation selection in the app from UI changes
    void setParcellationSelections_fromUi( const gui::ParcellationSelections_msgFromUi& );

    /// Update parcellation properties in the app from UI changes
    void setParcellationPropertiesPartial_fromUi( const gui::ParcellationPropertiesPartial_msgFromUi& );

    /// Update parcellation labels in the app from UI changes
    void setParcellationLabelsPartial_fromUi( const gui::ParcellationLabelsPartial_msgFromUi& );


    /// Set the functional that updates the UI with parcellation selection from the app
    void setParcellationSelectionsPublisher_msgToUi( gui::ParcellationSelections_msgToUi_PublisherType );

    /// Set the functional that updates the UI with some (partial) parcellation properties from the app
    void setParcellationPropertiesPartialPublisher_msgToUi( gui::ParcellationPropertiesPartial_msgToUi_PublisherType );

    /// Set the functional that updates the UI with all (complete) parcellation properties from the app
    void setParcellationPropertiesCompletePublisher_msgToUi( gui::ParcellationPropertiesComplete_msgToUi_PublisherType );

    /// Set the functional that updates the UI with all (complete) parcellation labels from the app
    void setParcellationLabelsCompletePublisher_msgToUi( gui::ParcellationLabelsComplete_msgToUi_PublisherType );


    /// Respond to UI request for parcellation selection
    gui::ParcellationSelections_msgToUi getParcellationSelections_msgToUi() const;

    /// Respond to UI request for all properties of given parcellation
    std::optional< gui::ParcellationPropertiesComplete_msgToUi >
    getParcellationPropertiesComplete_msgToUi( const UID& parcelUid ) const;

    /// Respond to UI request for all labels of given parcellation
    std::optional< gui::ParcellationLabelsComplete_msgToUi >
    getParcellationLabelsComplete_msgToUi( const UID& parcelUid ) const;

    /// Respond to UI request for header of given parcellation
    std::optional< gui::ImageHeader_msgToUi >
    getParcellationHeader_msgToUi( const UID& parcelUid ) const;


private:

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

#endif // PARCELLATION_DATA_UI_MAPPER_H
