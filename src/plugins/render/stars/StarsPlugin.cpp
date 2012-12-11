//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

#include "StarsPlugin.h"

#include "ui_StarsConfigWidget.h"
#include <QtCore/QRectF>
#include <QtCore/QSize>
#include <QtCore/QDateTime>
#include <QtGui/QRegion>

#include "MarbleClock.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "GeoPainter.h"
#include "SunLocator.h"
#include "ViewportParams.h"

namespace Marble
{

StarsPlugin::StarsPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
      m_configDialog( 0 ),
      ui_configWidget( 0 ),
      m_renderStars( false ),
      m_renderConstellations( true ),
      m_starsLoaded( false ),
      m_constellationsLoaded( false ),
      m_dsosLoaded( false )
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
    return QStringList() << "STARS";
}

QString StarsPlugin::name() const
{
    return tr( "Stars" );
}

QString StarsPlugin::guiString() const
{
    return tr( "&Stars" );
}

QString StarsPlugin::nameId() const
{
    return QString( "stars" );
}

QString StarsPlugin::version() const
{
    return "1.2";
}

QString StarsPlugin::description() const
{
    return tr( "A plugin that shows the Starry Sky and the Sun." );
}

QString StarsPlugin::copyrightYears() const
{
    return "2008-2012";
}

QList<PluginAuthor> StarsPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
           << PluginAuthor( "Torsten Rahn", "tackat@kde.org" )
           << PluginAuthor( "Rene Kuettner", "rene@bitkanal.net" )
           << PluginAuthor( "Timothy Lanzi", "trlanzi@gmail.com" );
}

QIcon StarsPlugin::icon() const
{
    return QIcon();
}

void StarsPlugin::initialize()
{
}

bool StarsPlugin::isInitialized() const
{
    return true;
}

QDialog *StarsPlugin::configDialog()
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::StarsConfigWidget;
        ui_configWidget->setupUi( m_configDialog );

        readSettings();

        connect( ui_configWidget->m_buttonBox, SIGNAL( accepted() ), SLOT( writeSettings() ) );
        connect( ui_configWidget->m_buttonBox, SIGNAL( rejected() ), SLOT( readSettings() ) );
// FIXME: Could Not Make Apply Button Work.
//        QPushButton *applyButton = ui_configWidget->m_buttonBox->button( QDialogButtonBox::Apply );
//        connect( applyButton, SIGNAL( clicked() ), this, SLOT( writeSettings() ) );
    }
    return m_configDialog;
}

void StarsPlugin::readSettings()
{
    if ( !m_configDialog ) {
        return;
    }

    Qt::CheckState const state = m_renderConstellations ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_viewConstellationsCheckBox->setCheckState( state );
}

void StarsPlugin::writeSettings()
{
    m_renderConstellations = ui_configWidget->m_viewConstellationsCheckBox->checkState() == Qt::Checked;
    emit settingsChanged( nameId() );
}

void StarsPlugin::loadStars()
{
    //mDebug() << Q_FUNC_INFO;
    // Load star data
    m_stars.clear();

    QFile starFile( MarbleDirs::path( "stars/stars.dat" ) );
    starFile.open( QIODevice::ReadOnly );
    QDataStream in( &starFile );

    // Read and check the header
    quint32 magic;
    in >> magic;
    if ( magic != 0x73746172 ) {
        return;
    }

    // Read the version
    qint32 version;
    in >> version;
    if ( version > 002 ) {
        mDebug() << "stars.dat: file too new.";
        return;
    }
    int maxid = 0;
    int id = 0;
    int starIndex = 0;
    double ra;
    double de;
    double mag;

    mDebug() << "Star Catalog Version " << version;

    while ( !in.atEnd() ) {
        if ( version >= 2 ) {
            in >> id;
        }
        if ( id > maxid ) {
            maxid = id;
        }
        in >> ra;
        in >> de;
        in >> mag;
        StarPoint star( id, ( qreal )( ra ), ( qreal )( de ), ( qreal )( mag ) );
        // Create entry in stars database
        m_stars << star;
        // Create key,value pair in idHash table to map from star id to
        // index in star database vector
        m_idHash[id] = starIndex;
        // Increment Index for use in hash
        ++starIndex;
        //mDebug() << "id" << id << "RA:" << ra << "DE:" << de << "MAG:" << mag;
    }

    // load the Sun pixmap
    // TODO: adjust pixmap size according to distance
    m_pixmapSun.load( MarbleDirs::path( "svg/sun.png" ) );

    m_starsLoaded = true;
}


