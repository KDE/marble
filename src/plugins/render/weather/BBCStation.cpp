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
#include "BBCStation.h"

// Marble
#include "GeoDataCoordinates.h"

// Qt
#include <QAtomicInt>
#include <QString>

namespace Marble
{

class BBCStationPrivate
{
 public:
    BBCStationPrivate()
            : m_bbcId( 0 ),
              m_priority( 0 ),
              ref( 1 )
    {
    }

    BBCStationPrivate( const BBCStationPrivate &other )
            : m_name( other.m_name ),
              m_coordinate( other.m_coordinate ),
              m_bbcId( other.m_bbcId ),
              m_priority( other.m_priority ),
              ref( other.ref )
    {
    }

    BBCStationPrivate& operator=( const BBCStationPrivate &other )
    {
        m_name = other.m_name;
        m_coordinate = other.m_coordinate;
        m_bbcId = other.m_bbcId;
        m_priority = other.m_priority;
        ref = other.ref;
        return *this;
    }

    QString            m_name;
    GeoDataCoordinates m_coordinate;
    quint32            m_bbcId;
    quint8             m_priority;

    QAtomicInt ref;
};

BBCStation::BBCStation()
        : d ( new BBCStationPrivate() )
{
}

BBCStation::BBCStation( const BBCStation& other )
        : d( other.d )
{
    d->ref.ref();
}

BBCStation::~BBCStation()
{
    if ( !d->ref.deref() )
        delete d;
}

void BBCStation::detach()
{
    qAtomicDetach( d );
}

BBCStation& BBCStation::operator=( const BBCStation &other )
{
    qAtomicAssign( d, other.d );
    return *this;
}

bool BBCStation::operator<( const BBCStation& other ) const
{
    return priority() > other.priority();
}

QString BBCStation::name() const
{
    return d->m_name;
}

void BBCStation::setName( const QString& name )
{
    detach();
    d->m_name = name;
}

GeoDataCoordinates BBCStation::coordinate() const
{
    return d->m_coordinate;
}

void BBCStation::setCoordinate( const GeoDataCoordinates& coordinate )
{
    detach();
    d->m_coordinate = coordinate;
}

quint32 BBCStation::bbcId() const
{
    return d->m_bbcId;
}

void BBCStation::setBbcId( quint32 id )
{
    detach();
    d->m_bbcId = id;
}

quint8 BBCStation::priority() const
{
    return d->m_priority;
}

void BBCStation::setPriority( quint8 priority )
{
    detach();
    d->m_priority = priority;
}

} // namespace Marble
