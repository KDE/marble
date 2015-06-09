//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>
// Copyright 2010 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2010-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_REVERSEGEOCODINGRUNNERMANAGER_H
#define MARBLE_REVERSEGEOCODINGRUNNERMANAGER_H

#include <QObject>

#include "marble_export.h"

class QAbstractItemModel;

namespace Marble
{

class GeoDataCoordinates;
class GeoDataPlacemark;
class MarbleModel;
class ReverseGeocodingTask;

class MARBLE_EXPORT ReverseGeocodingRunnerManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param pluginManager The plugin manager that gives access to RunnerPlugins
     * @param parent Optional parent object
     */
    explicit ReverseGeocodingRunnerManager( const MarbleModel *marbleModel, QObject *parent = 0 );

    ~ReverseGeocodingRunnerManager();

    /**
     * Find the address and other meta information for a given geoposition.
     * @see reverseGeocoding is asynchronous with currently one result
     * returned using the @see reverseGeocodingFinished signal.
     * @see searchReverseGeocoding is blocking.
     * @see reverseGeocodingFinished signal indicates all runners are finished.
     */
    void reverseGeocoding( const GeoDataCoordinates &coordinates );
    QString searchReverseGeocoding( const GeoDataCoordinates &coordinates, int timeout = 30000 );

Q_SIGNALS:
    /**
     * The reverse geocoding request is finished, the result is stored
     * in the given placemark. This signal is emitted when the first
     * runner found a result, subsequent results are discarded and do not
     * emit further signals. If no result is found, this signal is emitted
     * with an empty (default constructed) placemark.
     */
    void reverseGeocodingFinished( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );

    /**
     * Emitted whenever all runners are finished for the query
     */
    void reverseGeocodingFinished();

private:
    Q_PRIVATE_SLOT( d, void addReverseGeocodingResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark ) )
    Q_PRIVATE_SLOT( d, void cleanupReverseGeocodingTask( ReverseGeocodingTask *task ) )

    class Private;
    friend class Private;
    Private *const d;
};

}

#endif
