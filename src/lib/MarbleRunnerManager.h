//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_MARBLERUNNERMANAGER_H
#define MARBLE_MARBLERUNNERMANAGER_H

#include "GeoDataCoordinates.h"
#include "GeoDataDocument.h"

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QMutex>

class QAbstractItemModel;

namespace Marble
{

class GeoDataPlacemark;
class MarbleModel;
class PluginManager;
class RouteRequest;
class RunnerTask;

class MarbleRunnerManagerPrivate;
class MARBLE_EXPORT MarbleRunnerManager : public QObject
{
    Q_OBJECT

public:
    /**
      * Constructor.
      * @param pluginManager The plugin manager that gives access to RunnerPlugins
      * @param parent Optional parent object
      */
    explicit MarbleRunnerManager( const PluginManager* pluginManager, QObject *parent = 0 );

    /** Destructor */
    ~MarbleRunnerManager();

    /**
      * Set a pointer to the map instance to be passed to MarbleAbstractRunner instances
      */
    void setModel( MarbleModel * model );

    /**
      * Search for placemarks matching the given search term. Results are returned
      * using the @see searchResultChanged and the @see searchFinished signals
      */
    void findPlacemarks( const QString& searchTerm );

    /**
      * Find the address and other meta information for a given geoposition.
      * The result is returned through the @see reverseGeocodingFinished signal
      */
    void reverseGeocoding( const GeoDataCoordinates &coordinates );

    /**
      * Download routes traversing the stopover points in the given route request
      * Each route found is returned through the @see routeRetrieved signal
      */
    void retrieveRoute( const RouteRequest *request );

    /**
      * Parse the file using the runners for various formats
      * The result is returned through the @see parsingFinished signal
      */
    void parseFile( const QString& fileName, DocumentRole role );

signals:
    /**
      * Placemarks were added to or removed from the model
      * @todo FIXME: this sounds like a duplication of QAbstractItemModel signals
      */
    void searchResultChanged( QAbstractItemModel *model );
    void searchResultChanged( QVector<GeoDataPlacemark*> result );

    /**
      * The search request for the given search term has finished, i.e. all
      * runners are finished and reported their results via the
      * @see searchResultChanged signal
      */
    void searchFinished( const QString &searchTerm );

    /**
      * The reverse geocoding request is finished, the result is stored
      * in the given placemark. This signal is emitted when the first
      * runner found a result, subsequent results are discarded and do not
      * emit further signals. If no result is found, this signal is emitted
      * with an empty (default constructed) placemark.
      */
    void reverseGeocodingFinished( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );

    /**
      * A route was retrieved
      */
    void routeRetrieved( GeoDataDocument* route );

    /**
      * The file was parsed and potential error message
      */
    void parsingFinished( GeoDataDocument* document, const QString& error = QString() );

    /** @todo: add signals that reverse geocoding and routing have finished
      * to be able to cope with misbehaving runners
      */

private:
    Q_PRIVATE_SLOT( d, void addSearchResult( QVector<GeoDataPlacemark*> result ) )

    Q_PRIVATE_SLOT( d, void addReverseGeocodingResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark ) )

    Q_PRIVATE_SLOT( d, void addRoutingResult( GeoDataDocument* route ) )

    Q_PRIVATE_SLOT( d, void addParsingResult( GeoDataDocument* document, const QString& error = QString() ) )

    Q_PRIVATE_SLOT( d, void cleanupSearchTask( RunnerTask* task ) )

    Q_PRIVATE_SLOT( d, void cleanupRoutingTask( RunnerTask* task ) )

    friend class MarbleRunnerManagerPrivate;

    MarbleRunnerManagerPrivate* const d;
};

}

#endif
