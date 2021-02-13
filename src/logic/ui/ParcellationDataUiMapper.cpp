#include "logic/ui/ParcellationDataUiMapper.h"
#include "logic/ui/details/PackageHeader.h"

#include "logic/managers/ActionManager.h"
#include "logic/managers/AssemblyManager.h"
#include "logic/managers/DataManager.h"
#include "logic/data/DataLoading.h"

#include "rendering/utility/CreateGLObjects.h"

#include "common/UID.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/epsilon.hpp>

#include <boost/filesystem.hpp>

#include <cmath>
#include <functional>
#include <iostream>
#include <set>


struct ParcellationDataUiMapper::Impl
{
    Impl( ActionManager&, AssemblyManager&, DataManager&, AllViewsUpdaterType );
    ~Impl() = default;


    /// Update parcellation selection in app from UI changes
    void updateAppFromUi( const gui::ParcellationSelections_msgFromUi& );

    /// Update parcellation properties in app from UI changes
    void updateAppFromUi( const gui::ParcellationPropertiesPartial_msgFromUi& );

    /// Update parcellation labels in app from UI changes
    void updateAppFromUi( const gui::ParcellationLabelsPartial_msgFromUi& );


    /// Update the UI due to a generic parcellation data change
    void updateUiFromParcellationDataChange( const UID& parcelUid );

    /// Update the UI due to a parcellation label table data change
    void updateUiFromParcellationLabelDataChange( const UID& labelUid );

    /// Update the UI due to parcellation selection change
    void updateUiFromParcellationSelectionChange();


    /// Get the current parcellation selection data
    gui::ParcellationSelections_msgToUi getParcellationSelection() const;

    /// Get all current properties of the active parcellation (if there is one)
    std::optional< gui::ParcellationPropertiesComplete_msgToUi > getActiveParcellationProperties() const;

    /// Get header of the active parcellation (if there is one)
    std::optional< gui::ImageHeader_msgToUi > getActiveParcellationHeader() const;

    /// Get current label table of the active parcellation (if there is one)
    std::optional< gui::ParcellationLabelsComplete_msgToUi > getActiveParcellationLabels() const;


    ActionManager& m_actionManager;
    AssemblyManager& m_assemblyManager;
    DataManager& m_dataManager;


    /// Function that updates all rendered views
    AllViewsUpdaterType m_allViewsUpdater;

    /// Function that updates parcellation selection in the UI
    gui::ParcellationSelections_msgToUi_PublisherType m_parcellationSelectionPublisher;

    /// Function that updates some parcellation properties in the UI
    gui::ParcellationPropertiesPartial_msgToUi_PublisherType m_partialParcellationPropertiesPublisher;

    /// Function that updates all parcellation properties in the UI
    gui::ParcellationPropertiesComplete_msgToUi_PublisherType m_fullParcellationPropertiesPublisher;

    /// Function that updates all parcellation labels in the UI
    gui::ParcellationLabelsComplete_msgToUi_PublisherType m_fullParcellationLabelsPublisher;
};


ParcellationDataUiMapper::ParcellationDataUiMapper(
        ActionManager& actionManager,
        AssemblyManager& assemblyManager,
        DataManager& dataManager,
        AllViewsUpdaterType viewsUpdater )
    :
      m_impl( std::make_unique<Impl>( actionManager, assemblyManager, dataManager, viewsUpdater ) )
{}

ParcellationDataUiMapper::~ParcellationDataUiMapper() = default;


void ParcellationDataUiMapper::setParcellationSelections_fromUi(
        const gui::ParcellationSelections_msgFromUi& msg )
{
    m_impl->updateAppFromUi( msg );
}

void ParcellationDataUiMapper::setParcellationPropertiesPartial_fromUi(
        const gui::ParcellationPropertiesPartial_msgFromUi& msg )
{
    m_impl->updateAppFromUi( msg );
}

void ParcellationDataUiMapper::setParcellationLabelsPartial_fromUi(
        const gui::ParcellationLabelsPartial_msgFromUi& msg )
{
    m_impl->updateAppFromUi( msg );
}

void ParcellationDataUiMapper::setParcellationSelectionsPublisher_msgToUi(
        gui::ParcellationSelections_msgToUi_PublisherType publisher )
{
    m_impl->m_parcellationSelectionPublisher = publisher;
}

