// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010, 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

// Own
#include "ServerLayout.h"

#include "GeoDataLatLonBox.h"
#include "GeoSceneTileDataset.h"
#include "MarbleGlobal.h"
#include "TileId.h"

#include <QUrlQuery>

#include <cmath>

namespace Marble
{

ServerLayout::ServerLayout(GeoSceneTileDataset *textureLayer)
    : m_textureLayer(textureLayer)
{
}

ServerLayout::~ServerLayout() = default;

MarbleServerLayout::MarbleServerLayout(GeoSceneTileDataset *textureLayer)
    : ServerLayout(textureLayer)
{
}

QUrl MarbleServerLayout::downloadUrl(const QUrl &prototypeUrl, const TileId &id) const
{
    const QString path = QStringLiteral("%1/%2/%3/%3_%4.%5")
                             .arg(prototypeUrl.path())
                             .arg(id.zoomLevel())
                             .arg(id.y(), tileDigits, 10, QLatin1Char('0'))
                             .arg(id.x(), tileDigits, 10, QLatin1Char('0'))
                             .arg(m_textureLayer->fileFormat().toLower());

    QUrl url = prototypeUrl;
    url.setPath(path);

    return url;
}

QString MarbleServerLayout::name() const
{
    return QStringLiteral("Marble");
}

QString ServerLayout::sourceDir() const
{
    return m_textureLayer ? m_textureLayer->sourceDir() : QString();
}

OsmServerLayout::OsmServerLayout(GeoSceneTileDataset *textureLayer)
    : ServerLayout(textureLayer)
{
}

QUrl OsmServerLayout::downloadUrl(const QUrl &prototypeUrl, const TileId &id) const
{
    const QString suffix = m_textureLayer->fileFormat().toLower();
    const QString path = QStringLiteral("%1/%2/%3.%4").arg(id.zoomLevel()).arg(id.x()).arg(id.y()).arg(suffix);

    QUrl url = prototypeUrl;
    url.setPath(url.path() + path);

    return url;
}

QString OsmServerLayout::name() const
{
    return QStringLiteral("OpenStreetMap");
}

CustomServerLayout::CustomServerLayout(GeoSceneTileDataset *texture)
    : ServerLayout(texture)
{
}

QUrl CustomServerLayout::downloadUrl(const QUrl &prototypeUrl, const TileId &id) const
{
    const GeoDataLatLonBox bbox = m_textureLayer->tileProjection()->geoCoordinates(id);

    QString urlStr = prototypeUrl.toString(QUrl::DecodeReserved);

    urlStr.replace(QStringLiteral("{z}"), QString::number(id.zoomLevel()));
    urlStr.replace(QStringLiteral("{zoomLevel}"), QString::number(id.zoomLevel()));
    urlStr.replace(QStringLiteral("{x}"), QString::number(id.x()));
    urlStr.replace(QStringLiteral("{y}"), QString::number(id.y()));
    urlStr.replace(QStringLiteral("{west}"), QString::number(bbox.west(GeoDataCoordinates::Degree), 'f', 12));
    urlStr.replace(QStringLiteral("{south}"), QString::number(bbox.south(GeoDataCoordinates::Degree), 'f', 12));
    urlStr.replace(QStringLiteral("{east}"), QString::number(bbox.east(GeoDataCoordinates::Degree), 'f', 12));
    urlStr.replace(QStringLiteral("{north}"), QString::number(bbox.north(GeoDataCoordinates::Degree), 'f', 12));

    return QUrl(urlStr);
}

QString CustomServerLayout::name() const
{
    return QStringLiteral("Custom");
}

WmsServerLayout::WmsServerLayout(GeoSceneTileDataset *texture)
    : ServerLayout(texture)
{
}

QUrl WmsServerLayout::downloadUrl(const QUrl &prototypeUrl, const Marble::TileId &tileId) const
{
    const GeoDataLatLonBox box = m_textureLayer->tileProjection()->geoCoordinates(tileId);

    QUrlQuery url(prototypeUrl.query());
    url.addQueryItem(QStringLiteral("service"), QStringLiteral("WMS"));
    url.addQueryItem(QStringLiteral("request"), QStringLiteral("GetMap"));
    url.addQueryItem(QStringLiteral("version"), QStringLiteral("1.1.1"));
    if (!url.hasQueryItem(QStringLiteral("styles")))
        url.addQueryItem(QStringLiteral("styles"), {});
    if (!url.hasQueryItem(QStringLiteral("format"))) {
        url.addQueryItem(QStringLiteral("format"), QLatin1StringView("image/") + m_textureLayer->fileFormat().toLower());
    }
    if (!url.hasQueryItem(QStringLiteral("srs"))) {
        url.addQueryItem(QStringLiteral("srs"), epsgCode());
    }
    if (!url.hasQueryItem(QStringLiteral("layers")))
        url.addQueryItem(QStringLiteral("layers"), m_textureLayer->name());
    url.addQueryItem(QStringLiteral("width"), QString::number(m_textureLayer->tileSize().width()));
    url.addQueryItem(QStringLiteral("height"), QString::number(m_textureLayer->tileSize().height()));
    double west, south, east, north;
    if (m_textureLayer->tileProjectionType() == GeoSceneAbstractTileProjection::Mercator) {
        // Oddly enough epsg:3857 is measured in meters - so let's convert this accordingly
        west = (box.west(GeoDataCoordinates::Degree) * 20037508.34) / 180;
        south = 20037508.34 / M_PI * log(tan(((90 + box.south(GeoDataCoordinates::Degree)) * M_PI) / 360));
        east = (box.east(GeoDataCoordinates::Degree) * 20037508.34) / 180;
        north = 20037508.34 / M_PI * log(tan(((90 + box.north(GeoDataCoordinates::Degree)) * M_PI) / 360));
    } else {
        west = box.west(GeoDataCoordinates::Degree);
        south = box.south(GeoDataCoordinates::Degree);
        east = box.east(GeoDataCoordinates::Degree);
        north = box.north(GeoDataCoordinates::Degree);
    }

    url.addQueryItem(
        QStringLiteral("bbox"),
        QStringLiteral("%1,%2,%3,%4")
            .arg(QString::number(west, 'f', 12), QString::number(south, 'f', 12), QString::number(east, 'f', 12), QString::number(north, 'f', 12)));
    QUrl finalUrl = prototypeUrl;
    finalUrl.setQuery(url);
    return finalUrl;
}

QString WmsServerLayout::name() const
{
    return QStringLiteral("WebMapService");
}

QString WmsServerLayout::epsgCode() const
{
    switch (m_textureLayer->tileProjectionType()) {
    case GeoSceneAbstractTileProjection::Equirectangular:
        return QStringLiteral("EPSG:4326");
    case GeoSceneAbstractTileProjection::Mercator:
        return QStringLiteral("EPSG:3857");
    }

    Q_ASSERT(false); // not reached
    return {};
}

WmtsServerLayout::WmtsServerLayout(GeoSceneTileDataset *texture)
    : ServerLayout(texture)
{
}

QUrl WmtsServerLayout::downloadUrl(const QUrl &prototypeUrl, const Marble::TileId &id) const
{
    //    const GeoDataLatLonBox bbox = m_textureLayer->tileProjection()->geoCoordinates(id);

    QString urlStr = prototypeUrl.toString(QUrl::DecodeReserved);

    urlStr.replace(urlStr.indexOf(QLatin1StringView("{TileMatrix}")), QLatin1StringView("{TileMatrix}").size(), QString::number(id.zoomLevel()));
    urlStr.replace(urlStr.indexOf(QLatin1StringView("{TileRow}")), QLatin1StringView("{TileRow}").size(), QString::number(id.y()));
    urlStr.replace(urlStr.indexOf(QLatin1StringView("{TileCol}")), QLatin1StringView("{TileCol}").size(), QString::number(id.x()));
    return QUrl(urlStr);
}

QString WmtsServerLayout::name() const
{
    return QStringLiteral("WebMapTileService");
}

QString WmtsServerLayout::epsgCode() const
{
    switch (m_textureLayer->tileProjectionType()) {
    case GeoSceneAbstractTileProjection::Equirectangular:
        return QStringLiteral("EPSG:4326");
    case GeoSceneAbstractTileProjection::Mercator:
        return QStringLiteral("EPSG:3857");
    }

    Q_ASSERT(false); // not reached
    return {};
}

QuadTreeServerLayout::QuadTreeServerLayout(GeoSceneTileDataset *textureLayer)
    : ServerLayout(textureLayer)
{
}

QUrl QuadTreeServerLayout::downloadUrl(const QUrl &prototypeUrl, const Marble::TileId &id) const
{
    QString urlStr = prototypeUrl.toString(QUrl::DecodeReserved);

    urlStr.replace(QStringLiteral("{quadIndex}"), encodeQuadTree(id));

    return QUrl(urlStr);
}

QString QuadTreeServerLayout::name() const
{
    return QStringLiteral("QuadTree");
}

QString QuadTreeServerLayout::encodeQuadTree(const Marble::TileId &id)
{
    QString tileNum;

    for (int i = id.zoomLevel(); i >= 0; i--) {
        const int tileX = (id.x() >> i) % 2;
        const int tileY = (id.y() >> i) % 2;
        const int num = (2 * tileY) + tileX;

        tileNum += QString::number(num);
    }

    return tileNum;
}

TmsServerLayout::TmsServerLayout(GeoSceneTileDataset *textureLayer)
    : ServerLayout(textureLayer)
{
}

QUrl TmsServerLayout::downloadUrl(const QUrl &prototypeUrl, const TileId &id) const
{
    const QString suffix = m_textureLayer->fileFormat().toLower();
    // y coordinate in TMS start at the bottom of the map (South) and go upwards,
    // opposed to OSM which start at the top.
    //
    // https://wiki.osgeo.org/wiki/Tile_Map_Service_Specification
    int y_frombottom = (1 << id.zoomLevel()) - id.y() - 1;

    const QString path = QStringLiteral("%1/%2/%3.%4").arg(id.zoomLevel()).arg(id.x()).arg(y_frombottom).arg(suffix);
    QUrl url = prototypeUrl;
    url.setPath(url.path() + path);

    return url;
}

QString TmsServerLayout::name() const
{
    return QStringLiteral("TileMapService");
}

}
