//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_MARBLENAVIGATIONSETTINGSWIDGET_H
#define MARBLE_MARBLENAVIGATIONSETTINGSWIDGET_H

#include <QWidget>

#include "marble_export.h"
#include "MarbleGlobal.h"

namespace Marble
{

class MARBLE_EXPORT MarbleNavigationSettingsWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit MarbleNavigationSettingsWidget( QWidget *parent = 0 );
    ~MarbleNavigationSettingsWidget();

    DragLocation dragLocation() const;

    OnStartup startupLocation() const;

    bool isInertialEarthRotationEnabled() const;

    bool isTargetVoyageAnimationEnabled() const;

    QString externalMapEditor() const;

 public Q_SLOTS:
    void setDragLocation( DragLocation dragLocation );

    void setStartupLocation( OnStartup onStartup );

    void setInertialEarthRotationEnabled( bool inertialEarthRotation );

    void setAnimateTargetVoyage( bool animateTargetVoyage );

    void setExternalMapEditor( const QString &externalMapEditor );

private:
    class Private;
    Private *d;
};

}

#endif
