//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_RUNNERTASK_H
#define MARBLE_RUNNERTASK_H

#include "GeoDataCoordinates.h"
#include "GeoDataTypes.h"

#include <QtCore/QRunnable>
#include <QtCore/QString>

namespace Marble
{

class RouteRequest;
class MarbleAbstractRunner;

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
    explicit RunnerTask( MarbleAbstractRunner* runner );

    /** Overriding QRunnable to execute the runner task in a local event loop */
    virtual void run();

Q_SIGNALS:
    void finished( RunnerTask* task );

protected:
    /** Derived classes should execute their task and quit the provided event loop when done */
    virtual void runTask( QEventLoop *localEventLoop ) = 0;

    /** Access to the runner for derived classes */
    MarbleAbstractRunner* runner();

private:
    MarbleAbstractRunner* m_runner;
};

/** A RunnerTask that executes a placemark search */
class SearchTask : public RunnerTask
{
public:
    SearchTask( MarbleAbstractRunner* runner, const QString &searchTerm );

    virtual void runTask( QEventLoop *localEventLoop );

private:
  QString m_searchTerm;
};

/** A RunnerTask that executes reverse geocoding */
class ReverseGeocodingTask : public RunnerTask
{
public:
    ReverseGeocodingTask( MarbleAbstractRunner* runner, const GeoDataCoordinates &coordinates );

    virtual void runTask( QEventLoop *localEventLoop );

private:
  GeoDataCoordinates m_coordinates;
};


/** A RunnerTask that executes a route calculation */
class RoutingTask : public RunnerTask
{
public:
    RoutingTask( MarbleAbstractRunner* runner, RouteRequest* routeRequest );

    virtual void runTask( QEventLoop *localEventLoop );

private:
  RouteRequest* m_routeRequest;
};

/** A RunnerTask that executes a file Parsing */
class ParsingTask : public RunnerTask
{
public:
    ParsingTask( MarbleAbstractRunner* runner, const QString& fileName, DocumentRole role );

    virtual void runTask( QEventLoop *localEventLoop );

private:
  QString m_fileName;
  DocumentRole m_role;
};

}

#endif
