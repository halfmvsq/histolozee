#ifndef GUI_IMAGE_DOCK_WIDGET_H
#define GUI_IMAGE_DOCK_WIDGET_H

#include "common/UID.h"

#include "gui/docks/PublicTypes.h"
#include "gui/messages/image/ImageColorMapData.h"
#include "gui/messages/image/ImageHeaderData.h"
#include "gui/messages/image/ImagePropertyData.h"
#include "gui/messages/image/ImageSelectionData.h"
#include "gui/messages/image/ImageTransformationData.h"
#include "gui/messages/parcellation/ParcellationLabelData.h"
#include "gui/messages/parcellation/ParcellationPropertyData.h"
#include "gui/messages/parcellation/ParcellationSelectionData.h"

#include <QDockWidget>

#include <boost/optional.hpp>

#include <list>
#include <vector>


class ctkDoubleRangeSlider;
class ctkDoubleSlider;
class ctkDoubleSpinBox;
class ctkMatrixWidget;
class ctkPathLineEdit;

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QLayout;
class QLineEdit;
//class QListView;
class QPushButton;
class QRadioButton;
class QScrollArea;
class QSlider;
class QSpinBox;
//class QStandardItemModel;
class QTableView;
class QTableWidget;
class QTabWidget;
class QToolButton;
class QTreeView;
class QWidget;


namespace gui
{

class LabelTableModel;
class LabelColorDialogDelegate;
class TreeModel;


/**
 * @brief Dock widget for selecting reference images/parcellations and for interactively
 * modifying/viewing their properties.
 *
 * @todo This class has become enormous and needs to be split up!
 */
class RefFrameEditorDock : public QDockWidget
{
    Q_OBJECT

public:

    explicit RefFrameEditorDock( QWidget* parent = nullptr );

    ~RefFrameEditorDock() override;


    /// Set the function that notifies the app of image selection changes in the UI
    void setImageSelectionsPublisher( ImageSelections_msgFromUi_PublisherType );

    /// Set the function that notifies the app of image property changes in the UI
    void setImagePropertiesPartialPublisher( ImagePropertiesPartial_msgFromUi_PublisherType );

    /// Set the function that notifies the app of image transformation changes in the UI
    void setImageTransformationPublisher( ImageTransformation_msgFromUi_PublisherType );


    /// Set the function that notifies the app of parcellation selection changes in the UI
    void setParcellationSelectionsPublisher( ParcellationSelection_msgFromUi_PublisherType );

    /// Set the function that notifies the app of parcellation property changes in the UI
    void setParcellationPropertiesPartialPublisher( ParcellationPropertiesPartial_msgFromUi_PublisherType );

    /// Set the function that notifies the app of parcellation label changes in the UI
    void setParcellationLabelsPartialPublisher( ParcellationLabelsPartial_msgFromUi_PublisherType );


    /// Set the function that provides the UI with image selection items from the app on request
    void setImageSelectionsResponder( ImageSelections_msgToUi_ResponderType );

    /// Set the function that provides the UI with image header from the app on request
    void setImageHeaderResponder( ImageHeader_msgToUi_ResponderType );

    /// Set the function that provides the UI with all image properties from the app on request
    void setImagePropertiesCompleteResponder( ImagePropertiesComplete_msgToUi_ResponderType );

    /// Set the function that provides the UI with the image transformation from the app on request
    void setImageTransformationResponder( ImageTransformation_msgToUi_ResponderType );



    /// Set the function that provides the UI with parcellation selection items from the app on request
    void setParcellationSelectionsResponder( ParcellationSelections_msgToUi_ResponderType );

    /// Set the function that provides the UI with all parcellation properties from the app on request
    void setParcellationPropertiesCompleteResponder( ParcellationPropertiesComplete_msgToUi_ResponderType );

    /// Set the function that provides the UI with parcellation header from the app on request
    void setParcellationHeaderResponder( ImageHeader_msgToUi_ResponderType );

    /// Set the function that provides the UI with all parcellation labels from the app on request
    void setParcellationLabelsCompleteResponder( ParcellationLabelsComplete_msgToUi_ResponderType );


    /// Set the function that provides the UI with image color map items from the app on request
    void setImageColorMapsResponder( ImageColorMaps_msgToUi_ResponderType );

    /// Request data from app and update the dock's widgets
    void refresh();


public slots:

    // Returns a checkable action that can be used to show or close this dock widget.
    // The action's text is set to the dock widget's window title.
    // QAction *QDockWidget::toggleViewAction() const

