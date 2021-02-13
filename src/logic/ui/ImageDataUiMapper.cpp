#include "logic/ui/ImageDataUiMapper.h"
#include "logic/ui/details/PackageHeader.h"

#include "logic/managers/ActionManager.h"
#include "logic/managers/AssemblyManager.h"
#include "logic/managers/ConnectionManager.h"
#include "logic/managers/DataManager.h"

#include "common/UID.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <boost/filesystem.hpp>

#include <cmath>
#include <functional>
#include <iostream>


namespace
{

/**
 * @brief Compute a sensible step size by which the user can iterate over the given range
 * using controls in the UI (e.g. slider, spin boxes)
 *
 * @param range Range of numbers to show in the widget
 *
 * @return Sensible step size to cover the range
 */
double computeSingleStep( double range )
{
    const double a = std::abs( range );

    if ( a > 0.0 )
    {
        double l = std::floor( std::log10( a ) );
        return std::pow( 10.0, l - 2 );
    }
    else
    {
        return 0.0;
    }
}


/**
 * @brief Compute a sensible number of decimals (after the decimal point) with which to
 * to display numbers within a given range in the UI (e.g. for spin boxes)
 *
 * @param range Range of numbers to show in the widget
 * @return  Sensible number of decimals to display for the range
 */
int computeNumDecimals( double range )
{
    const double a = std::abs( range );

    if ( a > 0.0 )
    {
        int l = static_cast<int>( std::floor( std::log10( a ) ) );
        return std::max( 3 - l, 0 );
    }
    else
    {
        return 0;
    }
}

} // anonymous


struct ImageDataUiMapper::Impl
{
    Impl( ActionManager&, AssemblyManager&, DataManager&, AllViewsUpdaterType );
    ~Impl() = default;


    /// Update image selection in app from UI changes
    void updateAppFromUi( const gui::ImageSelections_msgFromUi& );

    /// Update active image properties in app from UI changes
    void updateAppFromUi( const gui::ImagePropertiesPartial_msgFromUi& );

    /// Update active image transformation in app from UI changes
    void updateAppFromUi( const gui::ImageTransformation_msgFromUi& );


    /// Update the UI due to a generic image data change
    void updateUiFromImageDataChange( const UID& imageUid );

    /// Update the UI due to an image window/level change
    void updateUiFromImageWindowLevelChange( const UID& imageUid );

    /// Update the UI due to an image transformation change
    void updateUiFromImageTransformationChange( const UID& imageUid );

    /// Update the UI due to an image slice assembly rendering properties change
    void updateUiFromImageSliceAssemblyRenderingPropertiesChange(
            const UID& imageUid, const ImageSliceAssemblyRenderingProperties& );

    //    /// Update the UI due to a label mesh assembly rendering properties change
    //    void updateUiFromLabelMeshAssemblyRenderingPropertiesChange( const MeshAssemblyRenderingProperties& );

//    /// Update the UI due to an iso-surface mesh assembly rendering properties change
//    void updateUiFromIsoMeshAssemblyRenderingPropertiesChange( const MeshAssemblyRenderingProperties& );

//    /// Update the UI due to slide assembly rendering properties change
//    void updateUiFromSlideAssemblyRenderingPropertiesChange( const SlideStackAssemblyRenderingProperties& );


    /// Update the UI due to color map change
    void updateUiFromImageColorMapDataChange( const UID& colorMapUid );

    /// Update the UI due to image selection change
    void updateUiFromImageSelectionChange();


    /// Get the current image selection data
    gui::ImageSelections_msgToUi getImageSelection() const;

    /// Get the current image color map data
    gui::ImageColorMaps_msgToUi getImageColorMaps() const;

    /// Get all current properties of the active image (if there is one)
    boost::optional< gui::ImagePropertiesComplete_msgToUi > getActiveImageProperties() const;

    /// Get current window/level properties of the active image (if there is one)
    boost::optional< gui::ImagePropertiesPartial_msgToUi > getActiveImageWindowLevel() const;

