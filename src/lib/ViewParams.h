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

#include "GeoSceneDocument.h"
#include "Quaternion.h"
#include "ViewportParams.h"
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
    ~ViewParams();

    // Getters and setters
    ViewportParams  *viewport();

/** 
 *  The actual parameters for the look get loaded 
 *  via setMapThemeID.
 *  mapTheme returns a pointer which then can get
 *  passed on to the MarbleModel and the MarbleLegendBrowser. 
 */
    void setMapThemeId( const QString& );
    QString mapThemeId();
    GeoSceneDocument *mapTheme();

    // Convenience methods:

    Projection projection() const;
    AbstractProjection *currentProjection() const;
    void setProjection(Projection newProjection);

    /**
     * @brief  Set the value of a map theme property
     * @param  name  the property name
     * @param  value  the value of the property
     * @return @c true  the property was found and changed accordingly
     *         @c false the property couldn't be found here
     */
    void setPropertyValue( const QString &name, bool value );

    /**
     * @brief  Get the value of a property across groups
     * @param  name  the property name
     */
    void propertyValue( const QString& name, bool &value );

    int radius() const;
    void setRadius(int newRadius);

    Quaternion planetAxis() const;
    void setPlanetAxis(const Quaternion &newAxis);

    void centerCoordinates( double &centerLon, double &centerLat );

 public:
    GeoSceneDocument *m_mapTheme;

    ViewportParams  m_viewport;


//  FIXME: We should try to get rid of these two:    
    Quaternion  m_planetAxisUpdated;
    int         m_radiusUpdated;

    // Parameters that determine the painting
    // Show/don't show options

    // FIXME: All of these parameters should get stored as a GeoSceneSettings 
    //        property object in the future instead ...
    bool        m_showAtmosphere;

    bool        m_showElevationModel;
    
    bool        m_showGps; //for gps layer

    // Cached data that will make painting faster.
    QImage  *m_canvasImage;     // Base image with space and atmosphere
    QImage  *m_coastImage;      // A slightly higher level image.
};


#endif // VIEWPARAMS_H
