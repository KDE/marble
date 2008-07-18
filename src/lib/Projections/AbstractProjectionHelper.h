//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Inge Wallin  <ingwa@kde.org>"
//


#ifndef ABSTRACTPROJECTIONHELPER_H
#define ABSTRACTPROJECTIONHELPER_H


/** @file
 * This file contains the headers for AbstractProjection.
 *
 * @author Inge Wallin  <inge@lysator.liu.se>
 */

#include <QtCore/QRect>

#include "GeoDataLatLonAltBox.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPoint.h"


class GeoPainter;
class ViewportParams;
class AbstractProjectionHelperPrivate;

/**
 * @short A base class for projection helpers in Marble.
 */

class AbstractProjectionHelper
{
    // Not a QObject so far because we don't need to send signals.
 public:

    /**
     * @brief Construct a new AbstractProjectionHelper.
     */
    explicit AbstractProjectionHelper();

    virtual ~AbstractProjectionHelper();

    virtual void paintBase( GeoPainter     *painter, 
			    ViewportParams *viewport, 
			    QPen           &pen,
			    QBrush         &brush,
			    bool            antialiasing ) = 0;

    virtual void createActiveRegion( ViewportParams *viewport ) = 0;
    void setActiveRegion( const QRegion& );

    virtual void createProjectedRegion( ViewportParams *viewport ) = 0;
    void setProjectedRegion( const QRegion& );

    virtual const QRegion activeRegion() const;
    virtual const QRegion projectedRegion() const;

    int navigationStripe() const;

 protected:
    AbstractProjectionHelperPrivate  * const d;  // Not exported so no need.
};


#endif // ABSTRACTPROJECTIONHELPER_H