    /// Get header of active image (if there is one)
    boost::optional< gui::ImageHeader_msgToUi > getActiveImageHeader() const;

    /// Get transformation of active image (if there is one)
    boost::optional< gui::ImageTransformation_msgToUi > getActiveImageTransformation() const;


    ActionManager& m_actionManager;
    AssemblyManager& m_assemblyManager;
    DataManager& m_dataManager;


    /// Function that updates all rendered views
    AllViewsUpdaterType m_allViewsUpdater;

    /// Function that updates image selection in the UI
    gui::ImageSelections_msgToUi_PublisherType m_imageSelectionPublisher;

    /// Function that updates image color maps in the UI
    gui::ImageColorMaps_msgToUi_PublisherType m_imageColorMapPublisher;

    /// Function that updates some image properties in the UI
    gui::ImagePropertiesPartial_msgToUi_PublisherType m_partialImagePropertiesPublisher;

    /// Function that updates all image properties in the UI
    gui::ImagePropertiesComplete_msgToUi_PublisherType m_fullImagePropertiesPublisher;

    /// Function that updates the image transformation in the UI
    gui::ImageTransformation_msgToUi_PublisherType m_imageTransformationPublisher;
};


ImageDataUiMapper::ImageDataUiMapper(
        ActionManager& actionManager,
        AssemblyManager& assemblyManager,
        DataManager& dataManager,
        AllViewsUpdaterType viewsUpdater )
    :
      m_impl( std::make_unique<Impl>( actionManager, assemblyManager, dataManager, viewsUpdater ) )
{}

ImageDataUiMapper::~ImageDataUiMapper() = default;


void ImageDataUiMapper::setImageSelections_msgFromUi(
        const gui::ImageSelections_msgFromUi& msg )
{
    m_impl->updateAppFromUi( msg );
}

void ImageDataUiMapper::setImagePropertiesPartial_msgFromUi(
        const gui::ImagePropertiesPartial_msgFromUi& msg )
{
    m_impl->updateAppFromUi( msg );
}

void ImageDataUiMapper::setImageTransformation_msgFromUi(
        const gui::ImageTransformation_msgFromUi& msg )
{
    m_impl->updateAppFromUi( msg );
}


void ImageDataUiMapper::setImageSelectionsPublisher_msgToUi(
        gui::ImageSelections_msgToUi_PublisherType publisher )
{
    m_impl->m_imageSelectionPublisher = publisher;
}

void ImageDataUiMapper::setImageColorMapsPublisher_msgToUi(
        gui::ImageColorMaps_msgToUi_PublisherType publisher )
{
    m_impl->m_imageColorMapPublisher = publisher;
}

void ImageDataUiMapper::setImagePropertiesPartialPublisher_msgToUi(
        gui::ImagePropertiesPartial_msgToUi_PublisherType publisher )
{
    m_impl->m_partialImagePropertiesPublisher = publisher;
}

void ImageDataUiMapper::setImagePropertiesCompletePublisher_msgToUi(
        gui::ImagePropertiesComplete_msgToUi_PublisherType publisher )
{
    m_impl->m_fullImagePropertiesPublisher = publisher;
}

void ImageDataUiMapper::setImageTransformationPublisher_msgToUi(
        gui::ImageTransformation_msgToUi_PublisherType publisher )
{
    m_impl->m_imageTransformationPublisher = publisher;
}


gui::ImageSelections_msgToUi ImageDataUiMapper::getImageSelections_msgToUi() const
{
    return m_impl->getImageSelection();
}

gui::ImageColorMaps_msgToUi ImageDataUiMapper::getImageColorMaps_msgToUi() const
{
    return m_impl->getImageColorMaps();
}


boost::optional< gui::ImagePropertiesComplete_msgToUi >
ImageDataUiMapper::getImagePropertiesComplete_msgToUi( const UID& imageUid ) const
{
    const auto activeImageUid = m_impl->m_dataManager.activeImageUid();
    if ( ! activeImageUid || *activeImageUid != imageUid )
    {
        // Request of properties of image that is not active
        std::cerr << "Requested properties of non-active image " << imageUid << std::endl;
        return boost::none;
    }

    return m_impl->getActiveImageProperties();
}


