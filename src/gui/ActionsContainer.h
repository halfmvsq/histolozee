#ifndef GUI_ACTIONS_CONTAINER_H
#define GUI_ACTIONS_CONTAINER_H

#include "common/PublicTypes.h"
#include "logic/interaction/InteractionHandlerType.h"

#include <QAction>
#include <QActionGroup>
#include <QObject>


namespace gui
{

/**
 * @brief Class for creating and holding on to all QActions used in the UI
 */
class ActionsContainer : public QObject
{
    Q_OBJECT

public:

    ActionsContainer(
            SetterType<InteractionModeType> pointerInteractionModeSetter,
            CrosshairsAlignerType crosshairsToAciveSlideAligner,
            CrosshairsAlignerType crosshairsToSlideStackAligner,
            CrosshairsAlignerType crosshairsToAnatomicalPlanesAligner,
            SetterType<bool> slideStackViews3dModeSetter,
            AllViewsResetterType allViewsResetter,
            std::function< void(void) > projectSaver,
            QAction* refImageDockTogglerAction,
            QAction* slideStackDockTogglerAction,
            QObject* parent = nullptr );

    ~ActionsContainer() = default;

    QActionGroup* pointerModeActionGroup();
    QActionGroup* showDockActionGroup();

    QAction* pointerAction();
    QAction* translateAction();
    QAction* rotateAction();
    QAction* zoomAction();
    QAction* windowLevelAction();

    QAction* refImageRotateAction();
    QAction* refImageTranslateAction();

    QAction* stackRotateAction();
    QAction* stackTranslateAction();

    QAction* slideRotateAction();
    QAction* slideStretchAction();
    QAction* slideTranslateAction();

    QAction* saveProject();
    QAction* alignCrosshairsToSlideAction();
    QAction* alignCrosshairsToAnatomicalPlanes();
    QAction* resetViewsAction();
    QAction* slideStackViews3dModeAction();

    QAction* refImageDockTogglerAction();
    QAction* slideStackDockTogglerAction();


private:

    /// Run me first!
    void createConnections();

    /// Function that sets the interaction mode
    SetterType<InteractionModeType> m_interactionModeSetter;

    CrosshairsAlignerType m_crosshairsToActiveSlideAligner;
    CrosshairsAlignerType m_crosshairsToSlideStackAligner;
    CrosshairsAlignerType m_crosshairsToAnatomicalPlanesAligner;

    /// Function that sets Slide Stack views to render slides in 3D mode
    SetterType<bool> m_slideStackViews3dModeSetter;

    AllViewsResetterType m_allViewsResetter;

    /// Function that saves the current project to its existing location
    std::function< void(void) > m_projectSaver;


    /// Pointer selection actions group
    QActionGroup* m_pointerModeSelectionGroup;

    QAction* m_crosshairsAction;
    QAction* m_cameraTranslateAction;
    QAction* m_cameraRotateAction;
    QAction* m_cameraZoomAction;
    QAction* m_refImageWindowLevelAction;

    QAction* m_refImageRotateAction;
    QAction* m_refImageTranslateAction;

    QAction* m_stackRotateAction;
    QAction* m_stackTranslateAction;

    QAction* m_slideRotateAction;
    QAction* m_slideStretchAction;
    QAction* m_slideTranslateAction;


    /// Save project
    QAction* m_saveProjectAction;

    /// Align crosshairs to the active slide
    QAction* m_alignCrosshairsToActiveSlideAction;

    /// Align crosshairs to the anatomical planes of the current reference image subject
    QAction* m_alignCrosshairsToAnatomicalPlanesAction;

    /// Reset views to default state
    /// @todo Call this "recenter"?
    QAction* m_resetViewsAction;

    /// Make Slide Stack views display slides in 3D
    QAction* m_slideStackViews3dModeAction;

    /// Toggling visibility of the reference image dock
    QAction* m_refImageDockTogglerAction;

    /// Toggling visibility of the slide stack dock
    QAction* m_slideStackDockTogglerAction;
};

} // namespace gui

#endif // GUI_ACTIONS_CONTAINER_H
