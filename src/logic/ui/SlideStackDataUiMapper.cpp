#include "logic/ui/SlideStackDataUiMapper.h"

#include "common/CoordinateFrame.h"
#include "common/HZeeException.hpp"
#include "common/UID.h"

#include "logic/managers/ActionManager.h"
#include "logic/managers/AssemblyManager.h"
#include "logic/managers/DataManager.h"
#include "logic/managers/InteractionManager.h"
#include "logic/data/DataLoading.h"

#include "slideio/SlideHelper.h"

#include <boost/range/algorithm.hpp>

#include <iostream>
#include <limits>
#include <list>
#include <sstream>


struct SlideStackDataUiMapper::Impl
{
    Impl( ActionManager&, AssemblyManager&, DataManager&, InteractionManager&,
          AllViewsUpdaterType );

    ~Impl() = default;


    void updateAppFromUi( const gui::SlideStackPartial_msgFromUi& );
    void updateAppFromUi( const gui::SlideStackOrder_msgFromUi& );
    void updateAppFromUi( const gui::ActiveSlide_msgFromUi& );
    void updateAppFromUi( const gui::SlideCommonPropertiesPartial_msgFromUi& );
    void updateAppFromUi( const gui::SlideHeaderPartial_msgFromUi& );
    void updateAppFromUi( const gui::SlideViewDataPartial_msgFromUi& );
    void updateAppFromUi( const gui::SlideTxDataPartial_msgFromUi& );
    void updateAppFromUi( const gui::MoveToSlide_msgFromUi& );


    gui::SlideStackComplete_msgToUi getSlideStack() const;
    gui::SlideStackPartial_msgToUi getSlideStackTransformation() const;
    gui::SlidePreview getSlide( const UID& slideUid ) const;
    gui::ActiveSlide_msgToUi getActiveSlide() const;
    gui::SlideCommonPropertiesComplete_msgToUi getSlideCommonProperties() const;

    gui::SlideHeaderComplete_msgToUi getSlideHeader( const UID& slideUid ) const;
    gui::SlideViewDataComplete_msgToUi getSlideViewData( const UID& slideUid ) const;
    gui::SlideTxDataComplete_msgToUi getSlideTxData( const UID& slideUid ) const;


    /// Update the UI due to change of a single slide's data
    void updateUiFromSlideDataChange( const UID& slideUid );

    /// Update the UI due to change of the composition of the slide stack
    void updateUiFromSlideStackChange();

    /// Update the UI due to change of the slide stack transformation
    void updateUiFromSlideStackTransformationChange();

    /// Update the UI due to change of the active slide selection
    void updateUiFromActiveSlideSelectionChange( const UID& activeSlideUid );

    /// Update the UI due to a slide stack assembly rendering properties change
    void updateUiFromSlideStackAssemblyRenderingPropertiesChange(
            const SlideStackAssemblyRenderingProperties& );

    /// Update the UI due to slide transformation changes
    void updateUiFromSlideTransformationChanges( const std::list<UID>& slideUids );


    ActionManager& m_actionManager;
    AssemblyManager& m_assemblyManager;
    DataManager& m_dataManager;
    InteractionManager& m_interactionManager;

    /// Function that updates all rendered views
    AllViewsUpdaterType m_allViewsUpdater;

    /// Slide stack frame provider
    GetterType<CoordinateFrame> m_stackFrameProvider;

    /// Slide stack frame broadcaster
    SetterType<const CoordinateFrame&> m_stackFrameBroadcaster;

    /// Move crosshairs to slide center
    CrosshairsToSlideCenterMover m_centerCrosshairsOnSlide;

    /// Functional that updates the UI with all slide stack changes from the app
    gui::SlideStackComplete_msgToUi_PublisherType m_slideStackCompletePublisher;

    /// Functional that updates the UI with some slide stack changes from the app
    gui::SlideStackPartial_msgToUi_PublisherType m_slideStackPartialPublisher;

    /// Functional that updates the UI with the active slide from the app
    gui::ActiveSlide_msgToUi_PublisherType m_activeSlidePublisher;

    /// Functional that updates the UI with some slide stack rendering properties from the app
    gui::SlideCommonPropertiesPartial_msgToUi_PublisherType m_slideStackRenderingPartialPublisher;

    /// Functional that updates the UI with all slide stack rendering properties from the app
    gui::SlideCommonPropertiesComplete_msgToUi_PublisherType m_slideStackRenderingCompletePublisher;

    gui::SlideHeaderComplete_msgToUi_PublisherType m_slideHeaderCompletePublisher;
    gui::SlideViewDataComplete_msgToUi_PublisherType m_slideViewDataCompletePublisher;
    gui::SlideViewDataPartial_msgToUi_PublisherType m_slideViewDataPartialPublisher;
    gui::SlideTxDataComplete_msgToUi_PublisherType m_slideTxDataCompletePublisher;
    gui::SlideTxDataPartial_msgToUi_PublisherType m_slideTxDataPartialPublisher;
};



SlideStackDataUiMapper::SlideStackDataUiMapper(
        ActionManager& actionManager,
        AssemblyManager& assemblyManager,
        DataManager& dataManager,
        InteractionManager& interactionManager,
        AllViewsUpdaterType viewsUpdater )
    :
      m_impl( std::make_unique<Impl>(
                  actionManager, assemblyManager, dataManager, interactionManager,
                  viewsUpdater ) )
{}

SlideStackDataUiMapper::~SlideStackDataUiMapper() = default;


