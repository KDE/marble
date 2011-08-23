//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESITEM_H
#define MARBLE_SATELLITESITEM_H

#include "AbstractDataPluginItem.h"

#include "sgp4/sgp4unit.h"

class QDialog;

namespace Marble {

class SatellitesItem : public AbstractDataPluginItem
{
    Q_OBJECT

public:
    SatellitesItem( const QString &name, const elsetrec &satrec, QObject *parent = 0 );
    virtual ~SatellitesItem();

    void paintViewport( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer = 0 );
    void paint( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer = 0 );
    QString itemType() const;
    bool initialized();
    bool operator<(const Marble::AbstractDataPluginItem *other ) const;
    QAction *action();

private slots:
    void showInfoDialog();

private:
    double m_earthRadius; // in km
    elsetrec m_satrec;
    QAction *m_action;
    QDialog *m_dialog;

    /**
     * @brief Create a GeoDataCoordinates object from cartesian coordinates(GEI)
     * @param x x coordinate in km
     * @param y y coordinate in km
     * @param z z coordinate in km
     */
    GeoDataCoordinates fromCartesian( double x, double y, double z );

    /**
     * @return The time since m_satrec epoch in minutes
     * @see m_satrec
     */
    double timeSinceEpoch();

    /**
     * @return The orbital period of the satellite in minutes
     */
    double period();

    /**
     * @return The apogee of the satellite in km
     */
    double apogee();
    /**
     * @return The perigee of the satellite in km
     */
    double perigee();
    /**
     * @return The semi-major axis in km
     */
    double semiMajorAxis();

    /**
     * @return The inclination in degrees
     */
    double inclination();
};

}

#endif // MARBLE_SATELLITESITEM_H
