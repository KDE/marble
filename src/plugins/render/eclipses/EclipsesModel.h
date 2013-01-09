//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_ECLIPSESMODEL_H
#define MARBLE_ECLIPSESMODEL_H

#include <QtCore/QObject>
#include <QDateTime>
#include <QPixmap>

#include "GeoDataCoordinates.h"

class EclSolar;

namespace Marble
{

class MarbleClock;
class EclipsesItem;
class GeoPainter;
class GeoDataCoordinates;

class EclipsesModel : public QObject
{
    Q_OBJECT
public:
    EclipsesModel( const MarbleClock *clock, QObject *parent = 0 );
    ~EclipsesModel();

    const GeoDataCoordinates& observationPoint() const;
    void setObservationPoint( const GeoDataCoordinates &coords );

    void setYear( int year );
    int year() const;

    EclipsesItem* currentItem() const;
    EclipsesItem* eclipseWithIndex( int index ) const;

    void synchronize( const MarbleClock *clock );

    QList<EclipsesItem*> items() const;

    void paint( GeoPainter *painter );

private:
    void paintItem( EclipsesItem *item, GeoPainter *painter );
    void addItem( EclipsesItem *item );
    void clear();
    void updateEclipses();
    void update();

    const MarbleClock *m_clock;
    EclSolar *m_ecps;
    QList<EclipsesItem*> m_items;
    EclipsesItem *m_currentItem;
    int m_currentYear;
    GeoDataCoordinates m_observationPoint;
};

}

#endif // MARBLE_ECLIPSESMODEL_H
