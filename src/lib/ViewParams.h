//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#ifndef MARBLE_VIEWPARAMS_H
#define MARBLE_VIEWPARAMS_H


/** @file
 * This file contains the headers for ViewParameters.
 * 
 * @author Inge Wallin  <inge@lysator.liu.se>
 */

#include <QtCore/QString>

#include "marble_export.h"

#include "Quaternion.h"
#include "global.h"


class QImage;

namespace Marble
{

class AbstractProjection;
class ViewportParams;
class ViewParamsPrivate;
class GeoSceneDocument;

/** 
 * @short A public class that controls the painting of a MarbleWidget
 *
 */

class MARBLE_EXPORT ViewParams
{
 public:
    ViewParams();
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
    //QString mapThemeId() const;
    GeoSceneDocument *mapTheme();

    // Convenience methods:

    Projection projection() const;
    const AbstractProjection *currentProjection() const;
    void setProjection(Projection newProjection);

    MapQuality mapQuality() const;
    void setMapQuality( MapQuality );

    /**
     * @brief  Set the value of a map theme property
     * @param  name  the property name
     * @param  value  the value of the property
     * @return @c true  the property was found and changed accordingly
     *         @c false the property couldn't be found here
     */
    void setPropertyValue( const QString &name, bool value );

    /**
     * @brief  Get the availability of a property across groups
     * @param  name  the property name
     * @param  available  availability of the property
     * @return @c true  the property was registered in the settings
     *         @c false the property wasn't registered in the settings
     */
    void propertyAvailable( const QString& name, bool& available  );

    /**
     * @brief  Get the value of a property across groups
     * @param  name  the property name
     */
    void propertyValue( const QString& name, bool &value );

    int radius() const;
    void setRadius( int newRadius );

    Quaternion planetAxis() const;
    void setPlanetAxis( const Quaternion &newAxis );

    void centerCoordinates( qreal &centerLon, qreal &centerLat );

    int width() const;
    int height() const;

    /**
     * @brief Sets the size of the viewport and the sizes of
     * canvasImage and coastImage.
     *
     * @param width new width of viewport
     * @param height new height of viewport
     */
    void setSize( int width, int height );

    QImage * canvasImage() const;

    QImage * coastImage() const;

    bool showGps() const;
    void setShowGps( bool );

    bool showElevationModel() const;
    void setShowElevationModel( bool );

    bool showAtmosphere() const;
    void setShowAtmosphere( bool );

    bool showClouds() const;
    void setShowClouds( bool const );

 private:
    Q_DISABLE_COPY( ViewParams )
    ViewParamsPrivate * const d;
};

}

#endif
