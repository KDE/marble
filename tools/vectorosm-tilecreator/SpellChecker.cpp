//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "SpellChecker.h"
#include "GeoDataPlacemark.h"
#include "MarbleMath.h"
#include "OsmPlacemarkData.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "ParsingRunnerManager.h"
#include "GeoSceneMercatorTileProjection.h"
#include "TileId.h"

#include <QSet>
#include <QDebug>
#include <QFile>

namespace Marble {

SpellChecker::SpellChecker(const QString &citiesFile) :
    m_tileLevel(10),
    m_tileHash(parseCities(citiesFile)),
    m_verbose(false)
{
    // nothing to do
}

void SpellChecker::correctPlaceLabels(const QVector<GeoDataPlacemark*> &placemarks)
{
    auto places = cityPlaces(placemarks);
    double const maxDistance = 5000.0 / EARTH_RADIUS;
    int hits = 0;
    int validated = 0;
    int misses = 0;
    for (auto place: places) {
        auto const places = candidatesFor(place);
        bool hasMatch = false;
        bool isValid = false;
        QString const placeName = place->name();
        if (!places.isEmpty()) {
            auto match = places.first();
            if (match->name() == place->name()) {
                ++validated;
                isValid = true;
            } else {
                if (distanceSphere(match->coordinate(), place->coordinate()) < maxDistance) {
                    if (levenshteinDistance(places.first()->name(), placeName) < 6) {
                        if (m_verbose) {
                            qDebug() << "Correcting" << placeName << "to" << match->name();
                        }
                        place->setName(match->name());
                        place->osmData().removeTag("name");
                        place->osmData().addTag("name", match->name());
                        hasMatch = true;
                    }
                }

                if (m_verbose && !hasMatch) {
                    qDebug() << "No match for " << placeName << ", candidates: ";
                    for (auto candidate: places) {
                        qDebug() << distanceSphere(candidate->coordinate(), place->coordinate()) * EARTH_RADIUS << " m, "
                                 << "levenshtein distance " << levenshteinDistance(placeName, candidate->name()) << ":" << candidate->name();
                    }
                }
            }
        } else if (m_verbose) {
            qDebug() << "No match for " << placeName << " at " << place->coordinate().toString(GeoDataCoordinates::Decimal) << " and no candidates for replacement";
        }
        hits += hasMatch ? 1 : 0;
        misses += (hasMatch || isValid) ? 0 : 1;
    }
    if (m_verbose) {
        qDebug() << "In total there are " << hits << " corrections, " << validated << " validations and " << misses << " misses";
    }
}

void SpellChecker::setVerbose(bool verbose)
{
    m_verbose = verbose;
}

QVector<GeoDataPlacemark *> SpellChecker::cityPlaces(const QVector<GeoDataPlacemark *> &placemarks) const
{
    QSet<GeoDataPlacemark::GeoDataVisualCategory> categories;
    categories << GeoDataPlacemark::PlaceCity;
    categories << GeoDataPlacemark::PlaceCityCapital;
    categories << GeoDataPlacemark::PlaceSuburb;
    categories << GeoDataPlacemark::PlaceHamlet;
    categories << GeoDataPlacemark::PlaceLocality;
    categories << GeoDataPlacemark::PlaceTown;
    categories << GeoDataPlacemark::PlaceTownCapital;
    categories << GeoDataPlacemark::PlaceVillage;
    categories << GeoDataPlacemark::PlaceVillageCapital;

    QVector<GeoDataPlacemark*> places;
    std::copy_if(placemarks.begin(), placemarks.end(), std::back_inserter(places),
                 [categories] (GeoDataPlacemark* placemark) {
        return categories.contains(placemark->visualCategory()); });
    return places;
}

QHash<TileId, QVector<GeoDataPlacemark *> > SpellChecker::parseCities(const QString &filename) const
{
    QHash<TileId, QVector<GeoDataPlacemark*> > placeLabels;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open " << filename << ":" << file.errorString();
        return placeLabels;
    }

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        auto const values = line.split('\t');
        if (values.size() > 15) {

            GeoDataPlacemark* city = new GeoDataPlacemark;
            city->setName(values[1]);
            bool ok;
            double const lon = values[5].toDouble(&ok);
            if (!ok) {
                qDebug() << values[5] << " is no longitude";
                continue;
            }
            double const lat = values[4].toDouble(&ok);
            if (!ok) {
                qDebug() << values[4] << " is no latitude";
                continue;
            }
            double const ele = values[15].toDouble();
            auto const coordinate = GeoDataCoordinates(lon, lat, ele, GeoDataCoordinates::Degree);
            city->setCoordinate(coordinate);

            auto const tile = TileId::fromCoordinates(coordinate, m_tileLevel);
            placeLabels[tile] << city;
        }
    }
    return placeLabels;
}

int SpellChecker::levenshteinDistance(const QString &a, const QString &b)
{
    // From https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
    unsigned int const len1 = a.size(), len2 = b.size();
    std::vector<std::vector<unsigned int>> distance(len1 + 1, std::vector<unsigned int>(len2 + 1));

    distance[0][0] = 0;
    for(unsigned int i = 1; i <= len1; ++i) {
        distance[i][0] = i;
    }
    for(unsigned int i = 1; i <= len2; ++i) {
        distance[0][i] = i;
    }

    for(unsigned int i = 1; i <= len1; ++i) {
        for(unsigned int j = 1; j <= len2; ++j) {
            distance[i][j] = std::min({ distance[i - 1][j] + 1, distance[i][j - 1] + 1, distance[i - 1][j - 1] + (a[i - 1] == b[j - 1] ? 0 : 1) });
        }
    }
    return distance[len1][len2];
}

QVector<GeoDataPlacemark *> SpellChecker::candidatesFor(GeoDataPlacemark *placemark) const
{
    int const N = pow(2, m_tileLevel);
    auto const tile = TileId::fromCoordinates(placemark->coordinate(), m_tileLevel);
    QVector<GeoDataPlacemark *> places;
    for (int x=qMax(0, tile.x()-1); x<qMin(N-1, tile.x()+1); ++x) {
        for (int y=qMax(0, tile.y()-1); y<qMin(N-1, tile.y()+1); ++y) {
            places << m_tileHash[TileId(0, m_tileLevel, x, y)];
        }
    }
    QString const placeName = placemark->name();
    std::sort(places.begin(), places.end(),
              [placeName] (GeoDataPlacemark* a, GeoDataPlacemark* b) {
        return levenshteinDistance(a->name(), placeName) < levenshteinDistance(b->name(), placeName);
    });
    return places;
}

}
