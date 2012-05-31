//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "AlternativeRoutesModel.h"

#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataExtendedData.h"
#include "MarbleMath.h"
#include "RoutingModel.h"
#include "RouteAnnotator.h"

#include <QtCore/QTimer>
#include <QtGui/QPainter>

namespace Marble {

class AlternativeRoutesModelPrivate
{
public:
    /** The currently shown alternative routes (model data) */
    QVector<GeoDataDocument*> m_routes;

    /** Pending route data (waiting for other results to come in) */
    QVector<GeoDataDocument*> m_restrainedRoutes;

    /** Counts the time between route request and first result */
    QTime m_responseTime;

    int m_currentIndex;

    AlternativeRoutesModelPrivate();

    /**
      * Returns true if there exists a route with high similarity to the given one
      */
    bool filter( const GeoDataDocument* document ) const;

    /**
      * Returns a similarity measure in the range of [0..1]. Two routes with a similarity of 0 can
      * be treated as totally different (e.g. different route requests), two routes with a similarity
      * of 1 are considered equal. Otherwise the routes overlap to an extent indicated by the
      * similarity value -- the higher, the more they do overlap.
      * @note: The direction of routes is important; reversed routes are not considered equal
      */
    static qreal similarity( const GeoDataDocument* routeA, const GeoDataDocument* routeB );

    /**
      * Returns the distance between the given polygon and the given point
      */
    static qreal distance( GeoDataLineString* wayPoints, const GeoDataCoordinates &position );

    /**
      * Returns the bearing of the great circle path defined by the coordinates one and two
      * Based on http://www.movable-type.co.uk/scripts/latlong.html
      */
    static qreal bearing( const GeoDataCoordinates &one, const GeoDataCoordinates &two );

    /**
      * Returns the distance between the given point and the line segment (not line) defined
      * by the two coordinates lineA and lineB
      */
    static qreal distance( const GeoDataCoordinates &satellite, const GeoDataCoordinates &lineA, const GeoDataCoordinates &lineB );

    /**
      * Returns the point reached when travelling the given distance from start with the given direction
      */
    static GeoDataCoordinates coordinates( const GeoDataCoordinates &start, qreal distance, qreal bearing );

    /**
      * Returns the similarity between routeA and routeB. This method is not symmetric, i.e. in
      * general unidirectionalSimilarity(a,b) != unidirectionalSimilarity(b,a)
      */
    static qreal unidirectionalSimilarity( const GeoDataDocument* routeA, const GeoDataDocument* routeB );

    /**
      * (Primitive) scoring for routes
      */
    static bool higherScore( const GeoDataDocument* one, const GeoDataDocument* two );

    /**
      * Returns true if the given route contains instructions (placemarks with turn instructions)
      */
    static qreal instructionScore( const GeoDataDocument* document );

    static GeoDataLineString* waypoints( const GeoDataDocument* document );

    static int nonZero( const QImage &image );

