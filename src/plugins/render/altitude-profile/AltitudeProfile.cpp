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

    marbleModel()->altitudeModel()->height(1, 2);
}

void AltitudeProfile::currentRouteChanged( GeoDataDocument* route )
{
    marbleModel()->altitudeModel()->height(1, 2);
    
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

