//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingModel.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleMath.h"
#include "GeoDataCoordinates.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"
#include "RouteSkeleton.h"

#include <QtCore/QBuffer>
#include <QtCore/QPointer>
#include <QtCore/QRegExp>
#include <QtCore/QVector>
#include <QtGui/QMessageBox>
#include <QtGui/QPixmap>
#include <QtXml/QDomDocument>

namespace Marble
{

struct RouteElement {
    GeoDataCoordinates position;
    RoutingModel::RoutingItemType type;
    QString description;
};

/** @todo: Eventually switch to GeoDataDocument as the underlying storage */
typedef QVector<RouteElement> RouteElements;

class RoutingModelPrivate
{
public:
    RouteElements m_route;

    RoutingModel::Duration m_totalDuration;

    qreal m_totalDistance;
    
    int m_consecutiveErrors;

    RoutingModelPrivate();

    RouteElement parseGmlPos( const QStringList &content ) const;
};

RoutingModelPrivate::RoutingModelPrivate() : m_totalDistance( 0.0 ), m_consecutiveErrors( 0 )
{
    // nothing to do
}

RouteElement RoutingModelPrivate::parseGmlPos( const QStringList &content ) const
{
    Q_ASSERT( content.length() == 2 );

    RouteElement element;
    GeoDataCoordinates position;
    position.setLongitude( content.at( 0 ).toDouble(), GeoDataCoordinates::Degree );
    position.setLatitude( content.at( 1 ).toDouble(), GeoDataCoordinates::Degree );
    element.position = position;

    return element;
}

RoutingModel::RoutingModel( QObject *parent ) :
        QAbstractListModel( parent ), d( new RoutingModelPrivate )
{
    // nothing to do
}

RoutingModel::~RoutingModel()
{
    delete d;
}

int RoutingModel::rowCount ( const QModelIndex &parent ) const
{
    return parent.isValid() ? 0 : d->m_route.size();
}

QVariant RoutingModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0 ) {
        return QString( "Instruction" );
    }

    return QAbstractListModel::headerData( section, orientation, role );
}

QVariant RoutingModel::data ( const QModelIndex & index, int role ) const
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    if ( index.row() < d->m_route.size() && index.column() == 0 ) {
        RouteElement element = d->m_route.at( index.row() );
        switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return element.description;
            break;
        case Qt::DecorationRole:
            if ( element.type == Instruction )
                return QPixmap( MarbleDirs::path( "bitmaps/routing_step.png" ) );

            return QVariant();
            break;
        case CoordinateRole:
            return QVariant::fromValue( d->m_route.at( index.row() ).position );
            break;
        case TypeRole:
            return QVariant::fromValue( d->m_route.at( index.row() ).type );
            break;
        default:
            return QVariant();
        }
    }

    return QVariant();
}

