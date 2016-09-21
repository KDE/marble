//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "RoutingManager.h"

#include "AlternativeRoutesModel.h"
#include "MarbleModel.h"
#include "RouteRequest.h"
#include "RoutingModel.h"
#include "RoutingProfilesModel.h"
#include "RoutingRunnerPlugin.h"
#include "GeoWriter.h"
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "GeoDataFolder.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTreeModel.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "PositionTracking.h"
#include "PluginManager.h"
#include "PositionProviderPlugin.h"
#include "Route.h"
#include "RoutingRunnerManager.h"
#include <KmlElementDictionary.h>

#include <QFile>
#include <QMessageBox>
#include <QCheckBox>
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

    RoutingManager::State m_state;

    const PluginManager *const m_pluginManager;

    GeoDataTreeModel *const m_treeModel;

    PositionTracking *const m_positionTracking;

    AlternativeRoutesModel m_alternativeRoutesModel;

    RoutingRunnerManager m_runnerManager;

    bool m_haveRoute;

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

    static QString stateFile( const QString &name = QString( "route.kml" ) );

    void saveRoute( const QString &filename );

    void loadRoute( const QString &filename );

    void addRoute( GeoDataDocument* route );

    void routingFinished();

    void setCurrentRoute( GeoDataDocument *route );

    void recalculateRoute( bool deviated );

    static void importPlacemark( RouteSegment &outline, QVector<RouteSegment> &segments, const GeoDataPlacemark *placemark );
};

RoutingManagerPrivate::RoutingManagerPrivate( MarbleModel *model, RoutingManager* manager, QObject *parent ) :
        q( manager ),
        m_routeRequest( manager ),
        m_routingModel( &m_routeRequest, model, manager ),
        m_profilesModel( model->pluginManager() ),
        m_state( RoutingManager::Retrieved ),
        m_pluginManager( model->pluginManager() ),
        m_treeModel( model->treeModel() ),
        m_positionTracking( model->positionTracking() ),
        m_alternativeRoutesModel( parent ),
        m_runnerManager( model, q ),
        m_haveRoute( false ),
        m_guidanceModeEnabled( false ),
        m_shutdownPositionTracking( false ),
        m_guidanceModeWarning( true ),
        m_routeColorStandard( Oxygen::skyBlue4 ),
        m_routeColorHighlighted( Oxygen::skyBlue1 ),
        m_routeColorAlternative( Oxygen::aluminumGray4 )
{
    m_routeColorStandard.setAlpha( 200 );
    m_routeColorHighlighted.setAlpha( 200 );
    m_routeColorAlternative.setAlpha( 200 );
}

GeoDataFolder* RoutingManagerPrivate::routeRequest() const
{
    GeoDataFolder* result = new GeoDataFolder;
    result->setName(QStringLiteral("Route Request"));
    for ( int i=0; i<m_routeRequest.size(); ++i ) {
        GeoDataPlacemark* placemark = new GeoDataPlacemark( m_routeRequest[i] );
        result->append( placemark );
    }

    return result;
}

QString RoutingManagerPrivate::stateFile( const QString &name)
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
    writer.setDocumentType( kml::kmlTag_nameSpaceOgc22 );

    QMutexLocker locker( &m_fileMutex );
    QFile file( filename );
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        mDebug() << "Cannot write to " << file.fileName();
        return;
    }

    GeoDataDocument container;
    container.setName(QStringLiteral("Route"));
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
                    m_routeRequest[i] = *placemarks[i];
                } else {
                    m_routeRequest.append( *placemarks[i] );
                }
            }

            // clear unneeded via points
            const int viaPoints_needed = placemarks.size();
            for ( int i = m_routeRequest.size(); i > viaPoints_needed; --i ) {
                m_routeRequest.remove( viaPoints_needed );
            }
        } else {
            mDebug() << "Expected a GeoDataDocument with at least one child, didn't get one though";
        }
    }

    if ( container && container->size() == 2 ) {
        GeoDataDocument* route = dynamic_cast<GeoDataDocument*>(&container->last());
        if ( route ) {
            loaded = true;
            m_alternativeRoutesModel.clear();
            m_alternativeRoutesModel.addRoute( route, AlternativeRoutesModel::Instant );
            m_alternativeRoutesModel.setCurrentRoute( 0 );
            m_state = RoutingManager::Retrieved;
            emit q->stateChanged( m_state );
            emit q->routeRetrieved( route );
        } else {
            mDebug() << "Expected a GeoDataDocument child, didn't get one though";
        }
    }

    if ( !loaded ) {
        mDebug() << "File " << filename << " is not a valid Marble route .kml file";
        if ( container ) {
            m_treeModel->addDocument( container );
        }
    }
}

