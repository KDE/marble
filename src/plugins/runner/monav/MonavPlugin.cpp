//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MonavPlugin.h"
#include "MonavRunner.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include <QtCore/QProcess>
#include <QtCore/QDir>
#include <QtCore/QMutexLocker>
#include <QtCore/QTimer>

namespace Marble
{

class MonavPluginPrivate
{
public:
    QProcess* m_monavProcess;

    QDir m_mapDir;

    QMutex m_processMutex;

    QTimer m_shutdownTimer;

    MonavPluginPrivate();
};

MonavPluginPrivate::MonavPluginPrivate() : m_monavProcess( 0 )
{
    m_shutdownTimer.setInterval( 30 * 1000 );
    m_shutdownTimer.setSingleShot( true );
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
    bool haveMap = QFileInfo( d->m_mapDir, "Contraction Hierarchies" ).exists();
    setCapabilities( haveMap ? Routing : None );

    connect( &d->m_shutdownTimer, SIGNAL( timeout() ), this, SLOT( killProcess( ) ) );
}

MonavPlugin::~MonavPlugin()
{
    killProcess();
    delete d;
}

MarbleAbstractRunner* MonavPlugin::newRunner() const
{
    QMutexLocker locker( &d->m_processMutex );
    if ( !d->m_monavProcess ) {
        d->m_monavProcess = new QProcess;
        d->m_monavProcess->start( "monav", QStringList() << "--pipe" << d->m_mapDir.absolutePath() );

        if ( !d->m_monavProcess->waitForStarted( 5000 ) )
        {
            mDebug() << "Couldn't start monav from the current PATH. Install it to retrieve routing results from monav.";
        }
    }

    d->m_shutdownTimer.start();
    return new MonavRunner( d->m_monavProcess );
}

void MonavPlugin::killProcess()
{
    QMutexLocker locker( &d->m_processMutex );

    if ( d->m_monavProcess ) {
        QProcess* dying = d->m_monavProcess;
        d->m_monavProcess = 0;
        dying->closeWriteChannel();
        if ( !dying->waitForFinished( 1000 ) ) {
            dying->terminate();
            if ( !dying->waitForFinished( 1000 ) ) {
                mDebug() << "monav process did not terminate properly.";
            }
        }
    }
}

}

Q_EXPORT_PLUGIN2( MonavPlugin, Marble::MonavPlugin )

#include "MonavPlugin.moc"
