//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "QNamDownloadJob.h"

#include "MarbleDebug.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

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
#if QT_VERSION >= 0x40600
    request.setAttribute( QNetworkRequest::HttpPipeliningAllowedAttribute, true );
#endif
    request.setRawHeader( "User-Agent", userAgent() );
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
    Q_UNUSED(bytesReceived);
    Q_UNUSED(bytesTotal);
//     mDebug() << "downloadProgress" << destinationFileName()
//              << bytesReceived << '/' << bytesTotal;
}

void QNamDownloadJob::error( QNetworkReply::NetworkError code )
{
    mDebug() << "error" << destinationFileName() << code;
}

void QNamDownloadJob::finished()
{
    QNetworkReply::NetworkError const error = m_networkReply->error();
//     mDebug() << "finished" << destinationFileName()
//              << "error" << error;

#if QT_VERSION >= 0x40600
    const QVariant httpPipeliningWasUsed =
        m_networkReply->attribute( QNetworkRequest::HttpPipeliningWasUsedAttribute );
    if ( !httpPipeliningWasUsed.isNull() )
        mDebug() << "http pipelining used:" << httpPipeliningWasUsed.toBool();
#endif

    switch ( error ) {
    case QNetworkReply::NoError: {
        // check if we are redirected
        const QVariant redirectionAttribute =
            m_networkReply->attribute( QNetworkRequest::RedirectionTargetAttribute );
        if ( !redirectionAttribute.isNull() ) {
            emit redirected( this, redirectionAttribute.toUrl() );
        }
        else {
            // no redirection occurred
            const QByteArray data = m_networkReply->readAll();
            emit dataReceived( this, data );
        }
    }
        break;

    default:
        emit jobDone( this, 1 );
    }

    m_networkReply->disconnect( this );
    // No delete. This method is called by a signal QNetworkReply::finished.
    m_networkReply->deleteLater();
    m_networkReply = 0;
}

}

#include "QNamDownloadJob.moc"