void ParcellationDataUiMapper::setParcellationPropertiesPartialPublisher_msgToUi(
        gui::ParcellationPropertiesPartial_msgToUi_PublisherType publisher )
{
    m_impl->m_partialParcellationPropertiesPublisher = publisher;
}

void ParcellationDataUiMapper::setParcellationPropertiesCompletePublisher_msgToUi(
        gui::ParcellationPropertiesComplete_msgToUi_PublisherType publisher )
{
    m_impl->m_fullParcellationPropertiesPublisher = publisher;
}

void ParcellationDataUiMapper::setParcellationLabelsCompletePublisher_msgToUi(
        gui::ParcellationLabelsComplete_msgToUi_PublisherType publisher )
{
    m_impl->m_fullParcellationLabelsPublisher = publisher;
}


gui::ParcellationSelections_msgToUi
ParcellationDataUiMapper::getParcellationSelections_msgToUi() const
{
    return m_impl->getParcellationSelection();
}

std::optional< gui::ParcellationPropertiesComplete_msgToUi >
ParcellationDataUiMapper::getParcellationPropertiesComplete_msgToUi( const UID& parcelUid ) const
{
    const auto activeParcelUid = m_impl->m_dataManager.activeParcellationUid();

    if ( ! activeParcelUid || *activeParcelUid != parcelUid )
    {
        // Requested of properties of a parcellation that is not active
        std::cerr << "Requested properties of non-active parcellation " << parcelUid << std::endl;
        return std::nullopt;
    }

    return m_impl->getActiveParcellationProperties();
}


std::optional< gui::ImageHeader_msgToUi >
ParcellationDataUiMapper::getParcellationHeader_msgToUi( const UID& parcelUid ) const
{
    const auto activeParcelUid = m_impl->m_dataManager.activeParcellationUid();

    if ( ! activeParcelUid || *activeParcelUid != parcelUid )
    {
        std::cerr << "Requested header of non-active parcellation " << parcelUid << std::endl;
        return std::nullopt;
    }

    return m_impl->getActiveParcellationHeader();
}

std::optional< gui::ParcellationLabelsComplete_msgToUi >
ParcellationDataUiMapper::getParcellationLabelsComplete_msgToUi( const UID& parcelUid ) const
{
    const auto activeParcelUid = m_impl->m_dataManager.activeParcellationUid();

    if ( ! activeParcelUid || *activeParcelUid != parcelUid )
    {
        std::cerr << "Requested labels of non-active parcellation " << parcelUid << std::endl;
        return std::nullopt;
    }

    return m_impl->getActiveParcellationLabels();
}


////////////////////////////////////////////////////////////////////////////////////////////////////


ParcellationDataUiMapper::Impl::Impl(
        ActionManager& actionManager,
        AssemblyManager& assemblyManager,
        DataManager& dataManager,
        AllViewsUpdaterType viewsUpdater )
    :
      m_actionManager( actionManager ),
      m_assemblyManager( assemblyManager ),
      m_dataManager( dataManager ),

      m_allViewsUpdater( viewsUpdater ),

      m_parcellationSelectionPublisher( nullptr ),
      m_partialParcellationPropertiesPublisher( nullptr ),
      m_fullParcellationPropertiesPublisher( nullptr ),
      m_fullParcellationLabelsPublisher( nullptr )
{
    using std::placeholders::_1;

    // Connect signal that parcellation data changed to slot that updates UI
    m_dataManager.connectToParcellationDataChangedSignal(
                std::bind( &ParcellationDataUiMapper::Impl::updateUiFromParcellationDataChange, this, _1 ) );

    // Connect signal that parcellation label table data changed to slot that updates UI
    m_dataManager.connectToLabelTableDataChangedSignal(
                std::bind( &ParcellationDataUiMapper::Impl::updateUiFromParcellationLabelDataChange, this, _1 ) );
}


void ParcellationDataUiMapper::Impl::updateUiFromParcellationDataChange( const UID& parcelUid )
{
    const auto activeParcelUid = m_dataManager.activeParcellationUid();
    if ( ! activeParcelUid || *activeParcelUid != parcelUid )
    {
        // Ignore changes not related to active parcellation
        return;
    }

    updateUiFromParcellationSelectionChange();
}


