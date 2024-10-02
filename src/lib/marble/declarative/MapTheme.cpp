#include "MapTheme.h"
#include <MarbleModel.h>
#include <MarbleQuickItem.h>

#include <GeoSceneDocument.h>
#include <GeoSceneHead.h>
#include <GeoSceneLicense.h>

namespace Marble
{
MapTheme::MapTheme(QObject *parent)
    : QObject(parent)
    , m_map(nullptr)
{
}

Marble::MarbleQuickItem *MapTheme::map() const
{
    return m_map;
}

QString MapTheme::license() const
{
    const GeoSceneDocument *const mapTheme = m_map->model()->mapTheme();
    if (!mapTheme)
        return {};

    const GeoSceneHead *const head = mapTheme->head();
    if (!head)
        return {};

    const GeoSceneLicense *license = head->license();
    return license->shortLicense();
}

void MapTheme::setMap(MarbleQuickItem *map)
{
    if (m_map == map)
        return;
    disconnect(m_map, &MarbleQuickItem::mapThemeIdChanged, this, &MapTheme::licenseChanged);
    connect(map, &MarbleQuickItem::mapThemeIdChanged, this, &MapTheme::licenseChanged);

    m_map = map;

    Q_EMIT mapChanged(m_map);
    Q_EMIT licenseChanged();
}
}

#include "moc_MapTheme.cpp"
