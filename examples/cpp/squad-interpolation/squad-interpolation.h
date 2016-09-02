//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef SQUAD_INTERPOLATION
#define SQUAD_INTERPOLATION

#include <marble/MarbleWidget.h>
#include <marble/MarbleMap.h>
#include <marble/MarbleModel.h>
#include <marble/GeoPainter.h>
#include <marble/GeoDataLineString.h>
#include <marble/LayerInterface.h>

#include <QApplication>

namespace Marble {

class MyPaintLayer : public QObject, public LayerInterface
{
    Q_OBJECT

public:
    // Constructor
    explicit MyPaintLayer( MarbleWidget* widget );

    // LayerInterface
    QStringList renderPosition() const;
    bool render( GeoPainter *painter, ViewportParams *viewport,
                const QString &renderPos, GeoSceneLayer *layer);
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