    /// Set image selection items
    void setImageSelections( const ImageSelections_msgToUi& );

    /// Set parcellation selection items
    void setParcellationSelections( const ParcellationSelections_msgToUi& );


    /// Set image color map items
    void setImageColorMaps( const ImageColorMaps_msgToUi& );


    /// Set some image properties (i.e. only those properties that have changed)
    void setImagePropertiesPartial( const ImagePropertiesPartial_msgToUi& );

    /// Set all image properties
    void setImagePropertiesComplete( const ImagePropertiesComplete_msgToUi& );

    /// Set image header
    void setImageHeader( const ImageHeader_msgToUi& );

    /// Set image transformation
    void setImageTransformation( const ImageTransformation_msgToUi& );


    /// Set some parcellation properties (i.e. only those properties that have changed)
    void setParcellationPropertiesPartial( const ParcellationPropertiesPartial_msgToUi& );

    /// Set all parcellation properties
    void setParcellationPropertiesComplete( const ParcellationPropertiesComplete_msgToUi& );

    /// Set all parcellation labels
    void setParcellationLabelsComplete( const ParcellationLabelsComplete_msgToUi& );

    /// Set parcellation header
    void setParcellationHeader( const ImageHeader_msgToUi& );


private:

    /// Get all properties for current image from UI
    boost::optional< ImagePropertiesComplete_msgFromUi > getImagePropertiesComplete() const;

    /// Get all properties for current parcellation from UI
    boost::optional< ParcellationPropertiesComplete_msgFromUi > getParcellationPropertiesComplete() const;

    /// Get all labels for current parcellation from UI
    boost::optional< ParcellationLabelsComplete_msgToUi > getParcellationLabelsComplete() const;


    /// Connect image widget signals and slots
    void connectImageWidgets();

    /// Connect parcellation widget signals and slots
    void connectParcellationWidgets();

    /// Connect transformation widget signals and slots
    void connectTransformationWidgets();


    /// Request image selection items from the app and set them in the UI
    void updateImageSelections();

    /// Request image color map items from the app and set them in the UI
    void updateImageColorMaps();

    /// Request image properties from the app and set them in the UI
    void updateImageProperties();

    /// Request image header from the app and set it in the UI
    void updateImageHeader();

    /// Request image transformation from the app and set it in the UI
    void updateImageTransformation();

    /// Update the color map description in the UI
    void updateImageColorMapDescription( int colorMapSelectionIndex );


    /// Request parcellation selection items from the app and set them in the UI
    void updateParcellationSelections();

    /// Request parcellation properties from the app and set them in the UI
    void updateParcellationProperties();

    /// Request parcellation header from the app and set it in the UI
    void updateParcellationHeader();

    /// Request parcellation labels from the app and set them in the UI
    void updateParcellationLabels();


    /// Block/unblock all signals from image widgets in order to avoid signal-slot ringing
    void blockWidgetSignals( bool block );

    /// Enable/disable all dock widgets
    void setWidgetsEnabled( bool enable );


    /// Create widget for selecting, loading, and unloading 3D reference images and labels
    QWidget* createImageSelectorWidget();

    QWidget* createImageTab();

    QLayout* createImagePropertiesLayout();
    QGroupBox* createImagePropertiesGroupBox();

    QWidget* createImageHeaderTableWidget();
    QGroupBox* createImageHeaderGroupBox();

    QLayout* createImageTransformLayout();
    QGroupBox* createImageTransformationGroupBox();

    QLayout* createImageLandmarksLayout();
    QGroupBox* createImageLandmarksGroupBox();

    QScrollArea* createImageScrollArea();
    QGroupBox* createImageSurfacesWidget();


    QWidget* createParcellationTab();

    QWidget* createLabelTableView();

    QGroupBox* createParcelPropertiesGroupBox();
    QLayout* createParcelPropertiesLayout();

    QGroupBox* createParcelLabelMeshPropertiesGroupBox();
    QLayout* createLabelMeshPropertiesLayout();

    QGroupBox* createParcelLabelTableGroupBox();

    QWidget* createParcelHeaderTableWidget();
    QGroupBox* createParcelHeaderGroupBox();
    QScrollArea* createParcellationScrollArea();


    /// Create the tabs for controlling image and parcellation properties
    QTabWidget* createTabWidget();


    /// Function that publishes UI changes to image selection to the app
    ImageSelections_msgFromUi_PublisherType m_imageSelectionsPublisher;

