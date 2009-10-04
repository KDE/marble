//
// This file is part of the Marble Desktop Globe.
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
};

HttpJobPrivate::HttpJobPrivate( const QUrl & sourceUrl, const QString & destFileName,
                                const QString &id )
    : m_sourceUrl( sourceUrl ),
      m_destinationFileName( destFileName ),
      m_initiatorId( id ),
      m_trialsLeft( 3 ),
      m_downloadUsage( DownloadBrowse )
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

#include "HttpJob.moc"