    static QPolygonF polygon( const GeoDataLineString* lineString, qreal x, qreal y, qreal sx, qreal sy );
};


AlternativeRoutesModelPrivate::AlternativeRoutesModelPrivate() :
        m_currentIndex( -1 )
{
    // nothing to do
}

int AlternativeRoutesModelPrivate::nonZero( const QImage &image )
{
  QRgb* destLine = 0;
  QRgb const black = qRgb( 0, 0, 0 );
  int count = 0;
  for ( int y = 0; y < image.height(); ++y ) {
      destLine = (QRgb*) image.scanLine( y );
      for ( int x = 0; x < image.width(); ++x ) {
          count += destLine[x] == black ? 0 : 1;
      }
  }
  return count;
}

QPolygonF AlternativeRoutesModelPrivate::polygon( const GeoDataLineString* lineString, qreal x, qreal y, qreal sx, qreal sy )
{
    QPolygonF poly;
    for ( int i=0; i<lineString->size(); ++i ) {
        poly << QPointF( qAbs( ( *lineString)[i].longitude() - x ) * sx,
                         qAbs( ( *lineString)[i].latitude()  - y ) * sy );
    }
    return poly;
}

bool AlternativeRoutesModelPrivate::filter( const GeoDataDocument* document ) const
{
    for ( int i=0; i<m_routes.size(); ++i ) {
        qreal similarity = AlternativeRoutesModelPrivate::similarity( document, m_routes.at( i ) );
        if ( similarity > 0.8 ) {
            return true;
        }
    }

    return false;
}

qreal AlternativeRoutesModelPrivate::similarity( const GeoDataDocument* routeA, const GeoDataDocument* routeB )
{
    return qMax<qreal>( unidirectionalSimilarity( routeA, routeB ),
                        unidirectionalSimilarity( routeB, routeA ) );
}

qreal AlternativeRoutesModelPrivate::distance( GeoDataLineString* wayPoints, const GeoDataCoordinates &position )
{
    Q_ASSERT( wayPoints && !wayPoints->isEmpty() );
    qreal minDistance = 0;
    for ( int i=1; i<wayPoints->size(); ++i ) {
        qreal dist = distance( position, wayPoints->at( i-1 ), wayPoints->at( i ) );
        if ( minDistance <= 0 || dist < minDistance ) {
            minDistance = dist;
        }
    }

    return minDistance;
}

qreal AlternativeRoutesModelPrivate::bearing( const GeoDataCoordinates &one, const GeoDataCoordinates &two )
{
    qreal delta = two.longitude() - one.longitude();
    qreal lat1 = one.latitude();
    qreal lat2 = two.latitude();
    return fmod( atan2( sin ( delta ) * cos ( lat2 ),
                 cos( lat1 ) * sin( lat2 ) - sin( lat1 ) * cos( lat2 ) * cos ( delta ) ), 2 * M_PI );
}

GeoDataCoordinates AlternativeRoutesModelPrivate::coordinates( const GeoDataCoordinates &start, qreal distance, qreal bearing )
{
    qreal lat1 = start.latitude();
    qreal lon1 = start.longitude();
    qreal lat2 = asin( sin( lat1 ) * cos( distance ) + cos( lat1 ) * sin( distance ) * cos( bearing ) );
    qreal lon2 = lon1 + atan2( sin( bearing ) * sin( distance ) * cos( lat1 ), cos( distance ) - sin( lat1 ) * sin( lat2 ) );
    return GeoDataCoordinates( lon2, lat2 );
}

qreal AlternativeRoutesModelPrivate::distance( const GeoDataCoordinates &satellite, const GeoDataCoordinates &lineA, const GeoDataCoordinates &lineB )
{
    qreal dist = distanceSphere( lineA, satellite );
    qreal bearA = bearing( lineA, satellite );
    qreal bearB = bearing( lineA, lineB );
    qreal result = asin( sin ( dist ) * sin( bearB - bearA ) );
    Q_ASSERT( qMax<qreal>( distanceSphere(satellite, lineA), distanceSphere(satellite, lineB) ) >= qAbs<qreal>(result) );

    result = acos( cos( dist ) / cos( result ) );
    /** @todo: This is a naive approach. Look into the maths. */
    qreal final = qMin<qreal>( distanceSphere( satellite, lineA ), distanceSphere( satellite, lineB ) );
    if ( result >= 0 && result <= distanceSphere( lineA, lineB ) ) {
        GeoDataCoordinates nearest = coordinates( lineA, result, bearB );
        return qMin<qreal>( final, distanceSphere( satellite, nearest ) );
    } else {
        return final;
    }
}

qreal AlternativeRoutesModelPrivate::unidirectionalSimilarity( const GeoDataDocument* routeA, const GeoDataDocument* routeB )
{
    GeoDataLineString* waypointsA = waypoints( routeA );
    GeoDataLineString* waypointsB = waypoints( routeB );
    if ( !waypointsA || !waypointsB )
    {
        return 0.0;
    }

    QImage image( 64, 64, QImage::Format_ARGB32_Premultiplied );
    image.fill( qRgb( 0, 0, 0 ) );
    GeoDataLatLonBox box = GeoDataLatLonBox::fromLineString( *waypointsA );
    box = box.united( GeoDataLatLonBox::fromLineString( *waypointsB ) );
    if ( !box.width() || !box.height() ) {
      return 0.0;
    }

    qreal const sw = image.width() / box.width();
    qreal const sh = image.height() / box.height();

    QPainter painter( &image );
    painter.setPen( QColor( Qt::white ) );

    painter.drawPoints( AlternativeRoutesModelPrivate::polygon( waypointsA, box.west(), box.north(), sw, sh ) );
    int const countA = AlternativeRoutesModelPrivate::nonZero( image );

    painter.drawPoints( AlternativeRoutesModelPrivate::polygon( waypointsB, box.west(), box.north(), sw, sh ) );
    int const countB = AlternativeRoutesModelPrivate::nonZero( image );
    Q_ASSERT( countA <= countB );
    return countB ? 1.0 - qreal( countB - countA ) / countB : 0;
}

bool AlternativeRoutesModelPrivate::higherScore( const GeoDataDocument* one, const GeoDataDocument* two )
{
    qreal instructionScoreA = instructionScore( one );
    qreal instructionScoreB = instructionScore( two );
    if ( instructionScoreA != instructionScoreB ) {
        return instructionScoreA > instructionScoreB;
    }

    qreal lengthA = waypoints( one )->length( EARTH_RADIUS );
    qreal lengthB = waypoints( two )->length( EARTH_RADIUS );

    return lengthA < lengthB;
}

qreal AlternativeRoutesModelPrivate::instructionScore( const GeoDataDocument* document )
{
    bool hasInstructions = false;

    QStringList blacklist = QStringList() << "" << "Route" << "Tessellated";
    QVector<GeoDataFolder*> folders = document->folderList();
    foreach( const GeoDataFolder *folder, folders ) {
        foreach( const GeoDataPlacemark *placemark, folder->placemarkList() ) {
            if ( !blacklist.contains( placemark->name() ) ) {
                hasInstructions = true;
                break;
            }
        }
    }

    foreach( const GeoDataPlacemark *placemark, document->placemarkList() ) {
        if ( !blacklist.contains( placemark->name() ) ) {
            hasInstructions = true;

            if ( placemark->extendedData().contains( "turnType" ) ) {
                return 1.0;
            }
        }
    }

    return hasInstructions ? 0.5 : 0.0;
}

GeoDataLineString* AlternativeRoutesModelPrivate::waypoints( const GeoDataDocument* document )
{
    QVector<GeoDataFolder*> folders = document->folderList();
    foreach( const GeoDataFolder *folder, folders ) {
        foreach( const GeoDataPlacemark *placemark, folder->placemarkList() ) {
            GeoDataGeometry* geometry = placemark->geometry();
            GeoDataLineString* lineString = dynamic_cast<GeoDataLineString*>( geometry );
            if ( lineString ) {
                return lineString;
            }
        }
    }

    foreach( const GeoDataPlacemark *placemark, document->placemarkList() ) {
        GeoDataGeometry* geometry = placemark->geometry();
        GeoDataLineString* lineString = dynamic_cast<GeoDataLineString*>( geometry );
        if ( lineString ) {
            return lineString;
        }
    }

    return 0;
}

AlternativeRoutesModel::AlternativeRoutesModel( QObject *parent ) :
        QAbstractListModel( parent ),
        d( new AlternativeRoutesModelPrivate() )
{
    // nothing to do
}

AlternativeRoutesModel::~AlternativeRoutesModel()
{
    delete d;
}

int AlternativeRoutesModel::rowCount ( const QModelIndex & ) const
{
    return d->m_routes.size();
}

QVariant AlternativeRoutesModel::headerData ( int, Qt::Orientation, int ) const
{
    return QVariant();
}

QVariant AlternativeRoutesModel::data ( const QModelIndex &index, int role ) const
{
    QVariant result;

    if ( role == Qt::DisplayRole && index.column() == 0 && index.row() >= 0 && index.row() < d->m_routes.size() ) {
        result = d->m_routes.at( index.row() )->name();
    }

    return result;
}

GeoDataDocument* AlternativeRoutesModel::route( int index )
{
    if ( index >= 0 && index < d->m_routes.size() ) {
        return d->m_routes.at(index);
    }

    return 0;
}

void AlternativeRoutesModel::newRequest( RouteRequest * )
{
    d->m_responseTime.start();
    d->m_currentIndex = -1;
    clear();
}

void AlternativeRoutesModel::addRestrainedRoutes()
{
    Q_ASSERT( d->m_routes.isEmpty() );
    qSort( d->m_restrainedRoutes.begin(), d->m_restrainedRoutes.end(), AlternativeRoutesModelPrivate::higherScore );

    foreach( GeoDataDocument* route, d->m_restrainedRoutes ) {
        if ( !d->filter( route ) ) {
            int affected = d->m_routes.size();
            beginInsertRows( QModelIndex(), affected, affected );
//            GeoDataDocument* base = d->m_routes.isEmpty() ? 0 : d->m_routes.first();
            d->m_routes.push_back( route );
            endInsertRows();
//            RouteAnnotator* task = new RouteAnnotator( d->m_marbleModel, this, route, base );
//            connect( task, SIGNAL( finished( GeoDataDocument*) ),
//                     this, SLOT( update(GeoDataDocument*) ) );
//            task->run();
        }
    }

    d->m_restrainedRoutes.clear();
    Q_ASSERT( !d->m_routes.isEmpty() );
    setCurrentRoute( 0 );
}

void AlternativeRoutesModel::addRoute( GeoDataDocument* document, WritePolicy policy )
{
    if ( policy == Instant ) {
        int affected = d->m_routes.size();
        beginInsertRows( QModelIndex(), affected, affected );
        d->m_routes.push_back( document );
        endInsertRows();
        return;
    }

    if ( d->m_routes.isEmpty() && d->m_restrainedRoutes.isEmpty() ) {
        // First
        int responseTime = d->m_responseTime.elapsed();
        d->m_restrainedRoutes.push_back( document );
        int timeout = qMin<int>( 500, qMax<int>( 50,  responseTime * 2 ) );
        QTimer::singleShot( timeout, this, SLOT( addRestrainedRoutes() ) );
        return;
    } else if ( d->m_routes.isEmpty() && !d->m_restrainedRoutes.isEmpty() ) {
        d->m_restrainedRoutes.push_back( document );
    } else {
        for ( int i=0; i<d->m_routes.size(); ++i ) {
            qreal similarity = AlternativeRoutesModelPrivate::similarity( document, d->m_routes.at( i ) );
            if ( similarity > 0.8 ) {
                if ( AlternativeRoutesModelPrivate::higherScore( document, d->m_routes.at( i ) ) ) {
                    d->m_routes[i] = document;
                    QModelIndex changed = index( i );
                    emit dataChanged( changed, changed );
                }

                return;
            }
        }

        Q_ASSERT( !d->m_routes.isEmpty() );
        int affected = d->m_routes.size();
        beginInsertRows( QModelIndex(), affected, affected );
        d->m_routes.push_back( document );
        endInsertRows();

//        RouteAnnotator* task = new RouteAnnotator( d->m_marbleModel, this, document, d->m_routes.first() );
//        connect( task, SIGNAL( finished( GeoDataDocument*) ),
//                 this, SLOT( update(GeoDataDocument*) ) );
//        task->run();
    }
}

qreal AlternativeRoutesModel::distance( const GeoDataCoordinates &satellite, const GeoDataCoordinates &lineA, const GeoDataCoordinates &lineB )
{
    return AlternativeRoutesModelPrivate::distance( satellite, lineA, lineB );
}

QVector<qreal> AlternativeRoutesModel::deviation( const GeoDataDocument* routeA, const GeoDataDocument* routeB )
{
    GeoDataLineString* waypointsA = waypoints( routeA );
    GeoDataLineString* waypointsB = waypoints( routeB );
    QVector<qreal> result;
    for ( int a=0; a<waypointsA->size(); ++a ) {
        result.push_back( AlternativeRoutesModelPrivate::distance( waypointsB, waypointsA->at( a ) ) );
    }
    return result;
}

void AlternativeRoutesModel::update( GeoDataDocument* route )
{
    for ( int i=0; i<d->m_routes.size(); ++i ) {
        if ( d->m_routes[i] == route ) {
            emit dataChanged( index( i), index( i ) );
        }
    }
}

GeoDataLineString* AlternativeRoutesModel::waypoints( const GeoDataDocument* document )
{
    return AlternativeRoutesModelPrivate::waypoints( document );
}

void AlternativeRoutesModel::setCurrentRoute( int index )
{
    if ( index >= 0 && index < rowCount() && d->m_currentIndex != index ) {
        d->m_currentIndex = index;
        emit currentRouteChanged( currentRoute() );
    }
}

GeoDataDocument * AlternativeRoutesModel::currentRoute()
{
    GeoDataDocument* result = 0;
    if ( d->m_currentIndex >= 0 && d->m_currentIndex < rowCount() ) {
        result = d->m_routes[d->m_currentIndex];
    }

    return result;
}

void AlternativeRoutesModel::clear()
{
    QVector<GeoDataDocument*> routes = d->m_routes;
    d->m_currentIndex = -1;
    d->m_routes.clear();
    reset();
    qDeleteAll(routes);
}

} // namespace Marble

#include "AlternativeRoutesModel.moc"
