//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ABSTRACTROUTINGPROVIDER_H
#define MARBLE_ABSTRACTROUTINGPROVIDER_H

#include "GeoDataLineString.h"

#include <QtCore/QByteArray>
#include <QtCore/QObject>

namespace Marble
{

class RouteSkeleton;

class AbstractRoutingProvider : public QObject
{
    Q_OBJECT

public:
    enum Format {
        OpenGIS,
        GPX,
        KML
    };

    /** Constructor */
    AbstractRoutingProvider( QObject *parent );

    /** Destructor */
    virtual ~AbstractRoutingProvider();

    /**
      * Retrieve routing directions. Upon success, the routeRetrieved signal should
      * be emitted.
      * @param route Set of points to include in the route. Size will be at least two,
      * first one is to be interpreted as source, last as destination, others (possibly
      * empty) as stopover points.
      */
    virtual void retrieveDirections( RouteSkeleton *route ) = 0;

Q_SIGNALS:
    void routeRetrieved( AbstractRoutingProvider::Format format, const QByteArray &data );
};

} // namespace Marble

Q_DECLARE_METATYPE( Marble::AbstractRoutingProvider::Format )

#endif
