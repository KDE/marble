//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "EclipsesItem.h"

#include <QDebug>

namespace Marble
{

EclipsesItem::EclipsesItem( int index, QObject *parent )
    : QObject( parent ),
      m_index( index ),
      m_phase( TotalSun )
{
}

EclipsesItem::~EclipsesItem()
{
}

int EclipsesItem::index() const
{
    return m_index;
}

void EclipsesItem::setDateTime( const QDateTime dateTime )
{
    m_dateTime = dateTime;
}

QDateTime EclipsesItem::dateTime() const
{
    return m_dateTime;
}

void EclipsesItem::setPhase( EclipsesItem::EclipsePhase phase )
{
    m_phase = phase;
}

EclipsesItem::EclipsePhase EclipsesItem::phase() const
{
    return m_phase;
}

QString EclipsesItem::phaseText() const
{
    switch( m_phase ) {
        case PartialSun:            return tr( "Partial Sun" );
        case NonCentralAnnularSun:  return tr( "non-central Annular Sun" );
        case NonCentralTotalSun:    return tr( "non-central Total Sun" );
        case AnnularSun:            return tr( "Annular Sun" );
        case TotalSun:              return tr( "Total Sun" );
        case AnnularTotalSun:       return tr( "Annular / Total Sun" );
    }

    return QString();
}

bool EclipsesItem::takesPlaceAt( const QDateTime &dateTime ) const
{
    return true;
}

}

#include "EclipsesItem.moc"

