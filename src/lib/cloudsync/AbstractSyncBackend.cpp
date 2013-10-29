//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#include "AbstractSyncBackend.h"

#include "MarbleDebug.h"

#include <QFile>

namespace Marble
{
class AbstractSyncBackend::Private {

public:
    QUrl m_apiUrl;
};

AbstractSyncBackend::AbstractSyncBackend( QObject *parent ) :
    QObject( parent ),
    d( new Private )
{
}

AbstractSyncBackend::~AbstractSyncBackend()
{
    delete d;
}

void AbstractSyncBackend::setApiUrl( const QUrl &apiUrl )
{
    d->m_apiUrl = apiUrl;
}

QUrl AbstractSyncBackend::endpointUrl( const QString &endpoint )
{
    QString endpointUrl = QString( "%0/%1" ).arg( d->m_apiUrl.toString() ).arg( endpoint );
    return QUrl( endpointUrl );
}

QUrl AbstractSyncBackend::endpointUrl( const QString &endpoint, const QString &parameter )
{
    QString endpointUrl = QString( "%0/%1/%2" ).arg( d->m_apiUrl.toString() ).arg( endpoint ).arg( parameter );
    return QUrl( endpointUrl );
}

void AbstractSyncBackend::removeFromCache( const QDir &cacheDir, const QString &timestamp )
{
    bool fileRemoved = QFile( QString( "%0/%1.kml" ).arg( cacheDir.absolutePath(), timestamp ) ).remove();
    bool previewRemoved = QFile( QString( "%0/preview/%1.jpg" ).arg( cacheDir.absolutePath(), timestamp ) ).remove();
    if ( !fileRemoved || !previewRemoved ) {
        mDebug() << "Failed to remove locally cached route " << timestamp << ". It might "
                    "have been removed already, or its directory is missing / not writable.";
    }

    emit removedFromCache( timestamp );
}

}

#include "AbstractSyncBackend.moc"
