//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingManager.h"

#include "AlternativeRoutesModel.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "RouteRequest.h"
#include "RoutingModel.h"
#include "RoutingProfilesModel.h"
#include "MarbleRunnerManager.h"
#include "RunnerPlugin.h"
#include "AutoNavigation.h"
#include "GeoWriter.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "GeoDataFolder.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "PositionTracking.h"
#include "PluginManager.h"
#include "PositionProviderPlugin.h"

#include <QtCore/QFile>
#include <QtGui/QMessageBox>
#include <QtGui/QCheckBox>
#include <QMutexLocker>

namespace Marble
{

class RoutingManagerPrivate
{
public:
    RoutingManager* q;

    RouteRequest m_routeRequest;

    RoutingModel m_routingModel;

    RoutingProfilesModel m_profilesModel;

    MarbleModel *const m_marbleModel;

    AlternativeRoutesModel m_alternativeRoutesModel;

    MarbleRunnerManager m_runnerManager;

    bool m_haveRoute;

    AutoNavigation *m_adjustNavigation;

    bool m_guidanceModeEnabled;

    QMutex m_fileMutex;

    bool m_shutdownPositionTracking;

    bool m_guidanceModeWarning;

    QString m_lastOpenPath;

    QString m_lastSavePath;

    QColor m_routeColorStandard;

    QColor m_routeColorHighlighted;

    QColor m_routeColorAlternative;

    RoutingManagerPrivate( MarbleModel *marbleModel, RoutingManager* manager, QObject *parent );

    GeoDataFolder* routeRequest() const;

    QString stateFile( const QString &name = QString( "route.kml" ) ) const;

    void saveRoute( const QString &filename );

    void loadRoute( const QString &filename );

    void addRoute( GeoDataDocument* route );

    void recalculateRoute( bool deviated );
};

RoutingManagerPrivate::RoutingManagerPrivate( MarbleModel *model, RoutingManager* manager, QObject *parent ) :
        q( manager ),
        m_routeRequest( manager ),
        m_routingModel( &m_routeRequest, model, manager ),
        m_profilesModel( model->pluginManager() ),
        m_marbleModel( model ),
        m_alternativeRoutesModel( parent ),
        m_runnerManager( model->pluginManager(), q ),
        m_haveRoute( false ),
        m_adjustNavigation( 0 ),
        m_guidanceModeEnabled( false ),
        m_shutdownPositionTracking( false ),
        m_guidanceModeWarning( true ),
        m_routeColorStandard   ( oxygenSkyBlue4 ),
        m_routeColorHighlighted( oxygenSeaBlue2 ),
        m_routeColorAlternative( oxygenAluminumGray4 )
{
    m_runnerManager.setModel( model );
    m_routeColorStandard.setAlpha( 200 );
    m_routeColorHighlighted.setAlpha( 200 );
    m_routeColorAlternative.setAlpha( 200 );
}

GeoDataFolder* RoutingManagerPrivate::routeRequest() const
{
    GeoDataFolder* result = new GeoDataFolder;
    result->setName( "Route Request" );
    for ( int i=0; i<m_routeRequest.size(); ++i ) {
        GeoDataPlacemark* placemark = new GeoDataPlacemark;
        placemark->setName( m_routeRequest.name( i ) );
        placemark->setCoordinate( GeoDataPoint( m_routeRequest.at( i ) ) );
        result->append( placemark );
    }

    return result;
}

QString RoutingManagerPrivate::stateFile( const QString &name) const
{
    QString const subdir = "routing";
    QDir dir( MarbleDirs::localPath() );
    if ( !dir.exists( subdir ) ) {
        if ( !dir.mkdir( subdir ) ) {
            mDebug() << "Unable to create dir " << dir.absoluteFilePath( subdir );
            return dir.absolutePath();
        }
    }

    if ( !dir.cd( subdir ) ) {
        mDebug() << "Cannot change into " << dir.absoluteFilePath( subdir );
    }

    return dir.absoluteFilePath( name );
}

void RoutingManagerPrivate::saveRoute(const QString &filename)
{
    GeoWriter writer;
    writer.setDocumentType( "http://earth.google.com/kml/2.2" );

    QMutexLocker locker( &m_fileMutex );
    QFile file( filename );
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        mDebug() << "Cannot write to " << file.fileName();
        return;
    }

