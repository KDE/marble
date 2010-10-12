//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Niko Sams <niko.sams@gmail.com>
//

#ifndef MARBLE_ROUTINGPROFILESWIDGET_H
#define MARBLE_ROUTINGPROFILESWIDGET_H

#include <QWidget>
#include <QHash>
#include <QVariant>
#include <QModelIndex>

#include "marble_export.h"
#include <RunnerPlugin.h>

class Ui_RoutingSettingsWidget;
class QStandardItemModel;

namespace Marble
{

class RoutingProfilesModel;
class RunnerPlugin;
class MarbleWidget;
class RoutingProfileSettingsDialog;

class MARBLE_EXPORT RoutingProfilesWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit RoutingProfilesWidget( MarbleWidget *marbleWidget );
    ~RoutingProfilesWidget();

 private slots:
    void add();
    void configure();
    void remove();
    void moveUp();
    void moveDown();
    void updateButtons();

 private:
    MarbleWidget *m_marbleWidget;
    Ui_RoutingSettingsWidget *m_ui;
    RoutingProfilesModel *m_profilesModel;

    RoutingProfileSettingsDialog *m_profileDialog;
};

}

#endif