boost::optional< gui::ImageHeader_msgToUi >
ImageDataUiMapper::getImageHeader_msgToUi( const UID& imageUid ) const
{
    const auto activeImageUid = m_impl->m_dataManager.activeImageUid();
    if ( ! activeImageUid || *activeImageUid != imageUid )
    {
        // Request of header of image that is not active
        std::cerr << "Requested header of non-active image " << imageUid << std::endl;
        return boost::none;
    }

    return m_impl->getActiveImageHeader();
}


boost::optional< gui::ImageTransformation_msgToUi >
ImageDataUiMapper::getImageTransformation_msgToUi( const UID& imageUid ) const
{
    const auto activeImageUid = m_impl->m_dataManager.activeImageUid();
    if ( ! activeImageUid || *activeImageUid != imageUid )
    {
        // Request of transformation of image that is not active
        std::cerr << "Requested transformation of non-active image " << imageUid << std::endl;
        return boost::none;
    }

    return m_impl->getActiveImageTransformation();
}


void ImageDataUiMapper::slot_updateUiFromImageWindowLevelChange( const UID& imageUid )
{
    if ( ! m_impl ) return;
    m_impl->updateUiFromImageWindowLevelChange( imageUid );
}


void ImageDataUiMapper::slot_updateUiFromImageTransformationChange( const UID& imageUid )
{
    if ( ! m_impl ) return;
    m_impl->updateUiFromImageTransformationChange( imageUid );
}

////////////////////////////////////////////////////////////////////////////////////////////////////


ImageDataUiMapper::Impl::Impl(
        ActionManager& actionManager,
        AssemblyManager& assemblyManager,
        DataManager& dataManager,
        AllViewsUpdaterType viewsUpdater )
    :
      m_actionManager( actionManager ),
      m_assemblyManager( assemblyManager ),
      m_dataManager( dataManager ),

      m_allViewsUpdater( viewsUpdater ),

      m_imageSelectionPublisher( nullptr ),
      m_imageColorMapPublisher( nullptr ),
      m_partialImagePropertiesPublisher( nullptr ),
      m_fullImagePropertiesPublisher( nullptr )
{
    using std::placeholders::_1;
    using std::placeholders::_2;

    // Connect signal that image data changed to slot that updates UI
    m_dataManager.connectToImageDataChangedSignal(
                std::bind( &ImageDataUiMapper::Impl::updateUiFromImageDataChange, this, _1 ) );

    // Connect signal that image slice rendering property has changed to slot that updates UI.
    m_assemblyManager.connectToImageSliceAssemblyRenderingPropertiesChangedSignal(
                std::bind( &ImageDataUiMapper::Impl::updateUiFromImageSliceAssemblyRenderingPropertiesChange,
                           this, _1, _2 ) );
}


void ImageDataUiMapper::Impl::updateUiFromImageDataChange( const UID& imageUid )
{
    if ( ! m_imageSelectionPublisher ||
         ! m_imageColorMapPublisher ||
         ! m_fullImagePropertiesPublisher )
    {
        return;
    }

    // Send messages:
    /// @todo These 2 should go in separate callback:
    m_imageSelectionPublisher( getImageSelection() );
    m_imageColorMapPublisher( getImageColorMaps() );


    const auto activeImageUid = m_dataManager.activeImageUid();
    if ( ! activeImageUid || *activeImageUid != imageUid )
    {
        // Ignore changes not related to active image
        return;
    }

    if ( const auto fullProps = getActiveImageProperties() )
    {
        m_fullImagePropertiesPublisher( *fullProps );
    }
}


void ImageDataUiMapper::Impl::updateUiFromImageWindowLevelChange( const UID& imageUid )
{
    if ( ! m_partialImagePropertiesPublisher )
    {
        return;
    }

    const auto activeImageUid = m_dataManager.activeImageUid();
    if ( ! activeImageUid || *activeImageUid != imageUid )
    {
        // Ignore changes not related to active image
        return;
    }

    if ( const auto partialProps = getActiveImageWindowLevel() )
    {
        m_partialImagePropertiesPublisher( *partialProps );
    }
}


