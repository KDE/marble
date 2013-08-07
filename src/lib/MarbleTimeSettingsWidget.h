//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_MARBLETIMESETTINGSWIDGET_H
#define MARBLE_MARBLETIMESETTINGSWIDGET_H

#include <QWidget>

#include "marble_export.h"

namespace Marble
{

class MARBLE_EXPORT MarbleTimeSettingsWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit MarbleTimeSettingsWidget( QWidget *parent = 0 );
    ~MarbleTimeSettingsWidget();

    bool isSystemTimezoneEnabled() const;

    bool isCustomTimezoneEnabled() const;

    int customTimezone() const;

    bool isUtcEnabled() const;

    bool isSystemTimeEnabled() const;

    bool isLastSessionTimeEnabled() const;

 public Q_SLOTS:
    void setSystemTimezoneEnabled( bool enabled );

    void setCustomTimezoneEnabled( bool enabled );

    void setCustomTimezone( int chosenTimezone );

    void setUtcEnabled( bool enabled );

    void setSystemTimeEnabled( bool enabled );

    void setLastSessionTimeEnabled( bool enabled );

 private:
    class Private;
    Private *d;
};

}

#endif
