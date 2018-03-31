//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "SatellitesPlugin.h"

#include "MarbleDebug.h"
#include "MarbleWidget.h"
#include "MarbleWidgetPopupMenu.h"
#include "MarbleModel.h"
#include "GeoDataPlacemark.h"
#include "SatellitesMSCItem.h"
#include "SatellitesTLEItem.h"
#include "SatellitesConfigLeafItem.h"
#include "SatellitesConfigNodeItem.h"
#include "SatellitesConfigModel.h"
#include "ViewportParams.h"

#include "ui_SatellitesConfigDialog.h"

#include <QAction>
#include <QUrl>
#include <QMouseEvent>

namespace Marble
{

SatellitesPlugin::SatellitesPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
     m_satModel( nullptr ),
     m_isInitialized( false ),
     m_configDialog(nullptr)
{
    connect( this, SIGNAL(settingsChanged(QString)), SLOT(updateSettings()) );
    connect( this, SIGNAL(enabledChanged(bool)), SLOT(enableModel(bool)) );
    connect( this, SIGNAL(visibilityChanged(bool,QString)), SLOT(visibleModel(bool)) );

    setVisible( false );
    setSettings(QHash<QString, QVariant>());

    m_showOrbitAction = new QAction( tr( "Display orbit" ), this );
    m_showOrbitAction->setCheckable( true );
    m_showOrbitAction->setData( 0 );

    m_trackPlacemarkAction = new QAction( tr( "Keep centered" ), this );
    m_trackPlacemarkAction->setData( 0 );
    connect( m_showOrbitAction, SIGNAL(triggered(bool)), SLOT(showOrbit(bool)) );
    connect( m_trackPlacemarkAction, SIGNAL(triggered(bool)), SLOT(trackPlacemark()) );
}

SatellitesPlugin::~SatellitesPlugin()
{
    delete m_satModel;

    delete m_configDialog;
    delete m_showOrbitAction;
    delete m_trackPlacemarkAction;
}

QStringList SatellitesPlugin::backendTypes() const
{
    return QStringList(QStringLiteral("satellites"));
}

QString SatellitesPlugin::renderPolicy() const
{
    return QStringLiteral("ALWAYS");
}

QStringList SatellitesPlugin::renderPosition() const
{
    return QStringList(QStringLiteral("ORBIT"));
}

QString SatellitesPlugin::name() const
{
    return tr( "Satellites" );
}

QString SatellitesPlugin::nameId() const
{
    return QStringLiteral("satellites");
}

QString SatellitesPlugin::guiString() const
{
    return tr( "&Satellites" );
}

QString SatellitesPlugin::version() const
{
    return QStringLiteral("2.0");
}

QString SatellitesPlugin::description() const
{
    return tr( "This plugin displays satellites and their orbits." );
}

QString SatellitesPlugin::copyrightYears() const
{
    return QStringLiteral("2012");
}

QVector<PluginAuthor> SatellitesPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Guillaume Martres"), QStringLiteral("smarter@ubuntu.com"))
            << PluginAuthor(QStringLiteral("Rene Kuettner"), QStringLiteral("rene@bitkanal.net"))
            << PluginAuthor(QStringLiteral("Gerhard Holtkamp"), QString());
}

QString SatellitesPlugin::aboutDataText() const
{
    return tr(
        "Earth-Satellites orbital elements from <ul><li>"
        "<a href=\"http://www.celestrak.com\">http://www.celestrak.com</a>"
        "</li></ul>"
        "Planetary-Satellites orbital elements from <ul><li>"
        "<a href=\"http://tasc.esa.int/\">ESA TASC service</a></li><li>"
        "<a href=\"http://ssd.jpl.nasa.gov/?horizons\">"
        "JPL Horizons</a></li></ul>" );
}

QIcon SatellitesPlugin::icon() const
{
    return QIcon(QStringLiteral(":/data/bitmaps/satellite.png"));
}

RenderPlugin::RenderType SatellitesPlugin::renderType() const
{
    return OnlineRenderType;
}

