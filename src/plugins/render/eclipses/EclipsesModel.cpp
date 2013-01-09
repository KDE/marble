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
    if( m_currentItem ) {
        paintItem( m_currentItem, painter );
    }
}

void EclipsesModel::paintItem( EclipsesItem *item, GeoPainter *painter )
{
    int np, kp, j;
    double lat1, lng1, lat2, lng2, lat3, lng3, lat4, lng4;
    double ltf[60], lnf[60];

    // plot central line for central eclipses

    m_ecps->putEclSelect( item->index() );
    np = m_ecps->eclPltCentral(true, lat1, lng1);
    kp = np;

    GeoDataLineString  centralLine ( Tessellate );
    centralLine << GeoDataCoordinates (lng1, lat1, 0.0, GeoDataCoordinates::Degree);

    if (np > 3)  // we have a central eclipse
    {
        while (np > 3)
        {
            np = m_ecps->eclPltCentral(false, lat1, lng1);
            if (np > 3)
            {
                centralLine << GeoDataCoordinates (lng1, lat1, 0.0, GeoDataCoordinates::Degree);
            };
        };
    }

    painter->setPen(Qt::black);
    painter->drawPolyline(centralLine);

    np = kp;
    if (np > 3)  // total or annular eclipse
    {
        // North/South Boundaries (umbra)

        np = m_ecps->centralBound(true, lat1, lng1, lat2, lng2);

        GeoDataLinearRing  lowerUmbra ( Tessellate );
        lowerUmbra << GeoDataCoordinates (lng1, lat1, 0.0, GeoDataCoordinates::Degree);
        GeoDataLinearRing  upperUmbra ( Tessellate );
        upperUmbra << GeoDataCoordinates (lng2, lat2, 0.0, GeoDataCoordinates::Degree);

        while (np > 0)
        {
            np = m_ecps->centralBound(false, lat1, lng1, lat2, lng2);

            if (lat1 <= 90.0) lowerUmbra << GeoDataCoordinates(lng1, lat1, 0.0, GeoDataCoordinates::Degree);
            if (lat2 <= 90.0) upperUmbra << GeoDataCoordinates(lng2, lat2, 0.0, GeoDataCoordinates::Degree);
        };

        // Invert upperUmbra linear ring
        GeoDataLinearRing invertedUpperUmbra (Tessellate);

        QVector<GeoDataCoordinates>::const_iterator iter = upperUmbra.constEnd();
        --iter;
        for( ; iter != upperUmbra.constBegin(); --iter ) {
            invertedUpperUmbra << *iter;
        }
        invertedUpperUmbra << upperUmbra.first();
        upperUmbra = invertedUpperUmbra;

        GeoDataLinearRing umbra ( Tessellate );

        umbra << lowerUmbra << upperUmbra;

        painter->setPen( Oxygen::aluminumGray4 );

        QColor sunBoundingBrush ( Oxygen::aluminumGray4 );
        sunBoundingBrush.setAlpha( 128 );
        painter->setBrush( sunBoundingBrush );

        painter->drawPolygon( umbra );

    }

    //  draw shadow cones at maximum eclipse time

    m_ecps->getMaxPos(lat1, lng1);
    m_ecps->setLocalPos(lat1, lng1,0);
    m_ecps->getLocalMax(lat2, lat3, lat4);

    m_ecps->getShadowCone(lat2, true, 40, ltf, lnf);  // umbra
    for (j=0; j<40; j++)
    {
        if(ltf[j] < 100.0)
            painter->drawEllipse(GeoDataCoordinates(lnf[j], ltf[j], 0.0, GeoDataCoordinates::Degree),2,2);
    }

    m_ecps->setPenumbraAngle(1.0, 0);  // full penumbra
    m_ecps->getShadowCone(lat2, false, 60, ltf, lnf);
    painter->setPen(Qt::blue);
    for (j=0; j<60; j++)
    {
        if(ltf[j] < 100.0)
            painter->drawEllipse(GeoDataCoordinates(lnf[j], ltf[j], 0.0, GeoDataCoordinates::Degree),2,2);
    }

    m_ecps->setPenumbraAngle(0.6, 1);  // 60% magnitude penumbra
    m_ecps->getShadowCone(lat2, false, 60, ltf, lnf);
    painter->setPen(Qt::magenta);
    for (j=0; j<60; j++)
    {
        if(ltf[j] < 100.0)
            painter->drawEllipse(GeoDataCoordinates(lnf[j], ltf[j], 0.0, GeoDataCoordinates::Degree),3,3);
    }
    m_ecps->setPenumbraAngle(1.0, 0);


    // mark point of maximum eclipse
    m_ecps->getMaxPos(lat1, lng1);

    GeoDataCoordinates home2(lng1, lat1, 0.0, GeoDataCoordinates::Degree);
    painter->setPen(Qt::white);
    QColor sunBoundingBrush ( Qt::white );
    sunBoundingBrush.setAlpha( 128 );
    painter->setBrush( sunBoundingBrush );

    painter->drawEllipse(home2, 15, 15);
    painter->setPen(Oxygen::brickRed4);
    painter->drawText(home2, "MaxEcl");

    // plot eclipse boundaries

    // North/South Boundaries (penumbra)

    painter->setPen(Qt::magenta);

    // southern boundary
    np = m_ecps->GNSBound(true, true, lat1, lng2);

    GeoDataLineString southernPenUmbra(Tessellate);
    while (np > 0)
    {
        np = m_ecps->GNSBound(false, true, lat1, lng1);
        if ((np > 0) && (lat1 <= 90.0)) southernPenUmbra  << GeoDataCoordinates (lng1, lat1, 0.0, GeoDataCoordinates::Degree);
    };

    painter->setPen(Oxygen::brickRed4);
    painter->drawPolyline( southernPenUmbra );

    // northern boundary
    np = m_ecps->GNSBound(true, false, lat1, lng1);

    GeoDataLineString northernPenUmbra(Tessellate);
    while (np > 0)
    {
        np = m_ecps->GNSBound(false, false, lat1, lng1);
        if ((np > 0) && (lat1 <= 90.0)) northernPenUmbra  << GeoDataCoordinates (lng1, lat1, 0.0, GeoDataCoordinates::Degree);
    };

    painter->setPen(Oxygen::brickRed4);
    painter->drawPolyline( northernPenUmbra );

    // Sunrise / Sunset Boundaries

    QList<GeoDataLinearRing*> sunBoundaries;

    np = m_ecps->GRSBound(true, lat1, lng1, lat3, lng3);
    GeoDataLinearRing * lowerBoundary = new GeoDataLinearRing( Tessellate );

    *lowerBoundary << GeoDataCoordinates (lng1, lat1, 0.0, GeoDataCoordinates::Degree);

    GeoDataLinearRing * upperBoundary = new GeoDataLinearRing( Tessellate );
    *upperBoundary << GeoDataCoordinates (lng3, lat3, 0.0, GeoDataCoordinates::Degree);

    while (np > 0)
    {
        np = m_ecps->GRSBound(false, lat2, lng2, lat4, lng4);
        bool pline = fabs(lng1 - lng2) < 10.0; // during partial eclipses, the Rise/Set lines switch at one stage.
        // This will prevent an ugly line between the switch points.
        // If there is a longitude jump then add the current section to our sun boundaries collection and start a new section
        if (!pline && !lowerBoundary->isEmpty()) {
            sunBoundaries.prepend(lowerBoundary);
            lowerBoundary = new GeoDataLinearRing( Tessellate );
        }
        if ((np > 0) && (lat2 <= 90.0) && (lat1 <= 90.0)) {
            *lowerBoundary << GeoDataCoordinates(lng2, lat2, 0.0, GeoDataCoordinates::Degree);
        }
        pline = fabs(lng3 - lng4) < 10.0; // during partial eclipses, the Rise/Set lines switch at one stage.
        // This will prevent an ugly line between the switch points.
        // If there is a longitude jump then add the current section to our sun boundaries collection and start a new section
        if (!pline && !upperBoundary->isEmpty()) {
            sunBoundaries.prepend(upperBoundary);
            upperBoundary = new GeoDataLinearRing( Tessellate );
        }
        if (pline && (np > 0) && (lat4 <= 90.0) && (lat3 <= 90.0)) {
            *upperBoundary << GeoDataCoordinates(lng4, lat4, 0.0, GeoDataCoordinates::Degree);
        }

        lng1 = lng2;
        lat1 = lat2;
        lng3 = lng4;
        lat3 = lat4;
    };

    if ( !lowerBoundary->isEmpty() ) {
        sunBoundaries.prepend(lowerBoundary);
    }
    if ( !upperBoundary->isEmpty() ) {
        sunBoundaries.prepend(upperBoundary);
    }


    for ( int result = 0; result < 2; ++result ) {
        GeoDataLinearRing sunBoundary( Tessellate );

        sunBoundary = *sunBoundaries.last();
        sunBoundaries.pop_back();

        int closestSection;

        while ( sunBoundaries.size() > 0) {

            closestSection = -1;
            // TODO: Now that MableMath is not public anymore we need a GeoDataCoordinates::distance() method in Marble.
            GeoDataLineString ruler;
            ruler << sunBoundary.last() << sunBoundary.first();
            qreal closestDistance = ruler.length(1);
            int closestEnd = 0;  // 0 = start of section, 1 = end of section

            // Look for the closest section that is closest to our sunBoundary section.
            for ( int it = 0; it < sunBoundaries.size(); ++it ) {
                GeoDataLineString distanceStartSection;
                distanceStartSection << sunBoundary.last() << sunBoundaries.at(it)->first();
                GeoDataLineString distanceEndSection;
                distanceEndSection << sunBoundary.last() << sunBoundaries.at(it)->last();
                if ( distanceStartSection.length(1) < closestDistance ) {
                    closestDistance = distanceStartSection.length(1);
                    closestSection = it;
                    closestEnd = 0;
                }
                if ( distanceEndSection.length(1) < closestDistance ) {
                    closestDistance = distanceEndSection.length(1);
                    closestSection = it;
                    closestEnd = 1;
                }
            }

            if (closestSection == -1) {
                // There is no other section that is closer to the end of our sunBoundary section than the startpoint of our sunBoundary itself
                break;
            }
            else {
                // We now concatenate the closest section to the sunBoundary.
                // First we might have to invert it so that we concatenate the right end
                if (closestEnd == 1) {
                    // TODO: replace this with a GeoDataLinearRing::invert() method that needs to be added to Marble ...
                    GeoDataLinearRing * invertedBoundary = new GeoDataLinearRing( Tessellate );

                    QVector<GeoDataCoordinates>::const_iterator iter = sunBoundaries.at(closestSection)->constEnd();
                    --iter;
                    for( ; iter != sunBoundaries.at(closestSection)->constBegin(); --iter ) {
                        *invertedBoundary << *iter;
                    }
                    *invertedBoundary << sunBoundaries.at(closestSection)->first();
                    delete sunBoundaries[closestSection];
                    sunBoundaries[closestSection] = invertedBoundary;
                }
                sunBoundary << *sunBoundaries[closestSection];

                // Now remove the section that we've just added from the list
                delete sunBoundaries[closestSection];
                sunBoundaries.removeAt(closestSection);
            }
        }

        // Now paint the processed Sunrise / Sunset area:
        painter->setPen( Oxygen::hotOrange5 );

        QColor sunBoundingBrush ( Oxygen::hotOrange5 );
        sunBoundingBrush.setAlpha( 64 );
        painter->setBrush( sunBoundingBrush );

        painter->drawPolygon(sunBoundary);

        // Stay only in the loop if there are two Sunrise / Sunset areas
        if (sunBoundaries.size() == 0) break;
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
    int year, month, day, hour, min, phase;
    double secs, tz, mag, start, stop;

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
        item->setMagnitude( mag );
        addItem( item );

        mDebug() << "Eclipse" << i << "added:" << dt
            << "(" << item->phaseText() << ")";
    }
}

} // namespace Marble

#include "EclipsesModel.moc"