void RoutingModel::importOpenGis( const QByteArray &content )
{
    d->m_route.clear();

    QDomDocument xml;
    if ( !xml.setContent( content ) ) {
        mDebug() << "Cannot parse xml file with routing instructions.";
        return;
    }

    QDomElement root = xml.documentElement();

    QDomNodeList errors = root.elementsByTagName( "xls:Error" );
    if ( errors.size() > 0 ) {
        ++d->m_consecutiveErrors;
        for ( unsigned int i = 0; i < errors.length(); ++i ) {
            QDomNode node = errors.item( i );
            QString errorMessage = node.attributes().namedItem( "message" ).nodeValue();
            QRegExp regexp = QRegExp( "^(.*) Please Check your Position: (-?[0-9]+.[0-9]+) (-?[0-9]+.[0-9]+) !" );
            if ( regexp.indexIn( errorMessage ) == 0 ) {
                RouteElement element;
                GeoDataCoordinates position;
                if ( regexp.capturedTexts().size() == 4 ) {
                    //element.description = regexp.capturedTexts().at( 1 );
                    element.description = tr( "Sorry, found no streets nearby. Try moving this point closer to a street." );
                    if ( d->m_consecutiveErrors > 1 ) {
                        element.description = tr( "Sorry, still found no streets nearby. Please check http://openrouteservice.org to see whether routing in this country is supported yet." );
                    }
                    position.setLongitude( regexp.capturedTexts().at( 2 ).toDouble(), GeoDataCoordinates::Degree );
                    position.setLatitude( regexp.capturedTexts().at( 3 ).toDouble(), GeoDataCoordinates::Degree );
                    element.position = position;
                    element.type = Error;
                    d->m_route.push_back( element );
                }
            } else {
                mDebug() << "Error message " << errorMessage << " not parsable.";
                QString message = tr( "Sorry, a problem occurred when calculating the route. Try adjusting start and destination points." );
                QPointer<QMessageBox> messageBox = new QMessageBox( QMessageBox::Warning, "Route Error", message );
                messageBox->setDetailedText( errorMessage );
                messageBox->exec();
                delete messageBox;
            }
        }
    } else {
        d->m_consecutiveErrors = 0;
    }

    QDomNodeList summary = root.elementsByTagName( "xls:RouteSummary" );
    if ( summary.size() > 0 ) {
        QDomNodeList time = summary.item( 0 ).toElement().elementsByTagName( "xls:TotalTime" );
        QDomNodeList distance = summary.item( 0 ).toElement().elementsByTagName( "xls:TotalDistance" );
        if ( time.size() == 1 && distance.size() == 1 ) {
            QRegExp regexp = QRegExp( "^P(?:(\\d+)D)?T(?:(\\d+)H)?(?:(\\d+)M)?(\\d+)S" );
            if ( regexp.indexIn( time.item( 0 ).toElement().text() ) == 0 ) {
                QStringList matches = regexp.capturedTexts();
                int days( 0 ), hours( 0 ), minutes( 0 ), seconds( 0 );
                switch ( matches.size() ) {
                case 5:
                    days    = regexp.cap( matches.size() - 4 ).toInt();
                    // Intentionally no break
                case 4:
                    hours   = regexp.cap( matches.size() - 3 ).toInt();
                    // Intentionally no break
                case 3:
                    minutes = regexp.cap( matches.size() - 2 ).toInt();
                    // Intentionally no break
                case 2:
                    seconds = regexp.cap( matches.size() - 1 ).toInt();
                    break;
                default:
                    mDebug() << "Unable to parse time string " << time.item( 0 ).toElement().text();
                }

                d->m_totalDuration.days = days;
                d->m_totalDuration.time = QTime( hours, minutes, seconds, 0 );
                d->m_totalDistance = distance.item( 0 ).attributes().namedItem( "value" ).nodeValue().toDouble();
                QString unit = distance.item( 0 ).attributes().namedItem( "uom" ).nodeValue();
                if ( unit == "M" ) {
                    d->m_totalDistance *= METER2KM;
                } else if ( unit != "KM" ) {
                    mDebug() << "Cannot parse distance unit " << unit << ", treated as km.";
                }
            }
        }
    }

    QDomNodeList geometry = root.elementsByTagName( "xls:RouteGeometry" );
    if ( geometry.size() > 0 ) {
        QDomNodeList waypoints = geometry.item( 0 ).toElement().elementsByTagName( "gml:pos" );
        for ( unsigned int i = 0; i < waypoints.length(); ++i ) {
            QDomNode node = waypoints.item( i );
            QStringList content = node.toElement().text().split( ' ' );
            if ( content.length() == 2 ) {
                RouteElement element = d->parseGmlPos( content );
                element.type = WayPoint;
                d->m_route.push_back( element );
            }
        }
    }

    QDomNodeList instructionList = root.elementsByTagName( "xls:RouteInstructionsList" );
    if ( instructionList.size() > 0 ) {
        QDomNodeList instructions = instructionList.item( 0 ).toElement().elementsByTagName( "xls:RouteInstruction" );
        for ( unsigned int i = 0; i < instructions.length(); ++i ) {
            QDomElement node = instructions.item( i ).toElement();

            QDomNodeList textNodes = node.elementsByTagName( "xls:Instruction" );
            QDomNodeList positions = node.elementsByTagName( "gml:pos" );

            if ( textNodes.size() > 0 && positions.size() > 0 ) {
                QStringList content = positions.at( 0 ).toElement().text().split( ' ' );
                if ( content.length() == 2 ) {
                    RouteElement element = d->parseGmlPos( content );
                    element.description = textNodes.item( 0 ).toElement().text();
                    element.type = Instruction;
                    d->m_route.push_back( element );
                }
            }
        }
    }

    reset();
}

