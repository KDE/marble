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
#include <QtCore/QHash>

class KAboutData;
class KAction;
class KConfigDialog;

class QLabel;
class ControlView;
class SunControlWidget;
class QStandardItemModel;

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
    void  createInfoBoxesMenu();

  public Q_SLOTS:
    bool openUrl( const KUrl &url );
    bool openFile();
    void  showPosition( const QString& position);
    void  showDistance( const QString& position);

  private Q_SLOTS:
    void  exportMapScreenShot();
    void  printMapScreenShot();
    void  copyMap();
    void  copyCoordinates();
    void  setShowClouds( bool );
    void  setShowAtmosphere( bool );
    void  showFullScreen( bool );
    void  showSideBar( bool );
    void  showStatusBar( bool );
    void  lockFloatItemPosition( bool );
    void  showSun();
    void  setupStatusBar();
    void  showNewStuffDialog();

    void  editSettings();

    void  slotEnableButtonApply();
    void  slotApply();
    void  slotCancel();

    void  slotUpdateSettings();

  private:
    void  setupActions();

    void  readSettings();
    void  writeSettings();

  private:
    // All the functionality is provided by this widget.
    ControlView  *m_controlView;
    SunControlWidget* m_sunControlDialog;

    // Actions for the GUI.
    KAction      *m_exportMapAction;
    KAction      *m_printMapAction;
    KAction      *m_copyMapAction;
    KAction      *m_copyCoordinatesAction;
    KAction      *m_showCloudsAction;
    KAction      *m_showAtmosphereAction;
    KAction      *m_sideBarAct;
    KAction      *m_fullScreenAct;
    KAction      *m_openAct;
    KAction      *m_newStuffAction;
    KAction      *m_showSunAct;
    KAction      *m_lockFloatItemsAct;

    QStandardItemModel* m_pluginModel;

    KConfigDialog *m_configDialog;

    QHash<QString, int> m_pluginEnabled;

    QString m_position;
    QString m_distance;

    // Zoom label for the statusbar.
    QLabel       *m_positionLabel;
    QLabel       *m_distanceLabel;

    void updateStatusBar();

    KParts::StatusBarExtension *m_statusBarExtension;

};

#endif