void ParcellationDataUiMapper::Impl::updateUiFromParcellationLabelDataChange( const UID& /*labelUid*/ )
{
    // Resend the label table
    if ( m_fullParcellationLabelsPublisher )
    {
        if ( const auto labels = getActiveParcellationLabels() )
        {
            m_fullParcellationLabelsPublisher( *labels );
        }
    }
}


void ParcellationDataUiMapper::Impl::updateUiFromParcellationSelectionChange()
{
    if ( m_parcellationSelectionPublisher )
    {
        m_parcellationSelectionPublisher( getParcellationSelection() );
    }

    // Since the parcellation selections changed, resend the properties
    if ( m_fullParcellationPropertiesPublisher )
    {
        if ( const auto props = getActiveParcellationProperties() )
        {
            m_fullParcellationPropertiesPublisher( *props );
        }
    }

    // Resend the label table
    if ( m_fullParcellationLabelsPublisher )
    {
        if ( const auto labels = getActiveParcellationLabels() )
        {
            m_fullParcellationLabelsPublisher( *labels );
        }
    }
}


void ParcellationDataUiMapper::Impl::updateAppFromUi( const gui::ParcellationSelections_msgFromUi& msg )
{
    // A new parcellation was selected in UI: update the active parcellation.

    // Make sure that the parcellation UID is valid and that the index of the selection matches
    // the index of the parcellation. If not, the data between UI and app is inconsistent.

    if ( ! msg.m_selectionIndex || ! msg.m_parcelUid )
    {
        return;
    }

    const auto orderedIndex = m_dataManager.orderedParcellationIndex( *msg.m_parcelUid );

    if ( ! orderedIndex )
    {
        std::cerr << "Invalid selection of parcellation UID " << *msg.m_parcelUid << std::endl;
        return; // Invalid UID
    }

    if ( *msg.m_selectionIndex != *orderedIndex )
    {
        std::cerr << "Invalid selection of parcellation index " << *msg.m_selectionIndex << std::endl;
        return; // Indices do not match
    }

    if ( m_dataManager.setActiveParcellationUid( *msg.m_parcelUid ) )
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
        std::cerr << "Unable to set active parcellation UID " << *msg.m_parcelUid << std::endl;
    }
}


void ParcellationDataUiMapper::Impl::updateAppFromUi(
        const gui::ParcellationPropertiesPartial_msgFromUi& msg )
{
    // Update properties of the active parcellation from changes to it on the UI.
    // Updates are applied to component 0 of the parcellation image.
    static constexpr size_t sk_comp = 0;

    const auto activeParcelUid = m_dataManager.activeParcellationUid();
    if ( ! activeParcelUid || *activeParcelUid != msg.m_parcelUid )
    {
        // Something has gone wrong, since the message is for UI changes done on a parcellation
        // that is not the active one
        std::cerr << "Error: cannot update properties of non-active parcellation." << std::endl;
        return;
    }

    auto activeParcelRecord = m_dataManager.activeParcellationRecord().lock();
    if ( ! activeParcelRecord )
    {
        return;
    }

    auto cpuRecord = activeParcelRecord->cpuData();
    if ( ! cpuRecord )
    {
        return;
    }

    // All fields in the message from the UI are optional.
    // Check each prior to using its value:

    if ( const auto displayName = msg.m_properties.m_displayName )
    {
        cpuRecord->setDisplayName( *displayName );
    }

    if ( const auto visibleIn2d = msg.m_properties.m_visibleIn2dViewsChecked )
    {
        m_assemblyManager.setParcellationVisibleIn2dViews( *visibleIn2d );
    }
    if ( const auto visibleIn3d = msg.m_properties.m_visibleIn3dViewsChecked )
    {
        m_assemblyManager.setParcellationVisibleIn3dViews( *visibleIn3d );
    }

    if ( const auto opacity = msg.m_properties.m_opacityValue )
    {
        if ( 0 <= *opacity && *opacity <= 100 )
        {
            cpuRecord->setOpacity( sk_comp, static_cast<double>( *opacity / 100.0 ) );
        }
    }

    if ( const auto showIn2d = msg.m_meshProperties.m_meshesVisibleIn2dViews )
    {
        m_assemblyManager.setLabelMeshesVisibleIn2dViews( *showIn2d );
    }
    if ( const auto showIn3d = msg.m_meshProperties.m_meshesVisibleIn3dViews )
    {
        m_assemblyManager.setLabelMeshesVisibleIn3dViews( *showIn3d );
    }

    if ( const auto xrayMode = msg.m_meshProperties.m_meshesXrayModeChecked )
    {
        m_assemblyManager.setLabelMeshesUseXrayMode( *xrayMode );
    }
    if ( const auto xrayPower = msg.m_meshProperties.m_meshXrayPowerValue )
    {
        m_assemblyManager.setLabelMeshesXrayPower( static_cast<float>( *xrayPower ) );
    }

    if ( const auto opacity = msg.m_meshProperties.m_meshOpacityValue )
    {
        if ( 0 <= *opacity && *opacity <= 100 )
        {
            m_assemblyManager.setLabelMeshMasterOpacity( static_cast<float>( *opacity / 100.0 ) );
        }
    }

    if ( m_allViewsUpdater )
    {
        m_allViewsUpdater();
    }
}


