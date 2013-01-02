//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "InhibitScreensaverPlugin.h"

#include "PositionTracking.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"

#include <QtCore/QTimer>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>

namespace Marble {

class InhibitScreensaverPluginPrivate
{
public:
    QDBusInterface *m_interface;

    QTimer m_timer;

    InhibitScreensaverPluginPrivate();

    ~InhibitScreensaverPluginPrivate();
};

InhibitScreensaverPluginPrivate::InhibitScreensaverPluginPrivate() :
        m_interface( 0 )
{
    m_timer.setInterval( 10 * 1000 ); // timeout of 10 seconds
}

InhibitScreensaverPluginPrivate::~InhibitScreensaverPluginPrivate()
{
    delete m_interface;
}

InhibitScreensaverPlugin::InhibitScreensaverPlugin() :
    RenderPlugin( 0 ),
    d( 0 )
{
}

InhibitScreensaverPlugin::InhibitScreensaverPlugin( const MarbleModel *marbleModel ) :
    RenderPlugin( marbleModel ),
    d ( new InhibitScreensaverPluginPrivate() )
{
    connect( &d->m_timer, SIGNAL(timeout()), this, SLOT(inhibitScreenSaver()) );

#ifdef Q_WS_MAEMO_5
    setEnabled( true );
    setVisible( true );
#else
    qDebug() << "The inhibit screensaver plugin is only useful on Maemo.";
    setEnabled( false );
    setVisible( false );
#endif
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

QString InhibitScreensaverPlugin::version() const
{
    return "1.0";
}

QString InhibitScreensaverPlugin::description() const
{
    return tr( "Inhibits the screensaver during turn-by-turn navigation" );
}

QString InhibitScreensaverPlugin::copyrightYears() const
{
    return "2010";
}

QList<PluginAuthor> InhibitScreensaverPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
}

QIcon InhibitScreensaverPlugin::icon() const
{
    return QIcon();
}

void InhibitScreensaverPlugin::initialize()
{
    Q_ASSERT( marbleModel() && marbleModel()->positionTracking() );

    d->m_interface= new QDBusInterface( "com.nokia.mce", "/com/nokia/mce/request",
       "com.nokia.mce.request", QDBusConnection::systemBus() );

    PositionTracking *tracking = marbleModel()->positionTracking();
    connect( tracking, SIGNAL(positionProviderPluginChanged(PositionProviderPlugin*)),
             this, SLOT(updateScreenSaverState(PositionProviderPlugin*)) );
    updateScreenSaverState( tracking->positionProviderPlugin() );
}

bool InhibitScreensaverPlugin::isInitialized() const
{
    return d->m_interface;
}

void InhibitScreensaverPlugin::updateScreenSaverState( PositionProviderPlugin *activePlugin )
{
    if ( !enabled() ) {
        return;
    }

    if ( activePlugin ) {
        d->m_timer.start(); // Inhibit screensaver
    } else {
      d->m_timer.stop();
    }
}

void InhibitScreensaverPlugin::inhibitScreenSaver()
{
    if ( d->m_interface && d->m_interface->isValid() ) {
        d->m_interface->call( "req_display_blanking_pause" );
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
