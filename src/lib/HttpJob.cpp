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

HttpJob::HttpJob( const QUrl & sourceUrl, const QString & destFileName, const QString &id )
        : m_sourceUrl ( sourceUrl ),
        m_destinationFileName ( destFileName ),
        m_originalDestinationFileName ( destFileName ),
        m_initiatorId ( id ),
        m_status ( NoStatus ),
        m_priority ( NoPriority ),
        m_storagePolicy( 0 )
{
}

HttpJob::~HttpJob()
{
}

void HttpJob::prepareExecution()
{
}

void HttpJob::setStoragePolicy( StoragePolicy *policy )
{
    m_storagePolicy = policy;
}

StoragePolicy *HttpJob::storagePolicy() const
{
    return m_storagePolicy;
}

#include "HttpJob.moc"
