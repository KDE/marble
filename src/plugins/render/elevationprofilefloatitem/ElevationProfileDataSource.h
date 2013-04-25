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

#ifndef ELEVATIONPROFILEDATASOURCE_H
#define ELEVATIONPROFILEDATASOURCE_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QPoint>

#include "GeoDataLineString.h"
#include "GeoDataTrack.h"
#include "MarbleModel.h"

namespace Marble
{

class RoutingModel;

class ElevationProfileDataSource : public QObject
{
    Q_OBJECT
signals:
    void dataUpdated(const GeoDataLineString &points, QList<QPointF> elevationData);
public slots:
    virtual void requestUpdate() = 0;
public:
    ElevationProfileDataSource( MarbleModel *marbleModel, QObject *parent = 0);

    /**
     * @brief isDataAvailable
     * @return true if data is available to display
     */
    virtual bool isDataAvailable() const = 0;
signals:
    void sourceCountChanged();
protected:
    QList<QPointF> calculateElevationData(const GeoDataLineString &lineString) const;
    virtual qreal getElevation(GeoDataCoordinates coord) const = 0;

    MarbleModel *marbleModel() const;
private:
    MarbleModel *m_marbleModel;
};

/**
 * @brief The ElevationProfileTrackDataSource provides elevation profile of GeoDataTrack Objects in the marblemodel
 */
class ElevationProfileTrackDataSource : public ElevationProfileDataSource
{
    Q_OBJECT
public:
    ElevationProfileTrackDataSource ( MarbleModel* marbleModel, QObject *parent = 0);
    QStringList sourceDescriptions() const;
    void setSourceIndex(int index);
    int currentSourceIndex() const;

    virtual void requestUpdate();
    virtual bool isDataAvailable() const;
protected:
    virtual qreal getElevation(GeoDataCoordinates coord) const;

private slots:
    void handleObjectAdded(GeoDataObject *obj);
    void handleObjectRemoved(GeoDataObject *obj);

private:

    QHash<QString, QList<const GeoDataTrack *> > m_trackHash;
    QStringList m_trackChooserList;
    QList<const GeoDataTrack *> m_trackList;
    int m_currentSourceIndex;
};

/**
 * @brief The ElevationProfileRouteDataSource provides elevation profile of the current route
 */
class ElevationProfileRouteDataSource : public ElevationProfileDataSource
{
    Q_OBJECT
public:
    ElevationProfileRouteDataSource (MarbleModel* marbleMdl, QObject *parent = 0);
public slots:
    virtual void requestUpdate();
    virtual bool isDataAvailable() const;
protected:
    virtual qreal getElevation(GeoDataCoordinates coord) const;

private slots:
private:
    const RoutingModel* m_routingModel;
    bool m_routeAvailable; // save state if route is available to notify FloatItem when this changes


};
}

#endif // ELEVATIONPROFILEDATASOURCE_H
