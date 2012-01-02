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
      * Search for placemarks matching the given search term.
      * @see findPlacemark is asynchronous with results returned using the
      * @see searchResultChanged signal.
      * @see searchPlacemark is blocking.
      * @see searchFinished signal indicates all runners are finished.
      */
    void findPlacemarks( const QString& searchTerm );
    QVector<GeoDataPlacemark*> searchPlacemarks( const QString& searchTerm );

    /**
      * Find the address and other meta information for a given geoposition.
      * @see reverseGeocoding is asynchronous with currently one result
      * returned using the @see reverseGeocodingFinished signal.
      * @see searchReverseGeocoding is blocking.
      * @see reverseGeocodingFinished signal indicates all runners are finished.
      */
    void reverseGeocoding( const GeoDataCoordinates &coordinates );
    QString searchReverseGeocoding( const GeoDataCoordinates &coordinates );

    /**
      * Download routes traversing the stopover points in the given route request
      * @see retrieveRoute is asynchronous with results returned using the
      * @see routeRetrieved signal.
      * @see searchRoute is blocking.
      * @see routingFinished signal indicates all runners are finished.
      */
    void retrieveRoute( const RouteRequest *request );
    QVector<GeoDataDocument*> searchRoute( const RouteRequest *request );

    /**
      * Parse the file using the runners for various formats
      * @see parseFile is asynchronous with results returned using the
      * @see parsingFinished signal.
      * @see openFile is blocking.
      * @see parsingFinished signal indicates all runners are finished.
      */
    void parseFile( const QString& fileName, DocumentRole role = UserDocument );
    GeoDataDocument* openFile( const QString& fileName, DocumentRole role = UserDocument );

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

    /** signal emitted whenever all runners are finished for the query
      */
    void placemarkSearchFinished();

    /**
      * The reverse geocoding request is finished, the result is stored
      * in the given placemark. This signal is emitted when the first
      * runner found a result, subsequent results are discarded and do not
      * emit further signals. If no result is found, this signal is emitted
      * with an empty (default constructed) placemark.
      */
    void reverseGeocodingFinished( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );

    /** signal emitted whenever all runners are finished for the query
      */
    void reverseGeocodingFinished();

    /**
      * A route was retrieved
      */
    void routeRetrieved( GeoDataDocument* route );

    /** signal emitted whenever all runners are finished for the query
      */
    void routingFinished();

    /**
      * The file was parsed and potential error message
      */
    void parsingFinished( GeoDataDocument* document, const QString& error = QString() );

    /** signal emitted whenever all runners are finished for the query
      */
    void parsingFinished();


private:
    Q_PRIVATE_SLOT( d, void addSearchResult( QVector<GeoDataPlacemark*> result ) )
    Q_PRIVATE_SLOT( d, void addReverseGeocodingResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark ) )
    Q_PRIVATE_SLOT( d, void addRoutingResult( GeoDataDocument* route ) )
    Q_PRIVATE_SLOT( d, void addParsingResult( GeoDataDocument* document, const QString& error = QString() ) )

    Q_PRIVATE_SLOT( d, void cleanupSearchTask( RunnerTask* task ) )
    Q_PRIVATE_SLOT( d, void cleanupReverseGeocodingTask( RunnerTask* task ) )
    Q_PRIVATE_SLOT( d, void cleanupRoutingTask( RunnerTask* task ) )
    Q_PRIVATE_SLOT( d, void cleanupParsingTask( RunnerTask* task ) )

    friend class MarbleRunnerManagerPrivate;

    MarbleRunnerManagerPrivate* const d;
};

}

#endif
