//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

#include "MarbleStarsPlugin.h"

#include <QtCore/QDebug>
#include <QtCore/QRectF>
#include <QtCore/QDateTime>
#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include "MarbleDirs.h"
#include "MarbleDataFacade.h"
#include "GeoPainter.h"

#include "Quaternion.h"
#include "ViewportParams.h"


QStringList MarbleStarsPlugin::backendTypes() const
{
    return QStringList( "stars" );
}

QString MarbleStarsPlugin::renderPolicy() const
{
    return QString( "STARS" );
}

QStringList MarbleStarsPlugin::renderPosition() const
{
    return QStringList( "SPECIFIED_ALWAYS" );
}

QString MarbleStarsPlugin::name() const
{
    return tr( "Stars Plugin" );
}

QString MarbleStarsPlugin::guiString() const
{
    return tr( "&Stars" );
}

QString MarbleStarsPlugin::nameId() const
{
    return QString( "stars" );
}

QString MarbleStarsPlugin::description() const
{
    return tr( "A plugin that shows the Starry Sky." );
}

QIcon MarbleStarsPlugin::icon () const
{
    return QIcon();
}


void MarbleStarsPlugin::initialize ()
{
    // Load star data
    m_stars.clear();

    QFile starFile( MarbleDirs::path( "stars/stars.dat" ) );
    starFile.open( QIODevice::ReadOnly );
    QDataStream in( &starFile );

    // Read and check the header
    quint32 magic;
    in >> magic;
    if ( magic != 0x73746172 )
        return;

    // Read the version
    qint32 version;
    in >> version;
    if ( version > 001 ) {
        qDebug() << "stars.dat: file too new.";
        return;
    }
    double ra;
    double de;
    double mag;

    while ( !in.atEnd() ) {
        in >> ra;
        in >> de;
        in >> mag;
        StarPoint star( ra, de, mag ); 
        m_stars << star;
//        qDebug() << "RA:" << ra << "DE:" << de << "MAG:" << mag;
    }
}

bool MarbleStarsPlugin::isInitialized () const
{
    return true;
}

bool MarbleStarsPlugin::render( GeoPainter *painter, ViewportParams *viewport,
				const QString& renderPos,
				GeoSceneLayer * layer )
{
    painter->autoMapQuality();

    QPen starPen( Qt::NoPen );
    QBrush starBrush( Qt::white );

    painter->setPen( starPen );
    painter->setBrush( starBrush );

    QDateTime currentDateTime = dataFacade()->dateTime();

    double gmst = siderealTime( currentDateTime );
    double skyRotationAngle = gmst / 12.0 * M_PI;

//    double gmst_hh = (int)( gmst );
//    double gmst_mm = (gmst - gmst_hh) * 60.0;
//    qDebug() << "Sidereal Time" << gmst_hh << ":" << gmst_mm;
//    qDebug() << "sky rotation angle: " << hourAngle;

    double centerLon, centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    Quaternion  skyAxis;
    skyAxis.createFromEuler( -centerLat , centerLon + skyRotationAngle, 0.0 );

    matrix       skyAxisMatrix;
    skyAxis.inverse().toMatrix( skyAxisMatrix );

    if ( !viewport->globeCoversViewport() && viewport->projection() == Spherical )
    {
        int x, y;

        QVector<StarPoint>::const_iterator i;
        for (i = m_stars.begin(); i != m_stars.end(); ++i)
        {
            Quaternion  qpos = (*i).quaternion();

            qpos.rotateAroundAxis( skyAxisMatrix );

            if ( qpos.v[Q_Z] > 0 ) {
                continue;
            }

            double  skyRadius      = 0.6 * sqrt( viewport->width() * viewport->width() + viewport->height() * viewport->height() );
            double  earthRadius    = viewport->radius();
            double  earthCenteredX = qpos.v[Q_X] * skyRadius;
            double  earthCenteredY = qpos.v[Q_Y] * skyRadius;

            // Don't draw high placemarks (e.g. satellites) that aren't visible.
            if ( qpos.v[Q_Z] < 0
                && ( ( earthCenteredX * earthCenteredX
                        + earthCenteredY * earthCenteredY )
                    < earthRadius * earthRadius ) ) {
                continue;
            }

            // Let (x, y) be the position on the screen of the placemark..
            x = (int)(viewport->width()  / 2 + skyRadius * qpos.v[Q_X]);
            y = (int)(viewport->height() / 2 - skyRadius * qpos.v[Q_Y]);

            // Skip placemarks that are outside the screen area
            if ( x < 0 || x >= viewport->width()
		 || y < 0 || y >= viewport->height() )
                continue;

            double size;
            if ( (*i).magnitude() < -1 ) size = 6.5;
            else if ( (*i).magnitude() < 0 ) size = 5.5;
            else if ( (*i).magnitude() < 1 ) size = 4.5;
            else if ( (*i).magnitude() < 2 ) size = 4.0;
            else if ( (*i).magnitude() < 3 ) size = 3.0;
            else if ( (*i).magnitude() < 4 ) size = 2.0;
            else if ( (*i).magnitude() < 5 ) size = 1.0;
            else size = 0.5;
            painter->drawEllipse( QRectF( x, y, size, size ) );
        }
    }

    return true;
}

double MarbleStarsPlugin::siderealTime( const QDateTime& localDateTime )
{
    QDateTime utcDateTime = localDateTime.toTimeSpec ( Qt::UTC );
    double mjdUtc = (double)( utcDateTime.date().toJulianDay() );

    double offsetUtcSecs = -utcDateTime.time().secsTo( QTime( 00, 00 ) );
    double d_days = mjdUtc - 2451545.5;
    double d = d_days + ( offsetUtcSecs / ( 24.0 * 3600 ) );

    //  Appendix A of USNO Circular No. 163 (1981):
    //  Approximate value for Greenwich mean sidereal time in hours: 
    //  (Loss of precision: 0.1 secs per century)
    double gmst = 18.697374558 + 24.06570982441908 * d;

    // Range (0..24) for gmst: 
    return gmst - (int)( gmst / 24.0 ) * 24.0; 
}

Q_EXPORT_PLUGIN2(MarbleStarsPlugin, MarbleStarsPlugin)

#include "MarbleStarsPlugin.moc"