void SlideStackDataUiMapper::setSlideStackFrameProvider( GetterType<CoordinateFrame> provider )
{
    if ( m_impl ) m_impl->m_stackFrameProvider = provider;
}

void SlideStackDataUiMapper::setSlideStackFrameChangeDoneBroadcaster(
        SetterType<const CoordinateFrame&> broadcaster )
{
    if ( m_impl ) m_impl->m_stackFrameBroadcaster = broadcaster;
}

void SlideStackDataUiMapper::setCrosshairsToSlideCenterMover( CrosshairsToSlideCenterMover mover )
{
    if ( m_impl ) m_impl->m_centerCrosshairsOnSlide = mover;
}

void SlideStackDataUiMapper::setSlideStackPartial_fromUi( const gui::SlideStackPartial_msgFromUi& msg )
{
    if ( m_impl ) m_impl->updateAppFromUi( msg );
}

void SlideStackDataUiMapper::setSlideStackOrder_fromUi( const gui::SlideStackOrder_msgFromUi& msg )
{
    if ( m_impl ) m_impl->updateAppFromUi( msg );
}

void SlideStackDataUiMapper::setActiveSlide_fromUi( const gui::ActiveSlide_msgFromUi& msg )
{
    if ( m_impl ) m_impl->updateAppFromUi( msg );
}

void SlideStackDataUiMapper::setSlideCommonPropertiesPartial_fromUi( const gui::SlideCommonPropertiesPartial_msgFromUi& msg )
{
    if ( m_impl ) m_impl->updateAppFromUi( msg );
}

void SlideStackDataUiMapper::setSlideHeaderPartial_fromUi( const gui::SlideHeaderPartial_msgFromUi& msg )
{
    if ( m_impl ) m_impl->updateAppFromUi( msg );
}

void SlideStackDataUiMapper::setSlideViewDataPartial_fromUi( const gui::SlideViewDataPartial_msgFromUi& msg )
{
    if ( m_impl ) m_impl->updateAppFromUi( msg );
}

void SlideStackDataUiMapper::setSlideTxDataPartial_fromUi( const gui::SlideTxDataPartial_msgFromUi& msg )
{
    if ( m_impl ) m_impl->updateAppFromUi( msg );
}

void SlideStackDataUiMapper::setMoveToSlide_fromUi( const gui::MoveToSlide_msgFromUi& msg )
{
    if ( m_impl ) m_impl->updateAppFromUi( msg );
}

void SlideStackDataUiMapper::setSlideStackCompletePublisher_msgToUi(
        gui::SlideStackComplete_msgToUi_PublisherType publisher )
{
    if ( m_impl ) m_impl->m_slideStackCompletePublisher = publisher;
}

void SlideStackDataUiMapper::setSlideStackPartialPublisher_msgToUi(
        gui::SlideStackPartial_msgToUi_PublisherType publisher )
{
    if ( m_impl ) m_impl->m_slideStackPartialPublisher = publisher;
}

void SlideStackDataUiMapper::setActiveSlidePublisher_msgToUi(
        gui::ActiveSlide_msgToUi_PublisherType publisher )
{
    if ( m_impl ) m_impl->m_activeSlidePublisher = publisher;
}

void SlideStackDataUiMapper::setSlideCommonPropertiesPartialPublisher_msgToUi(
        gui::SlideCommonPropertiesPartial_msgToUi_PublisherType publisher )
{
    if ( m_impl ) m_impl->m_slideStackRenderingPartialPublisher = publisher;
}

void SlideStackDataUiMapper::setSlideCommonPropertiesCompletePublisher_msgToUi(
        gui::SlideCommonPropertiesComplete_msgToUi_PublisherType publisher )
{
    if ( m_impl ) m_impl->m_slideStackRenderingCompletePublisher = publisher;
}


void SlideStackDataUiMapper::setSlideHeaderCompletePublisher_msgToUi(
        gui::SlideHeaderComplete_msgToUi_PublisherType publisher )
{
    if ( m_impl ) m_impl->m_slideHeaderCompletePublisher = publisher;
}

void SlideStackDataUiMapper::setSlideViewDataCompletePublisher_msgToUi(
        gui::SlideViewDataComplete_msgToUi_PublisherType publisher )
{
    if ( m_impl ) m_impl->m_slideViewDataCompletePublisher = publisher;
}

void SlideStackDataUiMapper::setSlideViewDataPartialPublisher_msgToUi(
        gui::SlideViewDataPartial_msgToUi_PublisherType publisher )
{
    if ( m_impl ) m_impl->m_slideViewDataPartialPublisher = publisher;
}

void SlideStackDataUiMapper::setSlideTxDataCompletePublisher_msgToUi(
        gui::SlideTxDataComplete_msgToUi_PublisherType publisher )
{
    if ( m_impl ) m_impl->m_slideTxDataCompletePublisher = publisher;
}

void SlideStackDataUiMapper::setSlideTxDataPartialPublisher_msgToUi(
        gui::SlideTxDataPartial_msgToUi_PublisherType publisher )
{
    if ( m_impl ) m_impl->m_slideTxDataPartialPublisher = publisher;
}


gui::SlideStackComplete_msgToUi SlideStackDataUiMapper::getSlideStackComplete_msgToUi() const
{
    return m_impl->getSlideStack();
}

gui::ActiveSlide_msgToUi SlideStackDataUiMapper::getActiveSlide_msgToUi() const
{
    return m_impl->getActiveSlide();
}

gui::SlideCommonPropertiesComplete_msgToUi
SlideStackDataUiMapper::getSlideCommonPropertiesComplete_msgToUi() const
{
    return m_impl->getSlideCommonProperties();
}

