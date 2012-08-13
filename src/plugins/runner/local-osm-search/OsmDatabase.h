//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_OSMDATABASE_H
#define MARBLE_OSMDATABASE_H

#include "OsmPlacemark.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtSql/QSqlDatabase>

namespace Marble {

class MarbleModel;
class GeoDataCoordinates;
class GeoDataLatLonAltBox;

class OsmDatabase
{
public:
    OsmDatabase();

    // Methods for read access

    /** Open the given file. Previously opened files remain valid. */
    void addFile( const QString &file );

    /** Remove all files */
    void clear();

    /** Search the database for matching regions and placemarks */
    QVector<OsmPlacemark> find( MarbleModel* model, const QString &searchTerm, const GeoDataLatLonAltBox &preferred );

private:
    QString wildcardQuery( const QString &term ) const;

    void unique( QVector<OsmPlacemark> &placemarks ) const;

    QStringList m_databases;

    QSqlDatabase m_database;

    QString formatDistance( const GeoDataCoordinates &a, const GeoDataCoordinates &b ) const;

    qreal bearing( const GeoDataCoordinates &a, const GeoDataCoordinates &b ) const;

    Q_DISABLE_COPY( OsmDatabase )
};

}

#endif // MARBLE_OSMDATABASE_H
