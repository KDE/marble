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

#ifndef MARBLE_MARBLEABSTRACTRUNNER_H
#define MARBLE_MARBLEABSTRACTRUNNER_H

#include "marble_export.h"
#include "GeoDataFeature.h"
#include "GeoDataDocument.h"

#include <QtCore/QVector>

namespace Marble
{

class GeoDataCoordinates;
class GeoDataLatLonAltBox;
class GeoDataPlacemark;
class MarbleModel;
class RouteRequest;

/**
 * This class is the base class for all Marble Runners.
 */
class MARBLE_EXPORT MarbleAbstractRunner : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor; note that @p parent should be 0 in order to move the
     * thread object into its own thread.
     */
    explicit MarbleAbstractRunner( QObject *parent = 0 );

    /**
      * Stores a pointer to the currently used map
      */
    void setModel( MarbleModel * model );

    /**
     * This function gives the  icon for this runner
     * @return the icon of the runner
     */
    virtual GeoDataFeature::GeoDataVisualCategory category() const;

    /**
      * Start a reverse geocoding request. Called by MarbleRunnerManager, runners
      * are expected to return the result via the reverseGeocodingFinished signal.
      * If implemented in a plugin, make sure to include ReverseGeocoding in the
      * plugin capabilities, otherwise MarbleRunnerManager will ignore the plugin
      */
    virtual void reverseGeocoding( const GeoDataCoordinates &coordinates );

    /**
      * Start a route download or calculation. Called by MarbleRunnerManager, runners
      * are expected to return the result via the routeCalculated signal.
      * If implemented in a plugin, make sure to include Routing in the
      * plugin capabilities, otherwise MarbleRunnerManager will ignore the plugin
      */
    virtual void retrieveRoute( const RouteRequest *request );

    /**
      * Start a placemark search. Called by MarbleRunnerManager, runners
      * are expected to return the result via the searchFinished signal.
      * If implemented in a plugin, make sure to include Search in the
      * plugin capabilities, otherwise MarbleRunnerManager will ignore the plugin
      */
    virtual void search( const QString &searchTerm, const GeoDataLatLonAltBox &preferred );

    /**
      * Start a file parsing. Called by MarbleRunnerManager, runners
      * are expected to return the result via the parsingFinished signal.
      * If implemented in a plugin, make sure to include Parsing in the
      * plugin capabilities, otherwise MarbleRunnerManager will ignore the plugin
      */
    virtual void parseFile( const QString &fileName, DocumentRole role );

Q_SIGNALS:

    /**
     * This is emitted to indicate that the runner has finished the placemark search.
     * @param result the result of the search.
     */
    void searchFinished( QVector<GeoDataPlacemark*> result );

    /**
      * Reverse geocoding is finished, result in the given placemark.
      * To be emitted by runners after a @see reverseGeocoding call.
      */
    void reverseGeocodingFinished( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );

    /**
      * Route download/calculation is finished, result in the given route object.
      * To be emitted by runners after a @see retrieveRoute call.
      */
    void routeCalculated( GeoDataDocument* route );

    /**
      * File parsing is finished, result in the given document object.
      * The signal should be emitted with null document and error description in case of fault.
      * To be emitted by runners after a @see parseFile call.
      */
    void parsingFinished( GeoDataDocument* document, const QString& error = QString() );

protected:
    /**
      * Access to the currently used map, or null if no was set with @see setMap
      */
    MarbleModel * model();

private:
    MarbleModel *m_model;
};

}

#endif
