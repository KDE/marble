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
#include "GeoDataTreeModel.h"
#include "GeoDataObject.h"
#include "GeoDataPlacemark.h"
#include "MarbleDebug.h"
#include "MarbleMath.h"
#include "routing/Route.h"
#include "routing/RoutingModel.h"
#include "routing/RoutingManager.h"
#include <QFileInfo>

namespace Marble
{

/*
 * ElevationProfileDataSource
 */
MarbleModel *ElevationProfileDataSource::marbleModel() const
{
    return m_marbleModel;
}

ElevationProfileDataSource::ElevationProfileDataSource(MarbleModel *marbleModel, QObject *parent):
    QObject(parent),
    m_marbleModel(marbleModel)
{
    // nothing to do
}

QList<QPointF> ElevationProfileDataSource::calculateElevationData( const GeoDataLineString &lineString ) const
{
    // TODO: Don't re-calculate the whole route if only a small part of it was changed
    QList<QPointF> result;
    qreal distance = 0;

    //GeoDataLineString path;
    for ( int i = 0; i < lineString.size(); i++ ) {
        qreal ele = getElevation( lineString[i] );

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

/*
 * ElevationProfileTrackDataSource
 */
ElevationProfileTrackDataSource::ElevationProfileTrackDataSource(MarbleModel *marbleModel, QObject *parent):
    ElevationProfileDataSource(marbleModel, parent)
{
    connect( marbleModel->treeModel(), SIGNAL(added(GeoDataObject*)), this, SLOT(handleObjectAdded(GeoDataObject*)) );
    connect( marbleModel->treeModel(), SIGNAL(removed(GeoDataObject*)), this, SLOT(handleObjectRemoved(GeoDataObject*)) );
}

QStringList ElevationProfileTrackDataSource::sourceDescriptions() const
{
    return m_trackChooserList;
}

void ElevationProfileTrackDataSource::setSourceIndex(int index)
{
    if(m_currentSourceIndex != index){
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
    const GeoDataLineString *routePoints = m_trackList[m_currentSourceIndex]->lineString();

    emit dataUpdated(*routePoints, calculateElevationData(*routePoints));
}

bool ElevationProfileTrackDataSource::isDataAvailable() const
{
    return !m_trackHash.isEmpty();
}

qreal ElevationProfileTrackDataSource::getElevation(GeoDataCoordinates coord) const
{
    return coord.altitude();
}

void ElevationProfileTrackDataSource::handleObjectAdded(GeoDataObject *obj)
{
    GeoDataDocument *doc = dynamic_cast<GeoDataDocument*>(obj);
    if(!doc){
        return;// don't know what to do if not a document
    }
    QList<const GeoDataTrack *> trackList;

    for(int i = 0; i<doc->size(); ++i){
        const GeoDataFeature * c = doc->child(i);
        const GeoDataPlacemark *placemark = dynamic_cast<const GeoDataPlacemark*>(c);
        if(!placemark) {
            continue;
        }
        const GeoDataMultiGeometry *multi = dynamic_cast<const GeoDataMultiGeometry *>(placemark->geometry());
        if(!multi){
            continue;
        }
        for(int i = 0; i<multi->size(); i++){
            const GeoDataTrack *track = dynamic_cast<const GeoDataTrack *>(multi->child(i));
            if (track){
                mDebug() << "new GeoDataTrack for ElevationProfile detected";
                trackList.append(track);
            }
        }
    }

    if(trackList.isEmpty()){
        return;
    }

    // update internal datastructures
    m_trackHash.insert(doc->fileName(), trackList);

    const GeoDataTrack *selectedTrack = 0;
    if(m_currentSourceIndex < m_trackList.size()){
        selectedTrack = m_trackList[m_currentSourceIndex];
    }

    m_trackChooserList.clear();
    m_trackList.clear();
    QHashIterator<QString,   QList<const GeoDataTrack *> > i(m_trackHash);
    while (i.hasNext()) {
        i.next();
        mDebug() << i.key() << ": " << i.value() << endl;
        QFileInfo info(i.key());
        QString filename = info.fileName();
        QList<const GeoDataTrack *> list = i.value();
        for(int i = 0; i<list.size(); ++i){
            m_trackList << list[i];
            m_trackChooserList << QString(filename + ": " + QString::number(i));
        }
    }
    if(selectedTrack){
        m_currentSourceIndex = m_trackList.indexOf(selectedTrack);
    }

    emit sourceCountChanged();
}

void ElevationProfileTrackDataSource::handleObjectRemoved(GeoDataObject *obj)
{
    if(m_trackList.size() == 0){
        // no track loaded, nothing to remove
        return;
    }
    GeoDataDocument *topLevelDoc = dynamic_cast<GeoDataDocument*>(obj);
    if(!topLevelDoc){
        return;// don't know what to do if not a document
    }

    QString key = topLevelDoc->fileName();
    QList<const GeoDataTrack *> list = m_trackHash.value(key);
    const GeoDataTrack *selectedTrack = m_trackList[m_currentSourceIndex];
    for(int i = 0; i<list.size(); i++){
        int idx = m_trackList.indexOf(list[i]);
        m_trackList.removeAt(idx);
        m_trackChooserList.removeAt(idx);
    }
    m_trackHash.remove(key);

    m_currentSourceIndex = m_trackList.indexOf(selectedTrack);
    if(m_currentSourceIndex == -1){
        m_currentSourceIndex = 0;
    }
    emit sourceCountChanged();
    requestUpdate();
}

// end of impl of ElevationProfileTrackDataSource

 /*
  * ElevationProfileRouteDataSource
  */
ElevationProfileRouteDataSource::ElevationProfileRouteDataSource(MarbleModel *marbleModel, QObject *parent):
    ElevationProfileDataSource(marbleModel, parent)
{
    connect( this->marbleModel()->elevationModel(), SIGNAL(updateAvailable()), SLOT(requestUpdate()) );

    m_routingModel = this->marbleModel()->routingManager()->routingModel();
    connect( m_routingModel, SIGNAL(currentRouteChanged()), this, SLOT(requestUpdate()) );
    m_routeAvailable = isDataAvailable();
}

void ElevationProfileRouteDataSource::requestUpdate()
{
    if(m_routeAvailable != isDataAvailable()){
        // availability of route changed
        emit sourceCountChanged();
        m_routeAvailable = isDataAvailable();
    }
    if(!isDataAvailable()){
        emit dataUpdated(GeoDataLineString(), QList<QPointF>());
    }
    GeoDataLineString routePoints = m_routingModel->route().path();

    emit dataUpdated(routePoints, calculateElevationData(routePoints));
}

bool ElevationProfileRouteDataSource::isDataAvailable() const
{
    return m_routingModel && m_routingModel->rowCount() > 0;
}

qreal ElevationProfileRouteDataSource::getElevation(GeoDataCoordinates coord) const
{
    const qreal lat = coord.latitude ( GeoDataCoordinates::Degree );
    const qreal lon = coord.longitude( GeoDataCoordinates::Degree );
    qreal ele = marbleModel()->elevationModel()->height( lon, lat );
    if ( ele == invalidElevationData ) { // no data
        ele = 0;
    }
    return ele;
}
// end of impl of ElevationProfileRouteDataSource

}

