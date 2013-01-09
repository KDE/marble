//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RouteAnnotator.h"

#include "AlternativeRoutesModel.h"
#include "MarbleRunnerManager.h"
#include "RoutingModel.h"
#include "MarbleModel.h"
#include "MarbleMath.h"
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"

#include <QtCore/QVector>

namespace Marble {

class RouteAnnotatorPrivate
{
public:
    MarbleModel* m_marbleModel;

    AlternativeRoutesModel* m_model;

    GeoDataDocument* m_route;

    const GeoDataDocument* m_base;

    MarbleRunnerManager m_manager;

    QVector<GeoDataPlacemark> m_placemarks;

    int m_requests;

    RouteAnnotatorPrivate( MarbleModel* marbleModel, AlternativeRoutesModel* model, GeoDataDocument* route, const GeoDataDocument* base );
};

RouteAnnotatorPrivate::RouteAnnotatorPrivate( MarbleModel* marbleModel, AlternativeRoutesModel* model, GeoDataDocument* route, const GeoDataDocument* base ) :
    m_marbleModel( marbleModel ),
    m_model( model ),
    m_route( route ),
    m_base( base ),
    m_manager( marbleModel->pluginManager() ),
    m_requests( 0 )
{
    // nothing to do
}

RouteAnnotator::RouteAnnotator( MarbleModel* marbleModel, AlternativeRoutesModel* model, GeoDataDocument* route, const GeoDataDocument* base ) :
        d( new RouteAnnotatorPrivate( marbleModel, model, route, base ) )
{
    connect( &d->m_manager, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark ) ),
             this, SLOT( retrieveGeocodeResult( GeoDataCoordinates, GeoDataPlacemark ) ) );
}

RouteAnnotator::~RouteAnnotator()
{
    delete d;
}

void RouteAnnotator::run()
{
    GeoDataLineString* points = AlternativeRoutesModel::waypoints( d->m_route );
    if (!d->m_base) {
        int half = points->size() / 2;
        for ( int i = points->size() / 4; i < points->size(); i += half )
        {
            ++d->m_requests;
            d->m_manager.reverseGeocoding( points->at( i ) );
        }
    } else {
        QVector<qreal> deviation = AlternativeRoutesModel::deviation( d->m_route, d->m_base );
        Q_ASSERT( points->size() == deviation.size() );
        bool deviated = false;
        qreal nearThreshold = 100.0 / EARTH_RADIUS;
        int maxStartOffset = 0;
        int maxEndOffset = 0;
        qreal maxDistance = 0.0;
        int startOffset = 0;
        int endOffset = 0;
        qreal distance = 0.0;
        for ( int i=1; i<deviation.size(); ++i ) {
            if ( deviation[i] > nearThreshold ) {
                distance += distanceSphere( points->at(i-1), points->at(i) );
                endOffset = i;
                if (!deviated) {
                    startOffset = i;
                    deviated = true;
                }
            } else {
                deviated = false;
                if (distance > maxDistance) {
                    maxDistance = distance;
                    maxStartOffset = startOffset;
                    maxEndOffset = endOffset;
                }
            }
        }

        int half = 1 + ( maxEndOffset-maxStartOffset ) / 2;
        for ( int i=maxStartOffset + ( maxEndOffset-maxStartOffset ) / 4; i<maxEndOffset; i += half ) {
            d->m_manager.reverseGeocoding( points->at( i ) );
            ++d->m_requests;
        }
    }
}

void RouteAnnotator::retrieveGeocodeResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark )
{
    Q_UNUSED( coordinates );
    d->m_placemarks.push_back( placemark );
    if ( d->m_placemarks.size() == d->m_requests ) {
        QMap<QString,int> counter;
        foreach( const GeoDataPlacemark &placemark, d->m_placemarks ) {
            GeoDataExtendedData extended = placemark.extendedData();
            ++counter[extended.value("road").value().toString()];
        }

        QMap<QString,int>::iterator itpoint = counter.begin();
        QMap<QString,int>::iterator const endpoint = counter.end();
        for (; itpoint != endpoint; ++itpoint ) {
            if ( counter[itpoint.key()] > 1 ) {
                GeoDataExtendedData extended = d->m_placemarks.first().extendedData();
                QString name = "Via %1";
                d->m_route->setName( name.arg( extended.value("road").value().toString() ) );
                emit finished( d->m_route );
                return;
            }
        }
    }
}

}

#include "RouteAnnotator.moc"
