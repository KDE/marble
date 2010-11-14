//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008,2009 Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2008      Pino Toscano <pino@kde.org>
//

#include "HttpJob.h"
#include "TinyWebBrowser.h"

#include <QtCore/QDebug>

using namespace Marble;

class Marble::HttpJobPrivate
{
 public:
    HttpJobPrivate( const QUrl & sourceUrl, const QString & destFileName, const QString &id );

    QUrl           m_sourceUrl;
    QString        m_destinationFileName;
    QString        m_initiatorId;
    int            m_trialsLeft;
    DownloadUsage  m_downloadUsage;
    QString m_pluginId;
};

HttpJobPrivate::HttpJobPrivate( const QUrl & sourceUrl, const QString & destFileName,
                                const QString &id )
    : m_sourceUrl( sourceUrl ),
      m_destinationFileName( destFileName ),
      m_initiatorId( id ),
      m_trialsLeft( 3 ),
      m_downloadUsage( DownloadBrowse ),
      // FIXME: remove initialization depending on if empty pluginId
      // results in valid user agent string
      m_pluginId( "unknown" )
{
}


HttpJob::HttpJob( const QUrl & sourceUrl, const QString & destFileName, const QString &id )
    : d( new HttpJobPrivate( sourceUrl, destFileName, id ))
{
}

HttpJob::~HttpJob()
{
    delete d;
}

QUrl HttpJob::sourceUrl() const
{
    return d->m_sourceUrl;
}

void HttpJob::setSourceUrl( const QUrl &url )
{
    d->m_sourceUrl = url;
}

QString HttpJob::initiatorId() const
{
    return d->m_initiatorId;
}

void HttpJob::setInitiatorId( const QString &id )
{
    d->m_initiatorId = id;
}

QString HttpJob::destinationFileName() const
{
    return d->m_destinationFileName;
}

void HttpJob::setDestinationFileName( const QString &fileName )
{
    d->m_destinationFileName = fileName;
}

bool HttpJob::tryAgain()
{
    if( d->m_trialsLeft > 0 ) {
	d->m_trialsLeft--;
	return true;
    }
    else {
	return false;
    }
}

DownloadUsage HttpJob::downloadUsage() const
{
    return d->m_downloadUsage;
}

void HttpJob::setDownloadUsage( const DownloadUsage usage )
{
    d->m_downloadUsage = usage;
}

void HttpJob::setUserAgentPluginId( const QString & pluginId ) const
{
    d->m_pluginId = pluginId;
}

QByteArray HttpJob::userAgent() const
{
    switch ( d->m_downloadUsage ) {
    case DownloadBrowse:
        return TinyWebBrowser::userAgent("Browser", d->m_pluginId);
        break;
    case DownloadBulk:
        return TinyWebBrowser::userAgent("BulkDownloader", d->m_pluginId);
        break;
    default:
        qCritical() << "Unknown download usage value:" << d->m_downloadUsage;
        return TinyWebBrowser::userAgent("unknown", d->m_pluginId);
    }
}

#include "HttpJob.moc"
