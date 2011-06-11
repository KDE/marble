//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Niko Sams <niko.sams@gmail.com>
//


#include "AltitudeProfile.h"
#include <GeoPainter.h>
#include <GeoSceneLayer.h>
#include <ViewportParams.h>
#include <routing/RoutingManager.h>
#include <routing/AlternativeRoutesModel.h>
#include <GeoDataDocument.h>
#include <MarbleDebug.h>
#include <KPlotWidget>
#include <KPlotObject>
#include <KPlotAxis>
#include <AltitudeModel.h>
#include <MarbleModel.h>

using namespace Marble;

AltitudeProfile::AltitudeProfile(const QPointF& point, const QSizeF& size)
    : AbstractFloatItem(point, size), m_isInitialized(false)
{

}

QStringList AltitudeProfile::backendTypes() const
{
    return QStringList( "altitudeProfile" );
}

bool AltitudeProfile::isInitialized() const
{
    return m_isInitialized;
}

void AltitudeProfile::initialize()
{
    m_isInitialized = true;
    connect( marbleModel()->routingManager()->alternativeRoutesModel(), SIGNAL( currentRouteChanged( GeoDataDocument* ) ), SLOT( currentRouteChanged( GeoDataDocument* ) ) );

    //marbleModel()->altitudeModel()->height(47.95, 13.23);
    qDebug() << marbleModel()->altitudeModel()->height(47.8481, 13.0734);
    qDebug() << marbleModel()->altitudeModel()->height(47.8478, 13.0731);
    //POINT 322 13.0734 47.8481 height 285
}

void AltitudeProfile::currentRouteChanged( GeoDataDocument* route )
{
    marbleModel()->altitudeModel()->height(47.95, 13.23);
    
    KPlotWidget *graphWidget = new KPlotWidget();
    graphWidget->setAntialiasing(true);
    graphWidget->axis(KPlotWidget::TopAxis)->setVisible(false);
    graphWidget->axis(KPlotWidget::RightAxis)->setVisible(false);
    graphWidget->resetPlot();
    graphWidget->axis(KPlotWidget::LeftAxis)->setLabel(QString());

    KPlotObject* plot = new KPlotObject(Qt::red, KPlotObject::Lines, 3);

    qint32 minY = INT_MAX;
    qint32 maxY = 0;
    quint32 numDataPoints = 0;
    qDebug() << "*************************";

    GeoDataPlacemark* routePlacemark = route->placemarkList().first();
    Q_ASSERT(routePlacemark->geometry()->geometryId() ==  GeoDataLineStringId);
    GeoDataLineString* routeWaypoints = static_cast<GeoDataLineString*>(routePlacemark->geometry());
    qDebug() << routeWaypoints->length( EARTH_RADIUS );
    qreal totalIncrease = 0;
    qreal lastAltitude = -100000;
    for(int i=1; i < routeWaypoints->size(); ++i) {
        GeoDataCoordinates coordinate = routeWaypoints->at( i );
        GeoDataCoordinates coordinatePrev = routeWaypoints->at( i - 1 );
        //qreal altitude = marbleModel()->altitudeModel()->height(coordinate.latitude(Marble::GeoDataCoordinates::Degree), coordinate.longitude(Marble::GeoDataCoordinates::Degree));
        QList<qreal> altitudes = marbleModel()->altitudeModel()->heightProfile(
            coordinatePrev.latitude(Marble::GeoDataCoordinates::Degree),
            coordinatePrev.longitude(Marble::GeoDataCoordinates::Degree),
            coordinate.latitude(Marble::GeoDataCoordinates::Degree),
            coordinate.longitude(Marble::GeoDataCoordinates::Degree)
        );
        foreach(const qreal altitude, altitudes) {
            qDebug() << "POINT" << numDataPoints << coordinate.longitude(Marble::GeoDataCoordinates::Degree) << coordinate.latitude(Marble::GeoDataCoordinates::Degree)
                    << "height" << altitude;
            if ( lastAltitude != -100000 && altitude > lastAltitude ) {
                totalIncrease += altitude - lastAltitude;
                qDebug() << "INCREASE +=" << altitude - lastAltitude << "totalIncrease is now" << totalIncrease;
            }

            double value = altitude;
            //qDebug() << "value" << value;
            plot->addPoint(numDataPoints++, value);
            if (value > maxY) maxY = value;
            if (value < minY) minY = value;
            lastAltitude = altitude;
        }
    }
    qDebug() << "TOTAL INCREASE" << totalIncrease;

/*
    QImage image;
    image.load("/home/niko/kdesvn/build/marble-git/tools/altitude-reader/N47E013.hgt.png");
    //foreach ( GeoDataPlacemark *placemark, route->placemarkList() ) {
    GeoDataPlacemark* routePlacemark = route->placemarkList().first();
    Q_ASSERT(routePlacemark->geometry()->geometryId() ==  GeoDataLineStringId);
    GeoDataLineString* routeWaypoints = static_cast<GeoDataLineString*>(routePlacemark->geometry());
    mDebug() << routeWaypoints->length( EARTH_RADIUS );
    for(int i=0; i < routeWaypoints->size(); ++i) {
        GeoDataCoordinates coordinate = routeWaypoints->at( i );
        //coordinate.altitude();
        int x = 1200 * (coordinate.longitude(Marble::GeoDataCoordinates::Degree)-13);
        int y = 1200 * (1-(coordinate.latitude(Marble::GeoDataCoordinates::Degree)-47));
        uint altitude = image.pixel(x, y) - 0xFF000000;
        mDebug() << numDataPoints << coordinate.longitude(Marble::GeoDataCoordinates::Degree) << coordinate.latitude(Marble::GeoDataCoordinates::Degree)
                 << x << y << altitude;

        double value = altitude;
        plot->addPoint(numDataPoints++, value);
        if (value > maxY) maxY = value;
        if (value < minY) minY = value;
    }
*/

    graphWidget->setLimits(0, numDataPoints, minY - minY / 5, maxY + maxY / 5);
    graphWidget->addPlotObject(plot);
    graphWidget->axis(KPlotWidget::LeftAxis)->setLabel(tr("Altitude"));
    graphWidget->axis(KPlotWidget::BottomAxis)->setTickLabelsShown(false);
    graphWidget->update();
    graphWidget->show();
}


QIcon AltitudeProfile::icon() const
{
    return QIcon();
}

QString AltitudeProfile::description() const
{
    return tr( "This is a float item that displays the altitude profile of a track." );
}

QString AltitudeProfile::nameId() const
{
    return QString( "altitudeProfile" );
}

QString AltitudeProfile::guiString() const
{
    return tr( "&Altitude Profile" );
}

QString AltitudeProfile::name() const
{
    return QString( "Altitude Profile" );
}

void AltitudeProfile::paintContent(GeoPainter* painter, ViewportParams* viewport, const QString& renderPos, GeoSceneLayer* layer)
{
    painter->drawRect( QRectF( QPoint( 10, 10 ), QPoint( 100, 100 ) ) );
}

Q_EXPORT_PLUGIN2( AltitudeProfile, Marble::AltitudeProfile )

#include "AltitudeProfile.moc"

