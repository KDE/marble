//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "signals.h"
#include "MonavPlugin.h"
#include "MonavRunner.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include <QtCore/QProcess>
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtNetwork/QLocalSocket>
#include <unistd.h>



namespace Marble
{

class MonavPluginPrivate
{
public:
    QDir m_mapDir;

    bool m_ownsServer;

    MonavPluginPrivate();

    bool startDaemon();

    void stopDaemon();

    bool isDaemonRunning() const;
};

MonavPluginPrivate::MonavPluginPrivate() : m_ownsServer( false )
{
    // nothing to do
}

bool MonavPluginPrivate::isDaemonRunning() const
{
    QLocalSocket socket;
    socket.connectToServer( "MoNavD" );
    return socket.waitForConnected();
}

bool MonavPluginPrivate::startDaemon()
{
    if ( !isDaemonRunning() ) {
        QProcess process;
        if ( process.startDetached( "MoNavD" ) ) {
            m_ownsServer = true;

            // Give MoNavD up to one second to set up its server
            // Without that, the first route request would fail
            for ( int i=0; i<10; ++i )
            {
                if ( isDaemonRunning() ) {
                    break;
                }
                usleep(100 * 1000);
            }

            return true;
        }

        return false;
    }

    return true;
}

void MonavPluginPrivate::stopDaemon()
{
    if ( m_ownsServer ) {
        m_ownsServer = false;
        QProcess process;
        process.startDetached( "MoNavD", QStringList() << "-t" );
    }
}

MonavPlugin::MonavPlugin( QObject *parent ) : RunnerPlugin( parent ), d( new MonavPluginPrivate )
{
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( true );
    setName( tr( "Monav" ) );
    setNameId( "monav" );
    setDescription( tr( "Retrieves routes from monav" ) );
    setGuiString( tr( "Monav Routing" ) );

    // Check installation
    d->m_mapDir = QDir( MarbleDirs::localPath() + "/maps/earth/monav/" );
    bool haveMap = QFileInfo( d->m_mapDir, "plugins.ini" ).exists();
    setCapabilities( haveMap ? Routing : None );
}

MonavPlugin::~MonavPlugin()
{
    delete d;
}

MarbleAbstractRunner* MonavPlugin::newRunner() const
{
    if ( !d->startDaemon() ) {
        mDebug() << "Failed to connect to MoNavD daemon";
    }

    return new MonavRunner;
}

}

Q_EXPORT_PLUGIN2( MonavPlugin, Marble::MonavPlugin )

#include "MonavPlugin.moc"
