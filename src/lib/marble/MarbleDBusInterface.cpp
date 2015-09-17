#include "MarbleDBusInterface.h"

#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MapThemeManager.h"
#include <GeoSceneDocument.h>
#include <GeoSceneSettings.h>
#include <GeoSceneProperty.h>

namespace Marble
{

class Q_DECL_HIDDEN MarbleDBusInterface::Private
{
public:
    Private( MarbleWidget* widget );

    MarbleWidget* m_marbleWidget;
    QPointF m_currentCenter;
};

MarbleDBusInterface::Private::Private( MarbleWidget *widget ) :
    m_marbleWidget( widget )
{
    // nothing to do
}

MarbleDBusInterface::MarbleDBusInterface( MarbleWidget* widget ) :
    QDBusAbstractAdaptor( widget ),
    d( new Private( widget ) )
{
    connect( widget, SIGNAL(themeChanged(QString)), this, SIGNAL(mapThemeChanged(QString)) );
    connect( widget, SIGNAL(tileLevelChanged(int)), this, SIGNAL(tileLevelChanged(int)) );
    connect( widget, SIGNAL(zoomChanged(int)), this, SIGNAL(zoomChanged(int)) );
    connect( widget, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
             this, SLOT(handleVisibleLatLonAltBoxChange()) );
}

MarbleDBusInterface::~MarbleDBusInterface()
{
    // nothing to do
    // (an explicit destructor is required by QScopePointer)
}

QString MarbleDBusInterface::mapTheme() const
{
    return d->m_marbleWidget->mapThemeId();
}

QStringList MarbleDBusInterface::mapThemes() const
{
    MapThemeManager mapThemeManager;
    return mapThemeManager.mapThemeIds();
}

int MarbleDBusInterface::tileLevel() const
{
    return d->m_marbleWidget->tileZoomLevel();
}

int MarbleDBusInterface::zoom() const
{
    return d->m_marbleWidget->zoom();
}

QPointF MarbleDBusInterface::center() const
{
    return d->m_currentCenter;
}

void MarbleDBusInterface::setMapTheme( const QString &mapTheme )
{
    d->m_marbleWidget->setMapThemeId( mapTheme );
}

void MarbleDBusInterface::setZoom(int zoom)
{
    d->m_marbleWidget->setZoom(zoom);
}

void MarbleDBusInterface::setPropertyEnabled(const QString &key, bool enabled )
{
    d->m_marbleWidget->setPropertyValue( key, enabled );
}

bool MarbleDBusInterface::isPropertyEnabled( const QString &key ) const
{
    bool value = false;
    GeoSceneDocument const * const mapTheme = d->m_marbleWidget->model()->mapTheme();
    if ( mapTheme ) {
        mapTheme->settings()->propertyValue( key, value );
    }

    return value;
}

QStringList MarbleDBusInterface::properties() const
{
    QStringList properties;
    GeoSceneDocument const * const mapTheme = d->m_marbleWidget->model()->mapTheme();
    if ( mapTheme ) {
        foreach( const GeoSceneProperty* property, mapTheme->settings()->allProperties() ) {
            properties << property->name();
        }
    }
    return properties;
}

void MarbleDBusInterface::setCenter( const QPointF &center ) const
{
    d->m_marbleWidget->centerOn( center.x(), center.y() );
}

void MarbleDBusInterface::handleVisibleLatLonAltBoxChange()
{
    QPointF const newCenter = QPointF( d->m_marbleWidget->centerLongitude(),
                                       d->m_marbleWidget->centerLatitude() );
    if ( newCenter != d->m_currentCenter ) {
        d->m_currentCenter = newCenter;
        emit centerChanged( d->m_currentCenter );
    }
}

}

#include "moc_MarbleDBusInterface.cpp"
