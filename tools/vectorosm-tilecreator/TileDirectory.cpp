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

#include <QFileInfo>
#include <QDebug>

namespace Marble {

TileDirectory::TileDirectory(const QString &baseDir, ParsingRunnerManager &manager, QString const &extension) :
    m_baseDir(baseDir),
    m_manager(manager),
    m_zoomLevel(QFileInfo(baseDir).baseName().toInt()),
    m_tileX(-1),
    m_tileY(-1),
    m_extension(extension),
    m_filterTags(false)
{
    // nothing to do
}

QSharedPointer<GeoDataDocument> TileDirectory::load(int zoomLevel, int tileX, int tileY)
{
    int const zoomDiff = zoomLevel - m_zoomLevel;
    int const x = tileX >> zoomDiff;
    int const y = tileY >> zoomDiff;
    if (x == m_tileX && y == m_tileY) {
        return m_landmass;
    }

    m_tileX = x;
    m_tileY = y;
    QString const filename = QString("%1/%2/%3.%4").arg(m_baseDir).arg(x).arg(y).arg(m_extension);
    m_landmass = open(filename, m_manager);
    return m_landmass;
}

GeoDataDocument* TileDirectory::clip(int zoomLevel, int tileX, int tileY)
{
    QSharedPointer<GeoDataDocument> oldMap = m_landmass;
    load(zoomLevel, tileX, tileY);
    if (!m_clipper || oldMap != m_landmass) {
        GeoDataDocument* input = m_landmass.data();
        if (m_filterTags) {
            QStringList const tags = tagsFilteredIn(zoomLevel);
            if (zoomLevel < 17) {
                m_tagsFilter = QSharedPointer<TagsFilter>(new TagsFilter(m_landmass.data(), tags));
                input = m_tagsFilter->accepted();
            }
        }

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

void TileDirectory::setFilterTags(bool filter)
{
    m_filterTags = filter;
}

}
