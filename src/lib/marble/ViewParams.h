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

#include "MarbleGlobal.h"


namespace Marble
{

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

    MapQuality mapQuality( ViewContext viewContext ) const;
    MapQuality mapQuality() const;
    void setMapQualityForViewContext( MapQuality quality, ViewContext viewContext );

    ViewContext viewContext() const;
    void setViewContext( ViewContext viewContext );

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