void SatellitesPlugin::initialize()
{
    // FIXME: remove the const_cast, it may be best to create a new type of
    // plugins where marbleModel() is not const, since traditional
    // RenderPlugins do not require that
    m_satModel = new SatellitesModel(
        const_cast<MarbleModel *>( marbleModel() )->treeModel(),
        marbleModel()->clock() );

    m_configModel = new SatellitesConfigModel( this );

    delete m_configDialog;
    m_configDialog = new SatellitesConfigDialog();
    connect( m_configDialog, SIGNAL(activatePluginClicked()), this, SLOT(activate()) );
    connect( this, SIGNAL(visibilityChanged(bool,QString)),
             m_configDialog, SLOT(setDialogActive(bool)) );
    m_configDialog->configWidget()->treeView->setModel( m_configModel );

    connect( m_satModel, SIGNAL(fileParsed(QString)),
        SLOT(dataSourceParsed(QString)) );
    connect( m_satModel, SIGNAL(fileParsed(QString)),
        SLOT(updateDataSourceConfig(QString)) );
    connect( m_configDialog, SIGNAL(dataSourcesReloadRequested()),
        SLOT(updateSettings()) );
    connect( m_configDialog, SIGNAL(accepted()), SLOT(writeSettings()) );
    connect( m_configDialog, SIGNAL(rejected()), SLOT(readSettings()) );
    connect( m_configDialog->configWidget()->buttonBox->button(
        QDialogButtonBox::Reset ),
        SIGNAL(clicked()), SLOT(restoreDefaultSettings()) );
    connect( m_configDialog, SIGNAL(userDataSourcesChanged()),
        SLOT(writeSettings()) );
    connect( m_configDialog, SIGNAL(userDataSourceAdded(QString)),
        SLOT(userDataSourceAdded(QString)) );

    m_isInitialized = true;
    readSettings();
    updateSettings();
    enableModel( enabled() );
}

bool SatellitesPlugin::isInitialized() const
{
    return m_isInitialized;
}

bool SatellitesPlugin::render( GeoPainter *painter, ViewportParams *viewport,
    const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( painter );
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    enableModel( enabled() );

    return true;
}

bool SatellitesPlugin::eventFilter( QObject *object, QEvent *event )
{
    // only if active plugin
    if( !enabled() || !visible() ) {
        return false;
    }

    if( event->type() != QEvent::MouseButtonPress )
    {
        return false;
    }

    MarbleWidget *widget = qobject_cast<MarbleWidget*> ( object );
    Q_ASSERT ( widget );

    QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
    Q_ASSERT( mouseEvent );

    if( mouseEvent->button() == Qt::LeftButton ) {
        m_trackerList.clear();
        QVector<const GeoDataFeature*> vector = widget->whichFeatureAt( mouseEvent->pos() );
        for (const GeoDataFeature *feature: vector) {
            const GeoDataPlacemark* placemark = dynamic_cast<const GeoDataPlacemark*>(feature);
            if ( placemark ) {
                for (TrackerPluginItem *obj: m_satModel->items() ) {
                    if( obj->placemark() == placemark ) {
                        m_showOrbitAction->data() = m_trackerList.size();
                        m_showOrbitAction->setChecked( obj->isTrackVisible() );
                        widget->popupMenu()->addAction( Qt::LeftButton, m_showOrbitAction );

                        m_trackPlacemarkAction->data() = m_trackerList.size();
                        widget->popupMenu()->addAction( Qt::LeftButton, m_trackPlacemarkAction );

                        m_trackerList.append( obj );
                    }
                }
            }
        }
    }
    return false;
}

void SatellitesPlugin::showOrbit(bool show)
{
    QAction *action = qobject_cast<QAction *>( sender() );
    Q_ASSERT( action );

    int actionIndex = action->data().toInt();
    TrackerPluginItem *item = m_trackerList.at( actionIndex );
    item->setTrackVisible( show );
    m_satModel->updateVisibility();
}

void SatellitesPlugin::trackPlacemark()
{
    QAction *action = qobject_cast<QAction *>( sender() );
    Q_ASSERT( action );

    int actionIndex = action->data().toInt();
    TrackerPluginItem *item = m_trackerList.at( actionIndex );
    const_cast<MarbleModel *>( marbleModel() )->setTrackedPlacemark( item->placemark() );
}

