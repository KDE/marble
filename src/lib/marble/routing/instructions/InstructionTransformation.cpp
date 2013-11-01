//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "InstructionTransformation.h"

#include <cmath>

namespace Marble
{

RoutingInstructions InstructionTransformation::process( const RoutingWaypoints &model )
{
    RoutingInstructions result;

    int lastAngle = 0;
    for( int i = 0; i < model.size(); ++i ) {
        const RoutingWaypoint &item = model[i];
        int newAngle = 180 + lastAngle;
        if ( i < model.size() - 1 ) {
            newAngle = qRound( 180.0 / M_PI * item.point().bearing( model[i+1].point() ) );
        }
        int angle = ( newAngle - lastAngle + 540 ) % 360;
        Q_ASSERT( angle >= 0 && angle <= 360 );
        if ( result.isEmpty() || !result.last().append( item, angle ) ) {
            result.push_back( RoutingInstruction( item ) );
        }
        lastAngle = newAngle;
    }

    for ( int i = 0; i < result.size(); ++i ) {
        result[i].setSuccessor( i < result.size() - 1 ? &result[i+1] : 0 );
        result[i].setPredecessor( i ? &result[i-1] : 0 );
    }

    return result;
}

} // namespace Marble
