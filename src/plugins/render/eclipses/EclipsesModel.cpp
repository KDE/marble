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
      m_currentItem( 0 ),
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

EclipsesItem* EclipsesModel::currentItem() const
{
    return m_currentItem;
}

EclipsesItem* EclipsesModel::eclipseWithIndex( int index ) const
{
    foreach( EclipsesItem *item, m_items ) {
        if( item->index() == index ) {
            return item;
        }
    }

    return 0;
}

void EclipsesModel::synchronize( const MarbleClock *clock )
{
    m_currentItem = 0;

    foreach( EclipsesItem *item, m_items ) {
        mDebug() << item->dateTime() << "==" << clock->dateTime();
        if( item->dateTime() == clock->dateTime() ) {
            mDebug() << "Found eclipse at current date";
            m_currentItem = item;
            return;
        }
    }

    mDebug() << "No eclipse found for current date";
}

QList<EclipsesItem*> EclipsesModel::items() const
{
    return m_items;
}

void EclipsesModel::paint( GeoPainter *painter )
{
    for( int i = 0; i < m_items.count(); ++i ) {
        if( m_items.at( i )->takesPlaceAt( m_clock->dateTime() ) ) {
            paintItem( m_items.at( i ), painter );
            return;
        }
    }
}

void EclipsesModel::paintItem( EclipsesItem *item, GeoPainter *painter )
{
    int phase = item->phase();

    // plot central line for central eclipses
    painter->setPen( Qt::black );
    painter->drawPolyline( item->centralLine() );

    if( phase > 3 )  // total or annular eclipse
    {
        painter->setPen( Oxygen::aluminumGray4 );

        QColor sunBoundingBrush ( Oxygen::aluminumGray4 );
        sunBoundingBrush.setAlpha( 128 );
        painter->setBrush( sunBoundingBrush );

        painter->drawPolygon( item->umbra() );
    }

    //  draw shadow cones at maximum eclipse time
    
    QList<GeoDataCoordinates>::const_iterator ci;
    
    painter->setPen( Qt::black );
    ci = item->shadowConeUmbra().constBegin();
    for ( ; ci != item->shadowConeUmbra().constEnd(); ++ci ) {
        painter->drawEllipse( *ci, 2, 2 );
    }

    painter->setPen( Qt::blue );
    ci = item->shadowConePenUmbra().constBegin();
    for ( ; ci != item->shadowConePenUmbra().constEnd(); ++ci ) {
        painter->drawEllipse( *ci, 2, 2 );
    }

    painter->setPen( Qt::magenta );
    ci = item->shadowCone60MagPenUmbra().constBegin();
    for ( ; ci != item->shadowCone60MagPenUmbra().constEnd(); ++ci ) {
        painter->drawEllipse( *ci, 3, 3 );
    }

    // mark point of maximum eclipse

    painter->setPen( Qt::white );
    QColor sunBoundingBrush ( Qt::white );
    sunBoundingBrush.setAlpha( 128 );
    painter->setBrush( sunBoundingBrush );

    painter->drawEllipse( item->maxLocation(), 15, 15 );
    painter->setPen( Oxygen::brickRed4 );
    painter->drawText( item->maxLocation(), "Eclipse Maximum");

    // southern boundary
    painter->setPen( Oxygen::brickRed4 );
    painter->drawPolyline( item->southernPenUmbra() );

    // northern boundary
    painter->setPen( Oxygen::brickRed4 );
    painter->drawPolyline( item->northernPenUmbra() );

    // Sunrise / Sunset Boundaries
    painter->setPen( Oxygen::hotOrange5 );
    const QList<GeoDataLinearRing> boundaries = item->sunBoundaries();
    QList<GeoDataLinearRing>::const_iterator i = boundaries.constBegin();
    for( ; i != boundaries.constEnd(); ++i ) {
        QColor sunBoundingBrush ( Oxygen::hotOrange5 );
        sunBoundingBrush.setAlpha( 64 );
        painter->setBrush( sunBoundingBrush );
        painter->drawPolygon( *i );
    }
}

void EclipsesModel::addItem( EclipsesItem *item )
{
    m_items.append( item );
}

void EclipsesModel::clear()
{
    qDeleteAll( m_items );
    m_currentItem = 0;
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

