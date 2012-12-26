//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OSRMRunner.h"

#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "routing/Maneuver.h"
#include "routing/RouteRequest.h"
#include "TinyWebBrowser.h"

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QUrl>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>

namespace Marble
{

QVector<QPair<GeoDataCoordinates,QString> > OSRMRunner:: m_cachedHints;

QString OSRMRunner:: m_hintChecksum;

OSRMRunner::OSRMRunner( QObject *parent ) :
    RoutingRunner( parent ),
    m_networkAccessManager()
{
    connect( &m_networkAccessManager, SIGNAL( finished( QNetworkReply * ) ),
             this, SLOT( retrieveData( QNetworkReply * ) ) );
}

OSRMRunner::~OSRMRunner()
{
    // nothing to do
}

void OSRMRunner::retrieveRoute( const RouteRequest *route )
{
    if ( route->size() < 2 ) {
        return;
    }

    QString url = "http://router.project-osrm.org/viaroute?output=json&instructions=true";
    GeoDataCoordinates::Unit const degree = GeoDataCoordinates::Degree;
    bool appendChecksum = false;
    typedef QPair<GeoDataCoordinates,QString> CachePair;
    QVector<CachePair> newChecksums;
    QString const invalidEntry = "invalid";
    for ( int i=0; i<route->size(); ++i ) {
        GeoDataCoordinates const coordinates = route->at( i );
        append( &url, "loc", QString::number( coordinates.latitude( degree ), 'f', 6 ) + "," + QString::number( coordinates.longitude( degree ), 'f', 6 ) );
        foreach( const CachePair &hint, m_cachedHints ) {
            if ( hint.first == coordinates && hint.second != invalidEntry && m_hintChecksum != invalidEntry ) {
                append( &url, "hint", hint.second );
                appendChecksum = true;
            }
        }
        newChecksums << CachePair( coordinates, invalidEntry );
    }

    if ( appendChecksum ) {
        append( &url, "checksum", m_hintChecksum );
    }

    m_cachedHints = newChecksums;
    m_hintChecksum = invalidEntry;

    m_request = QNetworkRequest( QUrl( url ) );
    m_request.setRawHeader( "User-Agent", TinyWebBrowser::userAgent( "Browser", "OSRMRunner" ) );

    QEventLoop eventLoop;

    connect( this, SIGNAL( routeCalculated( GeoDataDocument* ) ),
             &eventLoop, SLOT( quit() ) );

    // @todo FIXME Must currently be done in the main thread, see bug 257376
    QTimer::singleShot( 0, this, SLOT( get() ) );

    eventLoop.exec();
}

void OSRMRunner::retrieveData( QNetworkReply *reply )
{
    if ( reply->isFinished() ) {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        GeoDataDocument* document = parse( data );

        if ( !document ) {
            mDebug() << "Failed to parse the downloaded route data" << data;
        }

        emit routeCalculated( document );
    }
}

void OSRMRunner::handleError( QNetworkReply::NetworkError error )
{
    mDebug() << " Error when retrieving OSRM route: " << error;
}

void OSRMRunner::get()
{
    QNetworkReply *reply = m_networkAccessManager.get( m_request );
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( handleError( QNetworkReply::NetworkError ) ), Qt::DirectConnection );
}

void OSRMRunner::append(QString *input, const QString &key, const QString &value) const
{
    *input += "&" + key + "=" + value;
}

GeoDataLineString *OSRMRunner::decodePolyline( const QString &geometry ) const
{
    // See https://developers.google.com/maps/documentation/utilities/polylinealgorithm
    GeoDataLineString* lineString = new GeoDataLineString;
    int coordinates[2] = { 0, 0 };
    int const length = geometry.length();
    for( int i=0; i<length; /* increment happens below */ ) {
        for ( int j=0; j<2; ++j ) { // lat and lon
            int block( 0 ), shift( 0 ), result( 0 );
            do {
                block = geometry.at( i++ /* increment for outer loop */ ).toAscii() - 63;
                result |= ( block & 0x1F ) << shift;
                shift += 5;
            } while ( block >= 0x20 );
            coordinates[j] += ( ( result & 1 ) != 0 ? ~( result >> 1 ) : ( result >> 1 ) );
        }
        lineString->append( GeoDataCoordinates( double( coordinates[1] ) / 1E5,
                                                double( coordinates[0] ) / 1E5,
                                                0.0, GeoDataCoordinates::Degree ) );
    }
    return lineString;
}

