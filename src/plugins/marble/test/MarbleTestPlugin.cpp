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
#include "GeoDataPoint.h"


QStringList MarbleTestPlugin::backendTypes() const
{
    return QStringList( "test" );
}

QString MarbleTestPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QString MarbleTestPlugin::renderPosition() const
{
    return QString( "ALWAYS_ON_TOP" );
}

QString MarbleTestPlugin::name() const
{
    return QString( "Test Plugin" );
}

QString MarbleTestPlugin::nameId() const
{
    return QString( "test-plugin" );
}

QString MarbleTestPlugin::description() const
{
    return QString( "This is a simple test plugin." );
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

bool MarbleTestPlugin::render( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer )
{
    painter->autoMapQuality();

    GeoDataPoint northpole1( 0.0, 90.0, 0.0, GeoDataPoint::Degree );
    GeoDataPoint northpole2( 0.0, 90.0, 3000000.0, GeoDataPoint::Degree );

    painter->setPen( QColor( 255, 255, 255, 255 ) );

    painter->drawLine( northpole1, northpole2 );

    GeoDataPoint madrid( -3.7, 40.4, 0.0, GeoDataPoint::Degree );
    GeoDataPoint flensburg( 9.4, 54.8, 0.0, GeoDataPoint::Degree );
    GeoDataPoint linkoeping( 15.6, 58.4, 0.0, GeoDataPoint::Degree );
    GeoDataPoint istanbul( 28.0, 41.0, 0.0, GeoDataPoint::Degree );
    GeoDataPoint moscow( 37.6, 55.75, 0.0, GeoDataPoint::Degree );
    GeoDataPoint brasilia( -47.9, -15.75, 0.0, GeoDataPoint::Degree );
    GeoDataPoint orbit( 105.6, 0.0, 3000000.0, GeoDataPoint::Degree );

    painter->setPen( QColor( 200, 200, 200, 255 ) );
    static const GeoDataPoint points[5] = {
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

    GeoDataPoint m1(-180.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m2(-170.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m3(-160.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m4(-150.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m5(-140.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m6(-130.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m7(-120.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m8(-110.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m9(-100.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m10(-90.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m11(-80.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m12(-70.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m13(-60.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m14(-50.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m15(-40.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m16(-30.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m17(-20.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m18(-10.0, 0.0, 3000000.0, GeoDataPoint::Degree );

    GeoDataPoint m19(0.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m20(10.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m21(20.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m22(30.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m23(40.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m24(50.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m25(60.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m26(70.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m27(80.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m28(90.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m29(100.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m30(110.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m31(120.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m32(130.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m33(140.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m34(150.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m35(160.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m36(170.0, 0.0, 3000000.0, GeoDataPoint::Degree );
    GeoDataPoint m37(180.0, 0.0, 3000000.0, GeoDataPoint::Degree );

    static const GeoDataPoint ring[37] = {
        m1, m2, m3, m4, m5, m6, m7, m8, m9,
        m10, m11, m12, m13, m14, m15, m16, m17, m18, m19,
        m20, m21, m22, m23, m24, m25, m26, m27, m28, m29,
        m30, m31, m32, m33, m34, m35, m36, m37
    };

    painter->drawPolyline( ring, 37 ); 

    GeoDataPoint t1(0.0, 90.0, 0.0, GeoDataPoint::Degree );
    GeoDataPoint t2(-45.0, 60.0, 0.0, GeoDataPoint::Degree );
    GeoDataPoint t3(-135.0, 60.0, 0.0, GeoDataPoint::Degree );

    static const GeoDataPoint triangle[3] = {
        t1, t2, t3
    };

    painter->setPen( QColor( 198, 99, 99, 255 ) );
    brush.setColor( QColor( 198, 99, 99, 180 ) );
    brush.setStyle( Qt::FDiagPattern );
    painter->setBrush( brush );

    painter->drawPolygon( triangle, 3 ); 

    GeoDataPoint sotm(-8.6, 52.66, 0.0, GeoDataPoint::Degree );

    painter->setPen( QColor( 198, 99, 99, 255 ) );
    brush.setColor( QColor( 255, 255, 255, 200 ) );
    brush.setStyle( Qt::SolidPattern );
    painter->setBrush( brush );

    painter->drawAnnotation (  sotm, "State of the Map,\n  12-13 July 2008,\n OSM conference" );

    GeoDataPoint akademy2008(4.5, 51.1, 0.0, GeoDataPoint::Degree );

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

Q_EXPORT_PLUGIN2(MarbleTestPlugin, MarbleTestPlugin)

#include "MarbleTestPlugin.moc"
