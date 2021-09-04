// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Dennis Nienhüser <nienhueser@kde.org>
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
    QStringList renderPosition() const override;
    bool render( GeoPainter *painter, ViewportParams *viewport,
                const QString &renderPos, GeoSceneLayer *layer) override;
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