RoutingInstruction::TurnType OSRMRunner::parseTurnType( const QString &instruction ) const
{   
    if ( instruction == "1" ) {
        return RoutingInstruction::Straight;
    } else if ( instruction == "2" ) {
        return RoutingInstruction::SlightRight;
    } else if ( instruction == "3" ) {
        return RoutingInstruction::Right;
    } else if ( instruction == "4" ) {
        return RoutingInstruction::SharpRight;
    } else if ( instruction == "5" ) {
        return RoutingInstruction::TurnAround;
    } else if ( instruction == "6" ) {
        return RoutingInstruction::SharpLeft;
    } else if ( instruction == "7" ) {
        return RoutingInstruction::Left;
    } else if ( instruction == "8" ) {
        return RoutingInstruction::SlightLeft;
    } else if ( instruction == "10" ) {
        return RoutingInstruction::Continue;
    } else if ( instruction.startsWith( "11-" ) ) {
        int const exit = instruction.mid( 3 ).toInt();
        switch ( exit ) {
        case 1: return RoutingInstruction::RoundaboutFirstExit; break;
        case 2: return RoutingInstruction::RoundaboutSecondExit; break;
        case 3: return RoutingInstruction::RoundaboutThirdExit; break;
        default: return RoutingInstruction::RoundaboutExit;
        }
    } else if ( instruction == "12" ) {
        return RoutingInstruction::RoundaboutExit;
    }

    // ignoring ReachViaPoint = 9;
    // ignoring StayOnRoundAbout = 13;
    // ignoring StartAtEndOfStreet = 14;
    // ignoring ReachedYourDestination = 15;

    return RoutingInstruction::Unknown;
}

GeoDataDocument *OSRMRunner::parse( const QByteArray &input )
{
    QScriptEngine engine;
    // Qt requires parentheses around json code
    QScriptValue const data = engine.evaluate( "(" + QString::fromUtf8( input ) + ")" );

    GeoDataDocument* result = 0;
    GeoDataLineString* routeWaypoints = 0;
    if ( data.property( "route_geometry" ).isString() ) {
        result = new GeoDataDocument();
        result->setName( "Open Source Routing Machine" );
        GeoDataPlacemark* routePlacemark = new GeoDataPlacemark;
        routePlacemark->setName( "Route" );
        routeWaypoints = decodePolyline( data.property( "route_geometry" ).toString() );
        routePlacemark->setGeometry( routeWaypoints );

        QString name = "%1 %2 (OSRM)";
        QString unit = "m";
        qreal length = routeWaypoints->length( EARTH_RADIUS );
        if (length >= 1000) {
            length /= 1000.0;
            unit = "km";
        }
        result->setName( name.arg( length, 0, 'f', 1 ).arg( unit ) );
        result->append( routePlacemark );
    }

    if ( result && routeWaypoints && data.property( "route_instructions" ).isArray() ) {
        bool first = true;
        QScriptValueIterator iterator( data.property( "route_instructions" ) );
        GeoDataPlacemark* instruction = new GeoDataPlacemark;
        int lastWaypointIndex = 0;
        while ( iterator.hasNext() ) {
            iterator.next();
            QVariantList details = iterator.value().toVariant().toList();
            if ( details.size() > 7 ) {
                QString const text = details.at( 0 ).toString();
                QString const road = details.at( 1 ).toString();
                int const waypointIndex = details.at( 3 ).toInt();

                if ( waypointIndex < routeWaypoints->size() ) {
                    if ( iterator.hasNext() ) {
                        GeoDataLineString *lineString = new GeoDataLineString;
                        for ( int i=lastWaypointIndex; i<=waypointIndex; ++i ) {
                            lineString->append(routeWaypoints->at( i ) );
                        }
                        instruction->setGeometry( lineString );
                        result->append( instruction );
                        instruction = new GeoDataPlacemark;
                    }
                    lastWaypointIndex = waypointIndex;
                    GeoDataExtendedData extendedData;
                    GeoDataData turnTypeData;
                    turnTypeData.setName( "turnType" );
                    RoutingInstruction::TurnType turnType = parseTurnType( text );
                    turnTypeData.setValue( turnType );
                    extendedData.addValue( turnTypeData );
                    if (!road.isEmpty()) {
                        GeoDataData roadName;
                        roadName.setName( "roadName" );
                        roadName.setValue( road );
                        extendedData.addValue( roadName );
                    }

                    if ( first ) {
                        turnType = RoutingInstruction::Continue;
                        first = false;
                    }

                    if ( turnType == RoutingInstruction::Unknown ) {
                        instruction->setName( text );
                    } else {
                        instruction->setName( RoutingInstruction::generateRoadInstruction( turnType, road ) );
                    }
                    instruction->setExtendedData( extendedData );

                    if ( !iterator.hasNext() && lastWaypointIndex > 0 ) {
                        GeoDataLineString *lineString = new GeoDataLineString;
                        for ( int i=lastWaypointIndex; i<waypointIndex; ++i ) {
                            lineString->append(routeWaypoints->at( i ) );
                        }
                        instruction->setGeometry( lineString );
                        result->append( instruction );
                    }
                }
            }
        }
    }

    if ( data.property( "hint_data" ).isValid() ) {
        QVariantList hints = data.property( "hint_data" ).property( "locations" ).toVariant().toList();
        if ( hints.size() == m_cachedHints.size() ) {
            for ( int i=0; i<m_cachedHints.size(); ++i ) {
                m_cachedHints[i].second = hints[i].toString();
            }
        }

        m_hintChecksum = data.property( "hint_data" ).property( "checksum" ).toString();
    }

    return result;
}

} // namespace Marble

#include "OSRMRunner.moc"
