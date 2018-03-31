//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2010      Niko Sams <niko.sams@gmail.com>
//

#include "RoutinoRunner.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "routing/RouteRequest.h"
#include "routing/instructions/WaypointParser.h"
#include "routing/instructions/InstructionTransformation.h"
#include "GeoDataDocument.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "GeoDataPlacemark.h"
#include "GeoDataLineString.h"

#include <QProcess>
#include <QTemporaryFile>
#include <MarbleMap.h>
#include <MarbleModel.h>
#include <routing/RoutingManager.h>
#include <routing/RoutingProfilesModel.h>

namespace Marble
{

class RoutinoRunnerPrivate
{
public:
    QDir m_mapDir;

    WaypointParser m_parser;

    QByteArray retrieveWaypoints( const QStringList &params ) const;

    static GeoDataDocument* createDocument( GeoDataLineString* routeWaypoints, const QVector<GeoDataPlacemark*> instructions );

    static GeoDataLineString* parseRoutinoOutput( const QByteArray &content );

    QVector<GeoDataPlacemark*> parseRoutinoInstructions( const QByteArray &content ) const;

    RoutinoRunnerPrivate();
};

RoutinoRunnerPrivate::RoutinoRunnerPrivate()
{
    m_parser.setLineSeparator("\n");
    m_parser.setFieldSeparator(QLatin1Char('\t'));
    m_parser.setFieldIndex( WaypointParser::RoadName, 10 );
}

class TemporaryDir
{
public:
    TemporaryDir() {
        QTemporaryFile f;
        f.setAutoRemove( false );
        f.open();
        m_dirName = f.fileName();
        f.close();
        f.remove();
        QFileInfo( m_dirName ).dir().mkdir( m_dirName );
    }

    ~TemporaryDir() {
        QDir dir( m_dirName );
        QFileInfoList entries = dir.entryInfoList( QDir::Files );
        for ( const QFileInfo &file: entries ) {
            QFile( file.absoluteFilePath() ).remove();
        }
        dir.rmdir( dir.absolutePath() );
    }

