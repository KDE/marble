//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include <QVector>

namespace Marble {

class GeoDataPlacemark;

class PeakAnalyzer
{
public:
    static void determineZoomLevel(const QVector<GeoDataPlacemark*> &placemarks);

private:
    typedef QVector<GeoDataPlacemark*> Peaks;
    typedef QVector<GeoDataPlacemark*> PeakCluster;
    typedef QVector<PeakCluster> PeakClusters;

    static Peaks peaksNear(const GeoDataPlacemark* placemark, const Peaks &peaks, double maxDistance);
    static void dbScan(const Peaks &peaks, double maxDistance, int minPoints);
};

}