QHash<QString, QVariant> SatellitesPlugin::settings() const
{
    QHash<QString, QVariant> result = RenderPlugin::settings();

    typedef QHash<QString, QVariant>::ConstIterator Iterator;
    Iterator end = m_settings.constEnd();
    for ( Iterator iter = m_settings.constBegin(); iter != end; ++iter ) {
        result.insert( iter.key(), iter.value() );
    }

    return result;
}

void SatellitesPlugin::setSettings( const QHash<QString, QVariant> &settings )
{
    RenderPlugin::setSettings( settings );

    // reset
    m_newDataSources.clear();
    // TODO: cancel also all on-going downloads

    // add default data sources
    if (!settings.contains(QStringLiteral("dataSources"))) {
        QStringList dsList;
        dsList << QStringLiteral("http://www.celestrak.com/NORAD/elements/visual.txt");
        m_settings.insert(QStringLiteral("dataSources"), dsList);
        m_settings.insert(QStringLiteral("idList"), dsList);
    }
    else {
        // HACK: KConfig can't guess the type of the settings, when we use
        // KConfigGroup::readEntry() in marble_part it returns a QString which
        // is then wrapped into a QVariant when added to the settings hash.
        // QVariant can handle the conversion for some types, like toDateTime()
        // but when calling toStringList() on a QVariant::String, it will
        // return a one element list
        if (settings.value(QStringLiteral("dataSources")).type() == QVariant::String) {
            m_settings.insert(QStringLiteral("dataSources"),
                settings.value(QStringLiteral("dataSources")).toString().split(QLatin1Char(',')));
        }
        if (settings.value(QStringLiteral("idList")).type() == QVariant::String) {
            m_settings.insert(QStringLiteral("idList"),
                settings.value(QStringLiteral("idList")).toString().split(QLatin1Char(',')));
        }
    }

    // add default user data source
    if (!settings.contains(QStringLiteral("userDataSources"))) {
        QStringList udsList;
        udsList << QStringLiteral("http://files.kde.org/marble/satellites/PlanetarySatellites.msc");
        m_settings.insert(QStringLiteral("userDataSources"), udsList);
        userDataSourceAdded( udsList[0] );
    }
    else if (settings.value(QStringLiteral("userDataSources")).type() == QVariant::String) {
        // same HACK as above
        m_settings.insert(QStringLiteral("userDataSources"),
            settings.value(QStringLiteral("userDataSources")).toString().split(QLatin1Char(',')));
    }

    emit settingsChanged( nameId() );
}

void SatellitesPlugin::readSettings()
{
    m_configDialog->setUserDataSources(
        m_settings.value(QStringLiteral("userDataSources")).toStringList());
    m_configModel->loadSettings( m_settings );
    m_satModel->loadSettings( m_settings );
}

void SatellitesPlugin::writeSettings()
{
    m_settings.insert(QStringLiteral("userDataSources"), m_configDialog->userDataSources());
    m_settings.insert(QStringLiteral("dataSources"), m_configModel->urlList());
    m_settings.insert(QStringLiteral("idList"), m_configModel->idList());

    emit settingsChanged( nameId() );
}

void SatellitesPlugin::updateSettings()
{
    if (!isInitialized()) {
        return;
    }

    // TODO: cancel also all on-going downloads
    m_satModel->clear();
    
    m_configModel->clear();
    addBuiltInDataSources();

    // (re)load data sources
    QStringList dsList = m_settings[QStringLiteral("dataSources")].toStringList();
    dsList << m_settings[QStringLiteral("userDataSources")].toStringList();
    dsList.removeDuplicates();
    for( const QString &ds: dsList ) {
        mDebug() << "Loading satellite data from:" << ds;
        m_satModel->downloadFile( QUrl( ds ), ds );
    }
}

void SatellitesPlugin::dataSourceParsed( const QString &source )
{
    m_configDialog->setUserDataSourceLoaded( source, true );
}

void SatellitesPlugin::userDataSourceAdded( const QString &source )
{
    // items contained in catalog data sources are not known before
    // the catalog has been parsed. so we store new data sources in
    // order to activate them later (new datasources are enabled by
    // default)
    if( !m_newDataSources.contains( source ) ) {
        m_newDataSources.append( source );
    }
}

