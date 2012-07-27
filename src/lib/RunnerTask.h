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
class SearchRunnerPlugin;
class ReverseGeocodingRunnerPlugin;
class RoutingRunnerPlugin;
class ParseRunnerPlugin;
class MarbleAbstractRunner;
class MarbleRunnerManager;

/**
  * An abstract QRunnable that executes one of the MarbleAbstractRunner tasks -- placemark
  * search, reverse geocoding or routing -- in the run() function and waits for the
  * result in a local event loop.
  */
class RunnerTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    /** Constructor. The runner instance given will be used to execute the actual task */
    explicit RunnerTask( MarbleRunnerManager *manager );

    /** Overriding QRunnable to execute the runner task in a local event loop */
    virtual void run();

Q_SIGNALS:
    void finished( RunnerTask* task );

protected:
    /** Derived classes should execute their task and quit the provided event loop when done */
    virtual void runTask() = 0;

    MarbleRunnerManager *manager();

private:
    MarbleRunnerManager *const m_manager;
};

/** A RunnerTask that executes a placemark search */
class SearchTask : public RunnerTask
{
    Q_OBJECT

public:
    SearchTask( const SearchRunnerPlugin *factory, MarbleRunnerManager *manager, MarbleModel *model, const QString &searchTerm, const GeoDataLatLonAltBox preferred );

    virtual void runTask();

private:
    const SearchRunnerPlugin *const m_factory;
    MarbleModel *const m_model;
    QString m_searchTerm;
    GeoDataLatLonAltBox m_preferredBbox;
};

/** A RunnerTask that executes reverse geocoding */
class ReverseGeocodingTask : public RunnerTask
{
    Q_OBJECT

public:
    ReverseGeocodingTask( const ReverseGeocodingRunnerPlugin*factory, MarbleRunnerManager *manager, MarbleModel *model, const GeoDataCoordinates &coordinates );

    virtual void runTask();

private:
    const ReverseGeocodingRunnerPlugin *const m_factory;
    MarbleModel *const m_model;
    GeoDataCoordinates m_coordinates;
};


/** A RunnerTask that executes a route calculation */
class RoutingTask : public RunnerTask
{
    Q_OBJECT

public:
    RoutingTask( RoutingRunnerPlugin *factory, MarbleRunnerManager *manager, MarbleModel *model, const RouteRequest* routeRequest );

    virtual void runTask();

private:
    RoutingRunnerPlugin *const m_factory;
    MarbleModel *const m_model;
    const RouteRequest *const m_routeRequest;
};

/** A RunnerTask that executes a file Parsing */
class ParsingTask : public RunnerTask
{
    Q_OBJECT

public:
    ParsingTask( const ParseRunnerPlugin* factory, MarbleRunnerManager *manager, const QString& fileName, DocumentRole role );

    virtual void runTask();

private:
    const ParseRunnerPlugin *const m_factory;
    QString m_fileName;
    DocumentRole m_role;
};

}

#endif
