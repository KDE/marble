//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MonavRunner.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "routing/RouteSkeleton.h"
#include "GeoDataDocument.h"

#include <QtCore/QProcess>

namespace Marble
{

class MonavRunnerPrivate
{
public:
    QDir m_mapDir;

    GeoDataLineString* retrieveWaypoints( const QStringList &params ) const;

    GeoDataDocument* createDocument( GeoDataLineString* routeWaypoints ) const;

    GeoDataLineString* parseMonavOutput( const QByteArray &content ) const;
};

GeoDataLineString* MonavRunnerPrivate::retrieveWaypoints( const QStringList &params ) const
{
    QStringList monavParams;
    monavParams << params << m_mapDir.absolutePath();
    QProcess monavProcess;
    monavProcess.start( "monav", monavParams );

    if ( !monavProcess.waitForStarted( 5000 ) )
    {
        mDebug() << "Couldn't start monav from the current PATH. Install it to retrieve routing results from monav.";
        return 0;
    }

    if ( monavProcess.waitForFinished( 60 * 1000 ) )
    {
        return parseMonavOutput( monavProcess.readAllStandardOutput() );
    }
    else
    {
        mDebug() << "Couldn't stop monav";
        return 0;
    }
}

GeoDataLineString* MonavRunnerPrivate::parseMonavOutput( const QByteArray &content ) const
{
    GeoDataLineString* routeWaypoints = new GeoDataLineString;

    QStringList lines = QString::fromUtf8( content ).split( '\n' );
    foreach( const QString &line, lines )
    {
        QStringList fields = line.split( ';' );
        if ( fields.size() >= 2 )
        {
            qreal lat = fields.at( 0 ).trimmed().toDouble();
            qreal lon = fields.at( 1 ).trimmed().toDouble();
            GeoDataCoordinates coordinates( lon, lat, 0.0, GeoDataCoordinates::Degree );
            routeWaypoints->append( coordinates );
        }
    }

    return routeWaypoints;
}

GeoDataDocument* MonavRunnerPrivate::createDocument( GeoDataLineString* routeWaypoints ) const
{
    if ( !routeWaypoints || routeWaypoints->isEmpty() )
    {
        return 0;
    }

    GeoDataDocument* result = new GeoDataDocument();
    GeoDataPlacemark* routePlacemark = new GeoDataPlacemark;
    routePlacemark->setName( "Route" );
    routePlacemark->setGeometry( routeWaypoints );
    result->append( routePlacemark );

    QString name = "%1 %2 (Monav)";
    QString unit = "m";
    qreal length = routeWaypoints->length( EARTH_RADIUS );
    if ( length >= 1000 )
    {
        length /= 1000.0;
        unit = "km";
    }
    result->setName( name.arg( length, 0, 'f', 1 ).arg( unit ) );
    return result;
}

MonavRunner::MonavRunner( QObject *parent ) :
        MarbleAbstractRunner( parent ),
        d( new MonavRunnerPrivate )
{
    // Check installation
    d->m_mapDir = QDir( MarbleDirs::localPath() + "/maps/earth/monav/" );
}

MonavRunner::~MonavRunner()
{
    delete d;
}

GeoDataFeature::GeoDataVisualCategory MonavRunner::category() const
{
    return GeoDataFeature::OsmSite;
}

void MonavRunner::retrieveRoute( RouteSkeleton *route )
{
    if ( ! QFileInfo( d->m_mapDir, "Contraction Hierarchies" ).exists() )
    {
        emit routeCalculated( 0 );
        return;
    }

    QStringList params;

    for ( int i = 0; i < route->size(); ++i )
    {
        double lon = route->at( i ).longitude( GeoDataCoordinates::Degree );
        double lat = route->at( i ).latitude( GeoDataCoordinates::Degree );
        params << QString( "--lat%1=%2" ).arg( i + 1 ).arg( lat, 0, 'f', 8 );
        params << QString( "--lon%1=%2" ).arg( i + 1 ).arg( lon, 0, 'f', 8 );
    }
    GeoDataLineString* wayPoints = d->retrieveWaypoints( params );

    GeoDataDocument* result = d->createDocument( wayPoints );
    emit routeCalculated( result );
}

} // namespace Marble

#include "MonavRunner.moc"
