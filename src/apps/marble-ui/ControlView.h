//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// Testapplication with controls
//


#ifndef MARBLE_CONTROLVIEW_H
#define MARBLE_CONTROLVIEW_H


#include <QWidget>
#include <QPointer>

#include "MarbleWidget.h"

class QPrintDialog;
class QTextDocument;
class QMainWindow;
class QDockWidget;
class QPrinter;
class QActionGroup;
class QPixmap;

namespace Marble
{

class TourWidget;
class CurrentLocationWidget;
class MapThemeManager;
class ConflictDialog;
class MarbleModel;
class MergeItem;
class CloudSyncManager;

class ControlView : public QWidget
{
    Q_OBJECT

 public:
    explicit ControlView( QWidget * = 0 );
    virtual ~ControlView();

    /**
      * Returns the version of the Marble applications (which differs from
      * the Marble library version).
      */
    static QString applicationVersion();

    MarbleWidget      *marbleWidget()        { return m_marbleWidget; }
    MarbleModel       *marbleModel()         { return m_marbleWidget->model(); }
    MapThemeManager   *mapThemeManager();

    void zoomIn();
    void zoomOut();
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

    void addGeoDataFile( const QString &filename );

    QPixmap mapScreenShot() { return m_marbleWidget->mapScreenShot(); }
    
    /**
      * Returns a default map theme: earth/srtm/srtm.dgml if installed,
      * any other theme id if earth/srtm/srtm.dgml is not installed,
      * or an empty string if no themes are installed at all
      */
    QString defaultMapThemeId() const;

    /**
      * Returns the editor used to launch a map editor application
      */
    QString externalMapEditor() const;

    /**
      * Change the editor to launch via @see launchExternalMapEditor. Recognized values
      * are 'potlatch', 'josm', 'merkaartor'
      */
    void setExternalMapEditor( const QString &editor );

    QList<QAction*> setupDockWidgets( QMainWindow* mainWindow );

    CurrentLocationWidget* currentLocationWidget();

    void setWorkOffline( bool workOffline );

    CloudSyncManager* cloudSyncManager();

    /**
     * Opens the passed Geo URI
     * @return true if uri could be parsed and opened
     * @see Marble::GeoUriParser for details
     */
    bool openGeoUri( const QString& geoUriString );

    static QActionGroup* createViewSizeActionGroup( QObject* parent );

 public Q_SLOTS:
    void printMapScreenShot( QPointer<QPrintDialog> dialog );
    void printPreview();
    void paintPrintPreview( QPrinter * printer );

    /**
      * Start the configured external map editor (or update it if it is already running)
      */
    void launchExternalMapEditor();

    /**
     *  Toggles all of the docking panels on or off
     */
    void togglePanelVisibility();

    void handleTourLinkClicked( const QString &path );

    void openTour( const QString &filename );

Q_SIGNALS:
    void showMapWizard();
    void showUploadDialog();
    void mapThemeDeleted();

protected:
    void closeEvent( QCloseEvent *event );
    /**
     * @brief Reimplementation of the dragEnterEvent() function in QWidget.
     */
    void dragEnterEvent(QDragEnterEvent *event);

    /**
     * @brief Reimplementation of the dropEvent() function in QWidget.
     */
    void dropEvent(QDropEvent *event);

private Q_SLOTS:
    void showSearch();
    // Bookmark sync slots
    void showConflictDialog( MergeItem *item );
    void updateAnnotationDockVisibility();
    void updateAnnotationDock();
    
 private:
    /**
      * Try to reach an external application server at localhost:8111. If none is running,
      * start the given application
      * @param application Executable to start when no server is running
      * @param argument Argument to set the download region for the external application.
      * Use placeholders %1-%4 for the borders
      */
    void synchronizeWithExternalMapEditor( const QString &application, const QString &argument );

    static void printPixmap( QPrinter * printer, const QPixmap& pixmap );
    void printMap( QTextDocument &document, QString &text, QPrinter *printer );
    void printLegend( QTextDocument &document, QString &text );
    void printRouteSummary( QTextDocument &document, QString &text );
    void printDrivingInstructions( QTextDocument &document, QString &text );
    static void printDrivingInstructionsAdvice( QTextDocument &document, QString &text );
    static void addViewSizeAction( QActionGroup* actionGroup, const QString &nameTemplate, int width, int height );

    MapThemeManager   *const m_mapThemeManager;
    MarbleWidget      *m_marbleWidget;
    QString            m_externalEditor;
    QDockWidget       *m_searchDock;
    CurrentLocationWidget* m_locationWidget;
    ConflictDialog *m_conflictDialog;
    CloudSyncManager *m_cloudSyncManager;
    QAction         *m_togglePanelVisibilityAction;
    QList<QAction*>  m_panelActions;
    QList<bool>      m_panelVisibility;
    bool             m_isPanelVisible;
    TourWidget      *m_tourWidget;
    QDockWidget     *m_annotationDock;
    RenderPlugin    *m_annotationPlugin;
};

}

#endif
