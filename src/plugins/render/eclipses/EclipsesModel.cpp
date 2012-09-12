//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "EclipsesModel.h"

#include "EclipsesItem.h"
#include "MarbleDebug.h"
#include "MarbleClock.h"
#include "GeoPainter.h"

#include "ecl/eclsolar.h"

namespace Marble
{

EclipsesModel::EclipsesModel( const MarbleClock *clock, QObject *parent )
    : QObject( parent ),
      m_clock( clock ),
      m_currentYear( 0 )
{
    m_ecps = new EclSolar();
    m_ecps->setLunarEcl( false );
    m_ecps->setTimezone( clock->timezone() / 3600 );

    setYear( clock->dateTime().date().year() );
}

EclipsesModel::~EclipsesModel()
{
    clear();
    delete m_ecps;
}

void EclipsesModel::setYear( int year )
{
    if( m_currentYear != year ) {

        mDebug() << "Year changed - get eclipses...";
        m_currentYear = year;
        m_ecps->putYear( year );

        updateEclipses();
    }
}

int EclipsesModel::year() const
{
    return m_currentYear;
}

void EclipsesModel::paint( const QDateTime &dateTime, GeoPainter *painter )
{
    foreach( EclipsesItem *item, m_items ) {
        if( item->takesPlaceAt( dateTime ) ) {
            paintItem( item, painter );
            return; // there should be only one eclipse at the same time
        }
    }
}

void EclipsesModel::paintItem( EclipsesItem *item, GeoPainter *painter )
{
    // paint
}

void EclipsesModel::addItem( EclipsesItem *item )
{
    m_items.append( item );
}

void EclipsesModel::clear()
{
    qDeleteAll( m_items );
    m_items.clear();
}

void EclipsesModel::update()
{
    updateEclipses();
}

void EclipsesModel::updateEclipses()
{
    int year, month, day, hour, min, phase;
    double secs, tz, mag;

    clear();

    int num = m_ecps->getNumberEclYear();
    for( int i = 1; i <= num; ++i ) {
        phase = m_ecps->getEclYearInfo( i, year, month, day, hour, min, secs, tz, mag );
        QDateTime dt = QDateTime( QDate( year, month, day ),
                                  QTime( hour, min, secs ),
                                  Qt::UTC ).addSecs( - ( tz * 3600 ) );

        EclipsesItem::EclipsePhase p;
        if( phase < 1 || phase > 6 ) {
            mDebug() << "Invalid phase for eclipse at" << dt << "- skipping.";
            continue;
        }
        switch( phase ) {
            case 1: p = EclipsesItem::PartialSun; break;
            case 2: p = EclipsesItem::NonCentralAnnularSun; break;
            case 3: p = EclipsesItem::NonCentralTotalSun; break;
            case 4: p = EclipsesItem::AnnularSun; break;
            case 5: p = EclipsesItem::TotalSun; break;
            case 6: p = EclipsesItem::AnnularTotalSun; break;
        }

        EclipsesItem *item = new EclipsesItem( i );
        item->setDateTime( dt );
        item->setPhase( p );
        addItem( item );

        mDebug() << "Eclipse" << i << "added:" << dt
                 << "(" << item->phaseText() << ")";
    }
}

} // namespace Marble

#include "EclipsesModel.moc"