RoutingModel::Duration RoutingModel::duration() const
{
    return d->m_totalDuration;
}

qreal RoutingModel::totalDistance() const
{
    return d->m_totalDistance;
}

void RoutingModel::exportGpx( QIODevice *device ) const
{
    QString content( "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n" );
    content += "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" creator=\"Marble\" version=\"1.1\" ";
    content += "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
    content += "xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 ";
    content += "http://www.topografix.com/GPX/1/1/gpx.xsd\">\n";
    content += "<metadata>\n  <link href=\"http://edu.kde.org/marble\">\n    ";
    content += "<text>Marble Virtual Globe</text>\n  </link>\n</metadata>\n";

    QList<RouteElement> waypoints;
    content += "  <rte>\n    <name>Route</name>\n";
    foreach( const RouteElement &element, d->m_route ) {
        if ( element.type == Instruction ) {
            qreal lon = element.position.longitude( GeoDataCoordinates::Degree );
            qreal lat = element.position.latitude( GeoDataCoordinates::Degree );
            content += QString( "    <rtept lat=\"%1\" lon=\"%2\"><name>%3</name></rtept>\n" ).arg( lat, 0, 'f', 7 ).arg( lon, 0, 'f', 7 ).arg( element.description );
        } else if ( element.type == WayPoint ) {
            waypoints << element;
        }
    }
    content += "  </rte>\n";

    content += "<trk>\n  <name>Route</name>\n    <trkseg>\n";
    foreach( const RouteElement &element, waypoints ) {
        Q_ASSERT( element.type == WayPoint );
        qreal lon = element.position.longitude( GeoDataCoordinates::Degree );
        qreal lat = element.position.latitude( GeoDataCoordinates::Degree );
        content += QString( "      <trkpt lat=\"%1\" lon=\"%2\"></trkpt>\n" ).arg( lat, 0, 'f', 7 ).arg( lon, 0, 'f', 7 );
    }
    content += "    </trkseg>\n  </trk>\n";
    content += "</gpx>\n";

    device->write( content.toUtf8() );
}

void RoutingModel::clear()
{
    d->m_route.clear();
    reset();
}

int RoutingModel::rightNeighbor( const GeoDataCoordinates &position, RouteSkeleton const *const route ) const
{
    Q_ASSERT( route && "Must not pass a null route ");

    // Quick result for trivial cases
    if ( route->size() < 3 ) {
        return route->size() - 1;
    }

    // Generate an ordered list of all waypoints
    QVector<GeoDataCoordinates> waypoints;
    QMap<int,int> mapping;
    foreach( const RouteElement& element, d->m_route ) {
        if ( element.type == WayPoint ) {
            waypoints << element.position;
        }
    }

    // Force first mapping point to match the route start
    mapping[0] = 0;

    // Calculate the mapping between waypoints and via points
    // Need two for loops to avoid getting stuck in local minima
    for ( int j=1; j<route->size()-1; ++j ) {
        qreal minDistance = -1.0;
        for ( int i=mapping[j-1]; i<waypoints.size(); ++i ) {
            qreal distance = distanceSphere( waypoints[i], route->at(j) );
            if (minDistance < 0.0 || distance < minDistance ) {
                mapping[j] = i;
                minDistance = distance;
            }
        }
    }

    // Determine waypoint with minimum distance to the provided position
    qreal minWaypointDistance = -1.0;
    int waypoint=0;
    for ( int i=0; i<waypoints.size(); ++i ) {
        qreal waypointDistance = distanceSphere( waypoints[i], position );
        if ( minWaypointDistance < 0.0 || waypointDistance < minWaypointDistance ) {
            minWaypointDistance = waypointDistance;
            waypoint = i;
        }
    }

    // Force last mapping point to match the route destination
    mapping[route->size()-1] = waypoints.size()-1;

    // Determine neighbor based on the mapping
    QMap<int, int>::const_iterator iter = mapping.constBegin();
    for ( ; iter != mapping.constEnd(); ++iter ) {
        if ( iter.value() > waypoint ) {
            int index = iter.key();
            Q_ASSERT( index >= 0 && index <= route->size() );
            return index;
        }
    }

    return route->size()-1;
}

} // namespace Marble

#include "RoutingModel.moc"
