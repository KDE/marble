//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

#include "TestPlugin.h"

#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>
#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"

namespace Marble
{

QStringList TestPlugin::backendTypes() const
{
    return QStringList( "test" );
}

QString TestPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList TestPlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString TestPlugin::name() const
{
    return tr( "Test Plugin" );
}

QString TestPlugin::guiString() const
{
    return tr( "&Test Plugin" );
}

QString TestPlugin::nameId() const
{
    return QString( "test-plugin" );
}

QString TestPlugin::description() const
{
    return tr( "This is a simple test plugin." );
}

QIcon TestPlugin::icon () const
{
    return QIcon();
}


void TestPlugin::initialize ()
{
}

bool TestPlugin::isInitialized () const
{
    return true;
}

bool TestPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    painter->autoMapQuality();

    // Example: draw a straight line

    GeoDataCoordinates northpole1( 0.0, 90.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates northpole2( 0.0, 90.0, 3000000.0, GeoDataCoordinates::Degree );

    painter->setPen( QColor( 255, 255, 255, 255 ) );

    painter->drawLine( northpole1, northpole2 );

    // Example: draw a straight line string ("polyline")

    GeoDataCoordinates madrid( -3.7, 40.4, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates flensburg( 9.4, 54.8, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates linkoeping( 15.6, 58.4, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates istanbul( 28.0, 41.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates moscow( 37.6, 55.75, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates brasilia( -47.9, -15.75, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates orbit( 105.6, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates easteregg( 10.0, 70.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates easteregg2( 179.0, -40.0, 0.0, GeoDataCoordinates::Degree );

    painter->setPen( QColor( 200, 200, 200, 255 ) );

    GeoDataLineString lineString;
    lineString << madrid << flensburg << linkoeping << istanbul << moscow;

    painter->drawPolyline( lineString ); 

    // Example: draw plain filled circles with text on earth and in earth orbit

    painter->setPen( QColor( 99, 198, 99, 255 ) );
    painter->setBrush( QColor( 99, 198, 99, 80 ) );
    painter->drawEllipse( flensburg, 30, 30 ); 

    painter->drawText( flensburg, "Torsten" );

    painter->setPen( QColor( 198, 99, 99, 255 ) );
    painter->setBrush( QColor( 198, 99, 99, 80 ) );
    painter->drawEllipse( linkoeping, 30, 30 ); 

    painter->drawText( linkoeping, "Inge" );

    painter->drawEllipse( easteregg, 20, 10, true ); 

    painter->drawText( easteregg, "Easter Egg" );

    painter->drawEllipse( easteregg2, 20, 20, true );


    painter->setPen( QColor( 99, 99, 198, 255 ) );
    painter->setBrush( QColor( 99, 99, 198, 80 ) );
    painter->drawEllipse( orbit, 20, 20 ); 

    painter->drawText( orbit, "Claudiu" );

    // Example: draw plain pixmaps

    painter->drawPixmap( istanbul, QPixmap( MarbleDirs::path( "bitmaps/earth_apollo.jpg" ) ) ); 

    painter->drawImage( brasilia, QImage( MarbleDirs::path( "bitmaps/earth_apollo.jpg" ) ) ); 

    // Example: draw a plain rectangle and a rounded rectangle

    painter->setPen( QColor( 99, 198, 198, 255 ) );
    QBrush brush( QColor( 99, 198, 198, 80 ) );
    painter->setBrush( brush );

    painter->drawRect( madrid, 30, 30 ); 

    painter->setPen( QColor( 198, 99, 198, 255 ) );
    brush.setColor( QColor( 198, 99, 198, 180 ) );
    brush.setStyle( Qt::DiagCrossPattern );
    painter->setBrush( brush );

    painter->drawRoundRect( moscow, 40, 40 ); 

    // Example: draw earth orbit

    GeoDataCoordinates m1(-180.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m2(-90.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m3(0.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m4(+90.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates m5(+180.0, 0.0, 3000000.0, GeoDataCoordinates::Degree );

    GeoDataLineString ring( Tessellate );

    ring << m1 << m2 << m3 << m4 << m5;

    painter->drawPolyline( ring ); 

    // Example: draw a triangle with lines that follow the coordinate grid

    painter->setPen( QColor( 198, 99, 99, 255 ) );
    brush.setColor( QColor( 198, 99, 99, 180 ) );
    brush.setStyle( Qt::FDiagPattern );
    painter->setBrush( brush );

    GeoDataCoordinates t1(0.0, 90.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t2(-12.5, 45.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t3(-77.5, 45.0, 0.0, GeoDataCoordinates::Degree );

    GeoDataLinearRing triangle( Tessellate | RespectLatitudeCircle );

    triangle << t1 << t2 << t3;

    painter->drawPolygon( triangle, Qt::OddEvenFill ); 

    // Example: draw a triangle with lines that follow the great circles

    GeoDataLinearRing triangle2( Tessellate );

    GeoDataCoordinates t4(0.0, 90.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t5(-102.5, 45.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t6(-167.5, 45.0, 0.0, GeoDataCoordinates::Degree );

    triangle2 << t4 << t5 << t6;

    painter->drawPolygon( triangle2, Qt::OddEvenFill ); 

    // Example: draw a triangle with straight lines

    GeoDataLinearRing triangle3;

    GeoDataCoordinates t7(0.0, 90.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t8(102.5, 35.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t9(167.5, 45.0, 0.0, GeoDataCoordinates::Degree );

    triangle3 << t7 << t8 << t9;

    painter->drawPolygon( triangle3, Qt::OddEvenFill ); 


    // Example: draw a polygon across the dateline

    GeoDataLinearRing dateLinePolygon( Tessellate | RespectLatitudeCircle );

    GeoDataCoordinates t10(+170.0, 40.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t11(-170.0, 40.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t12(-170.0, 35.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t13(+175.0, 35.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t14(+175.0, 25.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t15(-170.0, 25.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t16(-170.0, 15.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates t17(+170.0, 20.0, 0.0, GeoDataCoordinates::Degree );

    dateLinePolygon << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17;

    painter->drawPolygon( dateLinePolygon, Qt::OddEvenFill );

    // Example: draw a rectangle with lines that follow the coordinate grid

    GeoDataCoordinates rectCenter( -45.0, 20.0, 0.0, GeoDataCoordinates::Degree );
    painter->drawRect( rectCenter, 20.0, 20.0, true ); 


    // Example: draw annotations

    GeoDataCoordinates sotm(4.89, 52.37, 0.0, GeoDataCoordinates::Degree );

    painter->setPen( QColor( 198, 99, 99, 255 ) );
    brush.setColor( QColor( 255, 255, 255, 200 ) );
    brush.setStyle( Qt::SolidPattern );
    painter->setBrush( brush );

    painter->drawAnnotation (  sotm, "State of the Map,\n  10-12 July 2009,\n OSM conference", QSize(140,100), 10, 30, 15, 15 );

    GeoDataCoordinates akademy2009( -15.43, 28.1, 0.0, GeoDataCoordinates::Degree );

    painter->setPen( QColor( 99, 99, 0 ) );

    QRadialGradient radialGrad(QPointF(100, 100), 100);
    radialGrad.setColorAt(0, QColor( 198, 198, 198, 200 ) );
    radialGrad.setColorAt(0.5, QColor( 199, 198, 99, 200  ) );
    radialGrad.setColorAt( 1, Qt::white );
    radialGrad.setSpread( QGradient::ReflectSpread );

    QBrush gradientBrush( radialGrad );
    painter->setBrush( gradientBrush );

    painter->drawAnnotation (  akademy2009, "Akademy 2009,\n  3-11 July 2009,\n KDE conference" );

    return true;
}

}

Q_EXPORT_PLUGIN2( TestPlugin, Marble::TestPlugin )

#include "TestPlugin.moc"