    QString dirName() const
    {
        return m_dirName;
    }
private:
    QString m_dirName;
};

QByteArray RoutinoRunnerPrivate::retrieveWaypoints( const QStringList &params ) const
{
    TemporaryDir dir;
    QProcess routinoProcess;
    routinoProcess.setWorkingDirectory( dir.dirName() );

    QStringList routinoParams;
    routinoParams << params;
    routinoParams << QLatin1String("--dir=") + m_mapDir.absolutePath();
    routinoParams << "--output-text-all";
    mDebug() << routinoParams;
    routinoProcess.start( "routino-router", routinoParams );
    if ( !routinoProcess.waitForStarted( 5000 ) ) {
        mDebug() << "Couldn't start routino-router from the current PATH. Install it to retrieve routing results from routino.";
        return nullptr;
    }

    if ( routinoProcess.waitForFinished(60 * 1000) ) {
        mDebug() << routinoProcess.readAll();
        mDebug() << "routino finished";
        QFile file(routinoProcess.workingDirectory() + QLatin1String("/shortest-all.txt"));
        if ( !file.exists() ) {
            file.setFileName(routinoProcess.workingDirectory() + QLatin1String("/quickest-all.txt"));
        }
        if ( !file.exists() ) {
            mDebug() << "Can't get results";
        } else {
            file.open( QIODevice::ReadOnly );
            return file.readAll();
        }
    }
    else {
        mDebug() << "Couldn't stop routino";
    }
    return nullptr;
}

GeoDataLineString* RoutinoRunnerPrivate::parseRoutinoOutput( const QByteArray &content )
{
    GeoDataLineString* routeWaypoints = new GeoDataLineString;

    const QStringList lines = QString::fromUtf8(content).split(QLatin1Char('\n'));
    mDebug() << lines.count() << "lines";
    for( const QString &line: lines ) {
        if (line.startsWith(QLatin1Char('#'))) {
            //skip comment
            continue;
        }
        const QStringList fields = line.split(QLatin1Char('\t'));
        if ( fields.size() >= 10 ) {
            qreal lon = fields.at(1).trimmed().toDouble();
            qreal lat = fields.at(0).trimmed().toDouble();
            GeoDataCoordinates coordinates( lon, lat, 0.0, GeoDataCoordinates::Degree );
            routeWaypoints->append( coordinates );
        }
    }

    return routeWaypoints;
}

QVector<GeoDataPlacemark*> RoutinoRunnerPrivate::parseRoutinoInstructions( const QByteArray &content ) const
{
    QVector<GeoDataPlacemark*> result;
    QTextStream stream( content );
    stream.setCodec("UTF8");
    stream.setAutoDetectUnicode( true );

    RoutingInstructions directions = InstructionTransformation::process( m_parser.parse( stream ) );
    for( int i=0; i<directions.size(); ++i ) {
        GeoDataPlacemark* placemark = new GeoDataPlacemark( directions[i].instructionText() );
        GeoDataExtendedData extendedData;
        GeoDataData turnType;
        turnType.setName(QStringLiteral("turnType"));
        turnType.setValue( qVariantFromValue<int>( int( directions[i].turnType() ) ) );
        extendedData.addValue( turnType );
        GeoDataData roadName;
        roadName.setName(QStringLiteral("roadName"));
        roadName.setValue( directions[i].roadName() );
        extendedData.addValue( roadName );
        placemark->setExtendedData( extendedData );
        Q_ASSERT( !directions[i].points().isEmpty() );
        GeoDataLineString* geometry = new GeoDataLineString;
        QVector<RoutingWaypoint> items = directions[i].points();
        for (int j=0; j<items.size(); ++j ) {
            RoutingPoint point = items[j].point();
            GeoDataCoordinates coordinates( point.lon(), point.lat(), 0.0, GeoDataCoordinates::Degree );
            geometry->append( coordinates );
        }
        placemark->setGeometry( geometry );
        result.push_back( placemark );
    }

    return result;
}

GeoDataDocument* RoutinoRunnerPrivate::createDocument( GeoDataLineString* routeWaypoints, const QVector<GeoDataPlacemark*> instructions )
{
    if ( !routeWaypoints || routeWaypoints->isEmpty() ) {
        return nullptr;
    }

    GeoDataDocument* result = new GeoDataDocument();
    GeoDataPlacemark* routePlacemark = new GeoDataPlacemark;
    routePlacemark->setName( "Route" );
    routePlacemark->setGeometry( routeWaypoints );
    result->append( routePlacemark );

    QString name = "%1 %2 (Routino)";
    QString unit = QLatin1String( "m" );
    qreal length = routeWaypoints->length( EARTH_RADIUS );
    if (length >= 1000) {
        length /= 1000.0;
        unit = "km";
    }
    result->setName( name.arg( length, 0, 'f', 1 ).arg( unit ) );

    for( GeoDataPlacemark* placemark: instructions )
    {
        result->append( placemark );
    }

    return result;
}

RoutinoRunner::RoutinoRunner( QObject *parent ) :
        RoutingRunner( parent ),
        d( new RoutinoRunnerPrivate )
{
    // Check installation
    d->m_mapDir = QDir(MarbleDirs::localPath() + QLatin1String("/maps/earth/routino/"));
}

RoutinoRunner::~RoutinoRunner()
{
    delete d;
}

void RoutinoRunner::retrieveRoute( const RouteRequest *route )
{
    mDebug();

    if ( ! QFileInfo( d->m_mapDir, "nodes.mem" ).exists() )
    {
        emit routeCalculated( nullptr );
        return;
    }

    QStringList params;
    for( int i=0; i<route->size(); ++i )
    {
        double fLon = route->at(i).longitude( GeoDataCoordinates::Degree );
        double fLat = route->at(i).latitude( GeoDataCoordinates::Degree );
        params << QString("--lat%1=%2").arg(i+1).arg(fLat, 0, 'f', 8);
        params << QString("--lon%1=%2").arg(i+1).arg(fLon, 0, 'f', 8);
    }

    QHash<QString, QVariant> settings = route->routingProfile().pluginSettings()[QStringLiteral("routino")];
    QString transport = settings[QStringLiteral("transport")].toString();
    params << QString( "--transport=%0" ).arg( transport );

    if (settings[QStringLiteral("method")] == QLatin1String("shortest")) {
        params << "--shortest";
    } else {
        params << "--quickest";
    }
    /*
    if ( route->avoidFeatures() & RouteRequest::AvoidHighway ) {
        params << "--highway-motorway=0";
    }
    */

    QByteArray output = d->retrieveWaypoints( params );
    GeoDataLineString* wayPoints = d->parseRoutinoOutput( output );
    QVector<GeoDataPlacemark*> instructions = d->parseRoutinoInstructions( output );

    GeoDataDocument* result = d->createDocument( wayPoints, instructions );
    mDebug() << this << "routeCalculated";
    emit routeCalculated( result );
}

} // namespace Marble

#include "moc_RoutinoRunner.cpp"
