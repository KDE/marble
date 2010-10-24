//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_MONAVMAP_H
#define MARBLE_MONAVMAP_H

#include "GeoDataLatLonBox.h"
#include "GeoDataLinearRing.h"

#include <QtCore/QDir>

namespace Marble
{

class MonavMap
{
public:
    void setDirectory( const QDir &dir );

    QDir directory() const;

    bool containsPoint( const GeoDataCoordinates &point ) const;

    qint64 size() const;

    void remove() const;

    static bool areaLessThan( const MonavMap &first, const MonavMap &second );

    static bool nameLessThan( const MonavMap &first, const MonavMap &second );

    QString transport() const;

    QString name() const;

    QString version() const;

    QString date() const;

    QString payload() const;

private:
    QList<QFileInfo> files() const;

    void parseBoundingBox( const QFileInfo &file );

    QDir m_directory;

    QString m_name;

    QString m_date;

    QString m_version;

    QString m_transport;

    QString m_payload;

    GeoDataLatLonBox m_boundingBox;

    QVector<GeoDataLinearRing> m_tiles;
};

}

#endif // MARBLE_MONAVMAP_H