boost::optional< gui::SlideHeaderComplete_msgToUi >
SlideStackDataUiMapper::getSlideHeaderComplete_msgToUi( const UID& slideUid ) const
{
    auto slideRecord = m_impl->m_dataManager.slideRecord( slideUid ).lock();
    if ( ! slideRecord || ! slideRecord->cpuData() )
    {
        std::cerr << "Requested header data for invalid slide " << slideUid << std::endl;
        return boost::none;
    }

    return m_impl->getSlideHeader( slideUid );
}

boost::optional< gui::SlideViewDataComplete_msgToUi >
SlideStackDataUiMapper::getSlideViewDataComplete_msgToUi( const UID& slideUid ) const
{
    auto slideRecord = m_impl->m_dataManager.slideRecord( slideUid ).lock();
    if ( ! slideRecord || ! slideRecord->cpuData() )
    {
        std::cerr << "Requested view data for invalid slide " << slideUid << std::endl;
        return boost::none;
    }

    return m_impl->getSlideViewData( slideUid );
}

boost::optional< gui::SlideTxDataComplete_msgToUi >
SlideStackDataUiMapper::getSlideTxDataComplete_msgToUi( const UID& slideUid ) const
{
    auto slideRecord = m_impl->m_dataManager.slideRecord( slideUid ).lock();
    if ( ! slideRecord || ! slideRecord->cpuData() )
    {
        std::cerr << "Requested transformation data for invalid slide " << slideUid << std::endl;
        return boost::none;
    }

    return m_impl->getSlideTxData( slideUid );
}


void SlideStackDataUiMapper::updateUiFromSlideStackFrameChange()
{
    if ( ! m_impl ) return;
    m_impl->updateUiFromSlideStackTransformationChange();
}



SlideStackDataUiMapper::Impl::Impl(
        ActionManager& actionManager,
        AssemblyManager& assemblyManager,
        DataManager& dataManager,
        InteractionManager& interactionManager,
        AllViewsUpdaterType viewsUpdater )
    :
      m_actionManager( actionManager ),
      m_assemblyManager( assemblyManager ),
      m_dataManager( dataManager ),
      m_interactionManager( interactionManager ),

      m_allViewsUpdater( viewsUpdater ),

      m_stackFrameProvider( nullptr ),
      m_stackFrameBroadcaster( nullptr ),
      m_centerCrosshairsOnSlide( nullptr ),

      m_slideStackCompletePublisher( nullptr ),
      m_slideStackPartialPublisher( nullptr ),
      m_activeSlidePublisher( nullptr ),
      m_slideStackRenderingPartialPublisher( nullptr ),
      m_slideStackRenderingCompletePublisher( nullptr ),

      m_slideHeaderCompletePublisher( nullptr ),
      m_slideViewDataCompletePublisher( nullptr ),
      m_slideViewDataPartialPublisher( nullptr ),
      m_slideTxDataCompletePublisher( nullptr ),
      m_slideTxDataPartialPublisher( nullptr )
{
    using std::placeholders::_1;

    // Connect signal that a single slide's data changed to slot that updates UI
    m_dataManager.connectToSlideDataChangedSignal(
                std::bind( &SlideStackDataUiMapper::Impl::updateUiFromSlideDataChange, this, _1 ) );

    // Connect signal that the slide stack composition has changed to slot that updates UI
    m_dataManager.connectToSlideStackChangedSignal(
                std::bind( &SlideStackDataUiMapper::Impl::updateUiFromSlideStackChange, this ) );

    // Connect signal that the active slide has changed to slot that updates UI
    m_dataManager.connectToActiveSlideChangedSignal(
                std::bind( &SlideStackDataUiMapper::Impl::updateUiFromActiveSlideSelectionChange, this, _1 ) );

    // Connect signal that parcellation label mesh rendering property has changed to slot that updates UI.
    m_assemblyManager.connectToSlideStackAssemblyRenderingPropertiesChangedSignal(
                std::bind( &SlideStackDataUiMapper::Impl::updateUiFromSlideStackAssemblyRenderingPropertiesChange, this, _1 ) );

    // Connect signal that slide transformations have changed to slot that updates UI.
    /// @todo remove assemblymanager dependency!
    m_assemblyManager.connectToSlideTransformationsChangedSignal(
                std::bind( &SlideStackDataUiMapper::Impl::updateUiFromSlideTransformationChanges, this, _1 ) );
}


void SlideStackDataUiMapper::Impl::updateAppFromUi( const gui::SlideStackPartial_msgFromUi& msg )
{
    bool updatedActiveSlide = false;

    const auto activeSlideUid = m_dataManager.activeSlideUid();

    for ( const auto& slide : msg.m_slides )
    {
        auto slideRecord = m_dataManager.slideRecord( slide.m_uid ).lock();
        if ( ! slideRecord || ! slideRecord->cpuData() )
        {
            std::ostringstream ss;
            ss << "Cannot update propeties of slide with invalid UID " << slide.m_uid << std::ends;
            throw_debug( ss.str() );
        }

        if ( ! updatedActiveSlide && activeSlideUid && ( slide.m_uid == *activeSlideUid ) )
        {
            updatedActiveSlide = true;
        }

        // Set slide properties
        auto& props = slideRecord->cpuData()->properties();

        props.setDisplayName( slide.m_name );
        props.setBorderColor( slide.m_borderColor );
        props.setVisible( slide.m_visible );
        props.setAnnotVisible( slide.m_annotVisible );

        if ( 0 <= slide.m_opacity && slide.m_opacity <= 100 )
        {
            props.setOpacity( static_cast<float>( slide.m_opacity ) / 100.0f );
        }

        if ( 0 <= slide.m_annotOpacity && slide.m_annotOpacity <= 100 )
        {
            props.setAnnotOpacity( static_cast<float>( slide.m_annotOpacity ) / 100.0f );
        }
    }

    if ( updatedActiveSlide )
    {
        updateUiFromSlideDataChange( *activeSlideUid );
    }


    bool updatedStackTx = false;

    if ( msg.m_set_world_O_stack_identity && *msg.m_set_world_O_stack_identity )
    {
        if ( m_stackFrameBroadcaster )
        {
            CoordinateFrame newStackFrame;
            newStackFrame.setIdentity();
            m_stackFrameBroadcaster( newStackFrame );
            updatedStackTx = true;
        }
    }

    if ( updatedStackTx )
    {
        // This is done to update the matrix widget:
        updateUiFromSlideStackTransformationChange();
    }

    if ( m_allViewsUpdater ) { m_allViewsUpdater(); }
}


