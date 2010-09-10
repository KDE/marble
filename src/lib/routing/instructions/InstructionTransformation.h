//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_INSTRUCTIONTRANSFORMATION_H
#define MARBLE_INSTRUCTIONTRANSFORMATION_H

#include "RoutingInstruction.h"
#include "RoutingWaypoint.h"
#include "marble_export.h"

namespace Marble
{

/**
  * Transforms waypoints and metadata into driving directions
  */
class MARBLE_EXPORT InstructionTransformation
{
public:
    /** Transforms waypoints and metadata into driving directions */
    static RoutingInstructions process( const RoutingWaypoints &waypoints );

private:
    // Pure static usage
    InstructionTransformation();
    Q_DISABLE_COPY( InstructionTransformation )
};

} // namespace Marble

#endif // MARBLE_INSTRUCTIONTRANSFORMATION_H
