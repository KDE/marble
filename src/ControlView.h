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


#include <QtGui/QWidget>
#include <QtGui/QPixmap>
#include <QtCore/QPointer>

#include "MarbleWidget.h"
#include "MarbleControlBox.h"

class QSplitter;
class QPrintDialog;
class QTextDocument;
class QMainWindow;
class QDockWidget;
class QMenu;

namespace Marble
{

class MarbleModel;

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

    MarbleWidget      *marbleWidget()  const { return m_marbleWidget; }
    MarbleModel       *marbleModel()         { return m_marbleWidget->model(); }

    void zoomIn();
    void zoomOut();
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

    void addGeoDataFile( QString filename );

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

    void showLegendDock( bool show );

 public slots:
    void printMapScreenShot( QPointer<QPrintDialog> dialog );
    void printPixmap( QPrinter * printer, const QPixmap& pixmap );
    void printPreview();
    void paintPrintPreview( QPrinter * printer );

    /**
      * Start the configured external map editor (or update it if it is already running)
      */
    void launchExternalMapEditor();

signals:
    void showMapWizard();
    void showUploadDialog();
    void mapThemeDeleted();

private Q_SLOTS:
     void showSearch();
    
 private:
    /**
      * Try to reach an external application server at localhost:8111. If none is running,
      * start the given application
      * @param application Executable to start when no server is running
      * @param argument Argument to set the download region for the external application.
      * Use placeholders %1-%4 for the borders
      */
    void synchronizeWithExternalMapEditor( const QString &application, const QString &argument );

    void printMap( QTextDocument &document, QString &text, QPrinter *printer );
    void printLegend( QTextDocument &document, QString &text );
    void printRouteSummary( QTextDocument &document, QString &text );
    void printDrivingInstructions( QTextDocument &document, QString &text );
    void printDrivingInstructionsAdvice( QTextDocument &document, QString &text );

    MarbleWidget      *m_marbleWidget;
    QString            m_externalEditor;
    QDockWidget       *m_searchDock;
    QDockWidget       *m_legendDock;
    CurrentLocationWidget* m_locationWidget;
};

}

#endif
