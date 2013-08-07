//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "MarbleTimeSettingsWidget.h"
#include "ui_MarbleTimeSettingsWidget.h"

namespace Marble {

class MarbleTimeSettingsWidget::Private : public Ui::MarbleTimeSettingsWidget
{
};

MarbleTimeSettingsWidget::MarbleTimeSettingsWidget( QWidget *parent ) :
    QWidget( parent ),
    d( new Private )
{
    d->setupUi( this );
}

MarbleTimeSettingsWidget::~MarbleTimeSettingsWidget()
{
    delete d;
}

bool MarbleTimeSettingsWidget::isSystemTimezoneEnabled() const
{
    return d->kcfg_systemTimezone->isChecked();
}

bool MarbleTimeSettingsWidget::isCustomTimezoneEnabled() const
{
    return d->kcfg_customTimezone->isChecked();
}

int MarbleTimeSettingsWidget::customTimezone() const
{
    return d->kcfg_chosenTimezone->currentIndex();
}

bool MarbleTimeSettingsWidget::isUtcEnabled() const
{
    return d->kcfg_utc->isChecked();
}

bool MarbleTimeSettingsWidget::isSystemTimeEnabled() const
{
    return d->kcfg_systemTime->isChecked();
}

bool MarbleTimeSettingsWidget::isLastSessionTimeEnabled() const
{
    return d->kcfg_lastSessionTime->isChecked();
}

void MarbleTimeSettingsWidget::setSystemTimezoneEnabled( bool enabled )
{
    d->kcfg_systemTimezone->setChecked( enabled );
}

void MarbleTimeSettingsWidget::setCustomTimezoneEnabled( bool enabled )
{
    d->kcfg_customTimezone->setChecked( enabled );
}

void MarbleTimeSettingsWidget::setCustomTimezone( int chosenTimezone )
{
    d->kcfg_chosenTimezone->setCurrentIndex( chosenTimezone );
}

void MarbleTimeSettingsWidget::setUtcEnabled( bool enabled )
{
    d->kcfg_utc->setChecked( enabled );
}

void MarbleTimeSettingsWidget::setSystemTimeEnabled( bool enabled )
{
    d->kcfg_systemTime->setChecked( enabled );
}

void MarbleTimeSettingsWidget::setLastSessionTimeEnabled( bool enabled )
{
    d->kcfg_lastSessionTime->setChecked( enabled );
}

} // namespace Marble

#include "MarbleTimeSettingsWidget.moc"



