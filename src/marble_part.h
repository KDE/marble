//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig  <tokoe@kde.org>"
//

#ifndef MARBLE_PART_H
#define MARBLE_PART_H

#include <kparts/part.h>

class KAboutData;
class KAction;

class QLabel;
class ControlView;

namespace KParts {
class StatusBarExtension;
}

class MarblePart: public KParts::ReadOnlyPart
{
  Q_OBJECT

  public:
    MarblePart( QWidget *parentWidget, QObject *parent, const QStringList& );
    virtual ~MarblePart();

    ControlView *controlView() const;

    static KAboutData* createAboutData();

  public Q_SLOTS:
    bool openUrl( const KUrl &url );
    bool openFile();

    void  showZoom(int zoom);

  private Q_SLOTS:
    void  exportMapScreenShot();
    void  printMapScreenShot();
    void  copyMap();
    void  showFullScreen( bool );
    void  showSideBar( bool );
    void  showStatusBar( bool );
    void  setupStatusBar();

  private:
    void  setupActions();

    void  readSettings();
    void  writeSettings();

  private:
    // All the functionality is provided by this widget.
    ControlView  *m_controlView;

    // Zoom label for the statusbar.
    QLabel       *m_zoomLabel;

    // Actions for the GUI.
    KAction      *m_exportMapAction;
    KAction      *m_printMapAction;
    KAction      *m_copyMapAction;
    KAction      *m_sideBarAct;
    KAction      *m_fullScreenAct;
    KAction      *m_openAct;

    KParts::StatusBarExtension *m_statusBarExtension;
};

#endif