void StarsPlugin::loadConstellations()
{
    // Load star data
    m_constellations.clear();

    QFile constellationFile( MarbleDirs::path( "stars/constellations.dat" ) );
    constellationFile.open( QIODevice::ReadOnly );
    QTextStream in( &constellationFile );
    QString line;
    QString indexList;

    while ( !in.atEnd() ) {
        line = in.readLine();

        // Check for null line at end of file
        if ( line.isNull() ) {
            continue;
        }

        // Ignore Comment lines in header and
        // between constellation entries
        if ( line.startsWith( '#' ) )    {
            continue;
        }

        indexList = in.readLine();

        // Make sure we have a valid label and indexList
        if ( indexList.isNull() ) {
            break;
        }

        Constellation constellation( line, indexList );
        m_constellations << constellation;

    }
    m_constellationsLoaded = true;

}

void StarsPlugin::loadDsos()
{
    // Load star data
    m_dsos.clear();

    QFile dsoFile( MarbleDirs::path( "stars/dso.dat" ) );
    dsoFile.open( QIODevice::ReadOnly );
    QTextStream in( &dsoFile );
    QString line;

    while ( !in.atEnd() ) {
        line = in.readLine();

        // Check for null line at end of file
        if ( line.isNull() ) {
            continue;
        }

        // Ignore Comment lines in header and
        // between dso entries
        if ( line.startsWith( '#' ) )    {
            continue;
        }

        QStringList entries = line.split( "," );

        double raH = entries.at( 1 ).toDouble();
        double raM = entries.at( 2 ).toDouble();
        double raS = entries.at( 3 ).toDouble();
        double decD = entries.at( 4 ).toDouble();
        double decM = entries.at( 5 ).toDouble();
        double decS = entries.at( 6 ).toDouble();

        double raRad = ( raH+raM/60.0+raS/3600.0 )*15.0*M_PI/180.0;
        double decRad;

        if ( decD >= 0.0 ) {
            decRad = ( decD+decM/60.0+decS/3600.0 )*M_PI/180.0;
        }
        else {
            decRad = ( decD-decM/60.0-decS/3600.0 )*M_PI/180.0;
        }

        DsoPoint dso( ( qreal )( raRad ), ( qreal )( decRad ) );
        // Create entry in stars database
        m_dsos << dso;
    }

    m_dsoImage.load( MarbleDirs::path( "stars/deepsky.png" ) );
    m_dsosLoaded = true;
}

