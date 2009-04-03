//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "QNamDownloadJob.h"

#include <QtCore/QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

#include "StoragePolicy.h"

namespace Marble
{

QNamDownloadJob::QNamDownloadJob( const QUrl & sourceUrl,
                                  const QString & destFileName,
                                  const QString & id,
                                  QNetworkAccessManager * const networkAccessManager )
    : HttpJob( sourceUrl, destFileName, id ),
      m_networkAccessManager( networkAccessManager ),
      m_networkReply( 0 )
{
}

void QNamDownloadJob::execute()
{
    QNetworkRequest request( sourceUrl() );
    m_networkReply = m_networkAccessManager->get( request );

    connect( m_networkReply, SIGNAL( downloadProgress( qint64, qint64 )),
             SLOT( downloadProgress( qint64, qint64 )));
    connect( m_networkReply, SIGNAL( error( QNetworkReply::NetworkError )),
             SLOT( error( QNetworkReply::NetworkError )));
    connect( m_networkReply, SIGNAL( finished()),
             SLOT( finished()));
}

void QNamDownloadJob::downloadProgress( qint64 bytesReceived, qint64 bytesTotal )
{
//     qDebug() << "downloadProgress" << destinationFileName()
//              << bytesReceived << '/' << bytesTotal;
}

void QNamDownloadJob::error( QNetworkReply::NetworkError code )
{
    qDebug() << "error" << destinationFileName() << code;
}

void QNamDownloadJob::finished()
{
    QNetworkReply::NetworkError const error = m_networkReply->error();
//     qDebug() << "finished" << destinationFileName()
//              << "error" << error;

    switch ( error ) {
    case QNetworkReply::NoError: {
        // check if we are redirected
        const QVariant redirectionAttribute =
            m_networkReply->attribute( QNetworkRequest::RedirectionTargetAttribute );
        if ( !redirectionAttribute.isNull() ) {
            emit redirected( this, redirectionAttribute.toUrl() );
        }
        else {
            // no redirection occured
            const QByteArray data = m_networkReply->readAll();
            if ( storagePolicy() ) {
                if ( storagePolicy()->updateFile( destinationFileName(), data ) ) {
                    emit statusMessage( tr( "Download finished." ));
                    emit jobDone( this, 0 );
                }
                else {
                    emit statusMessage( tr( "Download failed: %1." )
                                        .arg( storagePolicy()->lastErrorMessage() ) );
                    emit jobDone( this, 1 );
                }
            }
            else {
                emit statusMessage( tr( "Download finished, but not possible to save." ) );
                emit jobDone( this, 1 );
            }
        }
    }
        break;

    default:
        emit statusMessage( tr( "Download failed." ) );
        emit jobDone( this, 1 );
    }

    m_networkReply->disconnect( this );
    delete m_networkReply;
    m_networkReply = 0;
}

}

#include "QNamDownloadJob.moc"
