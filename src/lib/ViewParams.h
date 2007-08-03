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

#include "Quaternion.h"


class QImage;


/** 
 * @short A public class that controls the painting of a MarbleWidget
 *
 */

class ViewParams
{
 public:
    ViewParams( );

 public:
    // Parameters that determine the painting
    int         m_radius;       // Zoom level (pixels / earth radius)
    int         m_radiusUpdated;
    Quaternion  m_planetAxis;   // Position, coded in a quaternion
    Quaternion  m_planetAxisUpdated;

    // Show/don't show options
    bool        m_showGrid;
    bool        m_showPlaceMarks;
    bool        m_showElevationModel;

    bool        m_showRelief;   // for texcolorizer.

    bool        m_showIceLayer; // for vectorcomposer
    bool        m_showBorders;
    bool        m_showRivers;
    bool        m_showLakes;

    bool        m_showCities;   // About placemarks
    bool        m_showTerrain;

    // Cached data that will make painting faster.
    QImage  *m_canvasImage;     // Base image with space and atmosphere
    QImage  *m_coastImage;      // A slightly higher level image.
};


#endif // VIEWPARAMS_H
