//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#ifndef MARBLE_ROUTINGRUNNER_H
#define MARBLE_ROUTINGRUNNER_H

#include <QtCore/QObject>
#include "marble_export.h"
#include "GeoDataDocument.h"

#include <QtCore/QVector>

namespace Marble
{

class MarbleModel;
class RouteRequest;

class MARBLE_EXPORT RoutingRunner : public QObject
{
    Q_OBJECT

public:
    explicit RoutingRunner( QObject *parent );

    /**
     * Stores a pointer to the currently used model
     */
    void setModel( MarbleModel *model );

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
    /**
     * Access to the currently used model, or null if no was set with @see setModel
     */
    MarbleModel *model();

private:
    MarbleModel *m_model;
};

}

#endif
