//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

#include "MarbleTestPlugin.h"

#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>
#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoDataCoordinates.h"

namespace Marble
{

QStringList MarbleTestPlugin::backendTypes() const
{
    return QStringList( "test" );
}

QString MarbleTestPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList MarbleTestPlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString MarbleTestPlugin::name() const
{
    return tr( "Test Plugin" );
}

QString MarbleTestPlugin::guiString() const
{
    return tr( "&Test Plugin" );
}

QString MarbleTestPlugin::nameId() const
{
    return QString( "test-plugin" );
}

QString MarbleTestPlugin::description() const
{
    return tr( "This is a simple test plugin." );
}

QIcon MarbleTestPlugin::icon () const
{
    return QIcon();
}


void MarbleTestPlugin::initialize ()
{
}

bool MarbleTestPlugin::isInitialized () const
{
    return true;
}

bool MarbleTestPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    painter->autoMapQuality();

    GeoDataCoordinates northpole1( 0.0, 90.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates northpole2( 0.0, 90.0, 3000000.0, GeoDataCoordinates::Degree );

    painter->setPen( QColor( 255, 255, 255, 255 ) );

    painter->drawLine( northpole1, northpole2 );

    GeoDataCoordinates madrid( -3.7, 40.4, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates flensburg( 9.4, 54.8, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates linkoeping( 15.6, 58.4, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates istanbul( 28.0, 41.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates moscow( 37.6, 55.75, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates brasilia( -47.9, -15.75, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates orbit( 105.6, 0.0, 3000000.0, GeoDataCoordinates::Degree );

    painter->setPen( QColor( 200, 200, 200, 255 ) );
    static const GeoDataCoordinates points[5] = {
        madrid,
        flensburg,
        linkoeping,
        istanbul,
        moscow
    };

    painter->drawPolyline( points, 5 ); 


    painter->setPen( QColor( 99, 198, 99, 255 ) );
    painter->setBrush( QColor( 99, 198, 99, 80 ) );
    painter->drawEllipse( flensburg, 30, 30 ); 

    painter->drawText( flensburg, "Torsten" );

    painter->setPen( QColor( 198, 99, 99, 255 ) );
    painter->setBrush( QColor( 198, 99, 99, 80 ) );
    painter->drawEllipse( linkoeping, 40, 40 ); 

    painter->drawText( linkoeping, "Inge" );

    painter->setPen( QColor( 99, 99, 198, 255 ) );
    painter->setBrush( QColor( 99, 99, 198, 80 ) );
    painter->drawEllipse( orbit, 20, 20 ); 

    painter->drawText( orbit, "Claudiu" );

    painter->drawPixmap( istanbul, QPixmap( MarbleDirs::path( "bitmaps/earth_apollo.jpg" ) ) ); 

    painter->drawImage( brasilia, QImage( MarbleDirs::path( "bitmaps/earth_apollo.jpg" ) ) ); 


    painter->setPen( QColor( 99, 198, 198, 255 ) );
    QBrush brush( QColor( 99, 198, 198, 80 ) );
    painter->setBrush( brush );

    painter->drawRect( madrid, 30, 30 ); 

    painter->setPen( QColor( 198, 99, 198, 255 ) );
    brush.setColor( QColor( 198, 99, 198, 180 ) );
    brush.setStyle( Qt::DiagCrossPattern );
    painter->setBrush( brush );

    painter->drawRoundRect( moscow, 40, 40 ); 

    GeoDataCoordinates m1(-180.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m2(-170.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m3(-160.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m4(-150.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m5(-140.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m6(-130.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m7(-120.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m8(-110.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m9(-100.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m10(-90.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m11(-80.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m12(-70.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m13(-60.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m14(-50.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m15(-40.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m16(-30.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m17(-20.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m18(-10.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );

    GeoDataCoordinates m19(0.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m20(10.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m21(20.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m22(30.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m23(40.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m24(50.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m25(60.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m26(70.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m27(80.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m28(90.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m29(100.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m30(110.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m31(120.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m32(130.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m33(140.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m34(150.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m35(160.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m36(170.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m37(180.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );

    static const GeoDataCoordinates ring[37] = {
        m1, m2, m3, m4, m5, m6, m7, m8, m9,
        m10, m11, m12, m13, m14, m15, m16, m17, m18, m19,
        m20, m21, m22, m23, m24, m25, m26, m27, m28, m29,
        m30, m31, m32, m33, m34, m35, m36, m37
    };

    painter->drawPolyline( ring, 37 ); 

    GeoDataCoordinates t1(0.0, 90.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t2(-45.0, 60.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t3(-135.0, 60.0, 0.0, GeoDataCoordinates::Degree );

    static const GeoDataCoordinates triangle[3] = {
        t1, t2, t3
    };

    painter->setPen( QColor( 198, 99, 99, 255 ) );
    brush.setColor( QColor( 198, 99, 99, 180 ) );
    brush.setStyle( Qt::FDiagPattern );
    painter->setBrush( brush );

    painter->drawPolygon( triangle, 3 ); 

    GeoDataCoordinates sotm(-8.6, 52.66, 0.0, GeoDataCoordinates::Degree );

    painter->setPen( QColor( 198, 99, 99, 255 ) );
    brush.setColor( QColor( 255, 255, 255, 200 ) );
    brush.setStyle( Qt::SolidPattern );
    painter->setBrush( brush );

    painter->drawAnnotation (  sotm, "State of the Map,\n  12-13 July 2008,\n OSM conference" );

    GeoDataCoordinates akademy2008(4.5, 51.068, 0.0, GeoDataCoordinates::Degree );

    painter->setPen( QColor( 99, 99, 0 ) );

    QRadialGradient radialGrad(QPointF(100, 100), 100);
    radialGrad.setColorAt(0, QColor( 198, 198, 198, 200 ) );
    radialGrad.setColorAt(0.5, QColor( 199, 198, 99, 200  ) );
    radialGrad.setColorAt( 1, Qt::white );
    radialGrad.setSpread( QGradient::ReflectSpread );

    QBrush gradientBrush( radialGrad );
    painter->setBrush( gradientBrush );

    painter->drawAnnotation (  akademy2008, "Akademy 2008,\n  9-15 August 2008,\n KDE conference", QSize(130, 120), 10, 30, 15, 15 );

    return true;
}

}

Q_EXPORT_PLUGIN2(MarbleTestPlugin, Marble::MarbleTestPlugin)

#include "MarbleTestPlugin.moc"
