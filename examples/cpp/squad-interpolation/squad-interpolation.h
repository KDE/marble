//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef SQUAD_INTERPOLATION
#define SQUAD_INTERPOLATION

#include <marble/MarbleWidget.h>
#include <marble/MarbleMap.h>
#include <marble/MarbleModel.h>
#include <marble/GeoPainter.h>
#include <GeoDataLineString.h>
#include <marble/LayerInterface.h>

#include <QTime>
#include <QTimer>
#include <QApplication>
#include <QKeyEvent>

namespace Marble {

class MyPaintLayer : public QObject, public LayerInterface
{
    Q_OBJECT

public:
    // Constructor
    MyPaintLayer( MarbleWidget* widget );

    // LayerInterface
    QStringList renderPosition() const;
    bool render( GeoPainter *painter, ViewportParams *viewport,
       const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );
    GeoDataLatLonBox center() const;

private Q_SLOTS:
    void addInterpolatedPoint();

private:
    void addRandomCity( double minDistanceKm=500, double maxDistanceKm=1500 );
    static GeoDataCoordinates basePoint( const GeoDataCoordinates &q1, const GeoDataCoordinates &q2, const GeoDataCoordinates &q3 );

    MarbleWidget* m_widget;
    GeoDataLineString m_cities;
    GeoDataLineString m_interpolated;
    double m_fraction;
    double m_delta;
    int m_index;
};

}

#endif // SQUAD_INTERPOLATION