void SlideStackDataUiMapper::Impl::updateAppFromUi( const gui::SlideStackOrder_msgFromUi& msg )
{
    if ( m_dataManager.setSlideOrder( msg.m_orderedSlideUids ) )
    {
        if ( m_allViewsUpdater ) { m_allViewsUpdater(); }
    }
}


void SlideStackDataUiMapper::Impl::updateAppFromUi( const gui::ActiveSlide_msgFromUi& msg )
{
    if ( ! msg.m_activeSlideUid || ! msg.m_activeSlideIndex ||
         ! ( 0 <= *msg.m_activeSlideIndex ) )
    {
        // Invalid message
        return;
    }

    // Check that the message's active slide UID and index match:
    const boost::optional<size_t> slideIndex = m_dataManager.slideIndex( *msg.m_activeSlideUid );

    if ( ! slideIndex || ( *slideIndex != static_cast<size_t>( *msg.m_activeSlideIndex ) ) )
    {
        // Invalid active slide index
        return;
    }

    if ( m_dataManager.setActiveSlideUid( *msg.m_activeSlideUid ) )
    {
        if ( m_allViewsUpdater ) { m_allViewsUpdater(); }
    }
}


void SlideStackDataUiMapper::Impl::updateAppFromUi( const gui::SlideCommonPropertiesPartial_msgFromUi& msg )
{
    if ( const auto opacity = msg.m_properties.m_masterOpacityValue )
    {
        if ( 0 <= *opacity && *opacity <= 100 )
        {
            m_assemblyManager.setSlideStackMasterOpacityMultiplier(
                        static_cast<float>( *opacity ) / 100.0f );
        }
    }

    if ( const auto opacity = msg.m_properties.m_image3dOpacityValue )
    {
        if ( 0 <= *opacity && *opacity <= 100 )
        {
            m_assemblyManager.setSlideStackImage3dLayerOpacity(
                        static_cast<float>( *opacity ) / 100.0f );
        }
    }

    if ( const auto visibleIn2d = msg.m_properties.m_stackVisibleIn2dViewsChecked )
    {
        m_assemblyManager.setSlideStackVisibleIn2dViews( *visibleIn2d );
    }

    if ( const auto visibleIn3d = msg.m_properties.m_stackVisibleIn3dViewsChecked )
    {
        m_assemblyManager.setSlideStackVisibleIn3dViews( *visibleIn3d );
    }

    if ( const auto show2dSlides = msg.m_properties.m_activeSlideViewShows2dSlidesChecked )
    {
        m_assemblyManager.setActiveSlideViewShows2dSlides( *show2dSlides );
    }

    if ( const auto viewTopToBottom = msg.m_properties.m_activeSlideViewDirectionTopToBottomChecked )
    {
        m_interactionManager.setActiveSlideViewDirection(
                    ( *viewTopToBottom )
                    ? InteractionManager::ActiveSlideViewDirection::TopToBottomSlide
                    : InteractionManager::ActiveSlideViewDirection::BottomToTopSlide );

        // This also updates all views:
        /// @todo Perhaps redundant call to update views?
        m_actionManager.resetViews(); /// @todo Pass this in as callback instead of using ActionManager?
    }

    if ( m_allViewsUpdater ) { m_allViewsUpdater(); }
}


void SlideStackDataUiMapper::Impl::updateAppFromUi( const gui::SlideHeaderPartial_msgFromUi& msg )
{
    auto slideRecord = m_dataManager.slideRecord( msg.m_uid ).lock();

    if ( ! slideRecord || ! slideRecord->cpuData() )
    {
        std::cerr << "Unable to update header data for slide " << msg.m_uid << std::endl;
        return;
    }

    bool doUpdate = false;

    const auto& MH = msg.m_headerMutable;
    auto& SH = slideRecord->cpuData()->header();

    if ( MH.m_displayName )
    {
        slideRecord->cpuData()->properties().setDisplayName( *MH.m_displayName );
    }

    if ( MH.m_pixelSizeX )
    {
        SH.setPixelSizeX( *MH.m_pixelSizeX );
        doUpdate = true;
    }

    if ( MH.m_pixelSizeY )
    {
        SH.setPixelSizeY( *MH.m_pixelSizeY );
        doUpdate = true;
    }

    if ( MH.m_thickness )
    {
        SH.setThickness( *MH.m_thickness );
        doUpdate = true;
    }

    // Trigger render of views:
    if ( doUpdate && m_allViewsUpdater ) { m_allViewsUpdater(); }
}


