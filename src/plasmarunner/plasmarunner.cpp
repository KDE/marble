// SPDX-FileCopyrightText: 2011 Friedrich W. H. Kossebau <kossebau@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "plasmarunner.h"

// Marble
#include <BookmarkManager.h>
#include <GeoDataCoordinates.h>
#include <GeoDataFolder.h>
#include <GeoDataLookAt.h>
#include <GeoDataPlacemark.h>
#include <GeoDataTreeModel.h>

// KF
#include <KLocalizedString>

// Qt
#include <QIcon>
#include <QProcess>

#define TRANSLATION_DOMAIN "plasma_runner_marble"

namespace Marble
{

K_PLUGIN_CLASS_WITH_JSON(PlasmaRunner, "plasma-runner-marble.json")

static const int minContainsMatchLength = 3;

PlasmaRunner::PlasmaRunner(QObject *parent, const KPluginMetaData &pluginMetaData)
    : AbstractRunner(parent, pluginMetaData)
{
    addSyntax(KRunner::RunnerSyntax(QStringLiteral(":q:"), i18n("Shows the coordinates :q: in OpenStreetMap with Marble.")));
    addSyntax(KRunner::RunnerSyntax(QStringLiteral(":q:"), i18n("Shows the geo bookmark containing :q: in OpenStreetMap with Marble.")));
}

void PlasmaRunner::match(KRunner::RunnerContext &context)
{
    QList<KRunner::QueryMatch> matches;

    const QString query = context.query();

    bool success = false;
    // TODO: how to estimate that input is in Degree, not Radian?
    GeoDataCoordinates coordinates = GeoDataCoordinates::fromString(query, success);

    if (success) {
        const QVariant coordinatesData = QVariantList()
            << QVariant(coordinates.longitude(GeoDataCoordinates::Degree)) << QVariant(coordinates.latitude(GeoDataCoordinates::Degree))
            << QVariant(0.1); // TODO: make this distance value configurable

        KRunner::QueryMatch match(this);
        match.setIcon(QIcon::fromTheme(QStringLiteral("marble")));
        match.setText(i18n("Show the coordinates %1 in OpenStreetMap with Marble", query));
        match.setData(coordinatesData);
        match.setId(query);
        match.setRelevance(1.0);

        matches << match;
    }

    // TODO: BookmarkManager does not yet listen to updates, also does not sync between processes :(
    // So for now always load on demand, even if expensive possibly
    BookmarkManager bookmarkManager(new GeoDataTreeModel);
    bookmarkManager.loadFile(QStringLiteral("bookmarks/bookmarks.kml"));

    for (GeoDataFolder *folder : bookmarkManager.folders()) {
        collectMatches(matches, query, folder);
    }

    if (!matches.isEmpty()) {
        context.addMatches(matches);
    }
}

void PlasmaRunner::collectMatches(QList<KRunner::QueryMatch> &matches, const QString &query, const GeoDataFolder *folder)
{
    const QString queryLower = query.toLower();

    QList<GeoDataFeature *>::const_iterator it = folder->constBegin();
    QList<GeoDataFeature *>::const_iterator end = folder->constEnd();

    for (; it != end; ++it) {
        GeoDataFolder *folder = dynamic_cast<GeoDataFolder *>(*it);
        if (folder) {
            collectMatches(matches, query, folder);
            continue;
        }

        GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark *>(*it);
        if (placemark) {
            // For short query strings only match exactly, to get a sane number of matches
            if (query.length() < minContainsMatchLength) {
                if (placemark->name().toLower() != queryLower
                    && (placemark->descriptionIsCDATA() || // TODO: support also with CDATA
                        placemark->description().toLower() != queryLower)) {
                    continue;
                }
            } else {
                if (!placemark->name().toLower().contains(queryLower)
                    && (placemark->descriptionIsCDATA() || // TODO: support also with CDATA
                        !placemark->description().toLower().contains(queryLower))) {
                    continue;
                }
            }

            const GeoDataCoordinates coordinates = placemark->coordinate();
            const qreal lon = coordinates.longitude(GeoDataCoordinates::Degree);
            const qreal lat = coordinates.latitude(GeoDataCoordinates::Degree);
            const QVariant coordinatesData = QVariantList() << QVariant(lon) << QVariant(lat) << QVariant(placemark->lookAt()->range() * METER2KM);

            KRunner::QueryMatch match(this);
            match.setIcon(QIcon::fromTheme(QStringLiteral("marble")));
            match.setText(placemark->name());
            match.setSubtext(i18n("Show in OpenStreetMap with Marble"));
            match.setData(coordinatesData);
            match.setId(placemark->name() + QString::number(lat) + QString::number(lon));
            match.setRelevance(1.0);

            matches << match;
        }
    }
}

void PlasmaRunner::run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match)
{
    Q_UNUSED(context)

    const QVariantList data = match.data().toList();

    // pass in C locale, should be always understood
    const QString latLon = QString::number(data.at(1).toReal()) + QLatin1Char(' ') + QString::number(data.at(0).toReal());

    const QString distance = data.at(2).toString();

    const QStringList parameters = QStringList() << QStringLiteral("--latlon") << latLon << QStringLiteral("--distance") << distance << QStringLiteral("--map")
                                                 << QStringLiteral("earth/openstreetmap/openstreetmap.dgml");
    QProcess::startDetached(QStringLiteral("marble"), parameters);
}

}

#include "moc_plasmarunner.cpp"
#include "plasmarunner.moc"
