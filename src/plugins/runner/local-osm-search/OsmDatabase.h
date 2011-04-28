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

#include "OsmRegion.h"
#include "OsmPlacemark.h"

#include <QtCore/QString>

namespace Marble {

class OsmDatabase
{
public:
    OsmDatabase();

    // Methods for read access

    /** Open the given file. Previously opened files remain valid. */
    void addFile( const QString &file );

    /** Search the database for matching regions and placemarks */
    QList<OsmPlacemark> find( const QString &searchTerm ) const;

    // Methods for write access

    /** Add a new region to the database */
    void addOsmRegion( const OsmRegion &region );

    /** Add a new placemark to the database */
    void addOsmPlacemark( const OsmPlacemark &placemark );

    /** Write the database to the given file */
    void save( const QString &file );

private:
    Q_DISABLE_COPY( OsmDatabase )

    QList<OsmPlacemark> findOsmTerm( const QString &region ) const;

    QList<OsmPlacemark> findStreets( const QString &region, const QString &street ) const;

    QList<OsmRegion> m_regions;

    QList<OsmPlacemark> m_placemarks;
};

}

#endif // MARBLE_OSMDATABASE_H