    GeoDataDocument container;
    GeoDataFolder* request = routeRequest();
    if ( request ) {
        container.append( request );
    }

    GeoDataDocument *route = m_alternativeRoutesModel.currentRoute();
    if ( route ) {
        container.append( new GeoDataDocument( *route ) );
    }

    if ( !writer.write( &file, &container ) ) {
        mDebug() << "Can not write route state to " << file.fileName();
    }
    file.close();
}

void RoutingManagerPrivate::loadRoute(const QString &filename)
{
    QFile file( filename );
    if ( !file.open( QIODevice::ReadOnly ) ) {
        mDebug() << "Can not read route from " << file.fileName();
        return;
    }

    GeoDataParser parser( GeoData_KML );
    if ( !parser.read( &file ) ) {
        mDebug() << "Could not parse file: " << parser.errorString();
        return;
    }

    GeoDocument *doc = parser.releaseDocument();
    file.close();
    bool loaded = false;

    GeoDataDocument* container = dynamic_cast<GeoDataDocument*>( doc );
    if ( container && container->size() > 0 ) {
        GeoDataFolder* viaPoints = dynamic_cast<GeoDataFolder*>( &container->first() );
        if ( viaPoints ) {
            loaded = true;
            QVector<GeoDataPlacemark*> placemarks = viaPoints->placemarkList();
            for( int i=0; i<placemarks.size(); ++i ) {
                if ( i < m_routeRequest.size() ) {
                    m_routeRequest.setPosition( i, placemarks[i]->coordinate() );
                } else {
                    m_routeRequest.append( placemarks[i]->coordinate() );
                }
                m_routeRequest.setName( m_routeRequest.size()-1, placemarks[i]->name() );
            }

            for ( int i=placemarks.size(); i<m_routeRequest.size(); ++i ) {
                m_routeRequest.remove( i );
            }
        } else {
            mDebug() << "Expected a GeoDataDocument with at least one child, didn't get one though";
        }
    }

    if ( container && container->size() == 2 ) {
        GeoDataDocument* route = dynamic_cast<GeoDataDocument*>(&container->last());
        if ( route ) {
            loaded = true;
            m_alternativeRoutesModel.addRoute( route, AlternativeRoutesModel::Instant );
            m_alternativeRoutesModel.setCurrentRoute( 0 );
        } else {
            mDebug() << "Expected a GeoDataDocument child, didn't get one though";
        }
    }

    if ( !loaded ) {
        mDebug() << "File " << filename << " is not a valid Marble route .kml file";
        delete doc;
        m_marbleModel->addGeoDataFile( filename );
    }
}

RoutingManager::RoutingManager( MarbleModel *marbleModel, QObject *parent ) : QObject( parent ),
        d( new RoutingManagerPrivate( marbleModel, this, this ) )
{
    connect( &d->m_runnerManager, SIGNAL( routeRetrieved( GeoDataDocument* ) ),
             this, SLOT( addRoute( GeoDataDocument* ) ) );
    connect( &d->m_alternativeRoutesModel, SIGNAL( currentRouteChanged( GeoDataDocument* ) ),
             &d->m_routingModel, SLOT( setCurrentRoute( GeoDataDocument* ) ) );
    connect( &d->m_routingModel, SIGNAL( deviatedFromRoute( bool ) ),
             this, SLOT( recalculateRoute( bool ) ) );
}

RoutingManager::~RoutingManager()
{
    delete d;
}

RoutingProfilesModel *RoutingManager::profilesModel()
{
    return &d->m_profilesModel;
}

RoutingModel *RoutingManager::routingModel()
{
    return &d->m_routingModel;
}

RouteRequest* RoutingManager::routeRequest()
{
    return &d->m_routeRequest;
}