void ImageDataUiMapper::Impl::updateUiFromImageTransformationChange( const UID& imageUid )
{
    if ( ! m_imageTransformationPublisher )
    {
        return;
    }

    const auto activeImageUid = m_dataManager.activeImageUid();
    if ( ! activeImageUid || *activeImageUid != imageUid )
    {
        // Ignore changes not related to active image
        return;
    }

    if ( const auto tx = getActiveImageTransformation() )
    {
        m_imageTransformationPublisher( *tx );
    }
}


void ImageDataUiMapper::Impl::updateUiFromImageSliceAssemblyRenderingPropertiesChange(
        const UID& imageUid, const ImageSliceAssemblyRenderingProperties& props )
{
    if ( ! m_partialImagePropertiesPublisher )
    {
        return;
    }

    gui::ImagePropertiesPartial_msgToUi partialProps;
    partialProps.m_imageUid = imageUid;
    partialProps.m_commonProperties.m_planesVisibleIn2dViewsChecked = props.m_visibleIn2dViews;
    partialProps.m_commonProperties.m_planesVisibleIn3dViewsChecked = props.m_visibleIn3dViews;
    partialProps.m_commonProperties.m_planesAutoHidingChecked = props.m_useAutoHidingMode;

    m_partialImagePropertiesPublisher( partialProps );
}

//void ImageDataUiMapper::Impl::updateUiFromIsoMeshAssemblyRenderingPropertiesChange(
//        const MeshAssemblyRenderingProperties& )
//{
//    // need to change UI for this
//    // make window dedicated to Rendering Properties
//}

//void ImageDataUiMapper::Impl::updateUiFromLabelMeshAssemblyRenderingPropertiesChange(
//        const MeshAssemblyRenderingProperties& )
//{
//    // need to change UI for this
//    // make window dedicated to Rendering Properties
//}

//void ImageDataUiMapper::Impl::updateUiFromSlideAssemblyRenderingPropertiesChange(
//        const SlideStackAssemblyRenderingProperties& )
//{
//    // need to change UI for this
//    // make window dedicated to Rendering Properties
//}


void ImageDataUiMapper::Impl::updateUiFromImageColorMapDataChange( const UID& /*colorMapUid*/ )
{
    // Color map data changed, so resend color maps to UI
    if ( m_imageColorMapPublisher )
    {
        m_imageColorMapPublisher( getImageColorMaps() );
    }

    // In case selected color map changed, resend all image properties
    // (including the currently selected color map index)
    if ( m_fullImagePropertiesPublisher )
    {
        if ( const auto props = getActiveImageProperties() )
        {
            m_fullImagePropertiesPublisher( *props );
        }
    }
}


void ImageDataUiMapper::Impl::updateUiFromImageSelectionChange()
{
    if ( m_imageSelectionPublisher )
    {
        m_imageSelectionPublisher( getImageSelection() );
    }

    // Since the image selections changed, resend the image properties
    if ( m_fullImagePropertiesPublisher )
    {
        if ( const auto props = getActiveImageProperties() )
        {
            m_fullImagePropertiesPublisher( *props );
        }
    }
}


void ImageDataUiMapper::Impl::updateAppFromUi( const gui::ImageSelections_msgFromUi& msg )
{
    // A new image was selected in UI: update the active image.

    // Make sure that the image UID is valid and that the index of the selection matches
    // the index of the image. If not, the data between UI and app is inconsistent.

    if ( ! msg.m_imageUid || ! msg.m_selectionIndex )
    {
        return;
    }

    const auto orderedIndex = m_dataManager.orderedImageIndex( *msg.m_imageUid );

    if ( ! orderedIndex )
    {
        std::cerr << "Invalid selected image UID " << *msg.m_imageUid << std::endl;
        return;
    }

    if ( *msg.m_selectionIndex != *orderedIndex )
    {
        std::cerr << "Invalid selected image index " << *msg.m_selectionIndex << std::endl;
        return;
    }

    if ( m_dataManager.setActiveImageUid( *msg.m_imageUid ) )
    {
        m_actionManager.updateImageSliceAssembly();
        m_actionManager.updateLabelMeshAssembly();
        m_actionManager.updateIsoMeshAssembly();
        m_actionManager.updateSlideStackAssembly();

        if ( m_allViewsUpdater )
        {
            m_allViewsUpdater();
        }
    }
    else
    {
        std::cerr << "Invalid image UID " << *msg.m_imageUid << std::endl;
    }
}


