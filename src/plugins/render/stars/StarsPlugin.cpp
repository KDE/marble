//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

#include "StarsPlugin.h"

#include <QtCore/QRectF>
#include <QtCore/QSize>
#include <QtCore/QDateTime>
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleDataFacade.h"
#include "GeoPainter.h"
#include "SunLocator.h"
#include "ViewportParams.h"
#include "MarbleWidget.h"

namespace Marble
{

StarsPlugin::StarsPlugin()
    : m_isInitialized( false ),
      m_starsLoaded( false ),
      m_marbleWidget( 0 )
{
}

QStringList StarsPlugin::backendTypes() const
{
    return QStringList( "stars" );
}

QString StarsPlugin::renderPolicy() const
{
    return QString( "SPECIFIED_ALWAYS" );
}

QStringList StarsPlugin::renderPosition() const
{
    QStringList layers = QStringList() << "STARS" << "ALWAYS_ON_TOP";
    return layers;
}

QString StarsPlugin::name() const
{
    return tr( "Stars Plugin" );
}

QString StarsPlugin::guiString() const
{
    return tr( "&Stars" );
}

QString StarsPlugin::nameId() const
{
    return QString( "stars" );
}

QString StarsPlugin::description() const
{
    return tr( "A plugin that shows the Starry Sky." );
}

QIcon StarsPlugin::icon () const
{
    return QIcon();
}


void StarsPlugin::initialize ()
{
    m_isInitialized = true;
}

bool StarsPlugin::isInitialized () const
{
    return m_isInitialized;
}

void StarsPlugin::loadStars() {
    mDebug() << Q_FUNC_INFO;
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
        mDebug() << "stars.dat: file too new.";
        return;
    }
    double ra;
    double de;
    double mag;

    while ( !in.atEnd() ) {
        in >> ra;
        in >> de;
        in >> mag;
        StarPoint star( (qreal)(ra), (qreal)(de), (qreal)(mag) );
        m_stars << star;
//        mDebug() << "RA:" << ra << "DE:" << de << "MAG:" << mag;
    }
    m_starsLoaded = true;
}

bool StarsPlugin::render( GeoPainter *painter, ViewportParams *viewport,
                          const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( layer )

    if ( !(renderPos == "STARS" || renderPos == "ALWAYS_ON_TOP") ) {
        return true;
    }

    if( renderPos=="STARS" )
    {
        QString target = dataFacade()->target();

        // So far this starry sky plugin only supports displaying stars on earth.
        if ( target != "earth" ) {
            return true;
        }

        painter->save();

        painter->autoMapQuality();

        QPen starPen( Qt::NoPen );
        QBrush starBrush( Qt::white );

        painter->setPen( starPen );
        painter->setBrush( starBrush );

        QDateTime currentDateTime = dataFacade()->dateTime();

        qreal gmst = siderealTime( currentDateTime );
        qreal skyRotationAngle = gmst / 12.0 * M_PI;

        qreal centerLon, centerLat;
        viewport->centerCoordinates( centerLon, centerLat );

        Quaternion  skyAxis;
        skyAxis.createFromEuler( -centerLat , centerLon + skyRotationAngle, 0.0 );

        matrix       skyAxisMatrix;
        skyAxis.inverse().toMatrix( skyAxisMatrix );

        if ( !viewport->globeCoversViewport() && viewport->projection() == Spherical )
        {
            // Delayed initialization: 
            // Load the star database only if the sky is actually being painted...
            if ( !m_starsLoaded ) {
                loadStars();
                m_starsLoaded = true;
            }
    
            int x, y;
    
            const qreal  skyRadius      = 0.6 * sqrt( (qreal)viewport->width() * viewport->width() + viewport->height() * viewport->height() );
            const qreal  earthRadius    = viewport->radius();

            QVector<StarPoint>::const_iterator i = m_stars.constBegin();
            QVector<StarPoint>::const_iterator itEnd = m_stars.constEnd();
            for (; i != itEnd; ++i)
            {
                Quaternion  qpos = (*i).quaternion();
    
                qpos.rotateAroundAxis( skyAxisMatrix );
    
                if ( qpos.v[Q_Z] > 0 ) {
                   continue;
                }

                qreal  earthCenteredX = qpos.v[Q_X] * skyRadius;
                qreal  earthCenteredY = qpos.v[Q_Y] * skyRadius;

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
    
                qreal size;
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

        painter->restore();
    }

    if( renderPos == "ALWAYS_ON_TOP" )
    {
        m_marbleWidget = ( MarbleWidget* )painter->device();
        if( m_marbleWidget->sunLocator()->getCentered() == true )
        {
            GeoDataCoordinates point( m_marbleWidget->sunLocator()->getLon(), m_marbleWidget->sunLocator()->getLat(), 0, GeoDataCoordinates::Degree );
            QImage image( MarbleDirs::path( "svg/sunshine.png" ) );
            painter->drawImage( point, image.scaled( QSize( 30, 30 ) ) );
        }
    }

    return true;
}

qreal StarsPlugin::siderealTime( const QDateTime& localDateTime )
{
    QDateTime utcDateTime = localDateTime.toTimeSpec ( Qt::UTC );
    qreal mjdUtc = (qreal)( utcDateTime.date().toJulianDay() );

    qreal offsetUtcSecs = -utcDateTime.time().secsTo( QTime( 00, 00 ) );
    qreal d_days = mjdUtc - 2451545.5;
    qreal d = d_days + ( offsetUtcSecs / ( 24.0 * 3600 ) );

    //  Appendix A of USNO Circular No. 163 (1981):
    //  Approximate value for Greenwich mean sidereal time in hours: 
    //  (Loss of precision: 0.1 secs per century)
    qreal gmst = 18.697374558 + 24.06570982441908 * d;

    // Range (0..24) for gmst: 
    return gmst - (int)( gmst / 24.0 ) * 24.0; 
}

}

Q_EXPORT_PLUGIN2( StarsPlugin, Marble::StarsPlugin )

#include "StarsPlugin.moc"
