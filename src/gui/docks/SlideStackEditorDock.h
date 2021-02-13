#ifndef GUI_SLIDE_DOCK_WIDGET_H
#define GUI_SLIDE_DOCK_WIDGET_H

#include "common/UID.h"

#include "gui/docks/PublicSlideTypes.h"
#include "gui/messages/slide/SlideData.h"

#include <QDockWidget>

#include <list>
#include <optional>


class ctkDoubleSlider;
class ctkDoubleSpinBox;
class ctkMatrixWidget;
class ctkRangeSlider;

class QButtonGroup;
class QCheckBox;
class QDoubleSpinBox;
class QDoubleValidator;
class QGroupBox;
class QLabel;
class QLayout;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QScrollArea;
class QSlider;
class QSpinBox;
class QTableView;
class QTableWidget;
class QTabWidget;
class QToolButton;
class QWidget;


namespace gui
{

class PixmapDelegate;
class SlideSorterTableModel;


/**
 * @brief Dock widget for sorting slides and changing their properties
 */
class SlideStackEditorDock : public QDockWidget
{
    Q_OBJECT

public:

    explicit SlideStackEditorDock( QWidget* parent = nullptr );

    ~SlideStackEditorDock() override;


    /// Set function to notify the app of some slide stack data
    void setSlideStackPartialPublisher( SlideStackPartial_msgFromUi_PublisherType );

    /// Set function to notify the app of slide stack order
    void setSlideStackOrderPublisher( SlideStackOrder_msgFromUi_PublisherType );

    /// Set function to notify the app of the active slide
    void setActiveSlidePublisher( ActiveSlide_msgFromUi_PublisherType );

    /// Set function to notify the app of some slide stack common properties
    void setSlideCommonPropertiesPartialPublisher( SlideCommonPropertiesPartial_msgFromUi_PublisherType );

    /// Set function to notify the app of some slide header data
    void setSlideHeaderPartialPublisher( SlideHeaderPartial_msgFromUi_PublisherType );

    /// Set function to notify the app of some slide view data
    void setSlideViewDataPartialPublisher( SlideViewDataPartial_msgFromUi_PublisherType );

    /// Set function to notify the app of some slide transformation data
    void setSlideTxDataPartialPublisher( SlideTxDataPartial_msgFromUi_PublisherType );

    /// Set function to notify the app of the slide to move to
    void setMoveToSlidePublisher( MoveToSlide_msgFromUi_PublisherType );



    /// Set function that provides the UI with all slide stack data
    void setSlideStackCompleteResponder( SlideStackComplete_msgToUi_ResponderType );

    /// Set function that provides the UI with the active slide
    void setActiveSlideResponder( ActiveSlide_msgToUi_ResponderType );

    /// Set function that provides the UI with all slide stack common properties
    void setSlideCommonPropertiesCompleteResponder( SlideCommonPropertiesComplete_msgToUi_ResponderType );

    /// Set function that provides the UI with all slide header data
    void setSlideHeaderCompleteResponder( SlideHeaderComplete_msgToUi_ResponderType );

    /// Set function that provides the UI with all slide view data
    void setSlideViewDataCompleteResponder( SlideViewDataComplete_msgToUi_ResponderType );

    /// Set function that provides the UI with all slide transformation data
    void setSlideTxDataCompleteResponder( SlideTxDataComplete_msgToUi_ResponderType );

    /// Request data from app and update the dock's widgets
    void refresh();


public slots:

    /// Set all slide stack data
    void setSlideStackComplete( const SlideStackComplete_msgToUi& );

    /// Set some slide stack data
    void setSlideStackPartial( const SlideStackPartial_msgToUi& );

    /// Set active slide
    void setActiveSlide( const ActiveSlide_msgToUi& );

    /// Set some slide stack common properties
    void setSlideCommonPropertiesPartial( const SlideCommonPropertiesPartial_msgToUi& );

    /// Set all slide stack common properties
    void setCommonSlidePropertiesComplete( const SlideCommonPropertiesComplete_msgToUi& );

    /// Set all slide header data
    void setSlideHeaderComplete( const SlideHeaderComplete_msgToUi& );

    /// Set all slide view data
    void setSlideViewDataComplete( const SlideViewDataComplete_msgToUi& );

    /// Set some slide view data
    void setSlideViewDataPartial( const SlideViewDataPartial_msgToUi& );

    /// Set all slide transformation data
    void setSlideTxDataComplete( const SlideTxDataComplete_msgToUi& );

