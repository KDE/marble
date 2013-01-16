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
#include "GeoDataLineString.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLinearRing.h"
#include "MarbleColors.h"

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
}

EclipsesModel::~EclipsesModel()
{
    clear();
    delete m_ecps;
}
const GeoDataCoordinates& EclipsesModel::observationPoint() const
{
    return m_observationPoint;
}

void EclipsesModel::setObservationPoint( const GeoDataCoordinates &coords )
{
    m_observationPoint = coords;
    m_ecps->setLocalPos( coords.latitude(), coords.altitude(), 6000. );
}

void EclipsesModel::setYear( int year )
{
    if( m_currentYear != year ) {

        mDebug() << "Year changed - Calculating eclipses...";
        m_currentYear = year;
        m_ecps->putYear( year );

        updateEclipses();
    }
}

int EclipsesModel::year() const
{
    return m_currentYear;
}

EclipsesItem* EclipsesModel::eclipseWithIndex( int year, int index )
{
    setYear( year );

    foreach( EclipsesItem *item, m_items ) {
        if( item->index() == index ) {
            return item;
        }
    }

    return 0;
}

QList<EclipsesItem*> EclipsesModel::items() const
{
    return m_items;
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
    clear();

    int num = m_ecps->getNumberEclYear();
    for( int i = 1; i <= num; ++i ) {
        EclipsesItem *item = new EclipsesItem( m_ecps, i );
        addItem( item );
        mDebug() << "Eclipse" << i << "added:" << item->dateTime()
            << "(" << item->phaseText() << ")";
    }
}

} // namespace Marble

#include "EclipsesModel.moc"