void ParcellationDataUiMapper::Impl::updateAppFromUi(
        const gui::ParcellationLabelsPartial_msgFromUi& msg )
{
    const auto labelTableUid = msg.m_labelTableUid; // UID of label table that was changed

    // Compare labelsUid against the UID of the label table of the active parcellation.
    // If they do not match, then an error has occurred, since the user can only edit
    // this "active label table" in the UI.

    auto activeParcelUid = m_dataManager.activeParcellationUid();
    if ( ! activeParcelUid )
    {
        std::cerr << "Cannot edit labels of parcellation that is not active." << std::endl;
        return;
    }

    auto activeLabelsUid = m_dataManager.labelTableUid_of_parcellation( *activeParcelUid );
    if ( *activeLabelsUid != labelTableUid )
    {
        std::cerr << "Cannot edit labels of parcellation that is not active." << std::endl;
        return;
    }

    auto labelsRecord = m_dataManager.labelTableRecord( labelTableUid ).lock();
    if ( ! labelsRecord )
    {
        return;
    }

    auto cpuRecord = labelsRecord->cpuData();
    if ( ! cpuRecord )
    {
        return;
    }

    // Keep track of the following:
    bool labelNameChanged = false;  // whether a label name has changed
    bool labelColorChanged = false; // whether a label color/opacity/visibility has changed
    bool labelMeshChanged = false;  // whether a label mesh has changed


    for ( const auto& label : msg.m_labels )
    {
        if ( label.m_index < cpuRecord->numLabels() )
        {
            if ( cpuRecord->getName( label.m_index ) != label.m_name )
            {
                cpuRecord->setName( label.m_index, label.m_name );
                labelNameChanged = true;
            }

            if ( glm::any( glm::epsilonNotEqual(
                               cpuRecord->getColor( label.m_index ),
                               label.m_color, glm::epsilon<float>() ) ) )
            {
                cpuRecord->setColor( label.m_index, label.m_color );
                labelColorChanged = true;
            }

            if ( 0 <= label.m_alpha && label.m_alpha <= 100 )
            {
                const float a = static_cast<float>( label.m_alpha / 100.0f );

                if ( glm::epsilonNotEqual( cpuRecord->getAlpha( label.m_index ), a,
                                           glm::epsilon<float>() ) )
                {
                    cpuRecord->setAlpha( label.m_index, a );
                    labelColorChanged = true;
                }
            }

            if ( cpuRecord->getVisible( label.m_index ) != label.m_visible )
            {
                cpuRecord->setVisible( label.m_index, label.m_visible );
                labelColorChanged = true;
            }

            if ( cpuRecord->getShowMesh( label.m_index ) != label.m_showMesh )
            {
                cpuRecord->setShowMesh( label.m_index, label.m_showMesh );

                if ( label.m_showMesh )
                {
                    // UI turned on visibility for this label mesh. Generate the mesh
                    // if it has not been already generated:
                    const std::set<uint32_t> labelIndices = { label.m_index };
                    data::generateLabelMeshes( m_dataManager, *activeParcelUid, labelIndices );
                }

                labelMeshChanged = true;
            }
        }
        else
        {
            std::cerr << "Invalid label index " << label.m_index << std::endl;
            continue;
        }
    }


    if ( labelNameChanged )
    {
        /// @todo Update UI so that new name is shown under cursor
    }

    if ( labelColorChanged )
    {
        // Create new label table GPU record if a label color has changed
        auto labelsGpuRecord = gpuhelper::createLabelColorTableTextureBuffer( cpuRecord );
        if ( ! labelsGpuRecord )
        {
            std::cerr << "Error generating label table GPU record" << std::endl;
            return;
        }

        labelsRecord->setGpuData( std::move( labelsGpuRecord ) );

        static constexpr bool sk_doViewUpdate = false;
        m_assemblyManager.updateLabelColorTable( labelTableUid, sk_doViewUpdate );
    }

    if ( labelMeshChanged )
    {
        // Get map of all new label mesh UIDs
        const auto labelMeshUidMap = m_dataManager.labelMeshUids_of_parcellation( *activeParcelUid );

        std::vector<UID> labelMeshUids;
        for ( const auto& p : labelMeshUidMap )
        {
            labelMeshUids.push_back( p.second );
        }

        // Update the assembly with new label meshes
        m_assemblyManager.updateLabelMeshes( labelMeshUids, labelTableUid );
    }


    if ( m_allViewsUpdater )
    {
        m_allViewsUpdater();
    }
}