bool StarsPlugin::render( GeoPainter *painter, ViewportParams *viewport,
                          const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    QString target = marbleModel()->planetId();

    // So far this starry sky plugin only supports displaying stars on earth.
    if ( target != "earth" ) {
        return true;
    }

    painter->save();

    painter->autoMapQuality();

    QPen starPen( Qt::NoPen );
    QPen constellationPenSolid( Marble::Oxygen::aluminumGray4, 1, Qt::SolidLine );
    QPen constellationPenDash( Marble::Oxygen::aluminumGray4, 1, Qt::DashLine );
    QBrush starBrush( Qt::white );

    painter->setPen( starPen );
    painter->setBrush( starBrush );

    QDateTime currentDateTime = marbleModel()->clockDateTime();

    qreal gmst = siderealTime( currentDateTime );
    qreal skyRotationAngle = gmst / 12.0 * M_PI;

    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();

    const Quaternion skyAxis = Quaternion::fromEuler( -centerLat , centerLon + skyRotationAngle, 0.0 );

    matrix skyAxisMatrix;
    skyAxis.inverse().toMatrix( skyAxisMatrix );

    const bool renderStars = !viewport->mapCoversViewport() && viewport->projection() == Spherical;

    if ( renderStars ) {
        // Delayed initialization:
        // Load the star database only if the sky is actually being painted...
        if ( !m_starsLoaded ) {
            loadStars();
            m_starsLoaded = true;
        }

        if ( !m_constellationsLoaded && m_renderConstellations ) {
            loadConstellations();
            m_constellationsLoaded = true;
        }

        if ( !m_dsosLoaded ) {
            loadDsos();
            m_dsosLoaded = true;
        }

        int x, y;
        Quaternion qpos;

        const qreal  skyRadius      = 0.6 * sqrt( ( qreal )viewport->width() * viewport->width() + viewport->height() * viewport->height() );
        const qreal  earthRadius    = viewport->radius();

        // Render Deep Space Objects
        for ( int d = 0; d < m_dsos.size(); ++d ) {
            qpos = m_dsos.at( d ).quaternion();

            qpos.rotateAroundAxis( skyAxisMatrix );

            if ( qpos.v[Q_Z] > 0 ) {
                continue;
            }

            qreal earthCenteredX = qpos.v[Q_X] * skyRadius;
            qreal earthCenteredY = qpos.v[Q_Y] * skyRadius;

            // Don't draw high placemarks (e.g. satellites) that aren't visible.
            if ( qpos.v[Q_Z] < 0
                    && ( ( earthCenteredX * earthCenteredX
                           + earthCenteredY * earthCenteredY )
                         < earthRadius * earthRadius ) ) {
                continue;
            }

            // Let (x, y) be the position on the screen of the placemark..
            x = ( int )( viewport->width()  / 2 + skyRadius * qpos.v[Q_X] );
            y = ( int )( viewport->height() / 2 - skyRadius * qpos.v[Q_Y] );

            // Skip placemarks that are outside the screen area
            if ( x < 0 || x >= viewport->width() ||
                 y < 0 || y >= viewport->height() ) {
                continue;
            }

            // Hard Code DSO Size for now
            qreal size = 20;

            // Center Image on x,y location
            painter->drawImage( QRectF( x-size/2, y-size/2, size, size ),m_dsoImage );
        }


        if ( m_renderConstellations )
        {
            painter->setPen( constellationPenSolid );
            // Render Constellations
            for ( int c = 0; c < m_constellations.size(); ++c ) {
                int xMean = 0;
                int yMean = 0;
                int endptCount = 0;

                for ( int s = 0; s < ( m_constellations.at( c ).size() - 1 ); ++s ) {
                    int starId1 = m_constellations.at( c ).at( s );
                    int starId2 = m_constellations.at( c ).at( s + 1 );

                    if ( starId1 == -1 || starId2 == -1 ) {
                        // starId == -1 means we don't draw this segment
                        continue;
                    } else if ( starId1 == -2 || starId2 == -2 ) {
                        painter->setPen( constellationPenDash );
                    } else if ( starId1 == -3 || starId2 == -3 ) {
                        painter->setPen( constellationPenSolid );
                    }

                    int idx1 = m_idHash.value( starId1,-1 );
                    int idx2 = m_idHash.value( starId2,-1 );

                    if ( idx1 < 0 ) {
                        mDebug() << "unknown star, "
                                 << starId1 <<  ", in constellation "
                                 << m_constellations.at( c ).name();
                        continue;
                    }

                    if ( idx2 < 0 ) {
                        mDebug() << "unknown star, "
                                 << starId1 <<  ", in constellation "
                                 << m_constellations.at( c ).name();
                        continue;
                    }
                    // Fetch quaternion from star s in constellation c
                    Quaternion q1 = m_stars.at( idx1 ).quaternion();
                    // Fetch quaternion from star s+1 in constellation c
                    Quaternion q2 = m_stars.at( idx2 ).quaternion();

                    q1.rotateAroundAxis( skyAxisMatrix );
                    q2.rotateAroundAxis( skyAxisMatrix );

                    if ( q1.v[Q_Z] > 0 || q2.v[Q_Z] > 0 ) {
                        continue;
                    }

                    qreal  earthCenteredX1 = q1.v[Q_X] * skyRadius;
                    qreal  earthCenteredY1 = q1.v[Q_Y] * skyRadius;
                    qreal  earthCenteredX2 = q2.v[Q_X] * skyRadius;
                    qreal  earthCenteredY2 = q2.v[Q_Y] * skyRadius;

                    // Don't draw high placemarks (e.g. satellites) that aren't visible.
                    if ( q1.v[Q_Z] < 0
                            && ( ( earthCenteredX1 * earthCenteredX1
                                   + earthCenteredY1 * earthCenteredY1 )
                                 < earthRadius * earthRadius ) ) {
                        continue;
                    }
                    if ( q2.v[Q_Z] < 0
                            && ( ( earthCenteredX2 * earthCenteredX2
                                   + earthCenteredY2 * earthCenteredY2 )
                                 < earthRadius * earthRadius ) ) {
                        continue;
                    }

                    // Let (x, y) be the position on the screen of the placemark..
                    int x1 = ( int )( viewport->width()  / 2 + skyRadius * q1.v[Q_X] );
                    int y1 = ( int )( viewport->height() / 2 - skyRadius * q1.v[Q_Y] );
                    int x2 = ( int )( viewport->width()  / 2 + skyRadius * q2.v[Q_X] );
                    int y2 = ( int )( viewport->height() / 2 - skyRadius * q2.v[Q_Y] );

                    // Skip placemarks that are outside the screen area
                    if ( x1 < 0 || x1 >= viewport->width()
                            || y1 < 0 || y1 >= viewport->height() )
                        continue;
                    if ( x2 < 0 || x2 >= viewport->width()
                            || y2 < 0 || y2 >= viewport->height() )
                        continue;

                    xMean = xMean + x1 + x2;
                    yMean = yMean + y1 + y2;
                    endptCount = endptCount + 2;

                    painter->drawLine( x1, y1, x2, y2 );

                }

                // Skip constellation labels that are outside the screen area
                if ( endptCount > 0 ) {
                    xMean = xMean / endptCount;
                    yMean = yMean / endptCount;
                }

                if ( endptCount < 1 || xMean < 0 || xMean >= viewport->width()
                        || yMean < 0 || yMean >= viewport->height() )
                    continue;

                painter->drawText( xMean, yMean, m_constellations.at( c ).name() );


            }
        }

        // Render Stars
        painter->setPen( starPen );
        painter->setBrush( starBrush );
        QVector<StarPoint>::const_iterator i = m_stars.constBegin();
        QVector<StarPoint>::const_iterator itEnd = m_stars.constEnd();
        for ( ; i != itEnd; ++i ) {
            Quaternion  qpos = ( *i ).quaternion();

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
            x = ( int )( viewport->width()  / 2 + skyRadius * qpos.v[Q_X] );
            y = ( int )( viewport->height() / 2 - skyRadius * qpos.v[Q_Y] );

            // Skip placemarks that are outside the screen area
            if ( x < 0 || x >= viewport->width()
                    || y < 0 || y >= viewport->height() )
                continue;

            qreal size;
            if ( ( *i ).magnitude() < -1 ) size = 6.5;
            else if ( ( *i ).magnitude() < 0 ) size = 5.5;
            else if ( ( *i ).magnitude() < 1 ) size = 4.5;
            else if ( ( *i ).magnitude() < 2 ) size = 4.0;
            else if ( ( *i ).magnitude() < 3 ) size = 3.0;
            else if ( ( *i ).magnitude() < 4 ) size = 2.0;
            else if ( ( *i ).magnitude() < 5 ) size = 1.0;
            else size = 0.5;
            painter->drawEllipse( QRectF( x-size/2.0, y-size/2.0, size, size ) );
        }

        // sun
        const SunLocator *sun = marbleModel()->sunLocator();
        Quaternion::fromEuler( -centerLat , centerLon, 0.0 ).inverse().toMatrix( skyAxisMatrix );
        qpos = Quaternion::fromSpherical( sun->getLon() * DEG2RAD,
                                          sun->getLat() * DEG2RAD );
        qpos.rotateAroundAxis( skyAxisMatrix );

        if ( qpos.v[Q_Z] <= 0 ) {
            qreal deltaX  = m_pixmapSun.width()  / 2.;
            qreal deltaY  = m_pixmapSun.height() / 2.;
            x = (int)(viewport->width()  / 2 + skyRadius * qpos.v[Q_X]);
            y = (int)(viewport->height() / 2 - skyRadius * qpos.v[Q_Y]);
            painter->drawPixmap( x - deltaX, y - deltaY, m_pixmapSun );
        }
    }

    if ( renderStars != m_renderStars ) {
        if ( renderStars ) {
            connect( marbleModel()->clock(), SIGNAL( timeChanged() ),
                     this, SLOT( requestRepaint() ) );
        } else {
            disconnect( marbleModel()->clock(), SIGNAL( timeChanged() ),
                        this, SLOT( requestRepaint() ) );
        }

        m_renderStars = renderStars;
    }

    painter->restore();

    return true;
}

qreal StarsPlugin::siderealTime( const QDateTime& localDateTime )
{
    QDateTime utcDateTime = localDateTime.toTimeSpec( Qt::UTC );
    qreal mjdUtc = ( qreal )( utcDateTime.date().toJulianDay() );

    qreal offsetUtcSecs = -utcDateTime.time().secsTo( QTime( 00, 00 ) );
    qreal d_days = mjdUtc - 2451545.5;
    qreal d = d_days + ( offsetUtcSecs / ( 24.0 * 3600 ) );

    //  Appendix A of USNO Circular No. 163 (1981):
    //  Approximate value for Greenwich mean sidereal time in hours:
    //  (Loss of precision: 0.1 secs per century)
    qreal gmst = 18.697374558 + 24.06570982441908 * d;

    // Range (0..24) for gmst:
    return gmst - ( int )( gmst / 24.0 ) * 24.0;
}

void StarsPlugin::requestRepaint()
{
    emit repaintNeeded( QRegion() );
}

}

Q_EXPORT_PLUGIN2( StarsPlugin, Marble::StarsPlugin )

#include "StarsPlugin.moc"