    /// Function that publishes UI changes to parcellation selection to the app
    ParcellationSelection_msgFromUi_PublisherType m_parcelSelectionsPublisher;


    /// Function that publishes UI changes to image properties to the app
    ImagePropertiesPartial_msgFromUi_PublisherType m_imagePropertiesPartialPublisher;

    /// Function that publishes UI changes to image transformation to the app
    ImageTransformation_msgFromUi_PublisherType m_imageTransformationPublisher;

    /// Function that publishes UI changes to parcellation properties to the app
    ParcellationPropertiesPartial_msgFromUi_PublisherType m_parcelPropertiesPartialPublisher;

    /// Function that publishes UI changes to parcellation labels to the app
    ParcellationLabelsPartial_msgFromUi_PublisherType m_parcelLabelsPartialPublisher;


    /// Function that responds to request for image selection items from the app
    ImageSelections_msgToUi_ResponderType m_imageSelectionsResponder;

    /// Function that responds to request for parcellation selection items from the app
    ParcellationSelections_msgToUi_ResponderType m_parcelSelectionsResponder;


    /// Function that responds to request for image properties from the app
    ImagePropertiesComplete_msgToUi_ResponderType m_imagePropertiesCompleteResponder;

    /// Function that responds to request for image header from the app
    ImageHeader_msgToUi_ResponderType m_imageHeaderResponder;

    /// Function that responds to request for image transformation from the app
    ImageTransformation_msgToUi_ResponderType m_imageTransformationResponder;

    /// Function that responds to request for parcellation properties from the app
    ParcellationPropertiesComplete_msgToUi_ResponderType m_parcelPropertiesCompleteResponder;

    /// Function that responds to request for parcellation header from the app
    ParcellationHeader_msgToUi_ResponderType m_parcelHeaderResponder;

    /// Function that responds to request for parcellation labels from the app
    ParcellationLabelsComplete_msgToUi_ResponderType m_parcelLabelsCompleteResponder;

    /// Function that responds to request for image color map items from the app
    ImageColorMaps_msgToUi_ResponderType m_imageColorMapsResponder;


    struct SelectionWidgets
    {
        QComboBox* m_imageSelectionComboBox = nullptr; // image selections

//        QListView* m_imageSelectionListView = nullptr; // image selections
//        QStandardItemModel* m_imageSelectionItemModel = nullptr;

        QToolButton* m_imageLoadButton = nullptr; // image loading
        QToolButton* m_imageUnloadButton = nullptr; // image unloading

        QComboBox* m_parcelSelectionComboBox = nullptr; // parcellation selections
        QToolButton* m_parcelLoadButton = nullptr; // parcellation loading
        QToolButton* m_parcelUnloadButton = nullptr; // parcellation unloading
    } m_selectionWidgets;


    struct ImageWidgets
    {
        ctkPathLineEdit* m_pathLineEdit = nullptr; // path
        QLineEdit* m_displayNameLineEdit = nullptr; // name

        QComboBox* m_colorMapComboBox = nullptr; // color map
        QLineEdit* m_colorMapDescriptionLineEdit = nullptr;

        QSlider* m_opacitySlider = nullptr; // opacity
        QSpinBox* m_opacitySpinBox = nullptr;

        ctkDoubleRangeSlider* m_windowRangeSlider = nullptr; // windowing
        ctkDoubleSpinBox* m_windowMinSpinBox = nullptr;
        ctkDoubleSpinBox* m_windowMaxSpinBox = nullptr;

        ctkDoubleRangeSlider* m_threshRangeSlider = nullptr; // thresholding
        ctkDoubleSpinBox* m_threshLowSpinBox = nullptr;
        ctkDoubleSpinBox* m_threshHighSpinBox = nullptr;

        QRadioButton* m_samplingNNRadioButton = nullptr; // sampling
        QRadioButton* m_samplingLinearRadioButton = nullptr;

        QCheckBox* m_planesVisibleIn2dViewsCheckBox = nullptr; // image planes visibility in 2D views
        QCheckBox* m_planesVisibleIn3dViewsCheckBox = nullptr; // image planes visibility in 2D views
        QCheckBox* m_planesAutoHideCheckBox = nullptr; // image planes auto-hiding mode

        QTableWidget* m_headerTableWidget = nullptr; // header table

        // Widget with 4x4 affine tx from Pixels to Subject space:
        ctkMatrixWidget* m_subject_O_pixels_matrixWidget = nullptr;
    } m_imageWidgets;


