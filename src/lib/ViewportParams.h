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


class QImage;


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
    void setProjection(Projection newProjection);

    int radius() const;
    void setRadius(int newRadius);

    Quaternion axis() const;
    void setAxis(Quaternion newAxis);

    int width()  const;
    int height() const;
    QSize size() const;

    void setWidth(int newWidth);
    void setHeight(int newHeight);
    void setSize(QSize newSize);

    // Other functions
    void centerCoordinates( double &centerLon, double &centerLat );

 private:
    Projection  m_projection;

    // Parameters that determine the painting
    Quaternion  m_planetAxis;   // Position, coded in a quaternion
    int         m_radius;       // Zoom level (pixels / globe radius)

    QSize       m_size;         // width, height

    //BoundingBox m_boundingBox;  // What the view currently can see
};


#endif // VIEWPORTPARAMS_H
