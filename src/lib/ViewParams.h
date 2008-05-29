//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//


#ifndef VIEWPARAMS_H
#define VIEWPARAMS_H


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

class AbstractProjection;
class GeoSceneDocument;
class ViewportParams;

class ViewParamsPrivate;

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
    QString mapThemeId();
    GeoSceneDocument *mapTheme();

    // Convenience methods:

    Projection projection() const;
    AbstractProjection *currentProjection() const;
    void setProjection(Projection newProjection);

    Marble::MapQuality mapQuality();
    void setMapQuality( Marble::MapQuality );

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

    QImage * canvasImage() const;
    void setCanvasImage( QImage * const );

    QImage * coastImage() const;
    void setCoastImage( QImage * const );

    bool showGps() const;
    void setShowGps( const bool );

    bool showElevationModel() const;
    void setShowElevationModel( const bool );

    bool showAtmosphere() const;
    void setShowAtmosphere( const bool );

    // FIXME: We should try to get rid of these
    int radiusUpdated() const;
    void setRadiusUpdated( const int );
    Quaternion planetAxisUpdated() const;
    void setPlanetAxisUpdated( const Quaternion & );

 private:
    Q_DISABLE_COPY( ViewParams )
    ViewParamsPrivate * const d;
};


#endif // VIEWPARAMS_H
