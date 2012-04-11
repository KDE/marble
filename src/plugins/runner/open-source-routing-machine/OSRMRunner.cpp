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

#include "MarbleAbstractRunner.h"
#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "routing/Maneuver.h"
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

OSRMRunner::OSRMRunner( QObject *parent ) :
    MarbleAbstractRunner( parent ),
    m_networkAccessManager( new QNetworkAccessManager( this ) )
{
    connect( m_networkAccessManager, SIGNAL( finished( QNetworkReply * ) ),
             this, SLOT( retrieveData( QNetworkReply * ) ) );
}

OSRMRunner::~OSRMRunner()
{
    // nothing to do
}

GeoDataFeature::GeoDataVisualCategory OSRMRunner::category() const
{
    return GeoDataFeature::OsmSite;
}

void OSRMRunner::retrieveRoute( const RouteRequest *route )
{
    if ( route->size() < 2 ) {
        return;
    }

    QString url = "http://router.project-osrm.org/viaroute?output=json";
    GeoDataCoordinates::Unit const degree = GeoDataCoordinates::Degree;
    for ( int i=0; i<route->size(); ++i ) {
        append( &url, "loc", QString::number( route->at( i ).latitude( degree ), 'f', 6 ) + "," + QString::number( route->at( i ).longitude( degree ), 'f', 6 ) );
    }

    QNetworkRequest request = QNetworkRequest( QUrl( url ) );
    request.setRawHeader("User-Agent", TinyWebBrowser::userAgent( "Browser", "OSRMRunner" ) );
    QNetworkReply *reply = m_networkAccessManager->get( request );
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( handleError( QNetworkReply::NetworkError ) ) );

    QEventLoop eventLoop;

    connect( this, SIGNAL( routeCalculated( GeoDataDocument* ) ),
             &eventLoop, SLOT( quit() ) );

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

void OSRMRunner::append(QString *input, const QString &key, const QString &value) const
{
    *input += "&" + key + "=" + value;
}

RoutingInstruction::TurnType OSRMRunner::parseTurnType( const QString &instruction ) const
{
    /** @todo: FIXME Less fragile parsing, https://github.com/DennisOSRM/Project-OSRM/issues/216 */

    if ( instruction == "Continue" ) {
        return RoutingInstruction::Straight;
    } else if ( instruction == "Turn slight right" ) {
        return RoutingInstruction::SlightRight;
    } else if ( instruction == "Turn right" ) {
        return RoutingInstruction::Right;
    } else if ( instruction == "Turn sharp right" ) {
        return RoutingInstruction::SharpRight;
    } else if ( instruction == "U-Turn" ) {
        return RoutingInstruction::TurnAround;
    } else if ( instruction == "Turn sharp left" ) {
        return RoutingInstruction::SharpLeft;
    } else if ( instruction == "Turn left" ) {
        return RoutingInstruction::Left;
    } else if ( instruction == "Turn slight left" ) {
        return RoutingInstruction::SlightLeft;
    } else if ( instruction == "Enter roundabout and leave at first exit" ) {
        return RoutingInstruction::RoundaboutFirstExit;
    } else if ( instruction == "Enter roundabout and leave at second exit" ) {
        return RoutingInstruction::RoundaboutSecondExit;
    } else if ( instruction == "Enter roundabout and leave at third exit" ) {
        return RoutingInstruction::RoundaboutThirdExit;
    }

    return RoutingInstruction::Unknown;
}

GeoDataDocument *OSRMRunner::parse( const QByteArray &input ) const
{
    QScriptEngine engine;
    // Qt requires parentheses around json code
    QScriptValue const data = engine.evaluate( "(" + QString::fromUtf8( input ) + ")" );

    GeoDataDocument* result = 0;
    GeoDataLineString* routeWaypoints = 0;
    if ( data.property( "route_geometry" ).isArray() ) {
        result = new GeoDataDocument();
        result->setName( "Open Source Routing Machine" );
        GeoDataPlacemark* routePlacemark = new GeoDataPlacemark;
        routePlacemark->setName( "Route" );
        routeWaypoints = new GeoDataLineString;
        QScriptValueIterator iterator( data.property( "route_geometry" ) );
        while ( iterator.hasNext() ) {
            iterator.next();

            QVariantList coordinates = iterator.value().toVariant().toList();
            if ( coordinates.size() > 1 ) {
                double const lat = coordinates.at(0).toDouble();
                double const lon = coordinates.at(1).toDouble();
                routeWaypoints->append( GeoDataCoordinates( lon, lat, 0.0, GeoDataCoordinates::Degree ) );
            }
        }
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

                    if ( first && !road.isEmpty() ) {
                        instruction->setName( tr( "Head on %1" ).arg( road ) );
                        first = false;
                    } else if ( first ) {
                        instruction->setName( tr( "Start" ) );
                        first = false;
                    } else if ( turnType == RoutingInstruction::Unknown ) {
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

    return result;
}

} // namespace Marble

#include "OSRMRunner.moc"