void ImageDataUiMapper::Impl::updateAppFromUi( const gui::ImagePropertiesPartial_msgFromUi& msg )
{
    // Updates are applied to component 0 of the image.
    static constexpr size_t sk_comp = 0;

    const auto activeImageUid = m_dataManager.activeImageUid();
    if ( ! activeImageUid || *activeImageUid != msg.m_imageUid )
    {
        // Something has gone wrong, since message from UI is for changes done on image
        // that is not the active one.
        std::cerr << "Error: cannot update properties of non-active image." << std::endl;
        return;
    }

    auto activeImageRecord = m_dataManager.activeImageRecord().lock();
    if ( ! activeImageRecord )
    {
        return;
    }

    auto cpuRecord = activeImageRecord->cpuData();
    if ( ! cpuRecord )
    {
        return;
    }

    // All fields in the message from the UI are optional,
    // so check each prior to using its value:

    if ( const auto displayName = msg.m_properties.m_displayName )
    {
        cpuRecord->setDisplayName( *displayName );
    }

    if ( const auto opacity = msg.m_properties.m_opacityValue )
    {
        if ( 0 <= *opacity && *opacity <= 100 )
        {
            cpuRecord->setOpacity( sk_comp, static_cast<double>( *opacity / 100.0 ) );
        }
    }

    if ( const auto window = msg.m_properties.m_windowValues )
    {
        cpuRecord->setWindowWidth( sk_comp, window->second - window->first );
        cpuRecord->setLevel( sk_comp, 0.5 * ( window->first + window->second ) );
    }

    if ( const auto thresh = msg.m_properties.m_threshValues )
    {
        cpuRecord->setThresholdLow( sk_comp, thresh->first );
        cpuRecord->setThresholdHigh( sk_comp, thresh->second );
    }

    if ( const auto samplingNn = msg.m_properties.m_samplingNnChecked )
    {
        imageio::ImageSettings::InterpolationMode interpMode;
        tex::MinificationFilter minFilter;
        tex::MagnificationFilter magFilter;

        if ( true == *samplingNn )
        {
            interpMode = imageio::ImageSettings::InterpolationMode::NearestNeighbor;
            minFilter = tex::MinificationFilter::Nearest;
            magFilter = tex::MagnificationFilter::Nearest;
        }
        else
        {
            interpMode = imageio::ImageSettings::InterpolationMode::Linear;
            minFilter = tex::MinificationFilter::Linear;
            magFilter = tex::MagnificationFilter::Linear;
        }

        cpuRecord->setInterpolationMode( sk_comp, interpMode );

        if ( auto gpuRecord = activeImageRecord->gpuData() )
        {
            if ( auto texture = gpuRecord->texture().lock() )
            {
                texture->setMinificationFilter( minFilter );
                texture->setMagnificationFilter( magFilter );
            }
            else
            {
                std::cerr << "Null GPU image texture" << std::endl;
            }
        }
        else
        {
            std::cerr << "Null GPU image record" << std::endl;
        }
    }

    if ( const auto cmapIndex = msg.m_properties.m_colorMapIndex )
    {
        if ( auto colorMapUid = m_dataManager.orderedImageColorMapUid( *cmapIndex ) )
        {
            m_dataManager.associateColorMapWithImage( *activeImageUid, *colorMapUid );
            m_assemblyManager.updateImageColorMap( *colorMapUid, false );
        }
    }


    // Set common image slie rendering properties in app:
    /// @todo Put this in different function

    if ( const auto planesVisibleIn2d = msg.m_commonProperties.m_planesVisibleIn2dViewsChecked )
    {
        m_assemblyManager.setImageSlicesVisibleIn2dViews( *planesVisibleIn2d );
    }
    if ( const auto planesVisibleIn3d = msg.m_commonProperties.m_planesVisibleIn3dViewsChecked )
    {
        m_assemblyManager.setImageSlicesVisibleIn3dViews( *planesVisibleIn3d );
    }
    if ( const auto planesAutoHide = msg.m_commonProperties.m_planesAutoHidingChecked )
    {
        m_assemblyManager.setImageSlicesAutoHiding( *planesAutoHide );
    }


    // Trigger render of views:
    if ( m_allViewsUpdater )
    {
        m_allViewsUpdater();
    }
}