gui::ParcellationSelections_msgToUi
ParcellationDataUiMapper::Impl::getParcellationSelection() const
{
    gui::ParcellationSelections_msgToUi msg;

    const auto activeParcelUid = m_dataManager.activeParcellationUid();
    if ( ! activeParcelUid )
    {
        return msg;
    }

    const auto activeIndex = m_dataManager.orderedParcellationIndex( *activeParcelUid );
    if ( ! activeIndex )
    {
        return msg;
    }

    msg.m_selectionIndex = static_cast<int>( *activeIndex );

    // Iterate over all ordered parcellation UIDs
    for ( const auto& parcelUid : m_dataManager.orderedParcellationUids() )
    {
        auto parcelRecord = m_dataManager.parcellationRecord( parcelUid ).lock();

        if ( parcelRecord && parcelRecord->cpuData() )
        {
            gui::ParcellationSelectionItem item;
            item.m_parcelUid = parcelUid;
            item.m_displayName = parcelRecord->cpuData()->settings().displayName();

            msg.m_selectionItems.emplace_back( std::move( item ) );
        }
    }

    return msg;
}


std::optional< gui::ParcellationPropertiesComplete_msgToUi >
ParcellationDataUiMapper::Impl::getActiveParcellationProperties() const
{
    const auto activeParcelUid = m_dataManager.activeParcellationUid();
    if ( ! activeParcelUid )
    {
        return std::nullopt;
    }

    auto activeParcelRecord = m_dataManager.parcellationRecord( *activeParcelUid ).lock();
    if ( ! activeParcelRecord || ! activeParcelRecord->cpuData() ||
         ! activeParcelRecord->cpuData()->imageBaseData() )
    {
        return std::nullopt;
    }

    auto cpuRecord = activeParcelRecord->cpuData();
    const auto& settings = cpuRecord->settings();

    gui::ParcellationPropertiesComplete_msgToUi msg;
    msg.m_parcelUid = *activeParcelUid;

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

    // Visibility:
    const auto sliceProps = m_assemblyManager.getImageSliceRenderingProperties();
    msg.m_properties.m_visibleIn2dViewsChecked = sliceProps.m_showParcellationIn2dViews;
    msg.m_properties.m_visibleIn3dViewsChecked = sliceProps.m_showParcellationIn3dViews;

    // Opacity:
    msg.m_properties.m_opacityRange = std::make_pair( 0, 100 );
    msg.m_properties.m_opacitySingleStep = 1;
    msg.m_properties.m_opacitySliderPageStep = 10;
    msg.m_properties.m_opacityValue = static_cast<int>( settings.opacity( 0 ) * 100.0 );

    // Meshes
    const auto meshProps = m_assemblyManager.getLabelMeshRenderingProperties();
    msg.m_meshProperties.m_meshesVisibleIn2dViews = meshProps.m_visibleIn2dViews;
    msg.m_meshProperties.m_meshesVisibleIn3dViews = meshProps.m_visibleIn3dViews;

    msg.m_meshProperties.m_meshesXrayModeChecked = meshProps.m_useXrayMode;

    msg.m_meshProperties.m_meshXrayPowerRange = std::make_pair( 0.1, 10.0 );
    msg.m_meshProperties.m_meshXrayPowerSingleStep = 0.1;
    msg.m_meshProperties.m_meshXrayPowerSpinBoxDecimals = 1;
    msg.m_meshProperties.m_meshXrayPowerValue = static_cast<double>( meshProps.m_xrayPower );

    msg.m_meshProperties.m_meshOpacityRange = std::make_pair( 0, 100 );
    msg.m_meshProperties.m_meshOpacitySingleStep = 1;
    msg.m_meshProperties.m_meshOpacitySliderPageStep = 10;
    msg.m_meshProperties.m_meshOpacityValue = static_cast<int>( meshProps.m_masterOpacityMultiplier * 100.0f );

    return msg;
}


