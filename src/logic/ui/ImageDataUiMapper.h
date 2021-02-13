#ifndef IMAGE_DATA_UI_MAPPER_H
#define IMAGE_DATA_UI_MAPPER_H

#include "gui/docks/PublicTypes.h"
#include "gui/messages/image/ImageColorMapData.h"
#include "gui/messages/image/ImageHeaderData.h"
#include "gui/messages/image/ImagePropertyData.h"
#include "gui/messages/image/ImageSelectionData.h"
#include "gui/messages/image/ImageTransformationData.h"

#include "common/PublicTypes.h"

#include <boost/optional.hpp>

#include <functional>
#include <memory>


class ActionManager;
class AssemblyManager;
class DataManager;


/**
 * @brief Class for connecting image data between the Application and the UI
 */
class ImageDataUiMapper
{
public:

    ImageDataUiMapper( ActionManager&, AssemblyManager&, DataManager&, AllViewsUpdaterType );
    ~ImageDataUiMapper();


    /// Update image selection in the app from UI changes
    void setImageSelections_msgFromUi( const gui::ImageSelections_msgFromUi& );

    /// Update image properties in the app from UI changes
    void setImagePropertiesPartial_msgFromUi( const gui::ImagePropertiesPartial_msgFromUi& );

    /// Update image transformation in the app from UI changes
    void setImageTransformation_msgFromUi( const gui::ImageTransformation_msgFromUi& );


    /// Set the functional that updates the UI with image selection from the app
    void setImageSelectionsPublisher_msgToUi( gui::ImageSelections_msgToUi_PublisherType );

    /// Set the functional that updates the UI with image color maps from the app
    void setImageColorMapsPublisher_msgToUi( gui::ImageColorMaps_msgToUi_PublisherType );

    /// Set the functional that updates the UI with some image properties from the app
    void setImagePropertiesPartialPublisher_msgToUi( gui::ImagePropertiesPartial_msgToUi_PublisherType );

    /// Set the functional that updates the UI with all image properties from the app
    void setImagePropertiesCompletePublisher_msgToUi( gui::ImagePropertiesComplete_msgToUi_PublisherType );

    /// Set the functional that updates the UI with the image transformation from the app
    void setImageTransformationPublisher_msgToUi( gui::ImageTransformation_msgToUi_PublisherType );


    /// Respond to UI request for image selection
    gui::ImageSelections_msgToUi getImageSelections_msgToUi() const;

    /// Respond to UI request for image color maps
    gui::ImageColorMaps_msgToUi getImageColorMaps_msgToUi() const;

    /// Respond to UI request for all properties of given image
    boost::optional< gui::ImagePropertiesComplete_msgToUi >
    getImagePropertiesComplete_msgToUi( const UID& imageUid ) const;

    /// Respond to UI request for header of given image
    boost::optional< gui::ImageHeader_msgToUi >
    getImageHeader_msgToUi( const UID& imageUid ) const;

    /// Respond to UI request for transformation of given image
    boost::optional< gui::ImageTransformation_msgToUi >
    getImageTransformation_msgToUi( const UID& imageUid ) const;


    /// Slot that updates UI with window/level change. There is a specific slot for this change,
    /// as it can occur independent of any other changes to image properties in the app.
    void slot_updateUiFromImageWindowLevelChange( const UID& imageUid );

    /// Slot that updates UI with image transformation change. There is a specific slot for this change,
    /// as it can occur independent of any other changes to image properties in the app.
    void slot_updateUiFromImageTransformationChange( const UID& imageUid );


private:

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

#endif // IMAGE_DATA_UI_MAPPER_H
