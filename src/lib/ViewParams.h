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

#include <QtCore/QSharedPointer>
#include <QtCore/QString>

#include "Quaternion.h"
#include "global.h"


class QImage;

namespace Marble
{

class AbstractProjection;
class ViewportParams;
class ViewParamsPrivate;

/** 
 * @short A public class that controls the painting of a MarbleWidget
 *
 */

class ViewParams
{
 public:
    ViewParams();
    ~ViewParams();

    // Getters and setters
    ViewportParams  *viewport();

    // Convenience methods:

    Projection projection() const;
    const AbstractProjection *currentProjection() const;
    void setProjection(Projection newProjection);

    MapQuality mapQuality( ViewContext viewContext ) const;
    MapQuality mapQuality() const;
    void setMapQualityForViewContext( MapQuality quality, ViewContext viewContext );

    ViewContext viewContext() const;
    void setViewContext( ViewContext viewContext );

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

    QSharedPointer<QImage> canvasImagePtr() const;
    QImage * canvasImage() const;

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
