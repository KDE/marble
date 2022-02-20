#include "MapTheme.h"
#include <MarbleQuickItem.h>
#include <MarbleModel.h>

#include <GeoSceneDocument.h>
#include <GeoSceneHead.h>
#include <GeoSceneLicense.h>

namespace Marble
{
    MapTheme::MapTheme(QObject *parent) :
        QObject( parent ),
        m_map(nullptr)
    {
    }

    Marble::MarbleQuickItem *MapTheme::map() const
    {
        return m_map;
    }

    QString MapTheme::license() const
    {
        const GeoSceneDocument *const mapTheme = m_map->model()->mapTheme();
        if ( !mapTheme )
            return QString();

        const GeoSceneHead *const head = mapTheme->head();
        if ( !head )
            return QString();

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

        emit mapChanged(m_map);
        emit licenseChanged();
    }
}
