// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "GosmoreReverseGeocodingRunner.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "routing/RouteRequest.h"
#include "routing/instructions/WaypointParser.h"
#include "routing/instructions/InstructionTransformation.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "GeoDataPlacemark.h"

#include <QProcess>

namespace Marble
{

class GosmoreRunnerPrivate
{
public:
    QFileInfo m_gosmoreMapFile;

    WaypointParser m_parser;

    QByteArray retrieveWaypoints( const QString &query ) const;

    GosmoreRunnerPrivate();
};

GosmoreRunnerPrivate::GosmoreRunnerPrivate()
{
    m_parser.setLineSeparator("\r");
    m_parser.setFieldSeparator(QLatin1Char(','));
    m_parser.setFieldIndex( WaypointParser::RoadName, 4 );
    m_parser.addJunctionTypeMapping( "Jr", RoutingWaypoint::Roundabout );
}

QByteArray GosmoreRunnerPrivate::retrieveWaypoints( const QString &query ) const
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("QUERY_STRING", query);
    env.insert("LC_ALL", "C");
    QProcess gosmore;
    gosmore.setProcessEnvironment(env);

    gosmore.start("gosmore", QStringList() << m_gosmoreMapFile.absoluteFilePath() );
    if (!gosmore.waitForStarted(5000)) {
        mDebug() << "Couldn't start gosmore from the current PATH. Install it to retrieve routing results from gosmore.";
        return QByteArray();
    }

    if ( gosmore.waitForFinished(15000) ) {
        return gosmore.readAllStandardOutput();
    }
    else {
        mDebug() << "Couldn't stop gosmore";
    }

    return QByteArray();
}

GosmoreRunner::GosmoreRunner( QObject *parent ) :
        ReverseGeocodingRunner( parent ),
        d( new GosmoreRunnerPrivate )
{
    // Check installation
    QDir mapDir(MarbleDirs::localPath() + QLatin1String("/maps/earth/gosmore/"));
    d->m_gosmoreMapFile = QFileInfo ( mapDir, "gosmore.pak" );
}

GosmoreRunner::~GosmoreRunner()
{
    delete d;
}

void GosmoreRunner::reverseGeocoding( const GeoDataCoordinates &coordinates )
{
    if ( !d->m_gosmoreMapFile.exists() )
    {
        emit reverseGeocodingFinished( coordinates, GeoDataPlacemark() );
        return;
    }

    QString queryString = "flat=%1&flon=%2&tlat=%1&tlon=%2&fastest=1&v=motorcar";
    double lon = coordinates.longitude( GeoDataCoordinates::Degree );
    double lat = coordinates.latitude( GeoDataCoordinates::Degree );
    queryString = queryString.arg( lat, 0, 'f', 8).arg(lon, 0, 'f', 8 );
    QByteArray output = d->retrieveWaypoints( queryString );

    GeoDataPlacemark placemark;
    placemark.setCoordinate( coordinates );

    QStringList lines = QString::fromUtf8(output).split(QLatin1Char('\r'));
    if ( lines.size() > 2 ) {
        QStringList fields = lines.at( lines.size()-2 ).split(QLatin1Char(','));
        if ( fields.size() >= 5 ) {
            QString road = fields.last().trimmed();
            placemark.setAddress( road );
            GeoDataExtendedData extendedData;
            extendedData.addValue(GeoDataData(QStringLiteral("road"), road));
            placemark.setExtendedData( extendedData );
        }
    }

    emit reverseGeocodingFinished( coordinates, placemark );
}

} // namespace Marble
