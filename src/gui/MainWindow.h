#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "logic/interaction/InteractionHandlerType.h"

#include <boost/optional.hpp>

#include <QMainWindow>

#include <functional>
#include <string>


class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QProgressBar;
class QTabWidget;
class QToolBar;


namespace gui
{

class MainWindow : public QMainWindow
{
private:

    Q_OBJECT

    /// Functional for loading images and slides
    using ImageLoaderType =
        std::function< void (
            const std::string& filename,
            const boost::optional< std::string >& dicomSeriesUID ) >;

    /// Functional for loading images and parcellations
    using SlideLoaderType = std::function< void ( const std::string& filename ) >;

    /// Functional called when the current index of the QTabWidget for layouts changes.
    using ViewLayoutTabChangedPublisher = std::function< void ( int index ) >;


public:

    explicit MainWindow( QWidget* parent = nullptr );

    ~MainWindow() override = default;

    void setViewLayoutTabChangedPublisher( ViewLayoutTabChangedPublisher );

    void setImageLoader( ImageLoaderType );
    void setParcellationLoader( ImageLoaderType );
    void setSlideLoader( SlideLoaderType );


public slots:

    void setWorldPositionStatusText( const std::string& status );
    void setImageValueStatusText( const std::string& status );
    void setLabelValueStatusText( const std::string& status );

    void clearViewLayoutTabs();
    void insertViewLayoutTab( int index, QWidget* tab, const std::string& tabName );


signals:

    /// Signal emitted when the layout tab changees
    void layoutTabChanged( int index );


protected:

    void keyPressEvent( QKeyEvent* ) override;
    void resizeEvent( QResizeEvent* ) override;


private slots:

    /// @todo Implement loading/saving of projects

    //    void loadProjectFile(const QString &filename);

//    void newProject();
//    void openProject();
//    bool saveProject();
//    bool saveProjectAs();
//    bool maybeSaveProject();
//    void closeProject();

    // Import:
    void importImage();
    void importParcellation();
    void insertSlides();


private:

    void createUI();
    void createActions();
    void createMenuBar();
    void createStatusBar();
    void createViewLayoutTabWidget();


    QProgressBar* m_memoryUseProgressbar = nullptr;
    QLabel* m_memoryUseStatus = nullptr;

    QLabel* m_worldPosStatus = nullptr;
    QLabel* m_imageValueStatus = nullptr;
    QLabel* m_labelValueStatus = nullptr;

    QTabWidget* m_viewLayoutTabWidget = nullptr;
    QToolBar* m_toolbar = nullptr;
    QMenu* m_fileMenu = nullptr;

//    QActionGroup* projectActionGroup;
//    QAction* newProjectAction;
//    QAction* openProjectAction;
//    QAction* saveProjectAction;
//    QAction* saveProjectAsAction;
//    QAction* closeProjectAction;

    QActionGroup* m_imagesActionGroup = nullptr;
    QAction* m_importRefImageAction = nullptr;
    QAction* m_importParcellationAction = nullptr;

    QActionGroup* m_stackActionGroup = nullptr;
    QAction* m_insertSlidesAction = nullptr;

    ViewLayoutTabChangedPublisher m_viewLayoutTabChangedPublisher = nullptr;

    ImageLoaderType m_imageLoader = nullptr;
    ImageLoaderType m_parcellationLoader = nullptr;
    SlideLoaderType m_slideLoader = nullptr;
};

} // namespace gui

#endif // MAIN_WINDOW_H