void ImageDataUiMapper::Impl::updateAppFromUi( const gui::ImageTransformation_msgFromUi& msg )
{
    if ( ! m_imageTransformationPublisher )
    {
        return;
    }

    const auto activeImageUid = m_dataManager.activeImageUid();
    if ( ! activeImageUid || *activeImageUid != msg.m_imageUid )
    {
        // Ignore changes not related to active image
        return;
    }

    bool doUpdate = false;

    if ( msg.m_set_world_O_subject_identity )
    {
        auto record = m_dataManager.activeImageRecord().lock();
        if ( record && record->cpuData() )
        {
            record->cpuData()->resetSubjectToWorld();
            doUpdate = true;
        }
    }

    // Trigger render of views:
    if ( doUpdate && m_allViewsUpdater )
    {
        // Send message to update the image transformation:
        // (Needed to inform UI of new transformation)
        if ( const auto tx = getActiveImageTransformation() )
        {
            m_imageTransformationPublisher( *tx );
        }

        m_allViewsUpdater();
    }
}


gui::ImageSelections_msgToUi ImageDataUiMapper::Impl::getImageSelection() const
{
    gui::ImageSelections_msgToUi msg;

    const auto activeImageUid = m_dataManager.activeImageUid();
    if ( ! activeImageUid )
    {
        return msg;
    }

    const auto activeIndex = m_dataManager.orderedImageIndex( *activeImageUid );
    if ( ! activeIndex )
    {
        return msg;
    }

    msg.m_selectionIndex = static_cast<int>( *activeIndex );

    // Iterate over all ordered image UIDs
    for ( const auto& imageUid : m_dataManager.orderedImageUids() )
    {
        auto imageRecord = m_dataManager.imageRecord( imageUid ).lock();

        if ( imageRecord && imageRecord->cpuData() )
        {
            gui::ImageSelectionItem item;
            item.m_imageUid = imageUid;
            item.m_displayName = imageRecord->cpuData()->settings().displayName();

            msg.m_selectionItems.emplace_back( std::move( item ) );
        }
    }

    return msg;
}


gui::ImageColorMaps_msgToUi ImageDataUiMapper::Impl::getImageColorMaps() const
{
    gui::ImageColorMaps_msgToUi msg;

    for ( const auto& cmapUid : m_dataManager.orderedImageColorMapUids() )
    {
        auto cmapRecord = m_dataManager.imageColorMapRecord( cmapUid ).lock();
        if ( ! cmapRecord )
        {
            std::cerr << "Image color map with UID " << cmapUid << " is null" << std::endl;
            continue;
        }

        auto cmap = cmapRecord->cpuData();
        if ( ! cmap )
        {
            std::cerr << "Image color map with UID " << cmapUid << " is null" << std::endl;
            continue;
        }

        gui::ImageColorMapItem cmapItem;

        cmapItem.m_colorMapUid = cmapUid;
        cmapItem.m_name = cmap->name();
        cmapItem.m_description = cmap->description();

        // Each color has four components (RGBA), each component occupying four bytes:
        const size_t bufferLength = ( cmap->hasPreviewMap() )
                ? 4 * cmap->numPreviewMapColors()
                : 4 * cmap->numColors();

        cmapItem.m_iconBuffer.clear();
        cmapItem.m_iconBuffer.resize( bufferLength );

        const float* cmapComponents = ( cmap->hasPreviewMap() )
                ? cmap->getPreviewMap()
                : cmap->data_RGBA_F32();

        for ( size_t i = 0; i < bufferLength; ++i )
        {
            cmapItem.m_iconBuffer[i] = static_cast<uint8_t>( std::round( 255.0f * cmapComponents[i] ) );
        }

        msg.m_colorMapItems.emplace_back( std::move( cmapItem ) );
    }

    return msg;
}