std::optional< gui::ImageHeader_msgToUi >
ParcellationDataUiMapper::Impl::getActiveParcellationHeader() const
{
    const auto activeParcelUid = m_dataManager.activeParcellationUid();
    if ( ! activeParcelUid )
    {
        return std::nullopt;
    }

    auto activeParcelRecord = m_dataManager.parcellationRecord( *activeParcelUid ).lock();
    if ( ! activeParcelRecord || ! activeParcelRecord->cpuData() ||
         ! activeParcelRecord->cpuData()->imageBaseData() )
    {
        return std::nullopt;
    }

    gui::ImageHeader_msgToUi msg;
    msg.m_imageUid = *activeParcelUid;

    msg.m_items = details::packageImageHeaderForUi(
                activeParcelRecord->cpuData()->header(),
                activeParcelRecord->cpuData()->settings() );

    msg.m_subject_O_pixel = activeParcelRecord->cpuData()->transformations().subject_O_pixel();

    return msg;
}


std::optional< gui::ParcellationLabelsComplete_msgToUi >
ParcellationDataUiMapper::Impl::getActiveParcellationLabels() const
{
    // Active parcellation labels are the labels of the active parcellation

    const auto activeParcelUid = m_dataManager.activeParcellationUid();
    if ( ! activeParcelUid )
    {
        std::cerr << "No active parcellation" << std::endl;
        return std::nullopt;
    }

    auto activeParcelRecord = m_dataManager.parcellationRecord( *activeParcelUid ).lock();
    if ( ! activeParcelRecord )
    {
        std::cerr << "Null active parcellation " << *activeParcelUid << std::endl;
        return std::nullopt;
    }

    const auto activeParcelCpuRecord = activeParcelRecord->cpuData();
    if ( ! activeParcelCpuRecord )
    {
        std::cerr << "Null CPU record for active parcellation " << *activeParcelUid << std::endl;
        return std::nullopt;
    }

    const auto activeLabelsUid = m_dataManager.labelTableUid_of_parcellation( *activeParcelUid );
    if ( ! activeLabelsUid )
    {
        std::cerr << "Could not find label UID for active parcellation " << *activeParcelUid << std::endl;
        return std::nullopt;
    }

    auto activeLabelsRecord = m_dataManager.labelTableRecord( *activeLabelsUid ).lock();
    if ( ! activeLabelsRecord )
    {
        std::cerr << "Null label record for active parcellation " << *activeParcelUid << std::endl;
        return std::nullopt;
    }

    auto labelTable = activeLabelsRecord->cpuData();
    if ( ! labelTable )
    {
        std::cerr << "Null CPU record for active label table " << *activeParcelUid << std::endl;
        return std::nullopt;
    }


    gui::ParcellationLabelsComplete_msgToUi msg;
    msg.m_labelTableUid = *activeLabelsUid;

    // Loop over all label indices
    for ( uint32_t labelIndex = 0; labelIndex < labelTable->numLabels(); ++labelIndex )
    {
        const auto labelValue = activeParcelCpuRecord->labelValue( labelIndex );

        if ( ! labelValue )
        {
            // Invalid label index for the active parcellation.
            // The label table may contain more labels than the parcellation uses,
            // so just ignore these.
            continue;
        }

        gui::ParcellationLabel label;
        label.m_index = labelIndex;
        label.m_value = *labelValue;
        label.m_name = labelTable->getName( labelIndex );
        label.m_color = labelTable->getColor( labelIndex );
        label.m_alpha = static_cast<int>( labelTable->getAlpha( labelIndex ) * 100.0f );
        label.m_visible = labelTable->getVisible( labelIndex );
        label.m_showMesh = labelTable->getShowMesh( labelIndex );

        msg.m_labels.push_back( label );
    }

    return msg;
}