    /// Set some slide transformation data
    void setSlideTxDataPartial( const SlideTxDataPartial_msgToUi& );


private:

    /// Block/unblock all signals from widgets in order to avoid signal-slot ringing
    void blockWidgetSignals( bool block );

    /// Enable/disable all dock widgets
    void setWidgetsEnabled( bool enabled );

    /// Clear values in all widgets
    void clearAllWidgetValues();


    std::optional<int> getActiveSlideIndex() const;
    std::optional<UID> getActiveSlideUid() const;


    /**
     * @brief Select a row of the slide sorter table.
     * @param[in] row Row to select in table.
     */
    void selectSlideIndex( int row );


    /**
     * @brief Update the header, view, and transformation tab widgets with data for the active slide
     * @param activeSlideUid Active slide UID
     */
    void updateSlideTabWidgets( const UID& activeSlideUid );


    /**
     * @brief Check if slide is active
     * @param slideUid Slide to check
     * @return True iff this is the active slide
     */
    bool isActiveSlide( const UID& slideUid );


    /**
     * @brief Publish message that centers the crosshairs on a given slide
     * @param slideIndex Index of slide in Slide Sorter Model
     */
    void moveToSlide( int slideIndex );

    // Publishers from UI to app:
    SlideStackPartial_msgFromUi_PublisherType m_slideStackPartialPublisher;
    SlideStackOrder_msgFromUi_PublisherType m_slideStackOrderPublisher;
    ActiveSlide_msgFromUi_PublisherType m_activeSlidePublisher;
    SlideCommonPropertiesPartial_msgFromUi_PublisherType m_slideStackRenderingPartialPublisher;
    SlideHeaderPartial_msgFromUi_PublisherType m_slideHeaderPartialPublisher;
    SlideViewDataPartial_msgFromUi_PublisherType m_slideViewDataPartialPublisher;
    SlideTxDataPartial_msgFromUi_PublisherType m_slideTxDataPartialPublisher;
    MoveToSlide_msgFromUi_PublisherType m_moveToSlidePublisher;

    // Responders with data from app to UI:
    SlideStackComplete_msgToUi_ResponderType m_slideStackCompleteResponder;
    ActiveSlide_msgToUi_ResponderType m_activeSlideResponder;
    SlideCommonPropertiesComplete_msgToUi_ResponderType m_slideStackRenderingCompleteResponder;
    SlideHeaderComplete_msgToUi_ResponderType m_slideHeaderCompleteResponder;
    SlideViewDataComplete_msgToUi_ResponderType m_slideViewDataCompleteResponder;
    SlideTxDataComplete_msgToUi_ResponderType m_slideTxDataCompleteResponder;


    QWidget* createSlideSorderWidget();
    QTabWidget* createTabWidget();

    QLayout* createCommonPropertiesLayout();
    QGroupBox* createCommonPropertiesGroupBox();

    QLayout* createCommonStackTxLayout();
    QGroupBox* createCommonStackTxGroupBox();

    QScrollArea* createSlideStackCommonScrollArea();

    QWidget* createSlideSorterTableView();

    QWidget* createHeaderTab();
    QWidget* createViewTab();
    QWidget* createTxTab();
    QWidget* createAnnotationTab();

    QScrollArea* createHeaderScrollArea();
    QScrollArea* createViewScrollArea();
    QScrollArea* createTxScrollArea();
    QScrollArea* createAnnotationScrollArea();

    QLayout* createHeaderTabLayout();
    QLayout* createViewTabLayout();
    QLayout* createTxTabLayout();
    QLayout* createAnnotationTabLayout();

    void connectCommonWidgets();
    void connectHeaderWidgets();
    void connectViewWidgets();
    void connectTxWidgets();


    struct HeaderWidgets
    {
        QLineEdit* m_pixelSizeHorizLineEdit = nullptr;
        QLineEdit* m_pixelSizeVertLineEdit = nullptr;
        QLineEdit* m_thicknessLineEdit = nullptr;

        QDoubleValidator* m_pixelSizeHorizLineEditValidator = nullptr;
        QDoubleValidator* m_pixelSizeVertLineEditValidator = nullptr;
        QDoubleValidator* m_thicknessLineEditValidator = nullptr;

        QLineEdit* m_fileNameLineEdit = nullptr;
        QLineEdit* m_displayNameLineEdit = nullptr;
        QLineEdit* m_vendorIdLineEdit = nullptr;