SatellitesConfigDialog *SatellitesPlugin::configDialog()
{
    return m_configDialog;
}

void SatellitesPlugin::activate()
{
    action()->trigger();
}

void SatellitesPlugin::enableModel( bool enabled )
{
    if ( !m_isInitialized ) {
        return;
    }

    m_satModel->setPlanet( marbleModel()->planetId() );
    m_satModel->enable( enabled && visible() );
}

void SatellitesPlugin::visibleModel( bool visible )
{
    if ( !m_isInitialized ) {
        return;
    }

    m_satModel->setPlanet( marbleModel()->planetId() );
    m_satModel->enable( enabled() && visible );
}

void SatellitesPlugin::updateDataSourceConfig( const QString &source )
{
    mDebug() << "Updating orbiter configuration";

    for( TrackerPluginItem *obj: m_satModel->items() ) {
        // catalog items
        SatellitesMSCItem *item = dynamic_cast<SatellitesMSCItem*>( obj );
        if( ( item != nullptr ) && ( item->catalog() == source ) ) {
            m_configDialog->addSatelliteItem(
                item->relatedBody(),
                item->category(),
                item->name(),
                item->id() );
        }
    }

    // activate new datasources by default
    if( m_newDataSources.contains( source ) ) {
        m_newDataSources.removeAll( source );
        activateDataSource( source );
    }

    readSettings();
    m_configDialog->update();
}

void SatellitesPlugin::activateDataSource( const QString &source )
{
    // activate the given data source (select it)
    mDebug() << "Activating Data Source:" << source;
    QStringList list = m_configModel->fullIdList().filter( source );
    QStringList idList = m_settings[QStringLiteral("idList")].toStringList();
    idList << list;
    m_settings.insert(QStringLiteral("idList"), idList);
}

void SatellitesPlugin::addBuiltInDataSources()
{
    QString currentCategory;

    currentCategory = tr("Special-Interest Satellites" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Last 30 Days' Launches", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/tle-new.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Space Stations", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/stations.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "100 (or so) Brightest", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/visual.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "FENGYUN 1C Debris", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/1999-025.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "IRIDIUM 33 Debris", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/iridium-33-debris.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "COSMOS 2251 Debris", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/cosmos-2251-debris.txt" );

    currentCategory = tr( "Weather & Earth Resources Satellites" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Weather", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/weather.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "NOAA", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/noaa.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "GOES", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/goes.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Earth Resources", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/resource.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Search & Rescue (SARSAT)", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/sarsat.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Disaster Monitoring", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/dmc.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Tracking and Data Relay Satellite System (TDRSS)", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/tdrss.txt" );

    currentCategory = tr( "Communications Satellites" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Geostationary", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/geo.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Intelsat", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/intelsat.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Gorizont", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/gorizont.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Raduga", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/raduga.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Molniya", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/molniya.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Iridium", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/iridium.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Orbcomm", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/orbcomm.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Globalstar", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/globalstar.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Amateur radio", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/amateur.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Experimental", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/x-comm.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Other", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/other-comm.txt" );

    currentCategory = tr( "Navigation Satellites" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "GPS Operational", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/gps-ops.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Glonass Operational", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/glo-ops.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Galileo", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/galileo.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Satellite-Based Augmentation System (WAAS/EGNOS/MSAS)", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/sbas.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Navy Navigation Satellite System (NNSS)", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/nnss.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Russian LEO Navigation", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/musson.txt" );

    currentCategory = tr( "Scientific Satellites" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Space & Earth Science", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/science.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Geodetic", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/geodetic.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Engineering", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/engineering.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Education", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/education.txt" );

    currentCategory = tr( "Miscellaneous Satellites" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Miscellaneous Military", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/military.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Radar Calibration", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/radar.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "CubeSats", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/cubesat.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Other", "Name of a satellite group" ), "http://www.celestrak.com/NORAD/elements/other.txt" );

    readSettings();
    m_configDialog->update();
}

} // namespace Marble

#include "moc_SatellitesPlugin.cpp"

