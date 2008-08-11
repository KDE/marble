//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Inge Wallin  <ingwa@kde.org>
//


#ifndef SPHERICALPROJECTIONHELPER_H
#define SPHERICALPROJECTIONHELPER_H


/** @file
 * This file contains the headers for SphericalProjectionHelper.
 *
 * @author Inge Wallin  <inge@lysator.liu.se>
 */


#include "AbstractProjectionHelper.h"

/**
 * @short A base class for all projections in Marble.
 */

class SphericalProjectionHelper : public AbstractProjectionHelper
{
    // Not a QObject so far because we don't need to send signals.
 public:

    /**
     * @brief Construct a new SphericalProjection.
     */
    explicit SphericalProjectionHelper();

    virtual ~SphericalProjectionHelper();

    void paintBase( GeoPainter     *painter, 
		    ViewportParams *viewport,
		    QPen           &pen,
		    QBrush         &brush,
		    bool            antialiasing );

    void createActiveRegion( ViewportParams *viewport );
    void createProjectedRegion( ViewportParams *viewport );

 private:
    //SphericalProjectionPrivate  * const d;
};


#endif // SPHERICALPROJECTIONHELPER_H
