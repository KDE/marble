//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Inge Wallin  <ingwa@kde.org>
//


#ifndef MERCATORPROJECTIONHELPER_H
#define MERCATORPROJECTIONHELPER_H


/** @file
 * This file contains the headers for MercatorProjectionHelper.
 *
 * @author Inge Wallin  <inge@lysator.liu.se>
 */


#include "AbstractProjectionHelper.h"

/**
 * @short The projection helper for the Mercator projection
 */

namespace Marble
{

class MercatorProjectionHelper : public AbstractProjectionHelper
{
    // Not a QObject so far because we don't need to send signals.
 public:

    /**
     * @brief Construct a new MercatorProjection.
     */
    explicit MercatorProjectionHelper();

    virtual ~MercatorProjectionHelper();

    void paintBase( GeoPainter     *painter, 
                    ViewportParams *viewport,
                    QPen           &pen,
                    QBrush         &brush,
                    bool            antialiasing );

    void createActiveRegion( ViewportParams *viewport );
    void createProjectedRegion( ViewportParams *viewport );

 private:
    //MercatorProjectionPrivate  * const d;
};

}

#endif // MERCATORPROJECTIONHELPER_H
