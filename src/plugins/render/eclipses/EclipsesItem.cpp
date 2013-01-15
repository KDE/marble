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

#include "MarbleDebug.h"

namespace Marble
{

EclipsesItem::EclipsesItem( EclSolar *ecl, int index, QObject *parent )
    : QObject( parent ),
      m_ecl( ecl ),
      m_index( index ),
      m_calculationsNeedUpdate( true ),
      m_phase( TotalSun ),
      m_magnitude( 0. ),
      m_umbra( Tessellate )
{
    initialize();
}

EclipsesItem::~EclipsesItem()
{
}

int EclipsesItem::index() const
{
    return m_index;
}

QDateTime EclipsesItem::dateTime() const
{
    return m_dateTime;
}

bool EclipsesItem::takesPlaceAt( const QDateTime &dateTime ) const
{
    Q_UNUSED(dateTime);
    return true;
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

double EclipsesItem::magnitude() const
{
    return m_magnitude;
}

const GeoDataCoordinates EclipsesItem::maxLocation()
{
    if( m_calculationsNeedUpdate ) {
        calculate();
    }

    return m_maxLocation;
}

const GeoDataLineString EclipsesItem::centralLine()
{
    if( m_calculationsNeedUpdate ) {
        calculate();
    }

    return m_centralLine;
}

const GeoDataLinearRing EclipsesItem::umbra()
{
    if( m_calculationsNeedUpdate ) {
        calculate();
    }

    return m_umbra;
}

const GeoDataLineString EclipsesItem::southernPenUmbra()
{
    if( m_calculationsNeedUpdate ) {
        calculate();
    }

    return m_southernPenUmbra;
}

const GeoDataLineString EclipsesItem::northernPenUmbra()
{
    if( m_calculationsNeedUpdate ) {
        calculate();
    }

    return m_northernPenUmbra;
}

const QList<GeoDataCoordinates> EclipsesItem::shadowConeUmbra()
{
    if( m_calculationsNeedUpdate ) {
        calculate();
    }

    return m_shadowConeUmbra;
}

const QList<GeoDataCoordinates> EclipsesItem::shadowConePenUmbra()
{
    if( m_calculationsNeedUpdate ) {
        calculate();
    }

    return m_shadowConePenUmbra;
}

const QList<GeoDataCoordinates> EclipsesItem::shadowCone60MagPenUmbra()
{
    if( m_calculationsNeedUpdate ) {
        calculate();
    }

    return m_shadowCone60MagPenUmbra;
}

const QList<GeoDataLinearRing> EclipsesItem::sunBoundaries()
{
    if( m_calculationsNeedUpdate ) {
        calculate();
    }

    return m_sunBoundaries;
}

void EclipsesItem::initialize()
{
    // set basic information
    int year, month, day, hour, min, phase;
    double secs, tz;

    phase = m_ecl->getEclYearInfo( m_index, year, month, day,
                                            hour, min, secs,
                                            tz, m_magnitude );

    switch( phase ) {
        case 1: m_phase = EclipsesItem::PartialSun; break;
        case 2: m_phase = EclipsesItem::NonCentralAnnularSun; break;
        case 3: m_phase = EclipsesItem::NonCentralTotalSun; break;
        case 4: m_phase = EclipsesItem::AnnularSun; break;
        case 5: m_phase = EclipsesItem::TotalSun; break;
        case 6: m_phase = EclipsesItem::AnnularTotalSun; break;
        default:
            mDebug() << "Invalid phase for eclipse at" << year << "/" <<
                        day << "/" << month << "!";
    }

    m_dateTime = QDateTime( QDate( year, month, day ),
                            QTime( hour, min, secs ),
                            Qt::UTC ).addSecs( - ( tz * 3600 ) );

    m_calculationsNeedUpdate = true;
}

void EclipsesItem::calculate()
{
    int np, kp, j;
    double lat1, lng1, lat2, lng2, lat3, lng3, lat4, lng4;
    double ltf[60], lnf[60];

    m_ecl->putEclSelect( m_index );

    // FIXME: set observer location
    m_ecl->getMaxPos( lat1, lng1 );
    m_ecl->setLocalPos( lat1, lng1, 0 );

    // eclipse's maximum location
    m_maxLocation = GeoDataCoordinates( lng1, lat1, 0., GeoDataCoordinates::Degree );

    // calculate central line
    np = m_ecl->eclPltCentral( true, lat1, lng1 );
    kp = np;
    m_centralLine.clear();
    m_centralLine << GeoDataCoordinates( lng1, lat1, 0., GeoDataCoordinates::Degree );

    if( np > 3 ) { // central eclipse
        while( np > 3 ) {
            np = m_ecl->eclPltCentral( false, lat1, lng1 );
            if( np > 3 ) {
                m_centralLine << GeoDataCoordinates( lng1, lat1, 0., GeoDataCoordinates::Degree );
            }
        }
    }

    // calculate umbra
    np = kp;
    m_umbra.clear();
    if( np > 3 ) { // total or annual eclipse
        // northern /southern boundaries of umbra
        np = m_ecl->centralBound( true, lat1, lng1, lat2, lng2 );

        GeoDataLinearRing lowerUmbra( Tessellate ), upperUmbra( Tessellate );
        lowerUmbra << GeoDataCoordinates( lng1, lat1, 0., GeoDataCoordinates::Degree );
        upperUmbra << GeoDataCoordinates( lng2, lat2, 0., GeoDataCoordinates::Degree );

        while( np > 0 ) {
            np = m_ecl->centralBound( false, lat1, lng1, lat2, lng2 );
            if( lat1 <= 90. ) {
                lowerUmbra << GeoDataCoordinates( lng1, lat1, 0., GeoDataCoordinates::Degree );
            }
            if( lat1 <= 90. ) {
                upperUmbra << GeoDataCoordinates( lng2, lat2, 0., GeoDataCoordinates::Degree );
            }
        }

        GeoDataLinearRing invertedUpperUmbra( Tessellate );
        QVector<GeoDataCoordinates>::const_iterator iter = upperUmbra.constEnd() - 1;
        for( ; iter != upperUmbra.constBegin(); --iter ) {
            invertedUpperUmbra << *iter;
        }
        invertedUpperUmbra << upperUmbra.first();
        upperUmbra = invertedUpperUmbra;

        m_umbra << lowerUmbra << upperUmbra;
    }

    // shadow cones
    m_shadowConeUmbra.clear();
    m_shadowConePenUmbra.clear();
    m_shadowCone60MagPenUmbra.clear();

    m_ecl->getLocalMax( lat2, lat3, lat4 );

    m_ecl->getShadowCone( lat2, true, 40, ltf, lnf );
    for( j = 0; j < 40; j++ ) {
        if( ltf[j] < 100. ) {
            m_shadowConeUmbra << GeoDataCoordinates( lnf[j], ltf[j], 0., GeoDataCoordinates::Degree );
        }
    }

    m_ecl->setPenumbraAngle( 1., 0 );
    m_ecl->getShadowCone( lat2, false, 60, ltf, lnf );
    for( j = 0; j < 60; j++ ) {
        if( ltf[j] < 100. ) {
            m_shadowConePenUmbra << GeoDataCoordinates( lnf[j], ltf[j], 0., GeoDataCoordinates::Degree );
        }
    }

    m_ecl->setPenumbraAngle( 0.6, 1 );
    m_ecl->getShadowCone( lat2, false, 60, ltf, lnf );
    for( j = 0; j < 60; j++ ) {
        if( ltf[j] < 100. ) {
            m_shadowCone60MagPenUmbra << GeoDataCoordinates( lnf[j], ltf[j], 0., GeoDataCoordinates::Degree );
        }
    }

    m_ecl->setPenumbraAngle( 1., 0 );

    // eclipse boundaries
    m_southernPenUmbra.clear();
    m_northernPenUmbra.clear();

    np = m_ecl->GNSBound( true, true, lat1, lng2 );
    while( np > 0 ) {
        np = m_ecl->GNSBound( false, true, lat1, lng1 );
        if( ( np > 0 ) && ( lat1 <= 90. ) ) {
            m_southernPenUmbra << GeoDataCoordinates( lng1, lat1, 0., GeoDataCoordinates::Degree );
        }
    }

    np = m_ecl->GNSBound( true, false, lat1, lng1 );
    while( np > 0 ) {
        np = m_ecl->GNSBound( false, false, lat1, lng1 );
        if( ( np > 0 ) && ( lat1 <= 90. ) ) {
            m_northernPenUmbra << GeoDataCoordinates( lng1, lat1, 0., GeoDataCoordinates::Degree );
        }
    }

    // sunrise / sunset boundaries

    QList<GeoDataLinearRing*> sunBoundaries;
    np = m_ecl->GRSBound( true, lat1, lng1, lat3, lng3 );

    GeoDataLinearRing *lowerBoundary = new GeoDataLinearRing( Tessellate );
    *lowerBoundary << GeoDataCoordinates( lng1, lat1, 0., GeoDataCoordinates::Degree );

    GeoDataLinearRing *upperBoundary = new GeoDataLinearRing( Tessellate );
    *upperBoundary << GeoDataCoordinates( lng3, lat3, 0., GeoDataCoordinates::Degree );

    m_sunBoundaries.clear();

    while ( np > 0 ) {
        np = m_ecl->GRSBound( false, lat2, lng2, lat4, lng4 );
        bool pline = fabs( lng1 - lng2 ) < 10.; // during partial eclipses, the Rise/Set
                                                // lines switch at one stage.
                                                // This will prevent an ugly line between
                                                // the switch points. If there is a
                                                // longitude jump then add the current
                                                // section to our sun boundaries collection
                                                // and start a new section
        if ( !pline && !lowerBoundary->isEmpty() ) {
            sunBoundaries.prepend( lowerBoundary );
            lowerBoundary = new GeoDataLinearRing( Tessellate );
        }
        if ( ( np > 0 ) && ( lat2 <= 90. ) && ( lat1 <= 90. ) ) {
            *lowerBoundary << GeoDataCoordinates( lng2, lat2, 0., GeoDataCoordinates::Degree );
        }
        pline = fabs( lng3 - lng4 ) < 10.; // during partial eclipses, the Rise/Set lines
                                           // switch at one stage.
                                           // This will prevent an ugly line between the
                                           // switch points. If there is a longitude jump
                                           // then add the current section to our sun
                                           // boundaries collection and start a new section
        if ( !pline && !upperBoundary->isEmpty() ) {
            sunBoundaries.prepend( upperBoundary );
            upperBoundary = new GeoDataLinearRing( Tessellate );
        }
        if ( pline && ( np > 0 ) && ( lat4 <= 90. ) && ( lat3 <= 90. ) ) {
            //*upperBoundary << GeoDataCoordinates( lng4, lat4, 0., GeoDataCoordinates::Degree );
        }

        lng1 = lng2;
        lat1 = lat2;
        lng3 = lng4;
        lat3 = lat4;
    }

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

            // TODO: Now that MableMath is not public anymore we need a
            // GeoDataCoordinates::distance() method in Marble.
            GeoDataLineString ruler;
            ruler << sunBoundary.last() << sunBoundary.first();
            qreal closestDistance = ruler.length( 1 );
            int closestEnd = 0;  // 0 = start of section, 1 = end of section

            // Look for a section that is closest to our sunBoundary section.
            for ( int it = 0; it < sunBoundaries.size(); ++it ) {
                GeoDataLineString distanceStartSection;
                distanceStartSection << sunBoundary.last() << sunBoundaries.at( it )->first();

                GeoDataLineString distanceEndSection;
                distanceEndSection << sunBoundary.last() << sunBoundaries.at( it )->last();
                if ( distanceStartSection.length( 1 ) < closestDistance ) {
                    closestDistance = distanceStartSection.length( 1 );
                    closestSection = it;
                    closestEnd = 0;
                }
                if ( distanceEndSection.length(1) < closestDistance ) {
                    closestDistance = distanceEndSection.length( 1 );
                    closestSection = it;
                    closestEnd = 1;
                }
            }

            if ( closestSection == -1 ) {
                // There is no other section that is closer to the end of
                // our sunBoundary section than the startpoint of our
                // sunBoundary itself
                break;
            }
            else {
                // We now concatenate the closest section to the sunBoundary.
                // First we might have to invert it so that we concatenate
                // the right end
                if ( closestEnd == 1 ) {
                    // TODO: replace this with a GeoDataLinearRing::invert()
                    // method that needs to be added to Marble ...
                    GeoDataLinearRing * invertedBoundary = new GeoDataLinearRing( Tessellate );
                    QVector<GeoDataCoordinates>::const_iterator iter = sunBoundaries.at( closestSection )->constEnd();
                    --iter;
                    for( ; iter != sunBoundaries.at( closestSection )->constBegin(); --iter ) {
                        *invertedBoundary << *iter;
                    }
                    *invertedBoundary << sunBoundaries.at( closestSection )->first();
                    delete sunBoundaries[closestSection];
                    sunBoundaries[closestSection] = invertedBoundary;
                }
                sunBoundary << *sunBoundaries[closestSection];

                // Now remove the section that we've just added from the list
                delete sunBoundaries[closestSection];
                sunBoundaries.removeAt( closestSection );
            }
        }

        m_sunBoundaries << sunBoundary;

        if ( sunBoundaries.size() == 0 ) break;
    }

    m_calculationsNeedUpdate = false;
}

} // Namespace Marble

#include "EclipsesItem.moc"

