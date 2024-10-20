// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef TOOLS_SPELLCHECKER_H
#define TOOLS_SPELLCHECKER_H

#include "GeoDataPlacemark.h"
#include <TileId.h>

#include <QList>

namespace Marble
{

class GeoDataPlacemark;

class SpellChecker
{
public:
    explicit SpellChecker(const QString &citiesFile);
    void setVerbose(bool verbose);

    void correctPlaceLabels(const QList<GeoDataPlacemark *> &placemarks);

private:
    typedef QHash<TileId, QList<GeoDataPlacemark *>> TileHash;

    static int levenshteinDistance(const QString &a, const QString &b);

    QList<GeoDataPlacemark *> cityPlaces(const QList<GeoDataPlacemark *> &placemarks) const;
    TileHash parseCities(const QString &filename) const;
    QList<GeoDataPlacemark *> candidatesFor(GeoDataPlacemark *placemark) const;

    int const m_tileLevel;
    TileHash m_tileHash;
    bool m_verbose;
};

}
#endif
