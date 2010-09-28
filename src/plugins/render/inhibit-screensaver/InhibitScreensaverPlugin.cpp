//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "InhibitScreensaverPlugin.h"

#include "MarbleDataFacade.h"
#include "PositionTracking.h"

#include <QtSystemInfo/QSystemScreenSaver>

namespace Marble {

class InhibitScreensaverPluginPrivate
{
public:
    bool m_initialized;

    QtMobility::QSystemScreenSaver* m_screensaver;

    InhibitScreensaverPluginPrivate();
};

InhibitScreensaverPluginPrivate::InhibitScreensaverPluginPrivate() :
        m_initialized( false ),
        m_screensaver( 0 )
{
    // nothing to do
}


InhibitScreensaverPlugin::InhibitScreensaverPlugin() :
        d ( new InhibitScreensaverPluginPrivate() )
{
    setEnabled( true );
    setVisible( true );
}

InhibitScreensaverPlugin::~InhibitScreensaverPlugin()
{
    delete d;
}

QStringList InhibitScreensaverPlugin::backendTypes() const
{
    return QStringList( nameId() );
}

QStringList InhibitScreensaverPlugin::renderPosition() const
{
    // We're invisible, but need to be initialized
    return QStringList( "FLOAT_ITEM" );
}

QString InhibitScreensaverPlugin::name() const
{
    return tr( "Inhibit Screensaver" );
}

QString InhibitScreensaverPlugin::guiString() const
{
    return tr( "&Inhibit Screensaver" );
}

QString InhibitScreensaverPlugin::nameId() const
{
    return QString("inhibit-screensaver");
}

QString InhibitScreensaverPlugin::description() const
{
    return tr( "Inhibits the screensaver during turn-by-turn navigation" );
}

QIcon InhibitScreensaverPlugin::icon() const
{
    return QIcon();
}

void InhibitScreensaverPlugin::initialize()
{
    Q_ASSERT( dataFacade() && dataFacade()->positionTracking() );
    PositionTracking *tracking = dataFacade()->positionTracking();
    connect( tracking, SIGNAL( positionProviderPluginChanged( PositionProviderPlugin* ) ),
             this, SLOT( updateScreenSaverState( PositionProviderPlugin* ) ) );
    d->m_initialized = true;
}

bool InhibitScreensaverPlugin::isInitialized() const
{
    return d->m_initialized;
}

void InhibitScreensaverPlugin::updateScreenSaverState( PositionProviderPlugin *activePlugin )
{
    if ( !enabled() ) {
        return;
    }

    if ( activePlugin ) {
        // Inhibit screensaver
        if ( !d->m_screensaver ) {
            d->m_screensaver = new QtMobility::QSystemScreenSaver( this );
            d->m_screensaver->setScreenSaverInhibit();
        }
    } else if ( d->m_screensaver ) {
        // Do not inhibit the screensaver. The API here is cumbersome:
        // You can only revert a previous inhibition by deleting the screensaver object. Wtf?
        delete d->m_screensaver;
        d->m_screensaver = 0;
    }
}

bool InhibitScreensaverPlugin::render( GeoPainter *, ViewportParams *, const QString&, GeoSceneLayer *)
{
    // invisible
    return true;
}

QString InhibitScreensaverPlugin::renderPolicy() const
{
    return "NEVER";
}

}

Q_EXPORT_PLUGIN2( InhibitScreensaverPlugin, Marble::InhibitScreensaverPlugin )

#include "InhibitScreensaverPlugin.moc"