        QTableWidget* m_layerDimsTableWidget = nullptr;

        QLabel* m_labelImageLabel = nullptr;
        QLabel* m_macroImageLabel = nullptr;
    } m_headerWidgets;


    struct ViewWidgets
    {
        QCheckBox* m_showSlideCheckBox = nullptr;

        QToolButton* m_borderColorButton = nullptr;

        QSlider* m_opacitySlider = nullptr;
        QSpinBox* m_opacitySpinBox = nullptr;

        ctkRangeSlider* m_threshRangeSlider = nullptr;
        QSpinBox* m_threshLowSpinBox = nullptr;
        QSpinBox* m_threshHighSpinBox = nullptr;

        QCheckBox* m_showEdgesCheckBox = nullptr;

        ctkDoubleSlider* m_edgesMagnitudeSlider = nullptr;
        ctkDoubleSpinBox* m_edgesMagnitudeSpinBox = nullptr;

        ctkDoubleSlider* m_edgesSmoothingSlider = nullptr;
        ctkDoubleSpinBox* m_edgesSmoothingSpinBox = nullptr;
    } m_viewWidgets;


    struct TransformWidgets
    {
        QDoubleSpinBox* m_xTranslationSpinBox = nullptr;
        QDoubleSpinBox* m_yTranslationSpinBox = nullptr;
        QDoubleSpinBox* m_zTranslationSpinBox = nullptr;

        QDoubleSpinBox* m_zRotationSpinBox = nullptr;

        QDoubleSpinBox* m_xScaleSpinBox = nullptr;
        QDoubleSpinBox* m_yScaleSpinBox = nullptr;

        QDoubleSpinBox* m_zScaleRotationSpinBox = nullptr;

        QDoubleSpinBox* m_xShearSpinBox = nullptr;
        QDoubleSpinBox* m_yShearSpinBox = nullptr;

        QDoubleSpinBox* m_xOriginSpinBox = nullptr;
        QDoubleSpinBox* m_yOriginSpinBox = nullptr;

        QPushButton* m_setIdentityButton = nullptr;

        QButtonGroup* m_paramButtonGroup = nullptr;
        QRadioButton* m_paramScaleRotationRadioButton = nullptr;
        QRadioButton* m_paramShearAnglesRadioButton = nullptr;

        ctkMatrixWidget* m_stack_O_slide_matrixWidget = nullptr;
    } m_transformWidgets;


    struct CommonWidgets
    {
        // Global slide stack visibility in 2D/3D views:
        QCheckBox* m_visibleIn2dViewsCheckBox = nullptr;
        QCheckBox* m_visibleIn3dViewsCheckBox = nullptr;

        // Global slide stack opacity:
        QSlider* m_masterOpacitySlider = nullptr;
        QSpinBox* m_masterOpacitySpinBox = nullptr;

        // Image 3D layer opacity on slides:
        QSlider* m_image3dLayerOpacitySlider = nullptr;
        QSpinBox* m_image3dLayerOpacitySpinBox = nullptr;

        // Option for Active Slide view to show slides as either 2D or 3D objects:
        QRadioButton* m_activeSlideViewShows2dSlidesRadioButton = nullptr;
        QRadioButton* m_activeSlideViewShows3dSlidesRadioButton = nullptr;

        // Button to toggle view direction Active Slide view between negative (last to first slide)
        // and positive (first to last slide) slide stack direction.
        QPushButton* m_activeSlideViewDirectionButton;

        // Widget showing matrix that transforms slide Stack to World space:
        ctkMatrixWidget* m_world_O_stack_matrixWidget = nullptr;

        // Button to set world_O_stack to identity:
        QPushButton* m_setIdentityButton = nullptr;
    } m_commonWidgets;


    /// Slide sorter table view
    QTableView* m_slideSorterTableView = nullptr;

    /// Button to move crosshairs to currently selected (active) slide
    QPushButton* m_moveToSlideButton = nullptr;

    /// Slide sorter table model
    std::unique_ptr<SlideSorterTableModel> m_slideSorterTableModel;

    /// Delegate for decoration role in table
    std::unique_ptr<PixmapDelegate> m_slideSorterPixmapDelegate;

    /// List of all widgets in dock, used for iteration
    std::list< QWidget* > m_widgetsList;

    /// UID of currently selected/active slide
    std::optional<UID> m_activeSlideUid;
};

} // namespace gui

#endif // GUI_SLIDE_DOCK_WIDGET_H