void RoutingManager::retrieveRoute()
{
    d->m_haveRoute = false;

    int realSize = 0;
    for ( int i = 0; i < d->m_routeRequest.size(); ++i ) {
        // Sort out dummy targets
        if ( d->m_routeRequest.at( i ).longitude() != 0.0 && d->m_routeRequest.at( i ).latitude() != 0.0 ) {
            ++realSize;
        }
    }

    d->m_alternativeRoutesModel.newRequest( &d->m_routeRequest );
    if ( realSize > 1 ) {
        emit stateChanged( RoutingManager::Downloading, &d->m_routeRequest );
        d->m_runnerManager.retrieveRoute( &d->m_routeRequest );
    } else {
        d->m_routingModel.clear();
        emit stateChanged( RoutingManager::Retrieved, &d->m_routeRequest );
    }
}

void RoutingManagerPrivate::addRoute( GeoDataDocument* route )
{
    if ( route ) {
        m_alternativeRoutesModel.addRoute( route );
    }

    if ( !m_haveRoute ) {
        m_haveRoute = route != 0;
        emit q->stateChanged( RoutingManager::Retrieved, &m_routeRequest );
    }

    emit q->routeRetrieved( route );
}

AlternativeRoutesModel* RoutingManager::alternativeRoutesModel()
{
    return &d->m_alternativeRoutesModel;
}

void RoutingManager::setAutoNavigation( AutoNavigation* adjustNavigation )
{
    d->m_adjustNavigation = adjustNavigation;
}

const AutoNavigation* RoutingManager::adjustNavigation() const
{
    return d->m_adjustNavigation;
}

void RoutingManager::writeSettings() const
{
    d->saveRoute( d->stateFile() );
}

void RoutingManager::saveRoute( const QString &filename ) const
{
    d->saveRoute( filename );
}

void RoutingManager::loadRoute( const QString &filename )
{
    d->loadRoute( filename );
}

RoutingProfile RoutingManager::defaultProfile( RoutingProfile::TransportType transportType ) const
{
    RoutingProfile profile;
    RoutingProfilesModel::ProfileTemplate tpl = RoutingProfilesModel::CarFastestTemplate;
    switch ( transportType ) {
    case RoutingProfile::Motorcar:
        tpl = RoutingProfilesModel::CarFastestTemplate;
        profile.setName( "Motorcar" );
        profile.setTransportType( RoutingProfile::Motorcar );
        break;
    case RoutingProfile::Bicycle:
        tpl = RoutingProfilesModel::BicycleTemplate;
        profile.setName( "Bicycle" );
        profile.setTransportType( RoutingProfile::Bicycle );
        break;
    case RoutingProfile::Pedestrian:
        tpl = RoutingProfilesModel::PedestrianTemplate;
        profile.setName( "Pedestrian" );
        profile.setTransportType( RoutingProfile::Pedestrian );
        break;
    }

    const PluginManager* pluginManager = d->m_marbleModel->pluginManager();
    foreach( RunnerPlugin* plugin, pluginManager->runnerPlugins() ) {
        if ( !plugin->supports( RunnerPlugin::Routing ) ) {
            continue;
        }

        if ( plugin->supportsTemplate( tpl ) ) {
            profile.pluginSettings()[plugin->nameId()] = plugin->templateSettings( tpl );
        }
    }

    return profile;
}

void RoutingManager::readSettings()
{
    d->loadRoute( d->stateFile() );
    if ( d->m_profilesModel.rowCount() ) {
        d->m_routeRequest.setRoutingProfile( d->m_profilesModel.profiles().at( 0 ) );
    }
}

