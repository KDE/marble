//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_OSMDATABASE_H
#define MARBLE_OSMDATABASE_H

#include "OsmPlacemark.h"

#include <QString>
#include <QStringList>

namespace Marble {

class DatabaseQuery;
class GeoDataCoordinates;

class OsmDatabase
{
public:
    explicit OsmDatabase( const QStringList &databaseFiles );

    // Methods for read access

    /** Search the database for matching regions and placemarks */
    QVector<OsmPlacemark> find( const DatabaseQuery &userQuery );

private:
    static QString wildcardQuery( const QString &term );

    static void makeUnique( QVector<OsmPlacemark> &placemarks );

    QStringList m_databaseFiles;

    static QString formatDistance( const GeoDataCoordinates &a, const GeoDataCoordinates &b );

    static qreal bearing( const GeoDataCoordinates &a, const GeoDataCoordinates &b );

    Q_DISABLE_COPY( OsmDatabase )
};

}

#endif // MARBLE_OSMDATABASE_H
