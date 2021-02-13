#ifndef VIEW_WIDGET_H
#define VIEW_WIDGET_H

#include "gui/view/ViewSliderParams.h"
#include "common/UID.h"

#include <QWidget>

#include <functional>
#include <memory>
#include <tuple>


class IRenderer;
class ctkDoubleSlider;


namespace gui
{

class GLWidget;
class QRealScrollBar;


/**
 * @brief Widget wrapper around the widgets that make up a single view.
 * The GLWidget is parented by this class.
 */
class ViewWidget : public QWidget
{
    Q_OBJECT

    /// Functional providing the parameters for the horizontal/vertical scroll bars
    /// and slice slider for a given view
    using ScrollBarsAndSliderParamsProviderType =
        std::function< std::tuple< ViewSliderParams, ViewSliderParams, ViewSliderParams >
        ( const UID& viewUid ) >;

    /// Functional for broadcasting this view's horizontal and vertical scroll bar values.
    using ScrollBarValuesBroadcasterType =
        std::function< void ( const UID& viewUid, double xScrollBarValue, double yScrollBarValue ) >;

    /// Functional for broadcasting this view's slice slider value.
    using SliceSliderValueBroadcasterType =
        std::function< void ( const UID& viewUid, double sliderValue ) >;


public:

    explicit ViewWidget(
            const UID& viewUid,
            GLWidget* glWidget,
            ScrollBarValuesBroadcasterType xyScrollBarValuesBroadcaster,
            SliceSliderValueBroadcasterType sliceSliderValueBroadcaster,
            QWidget* parent = nullptr );

    ~ViewWidget() override = default;

    void resizeEvent( QResizeEvent* ) override;

    void setScrollBarsAndSliderParamsProvider( ScrollBarsAndSliderParamsProviderType );

    /// Set the functional that notifies the application of horizontal and
    /// vertical scroll bar value changes due to user movement of the scroll bars
    void setScrollBarValuesBroadcaster( ScrollBarValuesBroadcasterType );

    /// Set the functional that notifies the application of a slice slider
    /// value change due to a user movement of the slider
    void setSliceSliderValueBroadcaster( SliceSliderValueBroadcasterType );

    /// Set all horizontal scroll bar parameters at once
    void setHorizontalScrollBarParams( const ViewSliderParams& params );

    /// Set all horizontal scroll bar parameters at once
    void setVerticalScrollBarParams( const ViewSliderParams& params );

    /// Set all slice slider parameters at once
    void setSliceSliderParams( const ViewSliderParams& params );


    /// Get the UID of the view to which this widget belongs
    const UID& getViewUid() const;

    /// Get the renderer of the view
    IRenderer* getRenderer();

    /// Enqueue a re-render of the view
    void renderUpdate();


private:

    void updateScrollBarsAndSlider();

    UID m_viewUid; //!< Unique ID for this view
    GLWidget* m_glWidget;
    QRealScrollBar* m_xScrollBar;
    QRealScrollBar* m_yScrollBar;
    ctkDoubleSlider* m_zSlider;

    ScrollBarsAndSliderParamsProviderType m_scrollBarsAndSliderParamsProvider;
    ScrollBarValuesBroadcasterType m_xyScrollBarValuesBroadcaster;
    SliceSliderValueBroadcasterType m_sliceSliderValueBroadcaster;
};

} // namespace gui

#endif // VIEW_WIDGET_H