void RoutingManager::setGuidanceModeEnabled( bool enabled )
{
    d->m_guidanceModeEnabled = enabled;

    if ( enabled ) {
        d->saveRoute( d->stateFile( "guidance.kml" ) );

        if ( d->m_guidanceModeWarning ) {
            QString text = "<p>" + tr( "Caution: Driving instructions may be incomplete or wrong." );
            text += " " + tr( "Road construction, weather and other unforeseen variables can result in the suggested route not to be the most expedient or safest route to your destination." );
            text += " " + tr( "Please use common sense while navigating." ) + "</p>";
            text += "<p>" + tr( "The Marble development team wishes you a pleasant and safe journey." ) + "</p>";
            QMessageBox messageBox( QMessageBox::Information, tr( "Guidance Mode - Marble" ), text, QMessageBox::Ok );
            QCheckBox showAgain( tr( "Show again" ) );
            showAgain.setChecked( true );
            showAgain.blockSignals( true ); // otherwise it'd close the dialog
            messageBox.addButton( &showAgain, QMessageBox::ActionRole );
            bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
            messageBox.resize( 380, smallScreen ? 400 : 240 );
            messageBox.exec();
            d->m_guidanceModeWarning = showAgain.isChecked();
        }
    } else {
        d->loadRoute( d->stateFile( "guidance.kml" ) );
    }

    PositionTracking* tracking = d->m_marbleModel->positionTracking();
    PositionProviderPlugin* plugin = tracking->positionProviderPlugin();
    if ( !plugin && enabled ) {
        const PluginManager* pluginManager = d->m_marbleModel->pluginManager();
        QList<PositionProviderPlugin*> plugins = pluginManager->createPositionProviderPlugins();
        if ( plugins.size() > 0 ) {
            plugin = plugins.takeFirst();
        }
        qDeleteAll( plugins );
        tracking->setPositionProviderPlugin( plugin );
        d->m_shutdownPositionTracking = true;
    } else if ( plugin && !enabled && d->m_shutdownPositionTracking ) {
        d->m_shutdownPositionTracking = false;
        tracking->setPositionProviderPlugin( 0 );
    }

    d->m_adjustNavigation->setAutoZoom( enabled );
    d->m_adjustNavigation->setRecenter( enabled ? AutoNavigation::RecenterOnBorder : AutoNavigation::DontRecenter );
}

void RoutingManagerPrivate::recalculateRoute( bool deviated )
{
    if ( m_guidanceModeEnabled && deviated ) {
        for ( int i=m_routeRequest.size()-3; i>=0; --i ) {
            if ( m_routeRequest.visited( i ) ) {
                m_routeRequest.remove( i );
            }
        }

        if ( m_routeRequest.size() == 2 && m_routeRequest.visited( 0 ) && !m_routeRequest.visited( 1 ) ) {
            m_routeRequest.setPosition( 0, m_marbleModel->positionTracking()->currentLocation() );
            q->retrieveRoute();
        } else if ( m_routeRequest.size() != 0 && !m_routeRequest.visited( m_routeRequest.size()-1 ) ) {
            m_routeRequest.insert( 0, m_marbleModel->positionTracking()->currentLocation() );
            q->retrieveRoute();
        }
    }
}

void RoutingManager::reverseRoute()
{
    d->m_routeRequest.reverse();
    retrieveRoute();
}

void RoutingManager::clearRoute()
{
    d->m_routeRequest.clear();
    retrieveRoute();
}

void RoutingManager::setShowGuidanceModeStartupWarning( bool show )
{
    d->m_guidanceModeWarning = show;
}

bool RoutingManager::showGuidanceModeStartupWarning() const
{
    return d->m_guidanceModeWarning;
}

void RoutingManager::setLastOpenPath( const QString &path )
{
    d->m_lastOpenPath = path;
}

QString RoutingManager::lastOpenPath() const
{
    return d->m_lastOpenPath;
}

void RoutingManager::setLastSavePath( const QString &path )
{
    d->m_lastSavePath = path;
}

QString RoutingManager::lastSavePath() const
{
    return d->m_lastSavePath;
}

void RoutingManager::setRouteColorStandard( QColor color )
{
    d->m_routeColorStandard = color;
}

QColor RoutingManager::routeColorStandard()
{
    return d->m_routeColorStandard;
}

void RoutingManager::setRouteColorHighlighted( QColor color )
{
    d->m_routeColorHighlighted = color;
}

QColor RoutingManager::routeColorHighlighted()
{
    return d->m_routeColorHighlighted;
}

void RoutingManager::setRouteColorAlternative( QColor color )
{
    d->m_routeColorAlternative = color;
}

QColor RoutingManager::routeColorAlternative()
{
    return d->m_routeColorAlternative;
}

} // namespace Marble

#include "RoutingManager.moc"
