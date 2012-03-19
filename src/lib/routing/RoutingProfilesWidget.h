//
// This file is part of the Marble Virtual Globe.
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

#include "marble_export.h"

class Ui_RoutingSettingsWidget;

namespace Marble
{

class RoutingProfilesModel;
class MarbleModel;

class MARBLE_EXPORT RoutingProfilesWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit RoutingProfilesWidget( MarbleModel *marbleModel );
    ~RoutingProfilesWidget();

 private slots:
    void add();
    void configure();
    void remove();
    void moveUp();
    void moveDown();
    void updateButtons();

 private:
    MarbleModel *const m_marbleModel;
    Ui_RoutingSettingsWidget *m_ui;
    RoutingProfilesModel *m_profilesModel;

    void *const dummy;
};

}

#endif
