//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2013      Roman Karlstetter <roman.karlstetter@googlemail.com>
//
#include "ElevationProfileDataSource.h"

#include "ElevationModel.h"

#include "GeoDataDocument.h"
#include "GeoDataLineString.h"
#include "GeoDataObject.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTrack.h"
#include "GeoDataTreeModel.h"
#include "MarbleDebug.h"
#include "MarbleMath.h"
#include "MarbleModel.h"
#include "routing/Route.h"
#include "routing/RoutingModel.h"

#include <QFileInfo>

namespace Marble
{

ElevationProfileDataSource::ElevationProfileDataSource( QObject *parent ) :
    QObject( parent )
{
    // nothing to do
}

QVector<QPointF> ElevationProfileDataSource::calculateElevationData(const GeoDataLineString &lineString) const
{
    // TODO: Don't re-calculate the whole route if only a small part of it was changed
    QVector<QPointF> result;
    qreal distance = 0;

    //GeoDataLineString path;
    for ( int i = 0; i < lineString.size(); i++ ) {
        const qreal ele = getElevation( lineString[i] );

        if ( i ) {
            distance += EARTH_RADIUS * distanceSphere( lineString[i-1], lineString[i] );
        }

        if ( ele != invalidElevationData ) { // skip no data
            result.append( QPointF( distance, ele ) );
        }
    }

    return result;
}
// end of impl of ElevationProfileDataSource

ElevationProfileTrackDataSource::ElevationProfileTrackDataSource( const GeoDataTreeModel *treeModel, QObject *parent ) :
    ElevationProfileDataSource( parent ),
    m_currentSourceIndex( -1 )
{
    if ( treeModel ) {
        connect( treeModel, SIGNAL(added(GeoDataObject*)), SLOT(handleObjectAdded(GeoDataObject*)) );
        connect( treeModel, SIGNAL(removed(GeoDataObject*)), SLOT(handleObjectRemoved(GeoDataObject*)) );
    }
}

QStringList ElevationProfileTrackDataSource::sourceDescriptions() const
{
    return m_trackChooserList;
}

void ElevationProfileTrackDataSource::setSourceIndex(int index)
{
    if (m_currentSourceIndex != index) {
        m_currentSourceIndex = index;
        requestUpdate();
    }
}

int ElevationProfileTrackDataSource::currentSourceIndex() const
{
    return m_currentSourceIndex;
}

void ElevationProfileTrackDataSource::requestUpdate()
{
    if ( m_currentSourceIndex < 0 ) {
        return;
    }

    if ( m_currentSourceIndex >= m_trackList.size() ) {
        return;
    }

    const GeoDataLineString *routePoints = m_trackList[m_currentSourceIndex]->lineString();

    emit dataUpdated(*routePoints, calculateElevationData(*routePoints));
}

bool ElevationProfileTrackDataSource::isDataAvailable() const
{
    return !m_trackHash.isEmpty();
}

qreal ElevationProfileTrackDataSource::getElevation(const GeoDataCoordinates &coordinates) const
{
    return coordinates.altitude();
}

void ElevationProfileTrackDataSource::handleObjectAdded(GeoDataObject *object)
{
    const GeoDataDocument *document = dynamic_cast<const GeoDataDocument *>(object);
    if (!document) {
        return;// don't know what to do if not a document
    }
    QList<const GeoDataTrack *> trackList;

    for (int i = 0; i<document->size(); ++i) {
        const GeoDataFeature *feature = document->child(i);
        const GeoDataPlacemark *placemark = dynamic_cast<const GeoDataPlacemark*>(feature);
        if (!placemark) {
            continue;
        }
        const GeoDataMultiGeometry *multiGeometry = dynamic_cast<const GeoDataMultiGeometry *>(placemark->geometry());
        if (!multiGeometry) {
            continue;
        }
        for (int i = 0; i<multiGeometry->size(); i++) {
            const GeoDataTrack *track = dynamic_cast<const GeoDataTrack *>(multiGeometry->child(i));
            if (track && track->size() > 1) {
                mDebug() << "new GeoDataTrack for ElevationProfile detected";
                trackList.append(track);
            }
        }
    }

    if (trackList.isEmpty()) {
        return;
    }

    // update internal datastructures
    m_trackHash.insert(document->fileName(), trackList);

    const GeoDataTrack *selectedTrack = 0;
    if ( 0 <= m_currentSourceIndex && m_currentSourceIndex < m_trackList.size() ) {
        selectedTrack = m_trackList[m_currentSourceIndex];
    }

    m_trackChooserList.clear();
    m_trackList.clear();
    QHashIterator<QString, QList<const GeoDataTrack *> > i(m_trackHash);
    while (i.hasNext()) {
        i.next();
        mDebug() << i.key() << ": " << i.value() << endl;
        QFileInfo info(i.key());
        QString filename = info.fileName();
        QList<const GeoDataTrack *> list = i.value();
        for (int i = 0; i<list.size(); ++i) {
            m_trackList << list[i];
            m_trackChooserList << QString(filename + QLatin1String(": ") + QString::number(i));
        }
    }
    if (selectedTrack) {
        m_currentSourceIndex = m_trackList.indexOf(selectedTrack);
    }

    emit sourceCountChanged();
}

void ElevationProfileTrackDataSource::handleObjectRemoved(GeoDataObject *object)
{
    if (m_trackList.size() == 0) {
        // no track loaded, nothing to remove
        return;
    }

    const GeoDataDocument *topLevelDoc = dynamic_cast<const GeoDataDocument*>(object);
    if (!topLevelDoc) {
        return;// don't know what to do if not a document
    }

    const QString key = topLevelDoc->fileName();
    if ( !m_trackHash.contains( key ) ) {
        return;
    }

    const QList<const GeoDataTrack *> list = m_trackHash.value(key);
    const GeoDataTrack *const selectedTrack = m_currentSourceIndex == -1 ? 0 : m_trackList[m_currentSourceIndex];
    for (int i = 0; i<list.size(); i++) {
        int idx = m_trackList.indexOf(list[i]);
        m_trackList.removeAt(idx);
        m_trackChooserList.removeAt(idx);
    }
    m_trackHash.remove(key);

    m_currentSourceIndex = m_trackList.indexOf(selectedTrack);
    if (m_currentSourceIndex == -1) {
        m_currentSourceIndex = 0;
    }

    emit sourceCountChanged();
    requestUpdate();
}

// end of impl of ElevationProfileTrackDataSource

ElevationProfileRouteDataSource::ElevationProfileRouteDataSource( const RoutingModel *routingModel, const ElevationModel *elevationModel, QObject *parent ) :
    ElevationProfileDataSource( parent ),
    m_routingModel( routingModel ),
    m_elevationModel( elevationModel ),
    m_routeAvailable( false )
{
}

void ElevationProfileRouteDataSource::requestUpdate()
{
    if (m_routeAvailable != isDataAvailable()) {
        // availability of route changed
        emit sourceCountChanged();
        m_routeAvailable = isDataAvailable();
    }

    const GeoDataLineString routePoints = m_routingModel->route().path();
    const QVector<QPointF> elevationData = calculateElevationData(routePoints);
    emit dataUpdated( routePoints, elevationData );
}

bool ElevationProfileRouteDataSource::isDataAvailable() const
{
    return m_routingModel && m_routingModel->rowCount() > 0;
}

qreal ElevationProfileRouteDataSource::getElevation(const GeoDataCoordinates &coordinates) const
{
    const qreal lat = coordinates.latitude ( GeoDataCoordinates::Degree );
    const qreal lon = coordinates.longitude( GeoDataCoordinates::Degree );
    qreal ele = m_elevationModel->height( lon, lat );
    return ele;
}
// end of impl of ElevationProfileRouteDataSource

}

#include "moc_ElevationProfileDataSource.cpp"

