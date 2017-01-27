//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "BBCItemGetter.h"
#include "BBCStation.h"
#include "BBCWeatherItem.h"
#include "MarbleDebug.h"

// Qt
#include <QMutexLocker>

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

void BBCItemGetter::setSchedule( const GeoDataLatLonBox& box,
                                 qint32 number )
{
    m_scheduleMutex.lock();
    m_scheduledBox = box;
    m_scheduledNumber = number;
    m_scheduleMutex.unlock();
    ensureRunning();
}

void BBCItemGetter::setStationList( const QList<BBCStation>& items )
{
    m_items = items;
    ensureRunning();
}

BBCStation BBCItemGetter::station( const QString &id )
{
    QString const bbcIdTemplate = QString( "bbc%1" );
    for( const BBCStation &station: m_items ) {
        if ( bbcIdTemplate.arg( station.bbcId() ) == id ) {
            return station;
        }
    }

    return BBCStation();
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
    GeoDataLatLonBox box = m_scheduledBox;
    qint32 number = m_scheduledNumber;
    m_scheduledBox = GeoDataLatLonBox();
    m_scheduledNumber = 0;
    m_scheduleMutex.unlock();

    qint32 fetched = 0;
    QList<BBCStation>::ConstIterator it = m_items.constBegin();
    QList<BBCStation>::ConstIterator end = m_items.constEnd();

    while ( fetched < number && it != end ) {
        if ( box.contains( it->coordinate() ) ) {
            emit foundStation( (*it) );
            fetched++;
        }
        ++it;
    }
}

#include "moc_BBCItemGetter.cpp"
