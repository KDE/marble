//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2013 Roman Karlstetter <roman.karlstetter@googlemail.com>
//

#ifndef ELEVATIONPROFILEDATASOURCE_H
#define ELEVATIONPROFILEDATASOURCE_H

#include <QObject>

#include <QHash>
#include <QList>
#include <QPointF>
#include <QStringList>

namespace Marble
{

class ElevationModel;
class GeoDataCoordinates;
class GeoDataLineString;
class GeoDataObject;
class GeoDataTrack;
class GeoDataTreeModel;
class RoutingModel;

class ElevationProfileDataSource : public QObject
{
    Q_OBJECT

public:
    explicit ElevationProfileDataSource( QObject *parent = nullptr );

    /**
     * @brief isDataAvailable
     * @return true if data is available to display
     */
    virtual bool isDataAvailable() const = 0;

public Q_SLOTS:
    virtual void requestUpdate() = 0;

Q_SIGNALS:
    void sourceCountChanged();
    void dataUpdated(const GeoDataLineString &points, const QVector<QPointF> &elevationData);

protected:
    QVector<QPointF> calculateElevationData(const GeoDataLineString &lineString) const;
    virtual qreal getElevation(const GeoDataCoordinates &coordinates) const = 0;
};

/**
 * @brief The ElevationProfileTrackDataSource provides elevation profile of GeoDataTrack Objects in the marblemodel
 */
class ElevationProfileTrackDataSource : public ElevationProfileDataSource
{
    Q_OBJECT

public:
    explicit ElevationProfileTrackDataSource( const GeoDataTreeModel *treeModel, QObject *parent = nullptr );

    bool isDataAvailable() const override;

    QStringList sourceDescriptions() const;

    void setSourceIndex(int index);

    int currentSourceIndex() const;

public Q_SLOTS:
    void requestUpdate() override;

protected:
    qreal getElevation(const GeoDataCoordinates &coordinates) const override;

private Q_SLOTS:
    void handleObjectAdded( GeoDataObject *object );
    void handleObjectRemoved( GeoDataObject *object );

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
    ElevationProfileRouteDataSource( const RoutingModel *routingModel, const ElevationModel *elevationModel, QObject *parent = nullptr );

    bool isDataAvailable() const override;

public Q_SLOTS:
    void requestUpdate() override;

protected:
    qreal getElevation(const GeoDataCoordinates &coordinates) const override;

private:
    const RoutingModel *const m_routingModel;
    const ElevationModel *const m_elevationModel;
    bool m_routeAvailable; // save state if route is available to notify FloatItem when this changes
};

}

#endif // ELEVATIONPROFILEDATASOURCE_H
