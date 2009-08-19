//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "BBCItemGetter.h"
#include "BBCWeatherItem.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QMutexLocker>
#include <QtCore/QUrl>

using namespace Marble;

BBCItemGetter::BBCItemGetter( QObject *parent )
        : AbstractWorkerThread( parent ),
          m_scheduleMutex(),
          m_scheduledNumber( 0 )
{
}

BBCItemGetter::~BBCItemGetter()
{
}

void BBCItemGetter::setSchedule( const GeoDataLatLonAltBox& box,
                                 MarbleDataFacade *facade,
                                 qint32 number )
{
    Q_UNUSED( facade )
    m_scheduleMutex.lock();
    m_scheduledBox = box;
    m_scheduledNumber = number;
    m_scheduleMutex.unlock();
    ensureRunning();
}

void BBCItemGetter::setStationList( const QList<BBCWeatherItem*>& items )
{
    m_items = items;
    ensureRunning();
}

bool BBCItemGetter::workAvailable()
{
    return !m_scheduledBox.isNull()
           && m_scheduledNumber;

}

void BBCItemGetter::work()
{
    if ( m_items.isEmpty() ) {
        sleep( 1 );
        return;
    }

    m_scheduleMutex.lock();
    GeoDataLatLonAltBox box = m_scheduledBox;
    qint32 number = m_scheduledNumber;
    m_scheduledBox = GeoDataLatLonAltBox();
    m_scheduledNumber = 0;
    m_scheduleMutex.unlock();

    qint32 fetched = 0;
    QList<BBCWeatherItem *>::ConstIterator it = m_items.constBegin();
    QList<BBCWeatherItem *>::ConstIterator end = m_items.constEnd();

    while ( fetched < number && it != end ) {
        if ( (*it) && box.contains( (*it)->coordinate() ) ) {
            (*it)->setTarget( "earth" );
            emit requestedDownload( (*it)->observationUrl(), "bbcobservation", (*it) );
            emit requestedDownload( (*it)->forecastUrl(),    "bbcforecast",    (*it) );
            fetched++;
        }
        ++it;
    }
}

#include "BBCItemGetter.moc"
