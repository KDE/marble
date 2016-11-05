//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "PeakAnalyzer.h"
#include "GeoDataPlacemark.h"
#include "MarbleMath.h"
#include "OsmPlacemarkData.h"

#include <QSet>
#include <QMap>

namespace Marble {

PeakAnalyzer::Peaks PeakAnalyzer::peaksNear(const GeoDataPlacemark* placemark, const Peaks &peaks, double maxDistance)
{
    // If this turns out to become a bottleneck due to quadratic runtime, use kd-tree via nanoflann from
    // https://github.com/jlblancoc/nanoflann to speed it up.
    Peaks neighbors;
    for (auto peak: peaks) {
        if (distanceSphere(peak->coordinate(), placemark->coordinate()) < maxDistance) {
            neighbors << peak;
        }
    }
    return neighbors;
}

void PeakAnalyzer::dbScan(const Peaks &peaks, double maxDistance, int minPoints)
{
    QSet<GeoDataPlacemark*> visited;
    QMap<GeoDataPlacemark*, PeakCluster*> associations;
    Peaks noise;
    PeakClusters clusters;
    for(auto peak: peaks) {
        if (visited.contains(peak)) {
            continue;
        }
        visited << peak;
        auto neighbors = peaksNear(peak, peaks, maxDistance);
        if (neighbors.size() < minPoints) {
            noise << peak;
        } else {
            PeakCluster* fit = nullptr;
            for (auto &cluster: clusters) {
                for (auto placemark: cluster) {
                    if (distanceSphere(peak->coordinate(), placemark->coordinate()) < maxDistance) {
                        fit = &cluster;
                    }
                }
            }
            if (!fit) {
                clusters << PeakCluster();
                fit = &clusters.last();
            }

            while (!neighbors.isEmpty()) {
                auto neighbor = neighbors.front();
                neighbors.pop_front();
                if (!visited.contains(neighbor)) {
                    visited << neighbor;
                    auto const moreNeighbors = peaksNear(neighbor, peaks, maxDistance);
                    if (moreNeighbors.size() >= minPoints) {
                        neighbors += moreNeighbors;
                    }
                }
                if (associations[neighbor] == nullptr) {
                    *fit << neighbor;
                    associations[neighbor] = fit;
                }
            }
        }
    }

    for (auto &cluster: clusters) {
        Q_ASSERT(!cluster.isEmpty());
        std::sort(cluster.begin(), cluster.end(), [](GeoDataPlacemark* a, GeoDataPlacemark* b) {
            return a->coordinate().altitude() > b->coordinate().altitude();
        });
        bool first = true;
        for (auto peak: cluster) {
            peak->osmData().addTag(QLatin1String("marbleZoomLevel"), first ? QLatin1String("11") : QLatin1String("13"));
            first = false;
        }
    }
    for (auto peak: noise) {
        peak->osmData().addTag(QLatin1String("marbleZoomLevel"), QLatin1String("11"));
    }
}

void PeakAnalyzer::determineZoomLevel(const QVector<GeoDataPlacemark*> &placemarks)
{
    QVector<GeoDataPlacemark*> peaks;
    std::copy_if(placemarks.begin(), placemarks.end(), std::back_inserter(peaks), [](GeoDataPlacemark* placemark) {
        return placemark->visualCategory() == GeoDataPlacemark::NaturalPeak; });
    double const maxDistance = 3000.0 / EARTH_RADIUS;
    dbScan(peaks, maxDistance, 2);
}




}
