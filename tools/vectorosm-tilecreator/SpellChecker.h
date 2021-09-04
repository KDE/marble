//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef TOOLS_SPELLCHECKER_H
#define TOOLS_SPELLCHECKER_H

#include <TileId.h>
#include "GeoDataPlacemark.h"

#include <QVector>

namespace Marble {

class GeoDataPlacemark;

class SpellChecker
{
public:
    explicit SpellChecker(const QString &citiesFile);
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
#endif
