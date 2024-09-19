// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_MONAVMAP_H
#define MARBLE_MONAVMAP_H

#include "GeoDataLatLonBox.h"
#include "GeoDataLinearRing.h"

#include <QDir>

namespace Marble
{

class MonavMap
{
public:
    void setDirectory(const QDir &dir);

    QDir directory() const;

    bool containsPoint(const GeoDataCoordinates &point) const;

    qint64 size() const;

    void remove() const;

    static bool areaLessThan(const MonavMap &first, const MonavMap &second);

    static bool nameLessThan(const MonavMap &first, const MonavMap &second);

    QString transport() const;

    QString name() const;

    QString version() const;

    QString date() const;

    QString payload() const;

private:
    QList<QFileInfo> files() const;

    void parseBoundingBox(const QFileInfo &file);

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
