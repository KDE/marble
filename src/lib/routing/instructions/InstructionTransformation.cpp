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

namespace Marble
{

RoutingInstructions InstructionTransformation::process( const RoutingWaypoints &model )
{
    RoutingInstructions result;

    foreach( const RoutingWaypoint &item, model ) {
        if ( result.isEmpty() || !result.last().append( item ) ) {
            result.push_back( RoutingInstruction( item ) );
        }
    }

    for ( int i = 0; i < result.size(); ++i ) {
        result[i].setSuccessor( i < result.size() - 1 ? &result[i+1] : 0 );
        result[i].setPredecessor( i ? &result[i-1] : 0 );
    }

    return result;
}

} // namespace Marble
