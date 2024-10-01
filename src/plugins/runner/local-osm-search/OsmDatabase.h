// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_OSMDATABASE_H
#define MARBLE_OSMDATABASE_H

#include "OsmPlacemark.h"

#include <QString>
#include <QStringList>

namespace Marble
{

class DatabaseQuery;
class GeoDataCoordinates;

class OsmDatabase
{
public:
    explicit OsmDatabase(const QStringList &databaseFiles);

    // Methods for read access

    /** Search the database for matching regions and placemarks */
    QList<OsmPlacemark> find(const DatabaseQuery &userQuery);

private:
    static QString wildcardQuery(const QString &term);

    static void makeUnique(QList<OsmPlacemark> &placemarks);

    QStringList m_databaseFiles;

    static QString formatDistance(const GeoDataCoordinates &a, const GeoDataCoordinates &b);

    static qreal bearing(const GeoDataCoordinates &a, const GeoDataCoordinates &b);

    Q_DISABLE_COPY(OsmDatabase)
};

}

#endif // MARBLE_OSMDATABASE_H