boost::optional< gui::ImagePropertiesComplete_msgToUi >
ImageDataUiMapper::Impl::getActiveImageProperties() const
{
    static constexpr size_t sk_imageComp = 0;

    const auto activeImageUid = m_dataManager.activeImageUid();
    if ( ! activeImageUid )
    {
        return boost::none;
    }

    auto imageRecord = m_dataManager.imageRecord( *activeImageUid ).lock();
    if ( ! imageRecord || ! imageRecord->cpuData() ||
         ! imageRecord->cpuData()->imageBaseData() )
    {
        return boost::none;
    }

    auto cpuRecord = imageRecord->cpuData();
    const auto& settings = cpuRecord->settings();


    gui::ImagePropertiesComplete_msgToUi msg;
    msg.m_imageUid = *activeImageUid;

    msg.m_properties.m_path = cpuRecord->imageBaseData()->imageIOInfo().m_fileInfo.m_fileName;
    msg.m_properties.m_displayName = settings.displayName();

    /// @note The \c m_fileType property is not correctly set by ITK.
#if 0
    if ( ::itk::ImageIOBase::FileType::TypeNotApplicable ==
        cpuRecord->imageBaseData()->imageIOInfo().m_fileInfo.m_fileType )
    {
        msg.m_properties.m_loadedFromFile = false;
    }
    else
    {
        msg.m_properties.m_loadedFromFile = true;
    }
#endif

    msg.m_properties.m_loadedFromFile = boost::filesystem::exists(
                cpuRecord->imageBaseData()->imageIOInfo().m_fileInfo.m_fileName );


    boost::optional<UID> colorMapUid = m_dataManager.imageColorMapUid_of_image( *activeImageUid );

    if ( ! colorMapUid )
    {
        colorMapUid = m_dataManager.defaultImageColorMapUid();
    }

    if ( colorMapUid )
    {
        const auto colorMapIndex = m_dataManager.orderedImageColorMapIndex( *colorMapUid );

        if ( colorMapIndex )
        {
            msg.m_properties.m_colorMapIndex = static_cast<int>( *colorMapIndex );
        }
        else
        {
            std::cerr << "Image " << *activeImageUid << " has no color map" << std::endl;
        }
    }
    else
    {
        std::cerr << "Image " << *activeImageUid << " has no color map" << std::endl;
        return boost::none;
    }


    // Opacity:
    msg.m_properties.m_opacityRange = std::make_pair( 0, 100 );
    msg.m_properties.m_opacitySingleStep = 1;
    msg.m_properties.m_opacitySliderPageStep = 10;
    msg.m_properties.m_opacityValue = static_cast<int>( settings.opacity( 0 ) * 100.0 );


    // Window/level:
    const double level = settings.level( sk_imageComp );
    const double window = settings.window( sk_imageComp );
    const auto wRange = settings.levelRange( sk_imageComp );

    msg.m_properties.m_windowRange = wRange;
    msg.m_properties.m_windowSingleStep = computeSingleStep( wRange.second - wRange.first );
    msg.m_properties.m_windowSpinBoxesDecimals = computeNumDecimals( wRange.second - wRange.first );
    msg.m_properties.m_windowValues = std::make_pair( level - 0.5 * window, level + 0.5 * window );


    // Thresholding:
    const double thLow = settings.thresholdLow( sk_imageComp );
    const double thHigh = settings.thresholdHigh( sk_imageComp );
    const auto thRange = settings.thresholdRange( sk_imageComp );

    msg.m_properties.m_threshRange = thRange;
    msg.m_properties.m_threshSingleStep = computeSingleStep( thRange.second - thRange.first );
    msg.m_properties.m_threshSpinBoxesDecimals = computeNumDecimals( thRange.second - thRange.first );
    msg.m_properties.m_threshValues = std::make_pair( thLow, thHigh );


    // Image sampling:
    msg.m_properties.m_samplingNnChecked =
            ( imageio::ImageSettings::InterpolationMode::NearestNeighbor == settings.interpolationMode( 0 ) )
            ? true : false;

    msg.m_properties.m_samplingLinearChecked = ! ( msg.m_properties.m_samplingNnChecked() );


    // Image slice common properties:
    msg.m_commonProperties.m_planesVisibleIn2dViewsChecked =
            m_assemblyManager.getImageSliceRenderingProperties().m_visibleIn2dViews;

    msg.m_commonProperties.m_planesVisibleIn3dViewsChecked =
            m_assemblyManager.getImageSliceRenderingProperties().m_visibleIn3dViews;

    msg.m_commonProperties.m_planesAutoHidingChecked =
            m_assemblyManager.getImageSliceRenderingProperties().m_useAutoHidingMode;

    return msg;
}


