// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef PEAKANALYZER_H
#define PEAKANALYZER_H

#include <QList>

namespace Marble
{

class GeoDataPlacemark;

class PeakAnalyzer
{
public:
    static void determineZoomLevel(const QList<GeoDataPlacemark *> &placemarks);

private:
    using Peaks = QList<GeoDataPlacemark *>;
    using PeakCluster = QList<GeoDataPlacemark *>;
    using PeakClusters = QList<PeakCluster>;

    static Peaks peaksNear(const GeoDataPlacemark *placemark, const Peaks &peaks, double maxDistance);
    static void dbScan(const Peaks &peaks, double maxDistance, int minPoints);
};

}
#endif
