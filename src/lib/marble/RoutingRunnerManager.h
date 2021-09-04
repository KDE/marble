// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Henry de Valence <hdevalence@gmail.com>
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2010-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_ROUTINGRUNNERMANAGER_H
#define MARBLE_ROUTINGRUNNERMANAGER_H

#include <QObject>
#include <QVector>

#include "marble_export.h"

namespace Marble
{

class GeoDataDocument;
class MarbleModel;
class RouteRequest;
class RoutingTask;

class MARBLE_EXPORT RoutingRunnerManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param pluginManager The plugin manager that gives access to RunnerPlugins
     * @param parent Optional parent object
     */
    explicit RoutingRunnerManager( const MarbleModel *marbleModel, QObject *parent = nullptr );

    ~RoutingRunnerManager() override;

    /**
     * Download routes traversing the stopover points in the given route request
     * @see retrieveRoute is asynchronous with results returned using the
     * @see routeRetrieved signal.
     * @see searchRoute is blocking.
     * @see routingFinished signal indicates all runners are finished.
     */
    void retrieveRoute( const RouteRequest *request );
    QVector<GeoDataDocument *> searchRoute( const RouteRequest *request, int timeout = 30000 );

Q_SIGNALS:
    /**
     * A route was retrieved
     */
    void routeRetrieved( GeoDataDocument *route );

    /**
     * Emitted whenever all runners are finished for the query
     */
    void routingFinished();

private:
    Q_PRIVATE_SLOT( d, void addRoutingResult( GeoDataDocument *route ) )
    Q_PRIVATE_SLOT( d, void cleanupRoutingTask( RoutingTask *task ) )

    class Private;
    friend class Private;
    Private *const d;
};

}

#endif
