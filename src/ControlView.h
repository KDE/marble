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

namespace Marble
{

class MapThemeManager;

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
    MarbleControlBox  *marbleControl() const { return m_control;      }

    void zoomIn();
    void zoomOut();
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

    bool sideBarShown() const { return m_control->isVisible(); }

    QByteArray sideBarState() const;

    bool setSideBarState( const QByteArray &state );

    void addGeoDataFile( QString filename ) { m_marbleWidget->addGeoDataFile( filename ); }

    QPixmap mapScreenShot() { return m_marbleWidget->mapScreenShot(); }
    
    SunLocator* sunLocator() { return m_marbleWidget->sunLocator(); }
    
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

 public slots:
    void setSideBarShown( bool );    
    void setNavigationTabShown( bool );
    void setLegendTabShown( bool );
    void setMapViewTabShown( bool );
    void setCurrentLocationTabShown( bool );
    void setFileViewTabShown( bool );
    void printMapScreenShot( QPointer<QPrintDialog> dialog );
    void printPixmap( QPrinter * printer, const QPixmap& pixmap );
    void printPreview();
    void paintPrintPreview( QPrinter * printer );

    /**
      * Start the configured external map editor (or update it if it is already running)
      */
    void launchExternalMapEditor();
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

    //MarbleModel       *m_marbleModel;
    MarbleWidget      *m_marbleWidget;
    MarbleControlBox  *m_control;
    QSplitter         *m_splitter;
    MapThemeManager   *m_mapThemeManager;
    QString            m_externalEditor;
};

}

#endif
