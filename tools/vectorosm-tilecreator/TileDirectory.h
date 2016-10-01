//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//


#include "VectorClipper.h"
#include "TagsFilter.h"
#include <TileId.h>

#include <QSharedPointer>
#include <ParsingRunnerManager.h>

namespace Marble {

class TileDirectory
{
public:
    enum TileType
    {
        Landmass,
        OpenStreetMap
    };

    TileDirectory(TileType tileType, const QString &baseDir, ParsingRunnerManager &manager, const QString &extension);
    QSharedPointer<GeoDataDocument> load(int zoomLevel, int tileX, int tileY);
    void setInputFile(const QString &filename);

    TileId tileFor(int zoomLevel, int tileX, int tileY) const;
    GeoDataDocument *clip(int zoomLevel, int tileX, int tileY);
    QString name() const;

    static QSharedPointer<GeoDataDocument> open(const QString &filename, ParsingRunnerManager &manager);
    GeoDataLatLonBox boundingBox(const QString &filename) const;
    GeoDataLatLonBox boundingBox() const;
    void setBoundingBox(const GeoDataLatLonBox &boundingBox);
    void createTiles() const;

private:
    QStringList tagsFilteredIn(int zoomLevel) const;
    void setTagZoomLevel(int zoomLevel);

    QString m_baseDir;
    ParsingRunnerManager &m_manager;
    QSharedPointer<GeoDataDocument> m_landmass;
    int m_zoomLevel;
    int m_tileX;
    int m_tileY;
    int m_tagZoomLevel;
    QString m_extension;
    QSharedPointer<VectorClipper> m_clipper;
    QSharedPointer<TagsFilter> m_tagsFilter;
    TileType m_tileType;
    QString m_inputFile;
    GeoDataLatLonBox m_boundingBox;
};

}
