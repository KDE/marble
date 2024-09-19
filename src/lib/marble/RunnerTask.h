// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_RUNNERTASK_H
#define MARBLE_RUNNERTASK_H

#include "GeoDataCoordinates.h"
#include "GeoDataDocument.h"
#include "GeoDataLatLonBox.h"

#include <QRunnable>
#include <QString>

namespace Marble
{

class MarbleModel;
class ParsingRunner;
class SearchRunner;
class ReverseGeocodingRunner;
class RouteRequest;
class RoutingRunner;
class ParsingRunnerManager;
class SearchRunnerManager;
class ReverseGeocodingRunnerManager;
class RoutingRunnerManager;

/** A RunnerTask that executes a placemark search */
class SearchTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    SearchTask(SearchRunner *runner, SearchRunnerManager *manager, const MarbleModel *model, const QString &searchTerm, const GeoDataLatLonBox &preferred);

    /**
     * @reimp
     */
    void run() override;

Q_SIGNALS:
    void finished(SearchTask *task);

private:
    SearchRunner *const m_runner;
    QString m_searchTerm;
    GeoDataLatLonBox m_preferredBbox;
};

/** A RunnerTask that executes reverse geocoding */
class ReverseGeocodingTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    ReverseGeocodingTask(ReverseGeocodingRunner *runner,
                         ReverseGeocodingRunnerManager *manager,
                         const MarbleModel *model,
                         const GeoDataCoordinates &coordinates);

    /**
     * @reimp
     */
    void run() override;

Q_SIGNALS:
    void finished(ReverseGeocodingTask *task);

private:
    ReverseGeocodingRunner *const m_runner;
    GeoDataCoordinates m_coordinates;
};

/** A RunnerTask that executes a route calculation */
class RoutingTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    RoutingTask(RoutingRunner *runner, RoutingRunnerManager *manager, const RouteRequest *routeRequest);

    /**
     * @reimp
     */
    void run() override;

Q_SIGNALS:
    void finished(RoutingTask *task);

private:
    RoutingRunner *const m_runner;
    const RouteRequest *const m_routeRequest;
};

/** A RunnerTask that executes a file Parsing */
class ParsingTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    ParsingTask(ParsingRunner *runner, ParsingRunnerManager *manager, const QString &fileName, DocumentRole role);

    /**
     * @reimp
     */
    void run() override;

Q_SIGNALS:
    void parsed(GeoDataDocument *document, const QString &error);
    void finished();

private:
    ParsingRunner *const m_runner;
    QString m_fileName;
    DocumentRole m_role;
    ParsingRunnerManager *m_manager;
};

}

#endif
