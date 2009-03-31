//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Pino Toscano <pino@kde.org>
//

#include "KIONetworkPlugin.h"

#include "StoragePolicy.h"

#include <QtCore/QDebug>

#include <klocale.h>
#include <kio/job.h>

KIOHttpJob::KIOHttpJob( const QUrl &source, const QString &destination, const QString &id )
    : Marble::HttpJob( source, destination, id ), m_job( 0 )
{
}

KIOHttpJob::~KIOHttpJob()
{
}

void KIOHttpJob::execute()
{
    m_job = KIO::storedGet( KUrl( sourceUrl() ), KIO::Reload,
                            KIO::HideProgressInfo | KIO::Overwrite );

    connect( m_job, SIGNAL( canceled( KJob * ) ),
             this, SLOT( slotCanceled( KJob * ) ) );
    connect( m_job, SIGNAL( finished( KJob * ) ),
             this, SLOT( slotFinished( KJob * ) ) );
    connect( m_job, SIGNAL( infoMessage( KJob *, QString, QString ) ),
             this, SLOT( slotInfoMessage( KJob *, QString, QString ) ) );
    connect( m_job, SIGNAL( warning( KJob *, QString, QString ) ),
             this, SLOT( slotWarning( KJob *, QString, QString ) ) );
}

void KIOHttpJob::slotCanceled( KJob *job )
{
    if ( job != m_job )
        return;

    emit jobDone( this, job->error() );
}

void KIOHttpJob::slotFinished( KJob *job )
{
    if ( job != m_job )
        return;

    if ( job->error() != 0 )
    {
        emit statusMessage( i18n( "Download failed: %1.", job->errorString() ) );
        emit jobDone( this, job->error() );

        return;
    }

    if ( storagePolicy() &&
         !storagePolicy()->updateFile( originalDestinationFileName(),
                                       qobject_cast< KIO::StoredTransferJob * >( job )->data() ) )
    {
        emit statusMessage( i18n( "Download failed: %1.", storagePolicy()->lastErrorMessage() ) );
        emit jobDone( this, 1 );

        return;
    }

    emit statusMessage( i18n( "Download finished." ) );
    emit jobDone( this, 0 );
}

void KIOHttpJob::slotInfoMessage( KJob *job, const QString &plainText, const QString &richText )
{
    if ( job != m_job )
        return;

    Q_UNUSED( richText );
    emit statusMessage( plainText );
}

void KIOHttpJob::slotWarning( KJob *job, const QString &plainText, const QString &richText )
{
    if ( job != m_job )
        return;

    Q_UNUSED( richText );
    emit statusMessage( plainText );
}


KIONetworkPlugin::KIONetworkPlugin()
    : Marble::NetworkPlugin()
{
}

KIONetworkPlugin::~KIONetworkPlugin()
{
}

QString KIONetworkPlugin::name() const
{
    return tr( "KIO Network Plugin" );
}

QString KIONetworkPlugin::nameId() const
{
    return QString::fromLatin1( "KIO" );
}

QString KIONetworkPlugin::guiString() const
{
    return tr( "KIO based Network Plugin" );
}

QString KIONetworkPlugin::description() const
{
    return tr( "A network plugin which supports HTTP pipelining to deliver a "
               "better user experience." );
}

QIcon KIONetworkPlugin::icon() const
{
    return QIcon();
}

void KIONetworkPlugin::initialize()
{
}

bool KIONetworkPlugin::isInitialized() const
{
    return true;
}

Marble::NetworkPlugin * KIONetworkPlugin::newInstance() const
{
    return new KIONetworkPlugin;
}

Marble::HttpJob *KIONetworkPlugin::createJob( const QUrl &source, const QString &destination,
                                      const QString &id )
{
    return new KIOHttpJob( source, destination, id );
}


Q_EXPORT_PLUGIN2( KIONetworkPlugin, KIONetworkPlugin )

#include "KIONetworkPlugin.moc"
