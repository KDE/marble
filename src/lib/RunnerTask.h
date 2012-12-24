//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_RUNNERTASK_H
#define MARBLE_RUNNERTASK_H

#include "GeoDataCoordinates.h"
#include "GeoDataDocument.h"
#include "GeoDataLatLonAltBox.h"

#include <QtCore/QRunnable>
#include <QtCore/QString>

namespace Marble
{

class MarbleModel;
class RouteRequest;
class MarbleAbstractRunner;
class MarbleRunnerManager;

/** A RunnerTask that executes a placemark search */
class SearchTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    SearchTask( MarbleAbstractRunner *runner, MarbleRunnerManager *manager, MarbleModel *model, const QString &searchTerm, const GeoDataLatLonAltBox &preferred );

    /**
     * @reimp
     */
    void run();

Q_SIGNALS:
    void finished( SearchTask *task );

private:
    MarbleAbstractRunner *const m_runner;
    QString m_searchTerm;
    GeoDataLatLonAltBox m_preferredBbox;
};

/** A RunnerTask that executes reverse geocoding */
class ReverseGeocodingTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    ReverseGeocodingTask( MarbleAbstractRunner *runner, MarbleRunnerManager *manager, MarbleModel *model, const GeoDataCoordinates &coordinates );

    /**
     * @reimp
     */
    void run();

Q_SIGNALS:
    void finished( ReverseGeocodingTask *task );

private:
    MarbleAbstractRunner *const m_runner;
    GeoDataCoordinates m_coordinates;
};


/** A RunnerTask that executes a route calculation */
class RoutingTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    RoutingTask( MarbleAbstractRunner *runner, MarbleRunnerManager *manager, MarbleModel *model, const RouteRequest* routeRequest );

    /**
     * @reimp
     */
    void run();

Q_SIGNALS:
    void finished( RoutingTask *task );

private:
    MarbleAbstractRunner *const m_runner;
    const RouteRequest *const m_routeRequest;
};

/** A RunnerTask that executes a file Parsing */
class ParsingTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    ParsingTask( MarbleAbstractRunner *runner, MarbleRunnerManager *manager, const QString& fileName, DocumentRole role );

    /**
     * @reimp
     */
    void run();

Q_SIGNALS:
    void finished( ParsingTask *task );

private:
    MarbleAbstractRunner *const m_runner;
    QString m_fileName;
    DocumentRole m_role;
};

}

#endif