void SlideStackDataUiMapper::Impl::updateAppFromUi( const gui::SlideViewDataPartial_msgFromUi& msg )
{
    auto slideRecord = m_dataManager.slideRecord( msg.m_uid ).lock();

    if ( ! slideRecord || ! slideRecord->cpuData() )
    {
        std::cerr << "Unable to update view data for slide " << msg.m_uid << std::endl;
        return;
    }

    bool doUpdate = false;

    const auto& V = msg.m_viewData;
    auto& P = slideRecord->cpuData()->properties();

    if ( V.m_borderColor )
    {
        P.setBorderColor( *V.m_borderColor );
        doUpdate = true;
    }

    if ( V.m_slideVisibleChecked )
    {
        P.setVisible( *V.m_slideVisibleChecked );
        doUpdate = true;
    }

    if ( V.m_opacityValue )
    {
        P.setOpacity( static_cast<float>( *V.m_opacityValue ) / 100.0f );
        doUpdate = true;
    }

    if ( V.m_threshValues )
    {
        const auto threshValues = *V.m_threshValues;

        P.setIntensityThresholds( std::make_pair( static_cast<uint8_t>( threshValues.first ),
                                                  static_cast<uint8_t>( threshValues.second ) ) );
        doUpdate = true;
    }

    if ( V.m_edgesVisibleChecked )
    {
        P.setEdgesVisible( *V.m_edgesVisibleChecked );
        doUpdate = true;
    }

    if ( V.m_edgesMagnitudeValue )
    {
        P.setEdgesMagnitude( static_cast<float>( *V.m_edgesMagnitudeValue ) );
        doUpdate = true;
    }

    if ( V.m_edgesSmoothingValue )
    {
        P.setEdgesSmoothing( static_cast<float>( *V.m_edgesSmoothingValue ) );
        doUpdate = true;
    }


    if ( doUpdate )
    {
        if ( m_allViewsUpdater ) { m_allViewsUpdater(); }

        // Send message to update the Slide Preview rows in the Slide Sorter Table:
        if ( m_slideStackPartialPublisher )
        {
            gui::SlideStackPartial_msgToUi msgToUi;
            msgToUi.m_slides.insert( getSlide( msg.m_uid ) );
            m_slideStackPartialPublisher( msgToUi );
        }
    }
}


void SlideStackDataUiMapper::Impl::updateAppFromUi( const gui::SlideTxDataPartial_msgFromUi& msg )
{
    auto slideRecord = m_dataManager.slideRecord( msg.m_uid ).lock();

    if ( ! slideRecord || ! slideRecord->cpuData() )
    {
        std::cerr << "Unable to update transformation data for slide " << msg.m_uid << std::endl;
        return;
    }


    bool doUpdate = false;

    auto& T = slideRecord->cpuData()->transformation();
    const auto& D = msg.m_txData;

    const glm::vec3 physicalDims = slideio::physicalSlideDims( *slideRecord->cpuData() );


    if ( D.m_useScaleRotationParameterization )
    {
        if ( *D.m_useScaleRotationParameterization )
        {
            T.setShearParamMode( slideio::SlideTransformation::ShearParamMode::ScaleRotation );
        }
        else
        {
            T.setShearParamMode( slideio::SlideTransformation::ShearParamMode::ShearAngles );
        }
        doUpdate = true;
    }

    if ( D.m_xTranslationValueInMm )
    {
        T.setNormalizedTranslationX( static_cast<float>( *D.m_xTranslationValueInMm ) /  physicalDims.x );
        doUpdate = true;
    }
    if ( D.m_yTranslationValueInMm )
    {
        T.setNormalizedTranslationY( static_cast<float>( *D.m_yTranslationValueInMm ) / physicalDims.y );
        doUpdate = true;
    }
    if ( D.m_zTranslationValueInMm )
    {
        T.setStackTranslationZ( static_cast<float>( *D.m_zTranslationValueInMm ) );
        doUpdate = true;
    }

    if ( D.m_zRotationValueInDeg )
    {
        T.setRotationZAngle( static_cast<float>( *D.m_zRotationValueInDeg ) );
        doUpdate = true;
    }

    if ( D.m_xScaleValue )
    {
        T.setScaleFactorsX( static_cast<float>( *D.m_xScaleValue ) );
        doUpdate = true;
    }
    if ( D.m_yScaleValue )
    {
        T.setScaleFactorsY( static_cast<float>( *D.m_yScaleValue ) );
        doUpdate = true;
    }

    if ( D.m_xShearValueInDeg )
    {
        T.setShearAnglesX( static_cast<float>( *D.m_xShearValueInDeg ) );
        doUpdate = true;
    }
    if ( D.m_yShearValueInDeg )
    {
        T.setShearAnglesY( static_cast<float>( *D.m_yShearValueInDeg ) );
        doUpdate = true;
    }
    if ( D.m_zScaleRotationValueInDeg )
    {
        T.setScaleRotationAngle( static_cast<float>( *D.m_zScaleRotationValueInDeg ) );
        doUpdate = true;
    }

    if ( D.m_xOriginValueInMm )
    {
        T.setNormalizedRotationCenterX( static_cast<float>( *D.m_xOriginValueInMm ) / physicalDims.x );
        doUpdate = true;
    }
    if ( D.m_yOriginValueInMm )
    {
        T.setNormalizedRotationCenterY( static_cast<float>( *D.m_yOriginValueInMm ) / physicalDims.y );
        doUpdate = true;
    }


    if ( msg.m_set_stack_O_slide_identity && *msg.m_set_stack_O_slide_identity )
    {
        T.setIdentity();
        doUpdate = true;
    }


    // Trigger render of views:
    if ( doUpdate && m_allViewsUpdater )
    {
        m_allViewsUpdater();

        // Send message to update the slide transformation:
        // (Mainly needed to update the matrix widget)
        gui::SlideTxDataComplete_msgToUi msgToUi = getSlideTxData( msg.m_uid );
        m_slideTxDataCompletePublisher( msgToUi );
    }
}


