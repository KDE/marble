/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

 Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#ifndef MARBLE_GEOSCENETEXTURETILE_H
#define MARBLE_GEOSCENETEXTURETILE_H

#include "GeoSceneTiled.h"

namespace Marble
{

class GeoSceneTextureTile : public GeoSceneTiled
{
 public:

    explicit GeoSceneTextureTile( const QString& name );

    virtual const char* nodeType() const;
    virtual QString type();
};

}

#endif // MARBLE_GEOSCENETEXTURETILE_H
