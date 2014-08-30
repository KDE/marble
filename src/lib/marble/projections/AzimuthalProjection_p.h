//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
// Copyright 2014	   Gábor Péterffy <peterffy95@gmail.com>
//

#ifndef MARBLE_AZIMUTHALPROJECTIONPRIVATE_H
#define MARBLE_AZIMUTHALPROJECTIONPRIVATE_H

#include "AbstractProjection_p.h"


namespace Marble
{

// Maximum amount of nodes that are created automatically between actual nodes.
static const int maxTessellationNodes = 200;


class AzimuthalProjection;

class AzimuthalProjectionPrivate : public AbstractProjectionPrivate
{
public:
    explicit AzimuthalProjectionPrivate( AzimuthalProjection * parent )
        : AbstractProjectionPrivate( parent ),
          q_ptr( parent )
    {

    }

    virtual ~AzimuthalProjectionPrivate() {};

    AzimuthalProjection * const q_ptr;

    Q_DECLARE_PUBLIC( AzimuthalProjection )
};

} // namespace Marble

#endif