void SlideStackDataUiMapper::Impl::updateAppFromUi( const gui::MoveToSlide_msgFromUi& msg )
{
    if ( ! m_centerCrosshairsOnSlide )
    {
        return;
    }

    if ( msg.m_slideIndex < 0 )
    {
        std::cerr << "Cannot move to invalid slide index " << msg.m_slideIndex << std::endl;
        return;
    }

    // Check that the message's slide UID and index match:
    const boost::optional<size_t> slideIndex = m_dataManager.slideIndex( msg.m_slideUid );
    if ( ! slideIndex || ( *slideIndex != static_cast<size_t>( msg.m_slideIndex ) ) )
    {
        std::cerr << "Cannot move to slide index " << msg.m_slideIndex
                  << " for slide " << msg.m_slideUid << std::endl;
        return;
    }

    m_centerCrosshairsOnSlide( msg.m_slideUid );

    if ( m_allViewsUpdater ) { m_allViewsUpdater(); }
}


void SlideStackDataUiMapper::Impl::updateUiFromSlideStackAssemblyRenderingPropertiesChange(
        const SlideStackAssemblyRenderingProperties& /*props*/ )
{
    if ( m_slideStackRenderingCompletePublisher )
    {
        m_slideStackRenderingCompletePublisher( getSlideCommonProperties() );
    }
}


void SlideStackDataUiMapper::Impl::updateUiFromSlideTransformationChanges(
        const std::list<UID>& slideUids )
{
    if ( m_slideTxDataCompletePublisher )
    {
        for ( const auto& uid : slideUids )
        {
            m_slideTxDataCompletePublisher( getSlideTxData( uid ) );
        }
    }
}


gui::SlideStackComplete_msgToUi SlideStackDataUiMapper::Impl::getSlideStack() const
{
    gui::SlideStackComplete_msgToUi msg;

    for ( const auto& slideUid : m_dataManager.orderedSlideUids() )
    {
        msg.m_slides.emplace_back( getSlide( slideUid ) );
    }

    msg.m_activeSlideUid = m_dataManager.activeSlideUid();
    msg.m_activeSlideIndex = m_dataManager.activeSlideIndex();

    if ( m_stackFrameProvider )
    {
        msg.m_world_O_stack = glm::dmat4{ m_stackFrameProvider().world_O_frame() };
    }
    else
    {
        throw_debug( "Null stack frame provider" );
    }

    return msg;
}


gui::SlideStackPartial_msgToUi SlideStackDataUiMapper::Impl::getSlideStackTransformation() const
{
    gui::SlideStackPartial_msgToUi msg;

    if ( m_stackFrameProvider )
    {
        msg.m_world_O_stack = glm::dmat4{ m_stackFrameProvider().world_O_frame() };
    }
    else
    {
        throw_debug( "Null stack frame provider" );
    }

    return msg;
}


gui::SlidePreview SlideStackDataUiMapper::Impl::getSlide( const UID& slideUid ) const
{
    auto record = m_dataManager.slideRecord( slideUid ).lock();
    if ( ! record || ! record->cpuData() )
    {
        throw_debug( "Null slide record" );
    }

    auto slideIndex = m_dataManager.slideIndex( slideUid );
    if ( ! slideIndex )
    {
        throw_debug( "Invalid slide UID" );
    }

    const auto& header = record->cpuData()->header();
    const auto& props = record->cpuData()->properties();

    gui::SlidePreview slide;

    slide.m_uid = slideUid;
    slide.m_index = *slideIndex;

    slide.m_name = props.displayName();
    slide.m_borderColor = props.borderColor();
    slide.m_visible = props.visible();
    slide.m_annotVisible = props.annotVisible();
    slide.m_opacity = static_cast<int>( props.opacity() * 100.0f );
    slide.m_annotOpacity = static_cast<int>( props.annotOpacity() * 100.0f );

    slide.m_thumbnailBuffer = header.associatedImages().thumbImage().first;
    slide.m_thumbnailDims = header.associatedImages().thumbImage().second;

    return slide;
}


gui::ActiveSlide_msgToUi SlideStackDataUiMapper::Impl::getActiveSlide() const
{
    gui::ActiveSlide_msgToUi msg;
    msg.m_activeSlideUid = m_dataManager.activeSlideUid();
    msg.m_activeSlideIndex = m_dataManager.activeSlideIndex();
    return msg;
}


