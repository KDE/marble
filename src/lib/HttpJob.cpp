//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2008      Pino Toscano <pino@kde.org>
//

#include "HttpJob.h"

using namespace Marble;

class Marble::HttpJobPrivate
{
 public:
    HttpJobPrivate();

    QUrl           m_sourceUrl;
    QString        m_destinationFileName;
    // if there is a redirection, we have to know the original file name
    // for proper blacklisting etc.
    QString        m_originalDestinationFileName;
    QString        m_initiatorId;
    Status         m_status;
    Priority       m_priority;
    StoragePolicy *m_storagePolicy;
    int            m_trialsLeft;
};

HttpJobPrivate::HttpJobPrivate()
    : m_status( NoStatus ),
    m_priority( NoPriority ),
    m_storagePolicy( 0 )
{
}


HttpJob::HttpJob( const QUrl & sourceUrl, const QString & destFileName, const QString &id )
    : d( new HttpJobPrivate )
{
    d->m_sourceUrl = sourceUrl;
    d->m_destinationFileName = destFileName;
    d->m_originalDestinationFileName = destFileName;
    d->m_initiatorId = id;
    d->m_trialsLeft = 3;
}

HttpJob::~HttpJob()
{
    delete d;
}

void HttpJob::prepareExecution()
{
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

QString HttpJob::originalDestinationFileName() const
{
    return d->m_originalDestinationFileName;
}

void HttpJob::setStatus( const Status status )
{
    d->m_status = status;
}

void HttpJob::setStoragePolicy( StoragePolicy *policy )
{
    d->m_storagePolicy = policy;
}

StoragePolicy *HttpJob::storagePolicy() const
{
    return d->m_storagePolicy;
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

#include "HttpJob.moc"
