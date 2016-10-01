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
    m_extension(extension)
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

}
