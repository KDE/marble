//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ROUTEANNOTATOR_H
#define MARBLE_ROUTEANNOTATOR_H

#include <GeoDataPlacemark.h>

#include <QtCore/QObject>

/**
  * Summarizes a route verbally, optionally based on another route (focusing on the differences)
  */
namespace Marble
{

class GeoDataDocument;
class AlternativeRoutesModel;
class RouteAnnotatorPrivate;
class MarbleModel;

class RouteAnnotator : public QObject
{
    Q_OBJECT

public:
    /**
      * Constructor
      * @param marbleModel Pointer to the currently used map model
      * @param model Pointer to the currently used model of alternative routes
      * @param route The route to be summarized
      * @param base Optional second route. If non-zero, the summary will focus on the differences between both routes
      */
    RouteAnnotator( MarbleModel* marbleModel, AlternativeRoutesModel* model, GeoDataDocument* route, const GeoDataDocument* base = 0 );

    ~RouteAnnotator();

    /**
      * Creates the route summary and stores it in the original route.
      * The finished signal is emitted when the method is done
      * @note: This method involves several reverse geocoding requests and can take some time to complete
      */
    void run();

Q_SIGNALS:
    /**
      * Emitted when the run() operation finished successfully.
      */
    void finished( GeoDataDocument* route );

private Q_SLOTS:
    void retrieveGeocodeResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );

private:
    RouteAnnotatorPrivate* const d;
};

} // namespace Marble

#endif
