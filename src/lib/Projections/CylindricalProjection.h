//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007       Inge Wallin  <ingwa@kde.org>
// Copyright 2007-2009  Torsten Rahn  <rahn@kde.org>
// Copyright 2012		Cezar Mocan	<mocancezar@gmail.com>
//

#ifndef MARBLE_CYLINDRICALPROJECTION_H
#define MARBLE_CYLINDRICALPROJECTION_H

/** @file
 * This file contains the headers for CylindricalProjection.
 *
 */

#include "AbstractProjection.h"

namespace Marble 
{

class CylindricalProjectionPrivate;

/**
 * @short A base class for the Equirectangular and Mercator projections in Marble
 */

class CylindricalProjection : public AbstractProjection
{
    // Not a QObject so far because we don't need to send signals.	
  public:
	
	CylindricalProjection();

	virtual ~CylindricalProjection();

    virtual bool repeatableX() const { return true; };

    virtual bool traversablePoles()  const { return false; }
    virtual bool traversableDateLine()  const { return false; }

    virtual SurfaceType surfaceType() const { return Cylindrical; }

    virtual QPainterPath mapShape( const ViewportParams *viewport ) const;

 private:
    Q_DISABLE_COPY( CylindricalProjection )
    CylindricalProjectionPrivate  * d;

};

}

#endif