    struct ParcellationWidgets
    {
        ctkPathLineEdit* m_pathLineEdit = nullptr; // path
        QLineEdit* m_displayNameLineEdit = nullptr; // display name

        // parcellation visibility on image slices:
        QCheckBox* m_visibilityIn2dViewsCheckBox = nullptr; // visibility in 2D views
        QCheckBox* m_visibilityIn3dViewsCheckBox = nullptr; // visibility in 3D views

        // parcellation opacity on image slices:
        QSlider* m_opacitySlider = nullptr;
        QSpinBox* m_opacitySpinBox = nullptr;

        // label mesh visibility in 2D and 3D views:
        QCheckBox* m_meshesVisibleIn2dViewsCheckBox = nullptr;
        QCheckBox* m_meshesVisibleIn3dViewsCheckBox = nullptr;

        // advanced "x-ray" rendering style for meshes:
        QCheckBox* m_meshesXrayModeCheckBox = nullptr;
        ctkDoubleSpinBox* m_meshesXrayPowerSpinBox = nullptr;

        // overall label mesh opacity:
        QSlider* m_meshOpacitySlider = nullptr;
        QSpinBox* m_meshOpacitySpinBox = nullptr;

        QTableView* m_labelTableView = nullptr; // label table
        QTableWidget* m_headerTableWidget = nullptr; // header table

        // buttons to show/hide all labels:
        QPushButton* m_showAllLabelsButton = nullptr;
        QPushButton* m_hideAllLabelsButton = nullptr;

        // buttons to show/hide all label meshes:
        QPushButton* m_showAllMeshesButton = nullptr;
        QPushButton* m_hideAllMeshesButton = nullptr;

        // widget with 4x4 affine tx from Pixels to Subject space:
        ctkMatrixWidget* m_subject_O_pixels_matrixWidget = nullptr;
    } m_parcelWidgets;


    // Widgets for viewing and editing the image's transformation from Subject to World space
    struct TransformWidgets
    {
        // Widget with 4x4 matrix tx from Subject to World space
        ctkMatrixWidget* m_world_O_subject_matrixWidget = nullptr;

        QDoubleSpinBox* m_xTranslationSpinBox = nullptr;
        QDoubleSpinBox* m_yTranslationSpinBox = nullptr;
        QDoubleSpinBox* m_zTranslationSpinBox = nullptr;

        QDoubleSpinBox* m_xRotationSpinBox = nullptr;
        QDoubleSpinBox* m_yRotationSpinBox = nullptr;
        QDoubleSpinBox* m_zRotationSpinBox = nullptr;

        QDoubleSpinBox* m_xOriginSpinBox = nullptr;
        QDoubleSpinBox* m_yOriginSpinBox = nullptr;

        // Set identity Subject to World tx
        QPushButton* m_setIdentityButton = nullptr;
    } m_transformWidgets;


    // Widgets for landmark points
    struct LandmarkWidgets
    {
        QTreeView* m_landmarkTreeView = nullptr;
    } m_landmarkWidgets;


    /// List of all selection widgets, which is used for iteration
    std::list< QWidget* > m_selectionWidgetsList;

    /// List of all image widgets, which is used for iteration
    std::list< QWidget* > m_imageWidgetsList;

    /// List of all parcellation widgets, which is used for iteration
    std::list< QWidget* > m_parcelWidgetsList;

    /// List of all transformation widgets, which is used for iteration
    std::list< QWidget* > m_transformWidgetsList;

    /// List of all landmarks widgets, which is used for iteration
    std::list< QWidget* > m_landmarkWidgetsList;



    /// Current image selection items
    std::vector< ImageSelectionItem > m_imageSelections;

    /// Current parcellation selection items
    std::vector< ParcellationSelectionItem > m_parcelSelections;

    /// Current image color map items
    std::vector< ImageColorMapItem > m_imageColorMaps;

    boost::optional<UID> m_currentImageUid; //!< UID of currently selected image
    boost::optional<UID> m_currentParcelUid; //!< UID of currently selected parcellation
    boost::optional<UID> m_currentLabelsUid; //!< UID of current parcellation label table

    LabelTableModel* m_labelTableModel = nullptr; //!< Label table model

    TreeModel* m_refImageLandmarkTreeModel = nullptr; //!< Reference image landmark model

    /// Custom delegate for editing colors of labels
    LabelColorDialogDelegate* m_labelColorDialogDelegate = nullptr;
};

} // namespace gui

#endif // GUI_IMAGE_DOCK_WIDGET_H
