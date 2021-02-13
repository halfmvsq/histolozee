#ifndef GUI_VIEW_SLIDER_PARAMS_H
#define GUI_VIEW_SLIDER_PARAMS_H

namespace gui
{

/**
 * @brief Parameters of a slider or scrollbar in a view.
 */
struct ViewSliderParams
{
    double m_minimum; //!< Range minimum value
    double m_maximum; //!< Range maximum value
    double m_singleStep; //!< Single (small) step size
    double m_pageStep; //!< Page (large) step size
    double m_value; //!< Current value

    /// Flag to enable/disable the slider. Disabling the slider may be needed when
    /// valid parameters cannot be set for it.
    bool m_enabled;
};

} // namespace gui

#endif // GUI_VIEW_SLIDER_PARAMS_H
