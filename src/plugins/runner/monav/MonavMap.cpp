// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "MonavMap.h"
#include "MarbleDebug.h"

#include "GeoDataData.h"
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"

namespace Marble
{

void MonavMap::setDirectory(const QDir &dir)
{
    m_directory = dir;
    const QFileInfo boundingBox(dir, QStringLiteral("marble.kml"));
    if (boundingBox.exists()) {
        parseBoundingBox(boundingBox);
    } else {
        mDebug() << "No monav bounding box given for " << boundingBox.absoluteFilePath();
    }
}

void MonavMap::parseBoundingBox(const QFileInfo &file)
{
    GeoDataLineString points;
    bool tooLarge = false;
    QFile input(file.absoluteFilePath());
    if (input.open(QFile::ReadOnly)) {
        GeoDataParser parser(GeoData_KML);
        if (!parser.read(&input)) {
            mDebug() << "Could not parse file: " << parser.errorString();
            return;
        }

        GeoDocument *doc = parser.releaseDocument();
        input.close();
        auto document = dynamic_cast<GeoDataDocument *>(doc);
        QList<GeoDataPlacemark *> placemarks = document->placemarkList();
        if (placemarks.size() == 1) {
            GeoDataPlacemark *placemark = placemarks.first();
            m_name = placemark->name();
            m_version = placemark->extendedData().value(QStringLiteral("version")).value().toString();
            m_date = placemark->extendedData().value(QStringLiteral("date")).value().toString();
            m_transport = placemark->extendedData().value(QStringLiteral("transport")).value().toString();
            m_payload = placemark->extendedData().value(QStringLiteral("payload")).value().toString();
            const auto geometry = dynamic_cast<const GeoDataMultiGeometry *>(placemark->geometry());
            if (geometry->size() > 1500) {
                tooLarge = true;
            }
            for (int i = 0; geometry && i < geometry->size(); ++i) {
                const auto poly = dynamic_cast<const GeoDataLinearRing *>(geometry->child(i));
                if (poly) {
                    for (int j = 0; j < poly->size(); ++j) {
                        points << poly->at(j);
                    }
                    m_tiles.push_back(*poly);

                    if (poly->size() > 1500) {
                        tooLarge = true;
                    }
                }
            }
        } else {
            mDebug() << "File " << file.absoluteFilePath() << " does not contain one placemark, but " << placemarks.size();
        }

        delete doc;
    }
    m_boundingBox = points.latLonAltBox();

    if (tooLarge) {
        // The bounding box polygon is rather complicated, therefore not allowing a quick check
        // and also occupying memory. Discard the polygon and only store the rectangular bounding
        // box. Only happens for non-simplified bounding box polygons.
        mDebug() << "Discarding too large bounding box polygon for " << file.absoluteFilePath() << ". Please check for a map update.";
        m_tiles.clear();
    }
}

bool MonavMap::containsPoint(const GeoDataCoordinates &point) const
{
    // If we do not have a bounding box at all, we err on the safe side
    if (m_boundingBox.isEmpty()) {
        return true;
    }

    // Quick check for performance reasons
    if (!m_boundingBox.contains(point)) {
        return false;
    }

    if (m_tiles.isEmpty()) {
        return true; // Tiles discarded for performance reason
    }

    // GeoDataLinearRing does a 3D check, but we only have 2D data for
    // the map bounding box. Therefore the 3D info of e.g. the GPS position
    // must be ignored.
    GeoDataCoordinates flatPosition = point;
    flatPosition.setAltitude(0.0);
    for (const GeoDataLinearRing &box : m_tiles) {
        if (box.contains(flatPosition)) {
            return true;
        }
    }

    return false;
}

qint64 MonavMap::size() const
{
    qint64 result = 0;
    for (const QFileInfo &file : files()) {
        result += file.size();
    }

    return result;
}

QList<QFileInfo> MonavMap::files() const
{
    QList<QFileInfo> files;
    QStringList fileNames = QStringList() << QStringLiteral("config") << QStringLiteral("edges") << QStringLiteral("names") << QStringLiteral("paths")
                                          << QStringLiteral("types");
    for (const QString &file : std::as_const(fileNames)) {
        files << QFileInfo(m_directory, QLatin1StringView("Contraction Hierarchies_") + file);
    }

    fileNames = QStringList() << QStringLiteral("config") << QStringLiteral("grid") << QStringLiteral("index_1") << QStringLiteral("index_2")
                              << QStringLiteral("index_3");
    for (const QString &file : std::as_const(fileNames)) {
        files << QFileInfo(m_directory, QLatin1StringView("GPSGrid_") + file);
    }

    files << QFileInfo(m_directory, QStringLiteral("plugins.ini"));
    QFileInfo moduleDotIni(m_directory, QStringLiteral("Module.ini"));
    if (moduleDotIni.exists()) {
        files << moduleDotIni;
    }
    files << QFileInfo(m_directory, QStringLiteral("marble.kml"));
    return files;
}

void MonavMap::remove() const
{
    for (const QFileInfo &file : files()) {
        QFile(file.absoluteFilePath()).remove();
    }
}

bool MonavMap::areaLessThan(const MonavMap &first, const MonavMap &second)
{
    if (!first.m_tiles.isEmpty() && second.m_tiles.isEmpty()) {
        return true;
    }

    if (first.m_tiles.isEmpty() && !second.m_tiles.isEmpty()) {
        return false;
    }

    qreal const areaOne = first.m_boundingBox.width() * first.m_boundingBox.height();
    qreal const areaTwo = second.m_boundingBox.width() * second.m_boundingBox.height();
    return areaOne < areaTwo;
}

bool MonavMap::nameLessThan(const MonavMap &first, const MonavMap &second)
{
    return first.name() < second.name();
}

QDir MonavMap::directory() const
{
    return m_directory;
}

QString MonavMap::transport() const
{
    return m_transport;
}

QString MonavMap::name() const
{
    return m_name;
}

QString MonavMap::version() const
{
    return m_version;
}

QString MonavMap::date() const
{
    return m_date;
}

QString MonavMap::payload() const
{
    return m_payload;
}

}