gui::SlideCommonPropertiesComplete_msgToUi SlideStackDataUiMapper::Impl::getSlideCommonProperties() const
{
    const auto props = m_assemblyManager.getSlideRenderingProperties();
    const auto viewDir = m_interactionManager.getActiveSlideViewDirection();

    gui::SlideCommonPropertiesComplete_msgToUi msg;

    msg.m_properties.m_masterOpacityRange = std::make_pair( 0, 100 );
    msg.m_properties.m_masterOpacitySingleStep = 1;
    msg.m_properties.m_masterOpacitySliderPageStep = 10;
    msg.m_properties.m_masterOpacityValue = static_cast<int>( props.m_masterOpacityMultiplier * 100.0f );

    msg.m_properties.m_image3dOpacityRange = std::make_pair( 0, 100 );
    msg.m_properties.m_image3dOpacitySingleStep = 1;
    msg.m_properties.m_image3dOpacitySliderPageStep = 10;
    msg.m_properties.m_image3dOpacityValue = static_cast<int>( props.m_image3dLayerOpacity * 100.0f );

    msg.m_properties.m_stackVisibleIn2dViewsChecked = props.m_visibleIn2dViews;
    msg.m_properties.m_stackVisibleIn3dViewsChecked = props.m_visibleIn3dViews;

    msg.m_properties.m_activeSlideViewShows2dSlidesChecked = props.m_activeSlideViewShows2dSlides;

    msg.m_properties.m_activeSlideViewDirectionTopToBottomChecked =
            ( InteractionManager::ActiveSlideViewDirection::TopToBottomSlide == viewDir )
            ? true : false;

    return msg;
}


gui::SlideHeaderComplete_msgToUi SlideStackDataUiMapper::Impl::getSlideHeader( const UID& slideUid ) const
{
    auto slideRecord = m_dataManager.slideRecord( slideUid ).lock();

    if ( ! slideRecord || ! slideRecord->cpuData() )
    {
        std::ostringstream ss;
        ss << "Unable to get header data for slide " << slideUid << std::ends;
        throw_debug( ss.str() )
    }

    auto* R = slideRecord->cpuData();
    const auto& H = R->header();
    const auto& P = R->properties();

    gui::SlideHeaderComplete_msgToUi msg;

    msg.m_uid = slideUid;

    msg.m_headerImmutable.m_filePath = H.fileName();
    msg.m_headerImmutable.m_slideType = H.vendorId();

    for ( size_t i = 0; i < R->numFileLevels(); ++i )
    {
        msg.m_headerImmutable.m_layerDims.push_back( R->fileLevel( i ).m_dims );
    }

    for ( size_t i = 0; i < R->numCreatedLevels(); ++i )
    {
        msg.m_headerImmutable.m_layerDims.push_back( R->createdLevel( i ).m_dims );
    }

    msg.m_headerImmutable.m_labelImageBuffer = H.associatedImages().labelImage().first;
    msg.m_headerImmutable.m_labelImageDims = H.associatedImages().labelImage().second;

    msg.m_headerImmutable.m_macroImageBuffer = H.associatedImages().macroImage().first;
    msg.m_headerImmutable.m_macroImageDims = H.associatedImages().macroImage().second;

    msg.m_headerImmutable.m_pixelSizeRange = std::make_pair( 1.0e-6, 1.0e6 );
    msg.m_headerImmutable.m_thicknessRange = std::make_pair( 1.0e-6, 1.0e6 );

    msg.m_headerMutable.m_displayName = P.displayName();
    msg.m_headerMutable.m_pixelSizeX = H.pixelSize().x;
    msg.m_headerMutable.m_pixelSizeY = H.pixelSize().y;
    msg.m_headerMutable.m_thickness = H.thickness();

    return msg;
}


gui::SlideViewDataComplete_msgToUi SlideStackDataUiMapper::Impl::getSlideViewData( const UID& slideUid ) const
{
    auto slideRecord = m_dataManager.slideRecord( slideUid ).lock();

    if ( ! slideRecord || ! slideRecord->cpuData() )
    {
        std::ostringstream ss;
        ss << "Unable to get view data for slide " << slideUid << std::ends;
        throw_debug( ss.str() )
    }

    const auto& P = slideRecord->cpuData()->properties();

    gui::SlideViewDataComplete_msgToUi msg;

    msg.m_uid = slideUid;

    msg.m_viewData.m_slideVisibleChecked = P.visible();

    msg.m_viewData.m_borderColor = P.borderColor();

    msg.m_viewData.m_opacityRange = std::make_pair( 0, 100 );
    msg.m_viewData.m_opacitySingleStep = 1;
    msg.m_viewData.m_opacitySliderPageStep = 10;
    msg.m_viewData.m_opacityValue = static_cast<int>( P.opacity() * 100.0f );

    const auto& thresh = P.intensityThresholds();
    msg.m_viewData.m_threshRange = std::make_pair( 0, 255 );
    msg.m_viewData.m_threshSingleStep = 1;
    msg.m_viewData.m_threshSliderPageStep = 10;
    msg.m_viewData.m_threshValues = std::make_pair( static_cast<int>( thresh.first ),
                                                    static_cast<int>( thresh.second ) );

    msg.m_viewData.m_edgesVisibleChecked = P.edgesVisible();

    msg.m_viewData.m_edgesMagnitudeRange = std::make_pair( 0.0, 0.2 );
    msg.m_viewData.m_edgesMagnitudeSingleStep = 0.01;
    msg.m_viewData.m_edgesMagnitudePageStep = 0.5;
    msg.m_viewData.m_edgesMagnitudeDecimalPrecision = 2;
    msg.m_viewData.m_edgesMagnitudeValue = static_cast<double>( P.edgesMagnitude() );

    msg.m_viewData.m_edgesSmoothingRange = std::make_pair( 0.0, 2.0 );
    msg.m_viewData.m_edgesSmoothingSingleStep = 0.1;
    msg.m_viewData.m_edgesSmoothingPageStep = 0.5;
    msg.m_viewData.m_edgesSmoothingDecimalPrecision = 1;
    msg.m_viewData.m_edgesSmoothingValue = static_cast<double>( P.edgesSmoothing() );

    return msg;
}


