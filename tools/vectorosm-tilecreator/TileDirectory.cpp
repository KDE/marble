//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "TileDirectory.h"
#include "TileIterator.h"
#include <GeoDataDocumentWriter.h>

#include <QFileInfo>
#include <QDebug>
#include <QProcess>
#include <QDir>

#include <iostream>

namespace Marble {

TileDirectory::TileDirectory(TileType tileType, const QString &baseDir, ParsingRunnerManager &manager, QString const &extension) :
    m_baseDir(baseDir),
    m_manager(manager),
    m_zoomLevel(QFileInfo(baseDir).baseName().toInt()),
    m_tileX(-1),
    m_tileY(-1),
    m_tagZoomLevel(-1),
    m_extension(extension),
    m_tileType(tileType)
{
    // nothing to do
}

TileId TileDirectory::tileFor(int zoomLevel, int tileX, int tileY) const
{
    int const zoomDiff = zoomLevel - m_zoomLevel;
    int const x = tileX >> zoomDiff;
    int const y = tileY >> zoomDiff;
    return TileId(QString(), m_zoomLevel, x, y);
}

QSharedPointer<GeoDataDocument> TileDirectory::load(int zoomLevel, int tileX, int tileY)
{
    auto const tile = tileFor(zoomLevel, tileX, tileY);
    if (tile.x() == m_tileX && tile.y() == m_tileY) {
        return m_landmass;
    }

    m_tileX = tile.x();
    m_tileY = tile.y();
    QString const filename = QString("%1/%2/%3.%4").arg(m_baseDir).arg(tile.x()).arg(tile.y()).arg(m_extension);
    m_landmass = open(filename, m_manager);
    return m_landmass;
}

void TileDirectory::setInputFile(const QString &filename)
{
    m_inputFile = filename;
}

GeoDataDocument* TileDirectory::clip(int zoomLevel, int tileX, int tileY)
{
    QSharedPointer<GeoDataDocument> oldMap = m_landmass;
    load(zoomLevel, tileX, tileY);
    if (!m_clipper || oldMap != m_landmass || m_tagZoomLevel != zoomLevel) {
        setTagZoomLevel(zoomLevel);
        GeoDataDocument* input = m_tagsFilter ? m_tagsFilter->accepted() : m_landmass.data();
        m_clipper = QSharedPointer<VectorClipper>(new VectorClipper(input));
    }
    return m_clipper->clipTo(zoomLevel, tileX, tileY);
}

QString TileDirectory::name() const
{
    return QString("%1/%2/%3").arg(m_zoomLevel).arg(m_tileX).arg(m_tileY);
}

QSharedPointer<GeoDataDocument> TileDirectory::open(const QString &filename, ParsingRunnerManager &manager)
{
    // Timeout is set to 10 min. If the file is reaaally huge, set it to something bigger.
    GeoDataDocument* map = manager.openFile(filename, DocumentRole::MapDocument, 600000);
    if(map == nullptr) {
        qWarning() << "File" << filename << "couldn't be loaded.";
    }
    QSharedPointer<GeoDataDocument> result = QSharedPointer<GeoDataDocument>(map);
    return result;
}

QStringList TileDirectory::tagsFilteredIn(int zoomLevel) const
{
    QStringList tags;
    tags << "highway=motorway" << "highway=motorway_link";
    tags << "highway=trunk" << "highway=trunk_link";
    tags << "highway=primary" << "highway=primary_link";
    tags << "highway=secondary" << "highway=secondary_link";

    if (zoomLevel >= 13) {
        tags << "highway=tertiary" << "highway=tertiary_link";
        tags << "highway=unclassified";

        tags << "public_transport=station";
        tags << "railway=light_rail";
        tags << "railway=monorail";
        tags << "railway=narrow_gauge";
        tags << "railway=preserved";
        tags << "railway=rail";
        tags << "railway=subway";
        tags << "railway=tram";

        tags << "natural=scrub";
        tags << "natural=heath";
        tags << "natural=grassland";
        tags << "natural=glacier";
        tags << "natural=beach";
        tags << "natural=coastline";
        tags << "natural=water";
        tags << "natural=wood";
        tags << "leisure=stadium";
        tags << "tourism=alpine_hut";

        tags << "waterway=river";
        tags << "waterway=stream";
        tags << "waterway=canal";

        tags << "place=suburb";
        tags << "place=village";

        tags << "natural=peak";
    }

    if (zoomLevel <= 13) {
        tags << "landuse=commercial";
        tags << "landuse=farmland";
        tags << "landuse=farmyard";
        tags << "landuse=forest";
        tags << "landuse=industrial";
        tags << "landuse=meadow";
        tags << "landuse=military";
        tags << "landuse=recreation_ground";
        tags << "landuse=residential";
        tags << "landuse=retail";
    }

    if (zoomLevel >= 15) {
        tags << "highway=residential";
        tags << "highway=track";

        tags << "landuse=*";

        tags << "leisure=pitch";
        tags << "leisure=swimming_area";

        tags << "place=hamlet";
        tags << "place=isolated_dwelling";

        tags << "man_made=beacon";
        tags << "man_made=bridge";
        tags << "man_made=campanile";
        tags << "man_made=chimney";
        tags << "man_made=communications_tower";
        tags << "man_made=cross";
        tags << "man_made=gasometer";
        tags << "man_made=lighthouse";
        tags << "man_made=tower";
        tags << "man_made=water_tower";
        tags << "man_made=windmill";
    }

    tags << "leisure=nature_reserve";
    tags << "leisure=park";

    tags << "place=city";
    tags << "place=town";
    tags << "place=locality";

    tags << "boundary=administrative";
    tags << "boundary=political";
    tags << "boundary=national_park";
    tags << "boundary=protected_area";
    return tags;
}

void TileDirectory::setTagZoomLevel(int zoomLevel)
{
    m_tagZoomLevel = zoomLevel;
    if (m_tileType == OpenStreetMap) {
        QStringList const tags = tagsFilteredIn(m_tagZoomLevel);
        if (m_tagZoomLevel < 17) {
            m_tagsFilter = QSharedPointer<TagsFilter>(new TagsFilter(m_landmass.data(), tags));
        } else {
            m_tagsFilter.clear();
        }
    }
}

GeoDataLatLonBox TileDirectory::boundingBox() const
{
    return m_boundingBox;
}

void TileDirectory::setBoundingBox(const GeoDataLatLonBox &boundingBox)
{
    m_boundingBox = boundingBox;
}

void TileDirectory::createTiles() const
{
    int const zoomLevel = m_tileType == OpenStreetMap ? 10 : 7;
    QSharedPointer<GeoDataDocument> map;
    QSharedPointer<VectorClipper> clipper;
    TileIterator iter(m_boundingBox, zoomLevel);
    qint64 count = 0;
    foreach(auto const &tileId, iter) {
        ++count;
        QString const outputDir = QString("%1/%2").arg(m_baseDir).arg(tileId.x());
        QString const outputFile = QString("%1/%2.%3").arg(outputDir).arg(tileId.y()).arg(m_extension);
        if (QFileInfo(outputFile).exists()) {
            continue;
        }

        std::cout << "Creating " << (m_tileType == OpenStreetMap ? "osm" : "landmass");
        std::cout << " cache tile " << count << "/" << iter.total() << " (";
        std::cout << zoomLevel << "/" << tileId.x() << "/" << tileId.y() << ")      \r";
        std::cout.flush();

        QDir().mkpath(outputDir);
        if (m_tileType == OpenStreetMap) {
            QString const output = QString("-o=%1").arg(outputFile);
            int const N = pow(2, zoomLevel);
            double const minLon = TileId::tileX2lon(tileId.x(), N) * RAD2DEG;
            double const maxLon = TileId::tileX2lon(tileId.x()+1, N) * RAD2DEG;
            double const maxLat = TileId::tileY2lat(tileId.y(), N) * RAD2DEG;
            double const minLat = TileId::tileY2lat(tileId.y()+1, N) * RAD2DEG;
            QString const bbox = QString("-b=%1,%2,%3,%4").arg(minLon).arg(minLat).arg(maxLon).arg(maxLat);
            QProcess osmconvert;
            osmconvert.start("osmconvert", QStringList() << "--drop-author" << "--drop-version"
                             << "--complete-ways" << "--complex-ways" << bbox << output << m_inputFile);
            osmconvert.waitForFinished();
            if (osmconvert.exitCode() != 0) {
                qWarning() << bbox;
                qWarning() << osmconvert.readAllStandardError();
                qWarning() << "osmconvert failed: " << osmconvert.errorString();
            }
        } else {
            if (!map) {
                map = open(m_inputFile, m_manager);
                clipper = QSharedPointer<VectorClipper>(new VectorClipper(map.data()));
            }
            auto tile = clipper->clipTo(zoomLevel, tileId.x(), tileId.y());
            if (tile->size() > 0) {
                if (!GeoDataDocumentWriter::write(outputFile, *tile)) {
                    qWarning() << "Failed to write tile" << outputFile;
                }
            }
        }
    }

}

GeoDataLatLonBox TileDirectory::boundingBox(const QString &filename) const
{
    QProcess osmconvert;
    osmconvert.start("osmconvert", QStringList() << "--out-statistics" << filename);
    osmconvert.waitForFinished();
    QStringList const output = QString(osmconvert.readAllStandardOutput()).split('\n');
    GeoDataLatLonBox boundingBox;
    foreach(QString const &line, output) {
        if (line.startsWith("lon min:")) {
            boundingBox.setWest(line.mid(8).toDouble(), GeoDataCoordinates::Degree);
        } else if (line.startsWith("lon max")) {
            boundingBox.setEast(line.mid(8).toDouble(), GeoDataCoordinates::Degree);
        } else if (line.startsWith("lat min:")) {
            boundingBox.setSouth(line.mid(8).toDouble(), GeoDataCoordinates::Degree);
        } else if (line.startsWith("lat max:")) {
            boundingBox.setNorth(line.mid(8).toDouble(), GeoDataCoordinates::Degree);
        }
    }
    return boundingBox;
}

}
