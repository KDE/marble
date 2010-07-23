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
#include "GeoDataGeometry.h"
#include "GeoDataFolder.h"
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

    RoutingModelPrivate();

    void importPlacemark( const GeoDataPlacemark *placemark );
};

RoutingModelPrivate::RoutingModelPrivate() : m_totalDistance( 0.0 )
{
    // nothing to do
}

void RoutingModelPrivate::importPlacemark( const GeoDataPlacemark *placemark )
{
    GeoDataGeometry* geometry = placemark->geometry();
    GeoDataLineString* lineString = dynamic_cast<GeoDataLineString*>( geometry );
    if ( lineString ) {
        for ( int i=0; i<lineString->size(); ++i ) {
            RouteElement element;
            element.type = RoutingModel::WayPoint;
            element.position = lineString->at( i );
            m_route.push_back( element );
        }
    } else if ( !placemark->name().isEmpty() && placemark->name() != "Route" ) {
        RouteElement element;
        element.type = RoutingModel::Instruction;
        element.description = placemark->name();
        element.position = placemark->coordinate();
        m_route.push_back( element );
    }
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

bool RoutingModel::importGeoDataDocument( GeoDataDocument* document )
{
    d->m_route.clear();

    QVector<GeoDataFolder*> folders = document->folderList();
    foreach( const GeoDataFolder *folder, folders ) {
        foreach( const GeoDataPlacemark *placemark, folder->placemarkList() ) {
            d->importPlacemark( placemark );
        }
    }

    foreach( const GeoDataPlacemark *placemark, document->placemarkList() ) {
        d->importPlacemark( placemark );
    }

    reset();
    return true;
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

    QList<RouteElement> instructions;
    content += "<trk>\n  <name>Route</name>\n    <trkseg>\n";
    foreach( const RouteElement &element, d->m_route ) {
        if ( element.type == WayPoint ) {
            qreal lon = element.position.longitude( GeoDataCoordinates::Degree );
            qreal lat = element.position.latitude( GeoDataCoordinates::Degree );
            content += QString( "      <trkpt lat=\"%1\" lon=\"%2\"></trkpt>\n" ).arg( lat, 0, 'f', 7 ).arg( lon, 0, 'f', 7 );
        } else {
            instructions << element;
        }
    }
    content += "    </trkseg>\n  </trk>\n";

    content += "  <rte>\n    <name>Route</name>\n";
    foreach( const RouteElement &element, instructions ) {
        Q_ASSERT( element.type == Instruction );
        qreal lon = element.position.longitude( GeoDataCoordinates::Degree );
        qreal lat = element.position.latitude( GeoDataCoordinates::Degree );
        content += QString( "    <rtept lat=\"%1\" lon=\"%2\">%3</rtept>\n" ).arg( lat, 0, 'f', 7 ).arg( lon, 0, 'f', 7 ).arg( element.description );
    }

    content += "  </rte>\n";
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