RoutingManager::RoutingManager( MarbleModel *marbleModel, QObject *parent ) : QObject( parent ),
        d( new RoutingManagerPrivate( marbleModel, this, this ) )
{
    connect( &d->m_runnerManager, SIGNAL(routeRetrieved(GeoDataDocument*)),
             this, SLOT(addRoute(GeoDataDocument*)) );
    connect( &d->m_runnerManager, SIGNAL(routingFinished()),
             this, SLOT(routingFinished()) );
    connect( &d->m_alternativeRoutesModel, SIGNAL(currentRouteChanged(GeoDataDocument*)),
             this, SLOT(setCurrentRoute(GeoDataDocument*)) );
    connect( &d->m_routingModel, SIGNAL(deviatedFromRoute(bool)),
             this, SLOT(recalculateRoute(bool)) );
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

const RoutingModel *RoutingManager::routingModel() const
{
    return &d->m_routingModel;
}

RouteRequest* RoutingManager::routeRequest()
{
    return &d->m_routeRequest;
}

RoutingManager::State RoutingManager::state() const
{
    return d->m_state;
}

void RoutingManager::retrieveRoute()
{
    d->m_haveRoute = false;

    int realSize = 0;
    for ( int i = 0; i < d->m_routeRequest.size(); ++i ) {
        // Sort out dummy targets
        if ( d->m_routeRequest.at( i ).isValid() ) {
            ++realSize;
        }
    }

    d->m_alternativeRoutesModel.newRequest( &d->m_routeRequest );
    if ( realSize > 1 ) {
        d->m_state = RoutingManager::Downloading;
        d->m_runnerManager.retrieveRoute( &d->m_routeRequest );
    } else {
        d->m_routingModel.clear();
        d->m_state = RoutingManager::Retrieved;
    }
    emit stateChanged( d->m_state );
}

void RoutingManagerPrivate::addRoute( GeoDataDocument* route )
{
    if ( route ) {
        m_alternativeRoutesModel.addRoute( route );
    }

    if ( !m_haveRoute ) {
        m_haveRoute = route != 0;
    }

    emit q->routeRetrieved( route );
}

void RoutingManagerPrivate::routingFinished()
{
    m_state = RoutingManager::Retrieved;
    emit q->stateChanged( m_state );
}

void RoutingManagerPrivate::setCurrentRoute( GeoDataDocument *document )
{
    Route route;
    QVector<RouteSegment> segments;
    RouteSegment outline;

    QVector<GeoDataFolder*> folders = document->folderList();
    foreach( const GeoDataFolder *folder, folders ) {
        foreach( const GeoDataPlacemark *placemark, folder->placemarkList() ) {
            importPlacemark( outline, segments, placemark );
        }
    }

    foreach( const GeoDataPlacemark *placemark, document->placemarkList() ) {
        importPlacemark( outline, segments, placemark );
    }

    if ( segments.isEmpty() ) {
        segments << outline;
    }

    // Map via points onto segments
    if ( m_routeRequest.size() > 1 && segments.size() > 1 ) {
        int index = 0;
        for ( int j = 0; j < m_routeRequest.size(); ++j ) {
            QPair<int, qreal> minimum( -1, -1.0 );
            int viaIndex = -1;
            for ( int i = index; i < segments.size(); ++i ) {
                const RouteSegment &segment = segments[i];
                GeoDataCoordinates closest;
                const qreal distance = segment.distanceTo( m_routeRequest.at( j ), closest, closest );
                if ( minimum.first < 0 || distance < minimum.second ) {
                    minimum.first = i;
                    minimum.second = distance;
                    viaIndex = j;
                }
            }

            if ( minimum.first >= 0 ) {
                index = minimum.first;
                Maneuver viaPoint = segments[ minimum.first ].maneuver();
                viaPoint.setWaypoint( m_routeRequest.at( viaIndex ), viaIndex );
                segments[ minimum.first ].setManeuver( viaPoint );
            }
        }
    }

    if ( segments.size() > 0 ) {
        foreach( const RouteSegment &segment, segments ) {
            route.addRouteSegment( segment );
        }
    }

    m_routingModel.setRoute( route );
}

void RoutingManagerPrivate::importPlacemark( RouteSegment &outline, QVector<RouteSegment> &segments, const GeoDataPlacemark *placemark )
{
    const GeoDataGeometry* geometry = placemark->geometry();
    const GeoDataLineString* lineString = dynamic_cast<const GeoDataLineString*>( geometry );
    QStringList blacklist = QStringList() << "" << "Route" << "Tessellated";
    RouteSegment segment;
    bool isOutline = true;
    if ( !blacklist.contains( placemark->name() ) ) {
        if( lineString ) {
            Maneuver maneuver;
            maneuver.setInstructionText( placemark->name() );
            maneuver.setPosition( lineString->at( 0 ) );

            if (placemark->extendedData().contains(QStringLiteral("turnType"))) {
                QVariant turnType = placemark->extendedData().value(QStringLiteral("turnType")).value();
                // The enum value is converted to/from an int in the QVariant
                // because only a limited set of data types can be serialized with QVariant's
                // toString() method (which is used to serialize <ExtendedData>/<Data> values)
                maneuver.setDirection( Maneuver::Direction( turnType.toInt() ) );
            }

            if (placemark->extendedData().contains(QStringLiteral("roadName"))) {
                QVariant roadName = placemark->extendedData().value(QStringLiteral("roadName")).value();
                maneuver.setRoadName( roadName.toString() );
            }

            segment.setManeuver( maneuver );
            isOutline = false;
        }
    }

    if ( lineString ) {
        segment.setPath( *lineString );

        if ( isOutline ) {
            outline = segment;
        } else {
            segments.push_back( segment );
        }
    }
}

AlternativeRoutesModel* RoutingManager::alternativeRoutesModel()
{
    return &d->m_alternativeRoutesModel;
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
        profile.setName(QStringLiteral("Motorcar"));
        profile.setTransportType( RoutingProfile::Motorcar );
        break;
    case RoutingProfile::Bicycle:
        tpl = RoutingProfilesModel::BicycleTemplate;
        profile.setName(QStringLiteral("Bicycle"));
        profile.setTransportType( RoutingProfile::Bicycle );
        break;
    case RoutingProfile::Pedestrian:
        tpl = RoutingProfilesModel::PedestrianTemplate;
        profile.setName(QStringLiteral("Pedestrian"));
        profile.setTransportType( RoutingProfile::Pedestrian );
        break;
    }

    foreach( RoutingRunnerPlugin* plugin, d->m_pluginManager->routingRunnerPlugins() ) {
        if ( plugin->supportsTemplate( tpl ) ) {
            profile.pluginSettings()[plugin->nameId()] = plugin->templateSettings( tpl );
        }
    }

    return profile;
}

