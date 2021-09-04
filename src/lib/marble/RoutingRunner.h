// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2012, 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#ifndef MARBLE_ROUTINGRUNNER_H
#define MARBLE_ROUTINGRUNNER_H

#include <QObject>
#include "marble_export.h"

class QTime;

namespace Marble
{

class GeoDataExtendedData;
class GeoDataDocument;
class RouteRequest;

class MARBLE_EXPORT RoutingRunner : public QObject
{
    Q_OBJECT

public:
    explicit RoutingRunner( QObject *parent );

    /**
     * Start a route download orw calculation. Called by MarbleRunnerManager, runners
     * are expected to return the result via the routeCalculated signal.
     * If implemented in a plugin, make sure to include Routing in the
     * plugin capabilities, otherwise MarbleRunnerManager will ignore the plugin
     */
    virtual void retrieveRoute( const RouteRequest *request ) = 0;

Q_SIGNALS:
    /**
     * Route download/calculation is finished, result in the given route object.
     * To be emitted by runners after a @see retrieveRoute call.
     */
    void routeCalculated( GeoDataDocument* route );

protected:
    const QString nameString( const QString &name, qreal length, const QTime &duration ) const;
    const QString lengthString( qreal length ) const;
    const QString durationString( const QTime &duration ) const;
    const GeoDataExtendedData routeData( qreal length, const QTime &duration ) const;
};

}

#endif
