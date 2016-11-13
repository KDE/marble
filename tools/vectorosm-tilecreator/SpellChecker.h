//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include <TileId.h>
#include "GeoDataPlacemark.h"

#include <QVector>

namespace Marble {

class GeoDataPlacemark;

class SpellChecker
{
public:
    SpellChecker(const QString &citiesFile);
    void setVerbose(bool verbose);

    void correctPlaceLabels(const QVector<GeoDataPlacemark*> &placemarks);

private:
    typedef QHash<TileId, QVector<GeoDataPlacemark*> > TileHash;

    static int levenshteinDistance(const QString &a, const QString &b);

    QVector<GeoDataPlacemark*> cityPlaces(const QVector<GeoDataPlacemark*> &placemarks) const;
    TileHash parseCities(const QString &filename) const;
    QVector<GeoDataPlacemark*> candidatesFor(GeoDataPlacemark* placemark) const;

    int const m_tileLevel;
    TileHash m_tileHash;
    bool m_verbose;
};

}