gui::SlideTxDataComplete_msgToUi SlideStackDataUiMapper::Impl::getSlideTxData( const UID& slideUid ) const
{
    auto slideRecord = m_dataManager.slideRecord( slideUid ).lock();

    if ( ! slideRecord || ! slideRecord->cpuData() )
    {
        std::ostringstream ss;
        ss << "Unable to get transformation data for slide " << slideUid << std::ends;
        throw_debug( ss.str() )
    }

    const auto& T = slideRecord->cpuData()->transformation();
    const glm::vec3 physicalDims = slideio::physicalSlideDims( *slideRecord->cpuData() );

    gui::SlideTxDataComplete_msgToUi msg;

    msg.m_uid = slideUid;

    msg.m_txData.m_translationRange = std::make_pair( -1.0e9, 1.0e9 );
    msg.m_txData.m_translationSingleStep = 1.0e-3;
    msg.m_txData.m_translationDecimalPrecision = 6;

    msg.m_txData.m_rotationRange = std::make_pair( -180.0, 180.0 );
    msg.m_txData.m_rotationSingleStep = 1.0e-3;
    msg.m_txData.m_rotationDecimalPrecision = 6;

    msg.m_txData.m_scaleRange = std::make_pair( -1000.0, 1000.0 );
    msg.m_txData.m_scaleSingleStep = 1.0e-3;
    msg.m_txData.m_scaleDecimalPrecision = 6;

    msg.m_txData.m_scaleRotationRange = std::make_pair( -180.0, 180.0 );
    msg.m_txData.m_scaleRotationSingleStep = 1.0e-3;
    msg.m_txData.m_scaleRotationDecimalPrecision = 6;

    msg.m_txData.m_shearRange = std::make_pair( -90.0, 90.0 );
    msg.m_txData.m_shearSingleStep = 1.0e-3;
    msg.m_txData.m_shearDecimalPrecision = 6;

    msg.m_txData.m_originRange = std::make_pair( -1.0e9, 1.0e9 );
    msg.m_txData.m_originSingleStep = 1.0e-3;
    msg.m_txData.m_originDecimalPrecision = 6;

    msg.m_txData.m_xTranslationValueInMm = static_cast<double>( T.normalizedTranslationXY().x * physicalDims.x );
    msg.m_txData.m_yTranslationValueInMm = static_cast<double>( T.normalizedTranslationXY().y * physicalDims.y );
    msg.m_txData.m_zTranslationValueInMm = static_cast<double>( T.stackTranslationZ() );

    msg.m_txData.m_zRotationValueInDeg = static_cast<double>( T.rotationZAngle() );

    msg.m_txData.m_xScaleValue = static_cast<double>( T.scaleFactorsXY().x );
    msg.m_txData.m_yScaleValue = static_cast<double>( T.scaleFactorsXY().y );

    msg.m_txData.m_xShearValueInDeg = static_cast<double>( T.shearAnglesXY().x );
    msg.m_txData.m_yShearValueInDeg = static_cast<double>( T.shearAnglesXY().y );

    msg.m_txData.m_zScaleRotationValueInDeg = static_cast<double>( T.scaleRotationAngle() );

    msg.m_txData.m_xOriginValueInMm = static_cast<double>( T.normalizedRotationCenterXY().x * physicalDims.x );
    msg.m_txData.m_yOriginValueInMm = static_cast<double>( T.normalizedRotationCenterXY().y * physicalDims.y );

    msg.m_txData.m_useScaleRotationParameterization =
            ( slideio::SlideTransformation::ShearParamMode::ScaleRotation == T.shearParamMode() )
            ? true : false;

    msg.m_txData.m_stack_O_slide_matrix = glm::dmat4x4{ slideio::stack_O_slide( *slideRecord->cpuData() ) };

    return msg;
}


void SlideStackDataUiMapper::Impl::updateUiFromSlideDataChange( const UID& slideUid )
{
    if ( ! m_slideStackPartialPublisher || ! m_slideHeaderCompletePublisher ||
         ! m_slideViewDataCompletePublisher || ! m_slideTxDataCompletePublisher )
    {
        return;
    }

    gui::SlideStackPartial_msgToUi msg;

    msg.m_slides.insert( getSlide( slideUid ) );

    if ( m_stackFrameProvider )
    {
        msg.m_world_O_stack = glm::dmat4{ m_stackFrameProvider().world_O_frame() };
    }

    m_slideStackPartialPublisher( msg );


    const auto activeSlideUid = m_dataManager.activeSlideUid();

    if ( activeSlideUid && *activeSlideUid == slideUid )
    {
        // The active slide was changed, so publish all of its data to the UI
        m_slideHeaderCompletePublisher( getSlideHeader( slideUid ) );
        m_slideViewDataCompletePublisher( getSlideViewData( slideUid ) );
        m_slideTxDataCompletePublisher( getSlideTxData( slideUid ) );
    }
}


void SlideStackDataUiMapper::Impl::updateUiFromSlideStackChange()
{
    if ( m_slideStackCompletePublisher )
    {
        m_slideStackCompletePublisher( getSlideStack() );
    }
}

void SlideStackDataUiMapper::Impl::updateUiFromSlideStackTransformationChange()
{
    if ( m_slideStackPartialPublisher )
    {
        m_slideStackPartialPublisher( getSlideStackTransformation() );
    }
}

void SlideStackDataUiMapper::Impl::updateUiFromActiveSlideSelectionChange( const UID& /*activeSlideUid*/ )
{
    if ( m_activeSlidePublisher )
    {
        m_activeSlidePublisher( getActiveSlide() );
    }
}
