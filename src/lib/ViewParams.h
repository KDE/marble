//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef VIEWPARAMS_H
#define VIEWPARAMS_H


/** @file
 * This file contains the headers for ViewParameters.
 * 
 * @author Inge Wallin  <inge@lysator.liu.se>
 */


#include "marble_export.h"

#include <QtCore/QDebug>

#include "Quaternion.h"
#include "BoundingBox.h"
#include "global.h"


class QImage;


/** 
 * @short A public class that controls the painting of a MarbleWidget
 *
 */

class ViewParams
{
 public:
    ViewParams( );

    Projection  m_projection;
    Projection  m_oldProjection;

    // Parameters that determine the painting
    int         m_radius;       // Zoom level (pixels / earth radius)
    int         m_radiusUpdated;
    Quaternion  m_planetAxis;   // Position, coded in a quaternion
    Quaternion  m_planetAxisUpdated;

    void centerCoordinates( double &centerLon, double &centerLat ){
/*
            if ( m_planetAxis == m_planetAxisUpdated && m_radius == m_radiusUpdated )
            {
                centerLon = m_centerLon;
                centerLat = m_centerLat;
            }
            else
            {
*/
                // Calculate translation of center point
                centerLon = m_planetAxis.yaw() + M_PI;
                centerLat = m_planetAxis.pitch() + M_PI;
                if ( centerLat > M_PI ) centerLat -= 2 * M_PI; 
//            }
    }

    BoundingBox m_boundingBox;  // What the view currently can see

    // Show/don't show options
    bool        m_showGrid;
    bool        m_showPlaceMarks;
    bool        m_showElevationModel;

    bool        m_showRelief;   // for TextureColorizer.

    bool        m_showIceLayer; // for VectorComposer
    bool        m_showBorders;
    bool        m_showRivers;
    bool        m_showLakes;

    bool        m_showCities;   // About placemarks
    bool        m_showTerrain;
    bool        m_showOtherPlaces;
    
    bool        m_showGps; //for gps layer

    // Cached data that will make painting faster.
    QImage  *m_canvasImage;     // Base image with space and atmosphere
    QImage  *m_coastImage;      // A slightly higher level image.

 private:
    double     m_centerLon;         // cached value
    double     m_centerLat;         // cached value
};


#endif // VIEWPARAMS_H