boost::optional< gui::ImagePropertiesPartial_msgToUi >
ImageDataUiMapper::Impl::getActiveImageWindowLevel() const
{
    const auto activeImageUid = m_dataManager.activeImageUid();
    if ( ! activeImageUid )
    {
        return boost::none;
    }

    auto activeImageRecord = m_dataManager.imageRecord( *activeImageUid ).lock();
    if ( ! activeImageRecord || ! activeImageRecord->cpuData() ||
         ! activeImageRecord->cpuData()->imageBaseData() )
    {
        return boost::none;
    }

    auto cpuRecord = activeImageRecord->cpuData();

    const auto& settings = cpuRecord->settings();
    const double level = settings.level( 0 );
    const double window = settings.window( 0 );

    gui::ImagePropertiesPartial_msgToUi msg;
    msg.m_imageUid = *activeImageUid;
    msg.m_properties.m_windowValues = std::make_pair( level - 0.5 * window, level + 0.5 * window );

    return msg;
}


boost::optional< gui::ImageHeader_msgToUi >
ImageDataUiMapper::Impl::getActiveImageHeader() const
{
    const auto activeImageUid = m_dataManager.activeImageUid();
    if ( ! activeImageUid )
    {
        return boost::none;
    }

    auto activeImageRecord = m_dataManager.imageRecord( *activeImageUid ).lock();
    if ( ! activeImageRecord || ! activeImageRecord->cpuData() ||
         ! activeImageRecord->cpuData()->imageBaseData() )
    {
        return boost::none;
    }

    gui::ImageHeader_msgToUi msg;
    msg.m_imageUid = *activeImageUid;

    msg.m_items = details::packageImageHeaderForUi(
                activeImageRecord->cpuData()->header(),
                activeImageRecord->cpuData()->settings() );

    msg.m_subject_O_pixel = activeImageRecord->cpuData()->transformations().subject_O_pixel();

    return msg;
}


boost::optional< gui::ImageTransformation_msgToUi >
ImageDataUiMapper::Impl::getActiveImageTransformation() const
{
    const auto activeImageUid = m_dataManager.activeImageUid();
    if ( ! activeImageUid )
    {
        return boost::none;
    }

    auto activeImageRecord = m_dataManager.imageRecord( *activeImageUid ).lock();
    if ( ! activeImageRecord || ! activeImageRecord->cpuData() ||
         ! activeImageRecord->cpuData()->imageBaseData() )
    {
        return boost::none;
    }

    gui::ImageTransformation_msgToUi msg;
    msg.m_imageUid = *activeImageUid;
    msg.m_world_O_subject = activeImageRecord->cpuData()->transformations().world_O_subject();

    return msg;
}
