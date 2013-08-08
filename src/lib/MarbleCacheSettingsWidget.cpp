//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn  <rahn@kde.org>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "MarbleCacheSettingsWidget.h"
#include "ui_MarbleCacheSettingsWidget.h"

#include <QPushButton>

namespace Marble {

class MarbleCacheSettingsWidget::Private : public Ui::MarbleCacheSettingsWidget
{
};

MarbleCacheSettingsWidget::MarbleCacheSettingsWidget( QWidget *parent ) :
    QWidget( parent ),
    d( new Private )
{
    d->setupUi( this );

    connect( d->button_clearVolatileCache, SIGNAL(clicked()), SIGNAL(clearVolatileCache()) );
    connect( d->button_clearPersistentCache, SIGNAL(clicked()), SIGNAL(clearPersistentCache()) );
}

MarbleCacheSettingsWidget::~MarbleCacheSettingsWidget()
{
    delete d;
}

int MarbleCacheSettingsWidget::volatileTileCacheLimit() const
{
    return d->kcfg_volatileTileCacheLimit->value();
}

int MarbleCacheSettingsWidget::persistentTileCacheLimit() const
{
    return d->kcfg_persistentTileCacheLimit->value();
}

QString MarbleCacheSettingsWidget::proxyUrl() const
{
    return d->kcfg_proxyUrl->text();
}

quint16 MarbleCacheSettingsWidget::proxyPort() const
{
    return d->kcfg_proxyPort->value();
}

QString MarbleCacheSettingsWidget::proxyUser() const
{
    return d->kcfg_proxyUser->text();
}

QString MarbleCacheSettingsWidget::proxyPassword() const
{
    return d->kcfg_proxyPass->text();
}

ProxyType MarbleCacheSettingsWidget::proxyType() const
{
    return static_cast<ProxyType>( d->kcfg_proxyType->currentIndex() );
}

bool MarbleCacheSettingsWidget::isProxyAuthenticationEnabled() const
{
    return d->kcfg_proxyAuth->isChecked();
}

void MarbleCacheSettingsWidget::setVolatileTileCacheLimit( int volatileTileCacheLimit )
{
    d->kcfg_volatileTileCacheLimit->setValue( volatileTileCacheLimit );
}

void MarbleCacheSettingsWidget::setPersistentTileCacheLimit( int persistentTileCacheLimit )
{
    d->kcfg_persistentTileCacheLimit->setValue( persistentTileCacheLimit );
}

void MarbleCacheSettingsWidget::setProxyUrl( const QString &proxyUrl )
{
    d->kcfg_proxyUrl->setText( proxyUrl );
}

void MarbleCacheSettingsWidget::setProxyPort( quint16 proxyPort )
{
    d->kcfg_proxyPort->setValue( proxyPort );
}

void MarbleCacheSettingsWidget::setProxyUser( const QString &proxyUser )
{
    d->kcfg_proxyUser->setText( proxyUser );
}

void MarbleCacheSettingsWidget::setProxyPassword( const QString &proxyPassword )
{
    d->kcfg_proxyPass->setText( proxyPassword );
}

void MarbleCacheSettingsWidget::setProxyType( ProxyType proxyType )
{
    d->kcfg_proxyType->setCurrentIndex( proxyType );
}

void MarbleCacheSettingsWidget::setProxyAuthenticationEnabled( bool enabled )
{
    d->kcfg_proxyAuth->setChecked( enabled );
}

} // namespace Marble

#include "MarbleCacheSettingsWidget.moc"



