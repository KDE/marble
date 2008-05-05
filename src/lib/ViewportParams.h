//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef VIEWPORTPARAMS_H
#define VIEWPORTPARAMS_H


/** @file
 * This file contains the headers for ViewportParams.
 * 
 * @author Inge Wallin  <inge@lysator.liu.se>
 */


//#include "marble_export.h"

#include <QtCore/QSize>
#include <QtCore/QDebug>

#include "Quaternion.h"
#include "BoundingBox.h"
#include "global.h"
#include "SphericalProjection.h"
#include "EquirectProjection.h"
#include "MercatorProjection.h"

using namespace Marble;

/** 
 * @short A public class that controls what is visible in the viewport of a Marble map.
 *
 */

class ViewportParams
{
 public:
    ViewportParams( );
    ~ViewportParams();

    // Getters and setters
    Projection projection() const;
    AbstractProjection *currentProjection() const;
    void setProjection(Projection newProjection);

    int radius() const;
    void setRadius(int newRadius);

    Quaternion planetAxis() const;
    void setPlanetAxis(const Quaternion &newAxis);
    matrix * planetAxisMatrix() const;

    int width()  const;
    int height() const;
    QSize size() const;

    void setWidth(int newWidth);
    void setHeight(int newHeight);
    void setSize(QSize newSize);

    Marble::MapQuality mapQuality();
    void setMapQuality( Marble::MapQuality );

    // Other functions
    void centerCoordinates( double &centerLon, double &centerLat ) const;

    BoundingBox m_boundingBox;  // What the view currently can see

 private:
    // These two go together.  m_currentProjection points to one of
    // the static Projection classes at the bottom.
    Projection           m_projection;
    AbstractProjection  *m_currentProjection;
    MapQuality           m_mapQuality;

    // Parameters that determine the painting
    Quaternion           m_planetAxis;   // Position, coded in a quaternion
    mutable matrix       m_planetAxisMatrix;
    int                  m_radius;       // Zoom level (pixels / globe radius)

    QSize                m_size;         // width, height

    //BoundingBox m_boundingBox;  // What the view currently can see


    static SphericalProjection  s_sphericalProjection;
    static EquirectProjection   s_equirectProjection;
    static MercatorProjection   s_mercatorProjection;
};


#endif // VIEWPORTPARAMS_H