void RoutingManager::readSettings()
{
    d->loadRoute( d->stateFile() );
}

void RoutingManager::setGuidanceModeEnabled( bool enabled )
{
    if ( d->m_guidanceModeEnabled == enabled ) {
        return;
    }

    d->m_guidanceModeEnabled = enabled;

    if ( enabled ) {
        d->saveRoute( d->stateFile( "guidance.kml" ) );

        if ( d->m_guidanceModeWarning ) {
            QString text = QLatin1String("<p>") + tr("Caution: Driving instructions may be incomplete or wrong.") +
                QLatin1Char(' ') + tr("Road construction, weather and other unforeseen variables can result in the suggested route not to be the most expedient or safest route to your destination.") +
                QLatin1Char(' ') + tr("Please use common sense while navigating.") + QLatin1String("</p>") +
                QLatin1String("<p>") + tr("The Marble development team wishes you a pleasant and safe journey.") + QLatin1String("</p>");
            QPointer<QMessageBox> messageBox = new QMessageBox(QMessageBox::Information, tr("Guidance Mode"), text, QMessageBox::Ok);
            QCheckBox *showAgain = new QCheckBox( tr( "Show again" ) );
            showAgain->setChecked( true );
            showAgain->blockSignals( true ); // otherwise it'd close the dialog
            messageBox->addButton( showAgain, QMessageBox::ActionRole );
            const bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
            messageBox->resize( 380, smallScreen ? 400 : 240 );
            messageBox->exec();
            if ( !messageBox.isNull() ) {
                d->m_guidanceModeWarning = showAgain->isChecked();
            }
            delete messageBox;
        }
    } else {
        d->loadRoute( d->stateFile( "guidance.kml" ) );
    }

    PositionProviderPlugin* positionProvider = d->m_positionTracking->positionProviderPlugin();
    if ( !positionProvider && enabled ) {
        QList<const PositionProviderPlugin*> plugins = d->m_pluginManager->positionProviderPlugins();
        if ( plugins.size() > 0 ) {
            positionProvider = plugins.first()->newInstance();
        }
        d->m_positionTracking->setPositionProviderPlugin( positionProvider );
        d->m_shutdownPositionTracking = true;
    } else if ( positionProvider && !enabled && d->m_shutdownPositionTracking ) {
        d->m_shutdownPositionTracking = false;
        d->m_positionTracking->setPositionProviderPlugin( 0 );
    }

    emit guidanceModeEnabledChanged( d->m_guidanceModeEnabled );
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
            m_routeRequest.setPosition( 0, m_positionTracking->currentLocation(), QObject::tr( "Current Location" ) );
            q->retrieveRoute();
        } else if ( m_routeRequest.size() != 0 && !m_routeRequest.visited( m_routeRequest.size()-1 ) ) {
            m_routeRequest.insert( 0, m_positionTracking->currentLocation(), QObject::tr( "Current Location" ) );
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

void RoutingManager::setRouteColorStandard( const QColor& color )
{
    d->m_routeColorStandard = color;
}

QColor RoutingManager::routeColorStandard() const
{
    return d->m_routeColorStandard;
}

void RoutingManager::setRouteColorHighlighted( const QColor& color )
{
    d->m_routeColorHighlighted = color;
}

QColor RoutingManager::routeColorHighlighted() const
{
    return d->m_routeColorHighlighted;
}

void RoutingManager::setRouteColorAlternative( const QColor& color )
{
    d->m_routeColorAlternative = color;
}

QColor RoutingManager::routeColorAlternative() const
{
    return d->m_routeColorAlternative;
}

bool RoutingManager::guidanceModeEnabled() const
{
    return d->m_guidanceModeEnabled;
}

} // namespace Marble

#include "moc_RoutingManager.cpp"
